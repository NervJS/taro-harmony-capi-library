//
// Created on 2024/5/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

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
