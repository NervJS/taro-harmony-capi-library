//
// Created on 2024/6/22.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_COLOR_H
#define HARMONY_LIBRARY_COLOR_H

#include <cstdint>
#include <string_view>
#include <napi/native_api.h>

#include "helper/Optional.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

class TColor {
    public:
    TColor() = default;
    TColor(uint32_t value)
        : value_(value) {}
    TColor(std::string_view str) {
        setValue(str);
    }
    TColor(napi_value value) {
        setValue(value);
    }

    bool operator==(const TColor& other) const {
        return value_ == other.value_;
    }

    // 通过字符串创建实例
    static TColor MakeFromString(std::string_view str) {
        TColor color;
        color.setValue(str);
        return color;
    }

    // 通过 napi_value 创建实例
    static TColor MakeFromNapi(napi_value value) {
        TColor color;
        color.setValue(value);
        return color;
    }

    void setValue(uint32_t value) {
        value_ = value;
        has_value_ = true;
    }
    void setValue(std::string_view str);
    void setValue(napi_value value);

    TaroHelper::Optional<uint32_t> getValue();

    private:
    uint32_t value_ = 0x00000000;
    bool has_value_ = false;
};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // HARMONY_LIBRARY_COLOR_H
