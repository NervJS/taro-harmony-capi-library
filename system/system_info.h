/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once
#include <node_api.h>

namespace TaroSystem{
struct DeviceInfo{    
    // Obtains the major (M) version number, which increases with any updates to the overall architecture.0~99
    int major_version_ = 0;
    // Obtains the senior (S) version number, which increases with any updates to the partial.0~99
    int senior_version_ = 0;
    // Obtains the feature (F) version number, which increases with any planned new features..0~99
    int feature_version_ = 0;
    // Obtains the build (B) version number, which increases with each new development build.0~999
    int build_version_ = 0;
};

class SystemInfo{
public:
    static SystemInfo* instance();

    int initDeviceInfo(napi_value n_val);

    // 判断系统版本是否低于给定的版本id
    bool isLessVersion(int major_version, int senior_version, int feature_version, int build_version);
    
    // 判断系统版本是否高于给定的版本id
    bool isGreatVersion(int major_version, int senior_version, int feature_version, int build_version);

    const DeviceInfo& device();

private:
    DeviceInfo device_info_;
};
} //  namespace TaroSystem
