/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./dirty_vsync_task.h"

#include "arkjs/ArkJS.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/cssom/resolver/StyleTreeResolver.h"
#include "runtime/dom/ark_nodes/list.h"
#include "runtime/dom/ark_nodes/list_item_group.h"
#include "runtime/dom/ark_nodes/swiper.h"
#include "runtime/dom/ark_nodes/water_flow.h"
#include "runtime/dom/element/list.h"
#include "runtime/render.h"
#include "yoga/YGNodeStyle.h"

namespace TaroRuntime {

DirtyTaskPipeline::DirtyTaskPipeline() {}

DirtyTaskPipeline::~DirtyTaskPipeline() {
    CleanAllTasks();
}

void DirtyTaskPipeline::AddUpdateTask(std::function<void()>&& task, bool directExecute) {
    pendingUpdateTasks_.emplace(std::move(task));
    directExecute ? FlushUpdateTasks() : RequestNextFrame();
}

void DirtyTaskPipeline::RequestNextFrame() {
    if (!isRequestVsync_) {
        isRequestVsync_ = true;
        if (!isFlushing_) {
            dirtyKeyFrame_ = TaroRuntime::KeyframeVsync::getKeyFrameVsync(KEY_FRAME_VSYNC::DIRTY);
            dirtyKeyFrame_->attachOneVsync(
                [this](uint64_t time, uint64_t id) {
                    OnVsync(time, id);
                });
        }
    }
}

void DirtyTaskPipeline::OnVsync(uint64_t nanoTimestamp, uint64_t id) {
    std::shared_ptr<TaroThread::TaskExecutor> runner = Render::GetInstance()->GetTaskRunner();
    runner->runSyncTask(TaroThread::TaskThread::MAIN, [this]() {
        FlushUpdateTasks();
    });
}

void DirtyTaskPipeline::FlushUpdateTasks() {
    // 释放requestFrame信号
    isRequestVsync_ = false;
    isFlushing_ = true;
    SystraceSection s("TaroDirtyTaskProcess");
    FlushStart();
    FlushStyle();
    FlushLayout();
    FlushNotifyFixed();
    FlushPaint();
    FlushAdapter();
    FlushMount();
    FlushFinish();
    isFlushing_ = false;
    if (isRequestVsync_) {
        // 如果是任务过程中，请求下一帧
        dirtyKeyFrame_ = TaroRuntime::KeyframeVsync::getKeyFrameVsync(KEY_FRAME_VSYNC::DIRTY);
        dirtyKeyFrame_->attachOneVsync(
            [this](uint64_t time, uint64_t id) {
                OnVsync(time, id);
            });
    }
}

void DirtyTaskPipeline::RegistryNextTick(std::function<void()>&& callback_ref) {
    nextTickPendingTasks_.push_back(callback_ref);
    RequestNextFrame();
}

void DirtyTaskPipeline::AddNotifyFixedRootNodes(const std::shared_ptr<TaroElement>& renderNode) {
    notifyFixedRootNodes_.push(renderNode);
    RequestNextFrame();
}

void DirtyTaskPipeline::RegistryAdapterAttach(std::function<void()>&& callback_ref) {
    adapterAttachTasks_.push_back(callback_ref);
    RequestNextFrame();
}

void DirtyTaskPipeline::AddDirtyStyleNode(const std::shared_ptr<TaroElement>& renderNode) {
    dirtyStyleNodes_.push(renderNode);
    RequestNextFrame();
}

void DirtyTaskPipeline::AddDirtyLayoutNode(const std::shared_ptr<TaroRenderNode>& renderNode) {
    dirtyLayoutNodes_.push(renderNode);
    RequestNextFrame();
}

void DirtyTaskPipeline::AddDirtyPaintNode(const std::shared_ptr<TaroRenderNode>& renderNode) {
    dirtyPaintNodes_.push(renderNode);
    RequestNextFrame();
}

void DirtyTaskPipeline::FlushStart() {
    SystraceSection s("TaroDirtyPipeline:: FlushStart");
    // 触发NextTick回调
    while (!nextTickTasks_.empty()) {
        std::function<void()>& callback_func = nextTickTasks_.front();
        callback_func();
        nextTickTasks_.pop_front();
    }
}

void DirtyTaskPipeline::FlushNotifyFixed() {
    while (!notifyFixedRootNodes_.empty()) {
        if (auto pageRoot = notifyFixedRootNodes_.pop()) {
            pageRoot->NotifyFixedElementVisibility();
        }
    }
}

void DirtyTaskPipeline::FlushStyle() {
    SystraceSection s("TaroDirtyPipeline:: FlushStyle");
    while (!dirtyStyleNodes_.empty()) {
        if (auto element = dirtyStyleNodes_.pop()) {
            TaroCSSOM::StyleTreeResolver styleTreeResolver;
            styleTreeResolver.resolver(element);
        }
    }
}

void DirtyTaskPipeline::FlushLayout() {
    if (dirtyLayoutNodes_.size() > 0) {
        SystraceSection s("TaroDirtyPipeline:: FlushLayout");
        auto oldDirtyLayoutNodes = std::move(dirtyLayoutNodes_);
        dirtyLayoutNodes_ = UniqueWeakPtrQueue<TaroRenderNode>();

        while (!oldDirtyLayoutNodes.empty()) {
            if (auto renderNode = oldDirtyLayoutNodes.pop()) {
                bool isLocalMeasure = false;
                auto parent = renderNode->parent_ref_.lock();
                if (parent) {
                    if (
                        std::dynamic_pointer_cast<TaroDOM::TaroWaterFlowNode>(parent) ||
                        std::dynamic_pointer_cast<TaroDOM::TaroListNode>(parent) ||
                        std::dynamic_pointer_cast<TaroDOM::TaroSwiperNode>(parent)) {
                        isLocalMeasure = true;
                    }
                }
                // 局部布局
                if (isLocalMeasure) {
                    TaroSetCanMeasureChild(parent->ygNodeRef, true);
                }

                renderNode->Measure();
                renderNode->LayoutAll();

                if (isLocalMeasure) {
                    TaroSetCanMeasureChild(parent->ygNodeRef, false);
                }
            }
        }
    }
}

void DirtyTaskPipeline::FlushPaint() {
    SystraceSection s("TaroDirtyPipeline:: FlushPaint");
    while (!dirtyPaintNodes_.empty()) {
        if (auto renderNode = dirtyPaintNodes_.pop()) {
            renderNode->Paint();
            *renderNode->paintDiffer_.old_paint_style_ = *renderNode->paintDiffer_.paint_style_;
            renderNode->paintDiffer_.ClearForceUpdate();
        }
    }
}

void DirtyTaskPipeline::FlushPaintLocalSync(const std::shared_ptr<TaroElement>& paintRoot) {
    SystraceSection s("TaroDirtyPipeline::FlushPaintLocalSync");

    auto it = dirtyPaintNodes_.queue.begin();
    while (it != dirtyPaintNodes_.queue.end()) {
        if (auto sp = it->lock()) {
            bool shouldPaint = false;
            if (auto elementRef = sp->element_ref_.lock()) {
                if (auto measureRoot = elementRef->context_->measure_root_.lock()) {
                    if (measureRoot == paintRoot) {
                        shouldPaint = true;
                    }
                }
            }

            if (shouldPaint) {
                sp->Paint();
                *sp->paintDiffer_.old_paint_style_ = *sp->paintDiffer_.paint_style_;
                sp->paintDiffer_.ClearForceUpdate();
                dirtyPaintNodes_.set.erase(sp->uid_);
                it = dirtyPaintNodes_.queue.erase(it);
            } else {
                ++it;
            }
        } else {
            it = dirtyPaintNodes_.queue.erase(it);
        }
    }
}

void DirtyTaskPipeline::FlushAdapter() {
    SystraceSection s("TaroDirtyPipeline:: FlushAdapter");
    // 懒加载容器组件需要重新上屏任务队列
    while (!adapterAttachTasks_.empty()) {
        std::function<void()>& callback_func = adapterAttachTasks_.front();
        callback_func();
        adapterAttachTasks_.pop_front();
    }
}

void DirtyTaskPipeline::FlushMount() {
    SystraceSection s("TaroDirtyPipeline:: FlushMount");
    auto oldPendingUpdateTasks_ = std::move(pendingUpdateTasks_);
    while (!oldPendingUpdateTasks_.empty()) {
        const auto& task = oldPendingUpdateTasks_.front();
        if (task) {
            task();
        }
        oldPendingUpdateTasks_.pop();
    }
}

void DirtyTaskPipeline::FlushFinish() {
    SystraceSection s("TaroDirtyPipeline:: FlushFinish");
    // 触发NextTick回调
    bool requestNext = false;
    if (!nextTickPendingTasks_.empty()) {
        requestNext = true;
    }
    nextTickPendingTasks_.swap(nextTickTasks_);
    if (requestNext) {
        RequestNextFrame();
    }
    //     while (!nextTickTasks_.empty()) {
    //         std::function<void()>& callback_func = nextTickTasks_.front();
    //         callback_func();
    //         nextTickTasks_.pop_front();
    //     }
}

void DirtyTaskPipeline::CleanAllTasks() {
    notifyFixedRootNodes_.clear();
    dirtyStyleNodes_.clear();
    dirtyLayoutNodes_.clear();
    dirtyPaintNodes_.clear();
    while (!pendingUpdateTasks_.empty()) {
        pendingUpdateTasks_.pop();
    }
    adapterAttachTasks_.clear();
    nextTickTasks_.clear();
}

} // namespace TaroRuntime
