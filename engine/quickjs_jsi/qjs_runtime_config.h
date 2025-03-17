//
// Created on 2024/6/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <string>
#include <vector>

#include "engine/react_common/js_big_string.h"

struct QuickJSRuntimeConfig {
    QuickJSRuntimeConfig() {}
    ~QuickJSRuntimeConfig() = default;

    QuickJSRuntimeConfig(const QuickJSRuntimeConfig &rhs) = delete;
    QuickJSRuntimeConfig &operator=(const QuickJSRuntimeConfig &rhs) = delete;

    QuickJSRuntimeConfig(QuickJSRuntimeConfig &&rhs) = default;
    QuickJSRuntimeConfig &operator=(QuickJSRuntimeConfig &&rhs) = default;

    // Olson timezone ID
    std::string timezoneId;

    // true to enable quickjs inspector for Chrome DevTools
    bool enableInspector = false;

    // Application name
    std::string appName;

    // Device name
    std::string deviceName;

    // Startup snapshot blob
    std::unique_ptr<const JSBigString> snapshotBlob;

    enum struct CodecacheMode : uint8_t {
        // Disable bytecode caching
        kNone = 0,
        // Classic quickjs bytecode caching
        kNormal,
        // **EXPERIMENTAL** Classic quickjs bytecode caching + loading stub JS bundle
        // when cache existed
        kStubBundle,
    };

    // Bytecode caching mode
    CodecacheMode codecacheMode;

    // The directory to store codecache files
    std::string codecacheDir;
};
