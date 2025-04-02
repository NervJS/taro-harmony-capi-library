/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_STYLEVALIDITY_H
#define TARO_CAPI_HARMONY_DEMO_STYLEVALIDITY_H

#include <cstdint>

namespace TaroRuntime {
namespace TaroCSSOM {
    enum class Validity : std::uint8_t {
        Valid,
        // AnimationInvalid,
        ElementInvalid,
        SubtreeInvalid,
    };
}
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_STYLEVALIDITY_H
