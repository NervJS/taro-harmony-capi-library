/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./TaroAllocationMetrics.h"

#include <cstdlib>
#include <new>

namespace TaroAllocation {
static AllocationMetrics s_AllocationMetrics;

void PrintMemoryUsage() {
    TARO_LOG_DEBUG("TaroAllocation", "Memory Usage: %{public}d bytes.", s_AllocationMetrics.CurrentUsage());
}
} // namespace TaroAllocation

#if IS_DEBUG == 1
// 内存分配
void* operator new(size_t size) {
    void* ptr = malloc(size);
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }
    TaroAllocation::s_AllocationMetrics.TotalAllocated += size;
    return ptr;
}
void* operator new[](size_t size) {
    void* ptr = malloc(size);
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }
    TaroAllocation::s_AllocationMetrics.TotalAllocated += size;
    return ptr;
}
// 内存销毁
void operator delete(void* memory, size_t size) noexcept {
    TaroAllocation::s_AllocationMetrics.TotalFreed += size;
    free(memory);
}
void operator delete[](void* memory, size_t size) noexcept {
    TaroAllocation::s_AllocationMetrics.TotalFreed += size;
    free(memory);
}
#endif
