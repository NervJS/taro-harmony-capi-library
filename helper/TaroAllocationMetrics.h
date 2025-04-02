/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "./TaroLog.h"

namespace TaroAllocation {
// TODO: 单线程没问题，后续如果涉及多线程可能需要考虑原子变量
struct AllocationMetrics {
    int TotalAllocated = 0;
    int TotalFreed = 0;
    int CurrentUsage() {
        return TotalAllocated - TotalFreed;
    };
};

void PrintMemoryUsage();
} // namespace TaroAllocation

#if IS_DEBUG == 1
#define TARO_PRINT_MEMORY() TaroAllocation::PrintMemoryUsage()
#else
#define TARO_PRINT_MEMORY() do {} while(0)
#endif
