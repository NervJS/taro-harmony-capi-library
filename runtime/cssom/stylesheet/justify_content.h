//
// Created on 2024/5/21.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_JUSTIFY_CONTENT_H
#define TARO_CAPI_HARMONY_DEMO_JUSTIFY_CONTENT_H

#include <memory>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "attribute_base.h"
#include "helper/Optional.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
struct Stylesheet;

// NODE_COLUMN_JUSTIFY_CONTENT
class JustifyContent : public AttributeBase<ArkUI_FlexAlignment> {
    public:
    using AttributeBase<ArkUI_FlexAlignment>::operator=;
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_JUSTIFY_CONTENT_H
