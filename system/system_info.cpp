/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "system_info.h"
#include "arkjs/ArkJS.h"
#include "helper/TaroLog.h"
#include "runtime/NativeNodeApi.h"

namespace TaroSystem{
SystemInfo* SystemInfo::instance() {
    static auto s_instance = new SystemInfo();
    return s_instance;
}

int SystemInfo::initDeviceInfo(napi_value n_val){
    ArkJS ark_js(TaroRuntime::NativeNodeApi::env);
    try {
        napi_value n_detail_info = ark_js.getObjectProperty(n_val, "deviceInfo");
        
        auto n_major_version = ark_js.getObjectProperty(n_detail_info, "majorVersion");
        device_info_.major_version_ = ark_js.getInteger(n_major_version);
        auto n_senior_version = ark_js.getObjectProperty(n_detail_info, "seniorVersion");
        device_info_.senior_version_ = ark_js.getInteger(n_senior_version);
        auto n_feature_version = ark_js.getObjectProperty(n_detail_info, "featureVersion");
        device_info_.feature_version_ = ark_js.getInteger(n_feature_version);
        auto n_build_version = ark_js.getObjectProperty(n_detail_info, "buildVersion");
        device_info_.build_version_ = ark_js.getInteger(n_build_version);
    } catch (const std::runtime_error& e) {
        TARO_LOG_ERROR("TaroSystem", "get system info failed %{public}s", e.what());
        return -1;
    }
    return 0;
}

const DeviceInfo& SystemInfo::device(){
    return device_info_;
}

bool SystemInfo::isLessVersion(int major_version, int senior_version, int feature_version, int build_version) {
    return device_info_.major_version_ < major_version ||
           (device_info_.major_version_ == major_version && 
            (device_info_.senior_version_ < senior_version ||
             (device_info_.senior_version_ == senior_version && 
              (device_info_.feature_version_ < feature_version ||
               (device_info_.feature_version_ == feature_version &&
                device_info_.build_version_ < build_version)))));
}

bool SystemInfo::isGreatVersion(int major_version, int senior_version, int feature_version, int build_version) {
    return device_info_.major_version_ > major_version ||
       (device_info_.major_version_ == major_version && 
        (device_info_.senior_version_ > senior_version ||
         (device_info_.senior_version_ == senior_version && 
          (device_info_.feature_version_ > feature_version ||
           (device_info_.feature_version_ == feature_version &&
            device_info_.build_version_ > build_version)))));
}
} //  namespace TaroSystem
