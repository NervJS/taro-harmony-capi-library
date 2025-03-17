//
// Created on 2024/6/24.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_TRANGE_H
#define HARMONY_LIBRARY_TRANGE_H

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include "helper/scalar.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

// 用来表示空值

struct TRange {
    size_t start = EMPTY_INDEX;
    size_t end = EMPTY_INDEX;

    TRange()
        : start(), end() {}
    TRange(size_t s, size_t e)
        : start(s), end(e) {}

    bool operator==(const TRange& other) const {
        return start == other.start && end == other.end;
    }

    size_t width() const {
        return end - start;
    }
    
    bool empty() {
        return start == EMPTY_INDEX || end == EMPTY_INDEX;
    }

    void reset() {
        start = 0;
        end = 0;
    }

    void shift(std::make_signed_t<size_t> delta) {
        start += delta;
        end += delta;
    }

    // 是否包含 other
    bool contains(TRange other) const {
        return start <= other.start && end >= other.end;
    }

    bool contains(size_t index) const {
        return start <= index && end >= index;
    }

    // 是否相交
    bool intersects(TRange other) const {
        return std::max(start, other.start) <= std::min(end, other.end);
    }

    // 返回交集
    TRange intersection(TRange other) const {
        return TRange(std::max(start, other.start), std::min(end, other.end));
    }
};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // HARMONY_LIBRARY_TRANGE_H
