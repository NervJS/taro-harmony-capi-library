/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_ALIGNITEMS_H
#define TARO_CAPI_HARMONY_DEMO_ALIGNITEMS_H

#include <memory>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "attribute_base.h"
#include "helper/Optional.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

struct Stylesheet;

// NODE_COLUMN_ALIGN_ITEMS
class AlignItems : public AttributeBase<ArkUI_ItemAlignment> {
    public:
    using AttributeBase<ArkUI_ItemAlignment>::operator=;
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_ALIGNITEMS_H
