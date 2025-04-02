/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_FLEXDIRECTION_H
#define TARO_CAPI_HARMONY_DEMO_FLEXDIRECTION_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "attribute_base.h"
#include "helper/Optional.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_DIRECTION
class FlexDirection : public AttributeBase<ArkUI_FlexDirection> {
    public:
    using AttributeBase<ArkUI_FlexDirection>::operator=;
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_FLEXDIRECTION_H
