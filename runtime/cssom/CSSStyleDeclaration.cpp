//
// Created on 2024/5/7.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "CSSStyleDeclaration.h"

#include <iostream>

#include "CSSStyleSheet.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    CSSStyleDeclaration::CSSStyleDeclaration()
        : styles_ptr_(std::make_shared<TaroStylesheet::Stylesheet>()) {}

    void CSSStyleDeclaration::Init(napi_value napiValue) {
        if (napiValue) {
            CSSStyleSheet::setStylesheet(this->styles_ptr_, napiValue);
        }
    }
} // namespace TaroCSSOM
} // namespace TaroRuntime
