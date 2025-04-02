/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
