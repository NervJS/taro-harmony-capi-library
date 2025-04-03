/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <mutex>
#include <napi/native_api.h>

#include "helper/TaroTimer.h"

namespace TaroHelper {
class FunctionTimeCost final {
    public:
    explicit FunctionTimeCost(const std::string& func_name, napi_env env, napi_callback_info info);
    ~FunctionTimeCost();

    TaroHelper::MsTimer timer_;
    std::string func_name_;
    napi_env env_;
    napi_callback_info info_;
};

class TimeCostStatistic {
    public:
    static TimeCostStatistic* instance();
    void record(const std::string& key, uint32_t cost);
    void printCostInfo();
    void record(int32_t prop_name);
    std::mutex lock_;
    // <key, <total_cost, total_count, max_cost>>
    std::unordered_map<std::string, std::tuple<uint64_t, uint32_t, uint32_t>> cost_stats_;
    std::unordered_map<int32_t, int32_t> prop_stats_;
};

#if IS_DEBUG
#define API_FUNC_COST() TaroHelper::FunctionTimeCost capi_fun_cost##__LINE__(__FUNCTION__, env, info)
#else
#define API_FUNC_COST()
#endif
} // namespace TaroHelper
