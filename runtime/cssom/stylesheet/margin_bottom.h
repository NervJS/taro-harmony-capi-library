/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_MARGIN_BOTTOM_H
#define TARO_CAPI_HARMONY_DEMO_MARGIN_BOTTOM_H

#include "attribute_base.h"
#include "common.h"
#include "napi/native_api.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_MARGIN
class MarginBottom : public AttributeBase<Dimension> {};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // TARO_CAPI_HARMONY_DEMO_MARGIN_BOTTOM_H
