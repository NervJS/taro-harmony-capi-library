/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>
#include <sys/stat.h>

#include "./attribute_base.h"
#include "helper/Optional.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_VISIBILITY
class PointerEvents : public AttributeBase<PropertyType::PointerEvents> {
    public:
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
