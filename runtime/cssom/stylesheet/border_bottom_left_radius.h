//
// Created on 2024/6/19.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_BORDER_BOTTOM_LEFT_RADIUS_H
#define TARO_CAPI_HARMONY_DEMO_BORDER_BOTTOM_LEFT_RADIUS_H

#include "attribute_base.h"
#include "helper/string.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

class BorderBottomLeftRadius : public AttributeBase<Dimension> {
    public:
    void setValueFromStringView(std::string_view value) override {
        auto values = TaroHelper::string::splitBySpace(value);
        if (!values.empty()) {
            AttributeBase<Dimension>::setValueFromStringView(values[0]);
        }
    }
};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // TARO_CAPI_HARMONY_DEMO_BORDER_BOTTOM_LEFT_RADIUS_H
