/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "transform_origin.h"

#include "./utils.h"
#include "helper/string.h"
#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/dimension/dimension.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void TransformOrigin::setValueFromStringView(std::string_view str) {
    auto values = TaroHelper::string::splitBySpace(str);
    if (values.empty())
        return;
    if (values.size() >= 3) {
        // z轴只支持度量值
        auto z = Dimension::FromString(values[2]);
        auto vp = z.ParseToVp();
        if (vp.has_value()) {
            data_.z = vp.value();
        } else if (z.Unit() == DimensionUnit::PERCENT) {
            data_.pz = z;
        } else {
            return;
        }
    }

    // 保证至少有两个值
    if (values.size() == 1) {
        values.push_back("50%");
    }

    auto sx = values[0];
    auto sy = values[1];

    if (values[0] == "top" || values[0] == "bottom" || values[1] == "left" || values[1] == "right") {
        std::swap(sx, sy);
    }

    if (sx == "top" || sx == "left")
        sx = "0%";
    else if (sx == "bottom" || sx == "right")
        sx = "100%";
    else if (sx == "center")
        sx = "50%";

    if (sy == "top" || sy == "left") {
        sy = "0%";
    } else if (sy == "bottom" || sy == "right") {
        sy = "100%";
    } else if (sy == "center")
        sy = "50%";

    {
        auto lx = Dimension::FromString(sx);
        auto vp = lx.ParseToVp();
        if (vp.has_value()) {
            data_.x = vp.value();
        } else if (lx.Unit() == DimensionUnit::PERCENT) {
            data_.px = lx;
        }
    }
    {
        auto ly = Dimension::FromString(sy);
        auto vp = ly.ParseToVp();
        if (vp.has_value()) {
            data_.y = vp.value();
        } else if (ly.Unit() == DimensionUnit::PERCENT) {
            data_.py = ly;
        }
    }

    this->set(data_);
}

void TransformOrigin::setValueFromNapi(const napi_value& napiValue) {
    NapiGetter getter(napiValue);
    auto type = getter.GetType();
    if (type == napi_object) {
        {
            auto property = getter.GetProperty("x");
            napi_valuetype type = property.GetType();
            if (type == napi_number) {
                auto floatValue = property.Double();
                if (floatValue.has_value()) {
                    data_.x = Dimension{floatValue.value(), DimensionUnit::DESIGN_PX};
                    this->set(data_);
                }
            } else if (type == napi_string) {
                auto stringValue = property.String();
                if (stringValue.has_value()) {
                    auto px = Dimension::FromString(stringValue.value());
                    if (px.Unit() == DimensionUnit::PERCENT) {
                        data_.px = px;
                        this->set(data_);
                    }
                }
            }
        }
        {
            auto property = getter.GetProperty("y");
            napi_valuetype type = property.GetType();
            if (type == napi_number) {
                auto floatValue = property.Double();
                if (floatValue.has_value()) {
                    data_.y = Dimension{floatValue.value(), DimensionUnit::DESIGN_PX};
                    this->set(data_);
                }
            } else if (type == napi_string) {
                auto stringValue = property.String();
                if (stringValue.has_value()) {
                    auto py = Dimension::FromString(stringValue.value());
                    if (py.Unit() == DimensionUnit::PERCENT) {
                        data_.py = py;
                        this->set(data_);
                    }
                }
            }
        }
        {
            auto property = getter.GetProperty("z");
            napi_valuetype type = property.GetType();
            if (type == napi_number) {
                auto floatValue = property.Double();
                if (floatValue.has_value()) {
                    data_.z = Dimension{floatValue.value(), DimensionUnit::DESIGN_PX};
                    this->set(data_);
                }
            } else if (type == napi_string) {
                auto stringValue = property.String();
                if (stringValue.has_value()) {
                    auto pz = Dimension::FromString(stringValue.value());
                    if (pz.Unit() == DimensionUnit::PERCENT) {
                        data_.pz = pz;
                        this->set(data_);
                    }
                }
            }
        }
    }
}

void TransformOrigin::assign(const AttributeBase<TransformOriginData>& item) {
    if (item.has_value())
        this->set(item);
}

bool TransformOrigin::isEqual(const Optional<TransformOriginData>& other) const {
    bool hasValue = this->has_value();
    if (hasValue == other.has_value()) {
        if (hasValue) {
            return this->value().x == other.value().x &&
                   this->value().y == other.value().y &&
                   this->value().z == other.value().z &&
                   this->value().px == other.value().px &&
                   this->value().py == other.value().py &&
                   this->value().pz == other.value().pz;
        }
        return true;
    }
    return false;
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
