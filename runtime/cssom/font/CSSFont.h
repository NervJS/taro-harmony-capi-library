//
// Created on 2024/8/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_HARMONY_CPP_CSSFONT_H
#define TARO_HARMONY_CPP_CSSFONT_H

#include <js_native_api.h>

namespace TaroRuntime {
namespace TaroCSSOM {
    class CSSFont {
        public:
        int init(const napi_value &config);
    };
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_CPP_CSSFONT_H
