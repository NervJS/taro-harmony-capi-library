/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TESTNDK_MAX_WIDTH_H
#define TESTNDK_MAX_WIDTH_H

#include "attribute_base.h"
#include "common.h"
#include "napi/native_api.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_BORDER_WIDTH
class MaxWidth : public AttributeBase<Dimension> {};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TESTNDK_MAX_WIDTH_H
