/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_VERTICALALIGN_H
#define TARO_CAPI_HARMONY_DEMO_VERTICALALIGN_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>
#include <native_drawing/drawing_text_typography.h>

#include "./attribute_base.h"
#include "helper/Optional.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

// NODE_ALIGNMENT
class VerticalAlign
    : public AttributeBase<OH_Drawing_PlaceholderVerticalAlignment> {};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_VERTICALALIGN_H
