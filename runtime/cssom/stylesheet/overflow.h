//
// Created on 2024/05/23.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_OVERFLOW_H
#define TARO_CAPI_HARMONY_DEMO_OVERFLOW_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "./css_property.h"
#include "attribute_base.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
class Overflow : public AttributeBase<PropertyType::Overflow> {
    public:
    Overflow() = default;

    Overflow(const napi_value &);

    void setValueFromNapi(const napi_value &);
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_OVERFLOW_H
