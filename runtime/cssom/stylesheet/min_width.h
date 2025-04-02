/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TESTNDK_MIN_WIDTH_H
#define TESTNDK_MIN_WIDTH_H

#include "attribute_base.h"
#include "common.h"
#include "napi/native_api.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_BORDER_WIDTH
class MinWidth : public AttributeBase<Dimension> {};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // TESTNDK_MIN_WIDTH_H
