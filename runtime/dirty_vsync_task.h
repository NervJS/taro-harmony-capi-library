/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_CPP_DIRTY_VSYNC_TASK_H
#define TARO_HARMONY_CPP_DIRTY_VSYNC_TASK_H

#include <cstdint>
#include <functional>
#include <queue>
#include <deque>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include "runtime/dom/ark_nodes/arkui_node.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {

// 自定义哈希函数
struct WeakPtrHash {
    template <typename T>
    std::size_t operator()(const std::weak_ptr<T>& wp) const {
        return reinterpret_cast<std::size_t>(wp.lock().get());
    }
};

// 自定义比较函数
struct WeakPtrEqual {
    template <typename T>
    bool operator()(const std::weak_ptr<T>& wp1, const std::weak_ptr<T>& wp2) const {
        return wp1.lock().get() == wp2.lock().get();
    }
};

// ID 获取函数的主模板
template<typename T>
struct IDGetter {
    static int get(const T& obj) {
        static_assert(sizeof(T) == 0, "No specialization for IDGetter<T>");
        return 0;
    }
};

// TaroRenderNode 的特化
template<>
struct IDGetter<TaroRuntime::TaroDOM::TaroRenderNode> {
    static int get(const TaroRuntime::TaroDOM::TaroRenderNode& obj) {
        return obj.uid_;
    }
};

// TaroElement 的特化
template<>
struct IDGetter<TaroRuntime::TaroDOM::TaroElement> {
    static int get(const TaroRuntime::TaroDOM::TaroElement& obj) {
        return obj.nid_;
    }
};

template<typename T>
class UniqueWeakPtrQueue {

public:
    std::deque<std::weak_ptr<T>> queue;
    std::unordered_set<int> set;

    UniqueWeakPtrQueue() = default;

    void push(const std::shared_ptr<T>& value) {
        int id = IDGetter<T>::get(*value);
        if (set.insert(id).second) {
            queue.push_back(value);
        }
    }

    std::shared_ptr<T> pop() {
        while (!queue.empty()) {
            std::weak_ptr<T> wp = queue.front();
            queue.pop_front();
            if (auto sp = wp.lock()) {
                set.erase(IDGetter<T>::get(*sp));
                return sp;
            }
        }
        set.clear();
        return nullptr;
    }

    std::shared_ptr<T> front() {
        for (const auto& wp : queue) {
            if (auto sp = wp.lock()) {
                return sp;
            }
        }
        return nullptr;
    }

    bool empty() const {
        return set.empty();
    }

    size_t size() const {
        return set.size();
    }

    void cleanup() {
        auto it = std::remove_if(queue.begin(), queue.end(),
            [this](const std::weak_ptr<T>& wp) {
                if (wp.expired()) {
                    if (auto sp = wp.lock()) {
                        set.erase(IDGetter<T>::get(*sp));
                    }
                    return true;
                }
                return false;
            });
        queue.erase(it, queue.end());

        // 更新 set 以只包含有效元素
        set.clear();
        for (const auto& wp : queue) {
            if (auto sp = wp.lock()) {
                set.insert(IDGetter<T>::get(*sp));
            }
        }
    }

    void clear() {
        queue.clear();
        set.clear();
    }

    bool contains(const std::weak_ptr<T>& value) const {
        if (auto sp = value.lock()) {
            return set.find(IDGetter<T>::get(*sp)) != set.end();
        }
        return false;
    }

    bool contains(const std::shared_ptr<T>& value) const {
        return contains(std::weak_ptr<T>(value));
    }
};

class DirtyTaskPipeline {
    using TaroRenderNode = TaroRuntime::TaroDOM::TaroRenderNode;
    using TaroElement = TaroRuntime::TaroDOM::TaroElement;

    public:
    static DirtyTaskPipeline* GetInstance() {
        static DirtyTaskPipeline* pipeline = new DirtyTaskPipeline();
        return pipeline;
    }

    DirtyTaskPipeline();

    ~DirtyTaskPipeline();

    void AddNotifyFixedRootNodes(const std::shared_ptr<TaroElement>& renderNode);

    // 增加更新指令任务
    void AddUpdateTask(std::function<void()>&& task, bool directExecute = false);
    // 请求Vsync处理下一帧清理任务
    void RequestNextFrame();
    // 注册监听下一次Vsync的回调
    void RegistryNextTick(std::function<void()>&& callback_func);
    // 注册监听下一次Vsync时，节点上屏
    void RegistryAdapterAttach(std::function<void()>&& callback_func);
    // 执行任务队列：1、样式匹配；2、排版布局；3、绘制；4、挂载/卸载
    void FlushUpdateTasks();
    // 添加Style更新的节点
    void AddDirtyStyleNode(const std::shared_ptr<TaroElement>& renderNode);
    // 添加布局更新的节点
    void AddDirtyLayoutNode(const std::shared_ptr<TaroRenderNode>& renderNode);
    // 添加绘制更新的节点
    void AddDirtyPaintNode(const std::shared_ptr<TaroRenderNode>& renderNode);
    // 清空队列任务
    void CleanAllTasks();

    // 立马执行绘制任务，处理某个子树下的所有paint任务
    void FlushPaintLocalSync(const std::shared_ptr<TaroElement>& paintRoot);

    private:
    // vsync key frame
    std::shared_ptr<TaroRuntime::KeyframeVsync> dirtyKeyFrame_ = nullptr;
    // 是否已经请求了Vsync
    bool isRequestVsync_ = false;
    bool isFlushing_ = false;
    // TODO: 下面3个std::unordered_set更换为std::set更佳, 将depth较低的摆放在前面，优先处理顶层节点，可减少一些重复处理的情况

    UniqueWeakPtrQueue<TaroElement> notifyFixedRootNodes_;
    // 需要重新匹配样式的脏节点
    UniqueWeakPtrQueue<TaroElement> dirtyStyleNodes_;
    // 需要重新排版布局的脏节点
    UniqueWeakPtrQueue<TaroRenderNode> dirtyLayoutNodes_;
    // 需要重新绘制的脏节点
    UniqueWeakPtrQueue<TaroRenderNode> dirtyPaintNodes_;
    // 懒加载容器组件需要重新上屏任务队列
    std::list<std::function<void()>> adapterAttachTasks_;
    // 挂载任务队列
    std::queue<std::function<void()>> pendingUpdateTasks_;
    // NextTick即将执行任务队列
    std::list<std::function<void()>> nextTickTasks_;
    // NextTick等待执行任务队列
    std::list<std::function<void()>> nextTickPendingTasks_;
    // vsync回调
    void OnVsync(uint64_t nanoTimestamp, uint64_t id);
    // 任务执行
    void FlushStart();
    void FlushStyle();
    void FlushLayout();
    void FlushPaint();
    void FlushAdapter();
    void FlushMount();
    void FlushFinish();
    void FlushNotifyFixed();
};

} // namespace TaroRuntime

#endif // TARO_HARMONY_CPP_DIRTY_VSYNC_TASK_H
