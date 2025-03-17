#include "frame_trigger_master.h"

#include "helper/TaroLog.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/animation/animator/utils.h"
#include "../animation_lock.h"

namespace TaroRuntime {
namespace TaroAnimate {

TaroFrameTriggerMaster *TaroFrameTriggerMaster::instance() {
    static TaroFrameTriggerMaster *g_instance = new TaroFrameTriggerMaster();
    return g_instance;
}

TaroFrameTriggerMaster::TaroFrameTriggerMaster() {
    keyframe_vsync = KeyframeVsync::getKeyFrameVsync("TaroAnimation");
}
TaroFrameTriggerMaster::~TaroFrameTriggerMaster() {
    if (keyframe_vsync != nullptr) {
        keyframe_vsync = nullptr;
    }
}

    // static void onVSync(long long timestamp, void *data) {
    //     // TARO_LOG_DEBUG("TaroAnimation", "OnVsnc timestamep:%{public}ld",timestamp);
    //     uint64_t current_time = TaroUtils::getCurrentMsTime();
    //     TaroAnimate::TaroFrameTriggerMaster::instance()->onFrame(current_time);
    // }

void TaroFrameTriggerMaster::registerTrigger(
    uint32_t trigger_id, std::shared_ptr<TaroFrameTrigger> trigger,
    uint32_t xid) {
    TARO_LOG_DEBUG(
        "TaroAnimation",
        "registerTrigger trigger_id:%{public}u xid:%{public}u thread:%{public}d",
        trigger_id, xid, pthread_self());
    std::lock_guard lock(queue_lock_);
    OperateInfo operate_info;
    operate_info.is_delete = false;
    operate_info.trigger_id = trigger_id;
    operate_info.xid = xid;
    operate_info.trigger_ = trigger;
    operate_queue_.push_back(operate_info);
    if (vsync_id_ <= 0) {
        auto recall_fun =
        std::bind(&TaroFrameTriggerMaster::onFrame, this, std::placeholders::_1);
        vsync_id_ = keyframe_vsync->attachVsync(recall_fun);
    }
}

void TaroFrameTriggerMaster::unregisterTrigger(uint32_t trigger_id,
                                                uint32_t xid) {
    std::lock_guard lock(queue_lock_);
    OperateInfo operate_info;
    operate_info.is_delete = true;
    operate_info.trigger_id = trigger_id;
    operate_info.xid = xid;
    operate_queue_.push_back(operate_info);
}

void TaroFrameTriggerMaster::onFrame(uint64_t current_time) {
    std::lock_guard lock(AnimationLock::getAnimationLock());
    // TARO_LOG_DEBUG("TaroAnimation", "time:%{public}lu", current_time);
    {
        std::lock_guard lock(queue_lock_);
        for (auto elem : operate_queue_) {
            if (elem.is_delete) {
                map_triggers_.erase(elem.trigger_id); // 从map_triggers_删除
                auto iter_xid = xid_to_triggers_.find(elem.xid);
                if (iter_xid != xid_to_triggers_.end()) {
                    // 从xid_to_triggers_做删除
                    iter_xid->second.erase(elem.trigger_id);
                    if (iter_xid->second.empty()) {
                        xid_to_triggers_.erase(iter_xid);
                    }
                } else {
                    TARO_LOG_ERROR("TaroAnimation",
                                    "trigger_id:%{public}u x_id:%{public}u conflict",
                                    elem.trigger_id, elem.xid);
                }
            }
            else {
                map_triggers_[elem.trigger_id] = elem.trigger_;
                xid_to_triggers_[elem.xid].insert(elem.trigger_id);
                active_xids_.insert(elem.xid);
            }
        }
        operate_queue_.clear();
    }
    
    //  只触发active xcomponent上的trigger
    for (auto xid : active_xids_) {
        auto xid_iter = xid_to_triggers_.find(xid);
        // Component未挂载任何trigger
        if (xid_iter == xid_to_triggers_.end()) {
            continue;
        }
        for (auto trigger_id : xid_iter->second) { // 挨个执行触发器
            auto trigger_iter = map_triggers_.find(trigger_id);
            if (trigger_iter == map_triggers_.end()) { // 正常情况不会走到这里
                TARO_LOG_ERROR("TaroAnimation", "trigger_id:%{public}u conflict",
                                trigger_id);
                continue;
            }
            auto trigger = trigger_iter->second.lock();
            if (trigger == nullptr) { // 已被删除，需要清理
                unregisterTrigger(trigger_id, xid);
                continue;
            }
            trigger->tick(current_time);
        }
    }
}

void TaroFrameTriggerMaster::setXComponentActive(
    const std::vector<std::pair<uint32_t, bool>> &xids_to_active) {
    for (const auto &elem : xids_to_active) {
        changeXComponentState(elem.first, elem.second);
    }
}

void TaroFrameTriggerMaster::changeXComponentState(uint32_t xid,
                                                    bool b_active) {
    std::lock_guard lock(AnimationLock::getAnimationLock());
    auto iter = xid_to_triggers_.find(xid);
    if (iter == xid_to_triggers_.end()) {
        return;
    }

    for (auto elem : iter->second) {
        auto trigger_iter = map_triggers_.find(elem);
        if (trigger_iter == map_triggers_.end()) {
            TARO_LOG_ERROR("TaroAnimation", "trigger_id: %{public}u is not found",
                            elem);
            continue;
        }
        auto trigger = trigger_iter->second.lock();
        if (trigger) {
            if (b_active) {
                trigger->resume();
            } else {
                trigger->pause();
            }
        }
    }
}

} // namespace TaroAnimate
} // namespace TaroRuntime
