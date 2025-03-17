#include "keyframe_vsync.h"

#include <functional>
#include <string>
#include <native_vsync/native_vsync.h>
#include <stdint.h>

#include "helper/TaroLog.h"
#include "helper/Time.h"

namespace TaroRuntime {

KeyframeVsync::KeyframeVsync(const std::string& mark) {
    vsync_ = OH_NativeVSync_Create(mark.c_str(), mark.length());
    if (vsync_ == nullptr) {
        TARO_LOG_FATAL("KeyframeVsync", "OH_NativeVSync_Create failed");
        throw "OH_NativeVSync_Create failed";
    }
}

KeyframeVsync::~KeyframeVsync() {
    if (vsync_ != nullptr) {
        OH_NativeVSync_Destroy(vsync_);
        vsync_ = nullptr;
    }
}

std::shared_ptr<KeyframeVsync> KeyframeVsync::getKeyFrameVsync(const std::string& mark) {
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    static std::unordered_map<std::string, std::shared_ptr<KeyframeVsync>>* static_map = nullptr;
    if (static_map == nullptr) {
        static_map = new std::unordered_map<std::string, std::shared_ptr<KeyframeVsync>>();
    }
    auto iter = static_map->find(mark);
    if (iter != static_map->end()) {
        return iter->second;
    }
    auto ret_obj = std::make_shared<KeyframeVsync>(mark);
    (*static_map)[mark] = ret_obj;
    return ret_obj;
}

uint32_t KeyframeVsync::genId() {
    return id_no_.fetch_add(1);
}

void KeyframeVsync::attachInner(const VsyncFunInfo& info) {
    // TARO_LOG_DEBUG("KeyframeVsync", "attach id:%{public}d", info.id_);
    // lock
    std::lock_guard lock(tmp_vsync_fun_lock_);
    tmp_vsync_fun_.push_back(info);

    std::lock_guard has_vsync_lock(has_vsync_lock_);
    if (!has_vsync) { // 第一次绑定
        has_vsync = true;
        bindHMSync();
    }
}

uint32_t KeyframeVsync::attachVsync(const KeyframeVsyncFun& callback, VsyncPriority priority) {
    VsyncFunInfo fun_info;
    fun_info.id_ = genId();
    fun_info.fun_ = callback;
    fun_info.priority_ = priority;
    fun_info.is_one = false;
    attachInner(fun_info);
    return fun_info.id_;
}

// 只触发一次，返回id
uint32_t KeyframeVsync::attachOneVsync(const KeyframeVsyncFun& callback, VsyncPriority priority) {
    VsyncFunInfo fun_info;
    fun_info.id_ = genId();
    fun_info.fun_ = callback;
    fun_info.priority_ = priority;
    fun_info.is_one = true;
    attachInner(fun_info);
    return fun_info.id_;
}

void KeyframeVsync::onFrame(long long timestamp) {
    // 处理attach&delete
    {
        std::lock_guard lock(tmp_vsync_fun_lock_);
        // 处理attach
        for (const auto& elem : tmp_vsync_fun_) {
            auto iter = vsync_fun_.begin();
            while (iter != vsync_fun_.end() && iter->priority_ >= elem.priority_) {
                ++iter;
            }
            vsync_fun_.insert(iter, elem);
        }
        tmp_vsync_fun_.clear();

        // 处理delete
        for (const auto& id : tmp_delete_id_) {
            vsync_fun_.remove_if([id](const VsyncFunInfo& value) {
                return value.id_ == id;
            });
        }
        tmp_delete_id_.clear();
    }

    // 执行回调函数
    uint64_t current_time = TaroHelper::TaroTime::getCurrentMsTime();
    for (const auto& elem : vsync_fun_) {
        elem.fun_(current_time, elem.id_);
        if (elem.is_one) {
            // mark 删除
            deleteVsync(elem.id_);
        }
    }

    if (!vsync_fun_.empty()) { // 此处has_vsync必为true，重复绑定
        bindHMSync();
    } else {
        std::lock_guard lock(tmp_vsync_fun_lock_);
        if (tmp_vsync_fun_.empty()) { // 没有添加事件
            std::lock_guard lock2(has_vsync_lock_);
            has_vsync = false;
        } else { // 此处has_vsync必为true
            bindHMSync();
        }
    }
}

void KeyframeVsync::onSync(long long timestamp, void* data) {
    auto p_data = (KeyframeVsync*)data;
    if (p_data == nullptr) {
        return;
    }
    p_data->onFrame(timestamp);
}

// 删除触发
void KeyframeVsync::deleteVsync(uint32_t id) {
    // TARO_LOG_DEBUG("KeyframeVsync", "deleteVsync id=%{public}u", id);
    std::lock_guard lock(tmp_vsync_fun_lock_);
    tmp_delete_id_.push_back(id);
}

void KeyframeVsync::bindHMSync() {
    auto ret = OH_NativeVSync_RequestFrame(vsync_, KeyframeVsync::onSync, this);
    if (ret != 0) {
        TARO_LOG_ERROR("KeyframeVsync", "OH_NativeVSync_RequestFrame failed, ret=%{public}d",
                       ret);
        has_vsync = false; // 以期让其他的bind触发
    }
}

} // namespace TaroRuntime
