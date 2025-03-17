//
// Created on 2024/6/23.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#pragma once

#include "./attribute_base.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
class BackgroundPositionX : public AttributeBase<Dimension> {};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
