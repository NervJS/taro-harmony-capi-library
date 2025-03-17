//
// Created on 2024/6/24.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#pragma once
#include "../css_property.h"
#include "helper/string.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
class TAngle {
    public:
    TAngle() = default;
    TAngle(const float value, PropertyType::AngleUnit unit)
        : value(value), unit(unit) {}

    float value = 0.0f;
    PropertyType::AngleUnit unit;

    float getDegValue() const;

    float getGradValue();

    float getRadValue();

    float getTurnValue() const;

    void setDegValue(const float val);

    void setGradValue(const float val);

    void setRadValue(const float val);

    void setTurnValue(const float val);

    static TAngle MakeFromString(std::string_view str);
    
    bool operator == (const TAngle& other) const;
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
