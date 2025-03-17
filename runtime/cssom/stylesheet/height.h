//
// Created on 2024/5/21.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_HEIGHT_H
#define TARO_CAPI_HARMONY_DEMO_HEIGHT_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "./attribute_base.h"
#include "helper/Optional.h"
#include "runtime/cssom/stylesheet/common.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_HEIGHT
class Height : public AttributeBase<Dimension> {
    public:
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_HEIGHT_H
