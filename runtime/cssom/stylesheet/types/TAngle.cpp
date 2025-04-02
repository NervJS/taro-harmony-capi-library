/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./TAngle.h"

#include <cmath>
#include <string>
#include <string_view>
#include <sys/types.h>

#include "helper/Debug.h"
#include "runtime/cssom/stylesheet/css_property.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
const float PI = M_PI;
const float PI2 = 2 * PI;

TAngle TAngle::MakeFromString(std::string_view str) {
    str = TaroHelper::string::trim(str);
#if IS_DEBUG
    if (!TaroHelper::string::containsDigit(str)) {
        return {0, PropertyType::AngleUnit::UNKNOWN};
    }
#endif
    try {
        size_t pos;
        float value = std::stod(str.data(), &pos);
        std::string_view unit = str.substr(pos);
        if (unit == "deg")
            return {value, PropertyType::AngleUnit::DEGREES};
        if (unit == "grad")
            return {value, PropertyType::AngleUnit::GRADIANS};
        if (unit == "rad")
            return {value, PropertyType::AngleUnit::RADIANS};
        if (unit == "turn")
            return {value, PropertyType::AngleUnit::TURNS};

    } catch (...) {
        // stod 解析失败
        return {0, PropertyType::AngleUnit::UNKNOWN};
    }

    return {0, PropertyType::AngleUnit::UNKNOWN};
}

float TAngle::getDegValue() const {
    return getTurnValue() * 360;
}

float TAngle::getGradValue() {
    return getTurnValue() * 400;
}

float TAngle::getRadValue() {
    return getTurnValue() * PI2;
}

float TAngle::getTurnValue() const {
    switch (unit) {
        case PropertyType::AngleUnit::TURNS:
            return std::fmod(value, 1);
        case PropertyType::AngleUnit::RADIANS:
            return std::fmod(value, PI2) / PI2;
        case PropertyType::AngleUnit::GRADIANS:
            return std::fmod(value, 400) / 400;
        case PropertyType::AngleUnit::DEGREES:
            return std::fmod(value, 360) / 360;
        default:
            return std::nanf("");
    }
}

void TAngle::setDegValue(const float val) {
    value = val;
    unit = PropertyType::AngleUnit::DEGREES;
}

void TAngle::setGradValue(const float val) {
    value = val;
    unit = PropertyType::AngleUnit::GRADIANS;
}

void TAngle::setRadValue(const float val) {
    value = val;
    unit = PropertyType::AngleUnit::RADIANS;
}

void TAngle::setTurnValue(const float val) {
    value = val;
    unit = PropertyType::AngleUnit::TURNS;
}

bool TAngle::operator==(const TAngle& other) const {
    return std::fabs(this->getTurnValue() - other.getTurnValue()) < std::numeric_limits<float>::epsilon();
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
