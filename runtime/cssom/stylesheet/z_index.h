//
// Created on 2024/6/21.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_Z_INDEX_H
#define HARMONY_LIBRARY_Z_INDEX_H

#include <cstdint>
#include <arkui/native_node.h>
#include <arkui/native_type.h>

#include "./attribute_base.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_Z_INDEX
class ZIndex : public AttributeBase<int32_t> {
    public:
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // HARMONY_LIBRARY_Z_INDEX_H
