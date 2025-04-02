/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "ArkJS.h"

namespace Taro {

struct PhysicalPixels {
    float width;
    float height;
    float scale;
    float fontScale;
    float densityDpi;

    static PhysicalPixels fromNapiValue(napi_env env, napi_value value);
};

struct DisplayMetrics {
    PhysicalPixels windowPhysicalPixels;
    PhysicalPixels screenPhysicalPixels;

    static DisplayMetrics fromNapiValue(napi_env env, napi_value value);
};

class ArkTSBridge final {
    static std::shared_ptr<ArkTSBridge> instance;
    ArkTSBridge(napi_env env, napi_ref napiBridgeRef);

    public:
    using Shared = std::shared_ptr<ArkTSBridge>;

    static void initializeInstance(napi_env env, napi_ref arkTSBridgeHandlerRef);

    static ArkTSBridge::Shared getInstance();

    ArkTSBridge(ArkTSBridge const&) = delete;
    ArkTSBridge& operator=(ArkTSBridge const&) = delete;

    ~ArkTSBridge();

    void handleError(std::exception_ptr ex);
    DisplayMetrics getDisplayMetrics();

    protected:
    ArkJS m_arkJs;
    napi_ref m_arkTSBridgeRef;
};
} // namespace Taro