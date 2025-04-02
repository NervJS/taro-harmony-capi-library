/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <cstdio>
#include <string>
#include <hilog/log.h>

#include "./Debug.h"

namespace TaroLog {
const unsigned int LOG_PRINT_DOMAIN = 0xFF00;
std::string GetCallStack();

#define TARO_LOG_PREFIX "TARO_LOG:"

// 此处有性能开销，但符合通用printf写法
#define TARO_LOG_INTER1(debug_level, module_name, format, ...)                                                               \
    {                                                                                                                        \
        char taro_log_tmp_buf[1024];                                                                                         \
        snprintf(taro_log_tmp_buf, sizeof(taro_log_tmp_buf), format, __VA_ARGS__);                                           \
        OH_LOG_Print(LOG_APP, debug_level, TaroLog::LOG_PRINT_DOMAIN, TARO_LOG_PREFIX module_name,                           \
                     "[%{public}s] %{public}s --%{public}s:%{public}d", __FUNCTION__, taro_log_tmp_buf, __FILE__, __LINE__); \
    }

// 无额外性能开销，但是使用需加public
#define TARO_LOG_INTER2(debug_level, module_name, format, ...)                                                 \
    OH_LOG_Print(LOG_APP, debug_level, TaroLog::LOG_PRINT_DOMAIN, TARO_LOG_PREFIX module_name,                 \
                 "[%{public}s] %{public}s" #format " --%{public}s:%{public}d version(%{public}s_v%{public}s)", \
                 __FUNCTION__, __VA_ARGS__, __FILE__, __LINE__, CMAKE_BUILD_TYPE, TARO_PACKAGE_VERSION)

// 处理无__VA_ARGS__的情况
#define TARO_LOG_INTER(debug_level, module_name, format, ...) \
    TARO_LOG_INTER2(debug_level, module_name, format, "", ##__VA_ARGS__)

// 参数为：模块名称、format、format_vars
// #if IS_DEBUG == 1
#define TARO_LOG_DEBUG(...) TARO_LOG_INTER(LOG_DEBUG, __VA_ARGS__)
// #else
// #define TARO_LOG_DEBUG(...) do {} while(0)
// #endif
#define TARO_LOG_INFO(...) TARO_LOG_INTER(LOG_INFO, __VA_ARGS__)
#define TARO_LOG_WARN(...) TARO_LOG_INTER(LOG_WARN, __VA_ARGS__)
#define TARO_LOG_ERROR(...) TARO_LOG_INTER(LOG_ERROR, __VA_ARGS__)
#define TARO_LOG_FATAL(...)                 \
    TARO_LOG_INTER(LOG_FATAL, __VA_ARGS__); \
    TARO_LOG_INTER(LOG_FATAL, "CallStack", "%{public}s", TaroLog::GetCallStack().c_str())
} // namespace TaroLog
