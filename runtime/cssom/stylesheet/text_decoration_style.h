//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once
#include "runtime/cssom/stylesheet/attribute_base.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
class TextDecorationStyle : public AttributeBase<OH_Drawing_TextDecorationStyle> {};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet