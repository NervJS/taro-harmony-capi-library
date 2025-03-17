//
// Created on 2024/5/21.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_FONT_SIZE_H
#define TARO_CAPI_HARMONY_DEMO_FONT_SIZE_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>

#include "./attribute_base.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_FONT_SIZE
class FontSize : public AttributeBase<Dimension> {
    public:
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_FONT_SIZE_H
