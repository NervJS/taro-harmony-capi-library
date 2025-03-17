#include "life_statistic.h"

#include <thread>
#include <unistd.h>

#include "helper/TaroLog.h"
#include "api_cost_statistic.h"

namespace TaroHelper {
std::unordered_map<std::string, int32_t>* TaroClassLifeStatistic::life_stat_ = nullptr;
std::mutex TaroClassLifeStatistic::life_lock_;
std::unordered_set<std::string> TaroClassLifeStatistic::print_names_ = {
    "TaroElement", "TaroRenderNode", "NodeHandle"
};

void thread_fun(uint32_t print_interval) {
    while (true) {
        TaroClassLifeStatistic::printUseInfo();
        TimeCostStatistic::instance()->printCostInfo();
        sleep(print_interval);
    }
}

void TaroClassLifeStatistic::init(uint32_t print_interval) {
#if USE_CLASS_LIFE_CHECK == 1
    std::thread print_thread(thread_fun, print_interval);
    print_thread.detach();
#endif
}

void TaroClassLifeStatistic::markNew(const std::string& name) {
#if USE_CLASS_LIFE_CHECK == 1
    std::lock_guard<std::mutex> lock_guard(life_lock_);
    if (life_stat_ == nullptr) {
        life_stat_ = new std::unordered_map<std::string, int32_t>();
    }
    auto iter = life_stat_->find(name);
    if (iter == life_stat_->end()) {
        (*life_stat_)[name] = 1;
    } else {
        (*life_stat_)[name]++;
    }
#endif
}
void TaroClassLifeStatistic::markDelete(const std::string& name) {
#if USE_CLASS_LIFE_CHECK == 1
    std::lock_guard<std::mutex> lock_guard(life_lock_);
    if (life_stat_ == nullptr) {
        life_stat_ = new std::unordered_map<std::string, int32_t>();
    }
    auto iter = life_stat_->find(name);
    if (iter == life_stat_->end()) {
        TARO_LOG_ERROR("TaroLifeCheck", "invalid markDelete %{public}s", name.c_str());
        (*life_stat_)[name] = -1;
    } else {
        (*life_stat_)[name]--;
    }
#endif
}

bool TaroClassLifeStatistic::needPrint(const std::string& name) {
    for (const auto& elem : print_names_) {
        if (name.find(elem) != std::string::npos) {
            return true;
        }
    }
    return false;
}


void TaroClassLifeStatistic::printUseInfo() {
#if USE_CLASS_LIFE_CHECK == 1
    if (life_stat_ == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock_guard(life_lock_);
    TARO_LOG_DEBUG("TaroLifeCheck", "Taro Class Using Infomation start...");
    for (const auto& elem : *life_stat_) {
        if (elem.second != 0 && needPrint(elem.first)) {
            TARO_LOG_DEBUG("TaroLifeCheck", "Class:%{public}s using count:%{public}d", elem.first.c_str(), elem.second);
        }
    }
    TARO_LOG_DEBUG("TaroLifeCheck", "Taro Class Using Infomation end...");
#endif
};
} // namespace TaroHelper
