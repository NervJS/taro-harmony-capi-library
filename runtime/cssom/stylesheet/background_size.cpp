/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./background_size.h"

#include "./utils.h"
#include "helper/string.h"
#include "runtime/NapiGetter.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

BackgroundSize::BackgroundSize(const napi_value& napiValue) {
    setValueFromNapi(napiValue);
}

void BackgroundSize::setValueFromStringView(std::string_view str) {
    auto values = TaroHelper::string::splitBySpace(str);
    if (values.empty() || values.size() > 2)
        return;

    bool isContain = values.size() == 1 && values[0] == "contain";
    bool isCover = values.size() == 1 && values[0] == "cover";

    if (isContain || isCover) {
        param_.type = IMAGESIZE;
        param_.imageSize.set(isContain ? ArkUI_ImageSize::ARKUI_IMAGE_SIZE_CONTAIN : ArkUI_ImageSize::ARKUI_IMAGE_SIZE_COVER);
        this->set(param_);
        return;
    }

    for (size_t i = 0; i < values.size(); ++i) {
        auto& iter = values[i];
        auto& target = i == 0 ? param_.width : param_.height;
        target.set(Dimension::FromString(iter));
    }

    param_.type = SIZEOPTIONS;
    this->set(param_);
}

void BackgroundSize::setValueFromNapi(const napi_value& napiValue) {
    napi_valuetype type;
    NapiGetter getter(napiValue);
    getter.GetType(type);

    if (type == napi_number) {
        auto getterValue = getter.Int32();
        if (getterValue.has_value()) {
            param_.type = IMAGESIZE;
            param_.imageSize.set(static_cast<ArkUI_ImageSize>(getterValue.value()));
            this->set(param_);
        }
    } else if (type == napi_object) {
        auto widthGetter = getter.GetProperty("width");
        widthGetter.GetType(type);
        if (type == napi_number) {
            auto getterValue = widthGetter.Double();
            if (getterValue.has_value()) {
                param_.width.set(Dimension{getterValue.value(), DimensionUnit::DESIGN_PX});
            }
        } else if (type == napi_string) {
            auto getterValue = widthGetter.String();
            if (getterValue.has_value()) {
                auto w = Dimension::FromString(getterValue.value());
                if (static_cast<int>(w.Unit()) >= 0) {
                    param_.width.set(w);
                } else {
                    param_.width.reset();
                }
            }
        }

        auto heightGetter = getter.GetProperty("height");
        heightGetter.GetType(type);
        if (type == napi_number) {
            auto getterValue = heightGetter.Double();
            if (getterValue.has_value()) {
                param_.height.set(Dimension{getterValue.value(), DimensionUnit::DESIGN_PX});
            }
        } else if (type == napi_string) {
            auto getterValue = heightGetter.String();
            if (getterValue.has_value()) {
                auto h = Dimension::FromString(getterValue.value());
                if (static_cast<int>(h.Unit()) >= 0) {
                    param_.height.set(h);
                } else {
                    param_.height.reset();
                }
            }
        }

        param_.type = SIZEOPTIONS;
        this->set(param_);
    }
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
