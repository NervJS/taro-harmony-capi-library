/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
