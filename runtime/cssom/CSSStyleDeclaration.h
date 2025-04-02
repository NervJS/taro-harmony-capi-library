/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_CSSSTYLEDECLATION_H
#define TARO_CAPI_HARMONY_DEMO_CSSSTYLEDECLATION_H

#include "runtime/cssom/stylesheet/IAttribute.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    class CSSStyleDeclaration {
        public:
        std::shared_ptr<TaroStylesheet::Stylesheet> styles_ptr_; // 样式申明，键-值对

        CSSStyleDeclaration();

        virtual ~CSSStyleDeclaration() = default;

        void Init(napi_value napiValue);
    };
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_CSSSTYLEDECLATION_H
