/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
