//
// Created on 2024/5/7.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

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
