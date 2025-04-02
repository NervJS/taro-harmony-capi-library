/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_FONT_WEIGHT_H
#define TARO_CAPI_HARMONY_DEMO_FONT_WEIGHT_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "helper/Optional.h"
#include "runtime/cssom/stylesheet/attribute_base.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_FONT_WEIGHT
class FontWeight : public AttributeBase<ArkUI_FontWeight> {
    public:
    FontWeight() = default;

    FontWeight(const napi_value &);

    void setValueFromNapi(const napi_value &);
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_FONT_WEIGHT_H
