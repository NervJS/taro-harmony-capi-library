/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace TaroHelper {

class TaroClassLifeStatistic {
    public:
    static void init(uint32_t print_interval);
    static void markNew(const std::string& name);
    static void markDelete(const std::string& name);
    static void printUseInfo();

    private:
    static bool needPrint(const std::string& name);

    public:
    static std::unordered_map<std::string, int32_t>* life_stat_;
    static std::mutex life_lock_;
    static std::unordered_set<std::string> print_names_;
};

template <typename T>
class TaroClassLife {
    public:
    TaroClassLife() {
#if USE_CLASS_LIFE_CHECK == 1
        TaroClassLifeStatistic::markNew(typeid(T).name());
#endif
    }
    ~TaroClassLife() {
#if USE_CLASS_LIFE_CHECK == 1
        TaroClassLifeStatistic::markDelete(typeid(T).name());
#endif
    }
};
} // namespace TaroHelper
