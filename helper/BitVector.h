/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_BITVECTOR_H
#define TARO_CAPI_HARMONY_DEMO_BITVECTOR_H

#include <cstddef>
#include <string>
#include <vector>

namespace TaroHelper {
class BitVector {
    public:
    BitVector(size_t size)
        : bits((size + 63) / 64), size_(size) {}

    void set(size_t index) {
        bits[index / 64] |= (1ULL << (index % 64));
    }

    void reset(size_t index) {
        bits[index / 64] &= ~(1ULL << (index % 64));
    }

    bool test(size_t index) const {
        return bits[index / 64] & (1ULL << (index % 64));
    }

    size_t size() const {
        return size_;
    }

    std::string to_string() const {
        std::string result(size_, '0');
        for (size_t i = 0; i < size_; ++i) {
            if (test(i)) {
                result[i] = '1';
            }
        }
        return result;
    }

    private:
    std::vector<uint64_t> bits;
    size_t size_;
};
} // namespace TaroHelper
#endif // TARO_CAPI_HARMONY_DEMO_BITVECTOR_H
