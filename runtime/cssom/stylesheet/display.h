/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_DISPLAY_H
#define TARO_CAPI_HARMONY_DEMO_DISPLAY_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "./css_property.h"
#include "attribute_base.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

class Display : public AttributeBase<PropertyType::Display> {
    public:
    using AttributeBase<PropertyType::Display>::operator=;
};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // TARO_CAPI_HARMONY_DEMO_DISPLAY_H
