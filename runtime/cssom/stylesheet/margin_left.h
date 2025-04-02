/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_MARGIN_LEFT_H
#define TARO_CAPI_HARMONY_DEMO_MARGIN_LEFT_H

#include "attribute_base.h"
#include "common.h"
#include "napi/native_api.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_MARGIN
class MarginLeft : public AttributeBase<Dimension> {};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // TARO_CAPI_HARMONY_DEMO_MARGIN_LEFT_H
