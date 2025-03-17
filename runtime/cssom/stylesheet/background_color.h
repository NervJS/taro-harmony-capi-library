//
// Created on 2024/5/21.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_BACKGROUND_COLOR_H
#define TARO_CAPI_HARMONY_DEMO_BACKGROUND_COLOR_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "helper/Optional.h"
#include "runtime/cssom/stylesheet/attribute_base.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_BORDER_COLOR
class BackgroundColor : public AttributeBase<TColor> {};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_BACKGROUND_COLOR_H
