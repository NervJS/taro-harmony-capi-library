/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_WHITE_SPACE_H
#define HARMONY_LIBRARY_WHITE_SPACE_H

#include <cstdint>
#include <arkui/native_node.h>
#include <arkui/native_type.h>

#include "./attribute_base.h"
#include "runtime/cssom/stylesheet/css_property.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_Z_INDEX
class WhiteSpace : public AttributeBase<PropertyType::WhiteSpace> {
    public:
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // HARMONY_LIBRARY_WHITE_SPACE_H
