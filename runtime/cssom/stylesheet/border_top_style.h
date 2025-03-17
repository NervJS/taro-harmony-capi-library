//
// Created on 2024/6/19.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_BORDER_TOP_STYLE_H
#define TARO_CAPI_HARMONY_DEMO_BORDER_TOP_STYLE_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>
#include <sys/stat.h>

#include "./attribute_base.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_BORDER_STYLE
class BorderTopStyle : public AttributeBase<ArkUI_BorderStyle> {
    public:
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // TARO_CAPI_HARMONY_DEMO_BORDER_TOP_STYLE_H
