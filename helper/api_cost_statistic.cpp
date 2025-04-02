/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "api_cost_statistic.h"

#include "arkjs/ArkJS.h"

namespace TaroHelper {
FunctionTimeCost::FunctionTimeCost(const std::string& func_name, napi_env env, napi_callback_info info)
    : func_name_(func_name),
      env_(env),
      info_(info) {
    timer_.Start();
}
FunctionTimeCost::~FunctionTimeCost() {
    // if (func_name_ == "GetTaroNodeProperty") {
    //     ArkJS arkJs(env_);
    //     auto args = arkJs.getCallbackArgs(info_, 3);
    //     func_name_ += "_" + arkJs.getString(args[1]);
    // }
    TimeCostStatistic::instance()->record(func_name_, timer_.ClockUs());
}

TimeCostStatistic* TimeCostStatistic::instance() {
    static auto s_instance = new TimeCostStatistic();
    return s_instance;
}

void TimeCostStatistic::record(const std::string& key, uint32_t cost) {
    std::lock_guard<std::mutex> lock_guard(lock_);
    auto iter = cost_stats_.find(key);
    if (iter == cost_stats_.end()) {
        cost_stats_[key] = {cost, 1, cost};
        return;
    }
    std::get<0>(iter->second) += cost;
    std::get<1>(iter->second)++;
}

void TimeCostStatistic::record(int32_t prop_name) {
    std::lock_guard<std::mutex> lock_guard(lock_);
    auto iter = prop_stats_.find(prop_name);
    if (iter == prop_stats_.end()) {
        prop_stats_[prop_name] = 1;
        return;
    }
    iter->second++;
}

void TimeCostStatistic::printCostInfo() {
    std::lock_guard<std::mutex> lock_guard(lock_);
    uint32_t total_cost = 0;
    uint32_t total_count = 0;
    for (const auto& elem : cost_stats_) {
        TARO_LOG_ERROR("TimeCost", "Fun:%{public}s : cost:%{public}d us, count:%{public}d max:%{public}d us",
                       elem.first.c_str(), std::get<0>(elem.second), std::get<1>(elem.second), std::get<2>(elem.second));
        total_cost += std::get<0>(elem.second);
        total_count += std::get<1>(elem.second);
    }
    cost_stats_.clear();
    for (const auto& elem : prop_stats_) {
        TARO_LOG_ERROR("TimeCost", "Prop stat:%{public}d count:%{public}d",
                       elem.first, elem.second);
    }
    prop_stats_.clear();
    TARO_LOG_ERROR("TimeCost", "Function total cost:%{public}d us, count:%{public}d", total_cost, total_count);
}

} // namespace TaroHelper
