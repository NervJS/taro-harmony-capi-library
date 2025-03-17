//
// Created on 2024/6/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_TDEBUG_H
#define HARMONY_LIBRARY_TDEBUG_H

#include "helper/TaroLog.h"

// 打印整数
#define COUT_d(value) TARO_LOG_DEBUG("COUT_d", "%{public}s = %{public}d", #value, int(value));
#define COUT_dd(value, title) TARO_LOG_DEBUG("COUT_dd", "%{public}s = %{public}d", std::string(title).c_str(), int(value));

// 打印浮点数
#define COUT_f(value) TARO_LOG_DEBUG("COUT_f", "%{public}s = %{public}f", #value, double(value));
#define COUT_ff(value, title) TARO_LOG_DEBUG("COUT_ff", "%{public}s = %{public}f", std::string(title).c_str(), double(value));

// 打印字符串
#define COUT_s(value) TARO_LOG_DEBUG("COUT_s", "%{public}s = %{public}s", #value, std::string(value).c_str());
#define COUT_ss(value, title) TARO_LOG_DEBUG("COUT_ss", "%{public}s = %{public}s", std::string(title).c_str(), std::string(value).c_str());

#endif // HARMONY_LIBRARY_TDEBUG_H
