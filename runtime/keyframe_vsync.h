/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <functional>
#include <list>
#include <mutex>
#include <native_vsync/native_vsync.h>
#include <stdint.h>

namespace TaroRuntime {
using KeyframeVsyncFun = std::function<void(uint64_t, uint32_t)>;
enum class VsyncPriority {
    Low = 0,    // 低优先级
    Medium = 1, // 中优先级
    High = 2,    // 高优先级
    Highest = 3    // 最高优先级
};

class KeyframeVsync {
    private:
    struct VsyncFunInfo {
        uint32_t id_ = 0;
        KeyframeVsyncFun fun_ = nullptr;
        VsyncPriority priority_ = VsyncPriority::Low;
        bool is_one = false;
    };

    public:
    explicit KeyframeVsync(const std::string& mark);

    ~KeyframeVsync();

    static std::shared_ptr<KeyframeVsync> getKeyFrameVsync(const std::string& mark);

    public:
    // 一直触发，返回id
    uint32_t attachVsync(const KeyframeVsyncFun& callback, VsyncPriority priority = VsyncPriority::Low);

    // 只触发一次，返回id
    uint32_t attachOneVsync(const KeyframeVsyncFun& callback, VsyncPriority priority = VsyncPriority::Low);

    // 删除触发
    void deleteVsync(uint32_t id);

    // 触发所有sync函数
    void onFrame(long long timestamp);

    static void onSync(long long timestamp, void* data);

    private:
    uint32_t genId();

    void attachInner(const VsyncFunInfo& info);

    void bindHMSync();

    void unbindHMSync();

    std::list<VsyncFunInfo> vsync_fun_;

    // 为避免死锁使用，attachVsync需要添加到此临时变量
    std::list<VsyncFunInfo> tmp_vsync_fun_;
    std::list<uint32_t> tmp_delete_id_;
    // 保护tmp_vsync_fun_和tmp_delete_fun_
    std::mutex tmp_vsync_fun_lock_;

    OH_NativeVSync* vsync_ = nullptr;
    std::atomic<uint32_t> id_no_ = 1;

    bool has_vsync = false;
    std::mutex has_vsync_lock_;
};
} // namespace TaroRuntime
