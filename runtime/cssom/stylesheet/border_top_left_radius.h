/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_BORDER_TOP_LEFT_RADIUS_H
#define TARO_CAPI_HARMONY_DEMO_BORDER_TOP_LEFT_RADIUS_H

#include "attribute_base.h"
#include "helper/string.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

class BorderTopLeftRadius : public AttributeBase<Dimension> {
    public:
    void setValueFromStringView(std::string_view value) override {
        auto values = TaroHelper::string::splitBySpace(value);
        if (!values.empty()) {
            AttributeBase<Dimension>::setValueFromStringView(values[0]);
        }
    }
};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // TARO_CAPI_HARMONY_DEMO_BORDER_TOP_LEFT_RADIUS_H
