//
// Created on 2024/6/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <string>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "helper/Optional.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
enum BgImageType {
    PIC,
    LINEARGRADIENT,
    RADIALGRADIENT,
};

struct BackgroundImageItem {
    BgImageType type;

    std::variant<std::string> src;
    static const BackgroundImageItem emptyImg;

    // 渐变共同参数
    std::vector<uint32_t> colors;
    std::vector<float> stops;
    int size;

    // 径向参数
    float centerX;
    float centerY;

    // 线性参数
    TaroHelper::Optional<ArkUI_LinearGradientDirection> direction;
    float angle;
};
inline bool operator==(const BackgroundImageItem &lhs, const BackgroundImageItem &rhs) {
    if (lhs.type != rhs.type) return false;
    switch (lhs.type) {
        case PIC: {
            auto srclSrc = std::get_if<std::string>(&lhs.src);
            auto srcrSrc = std::get_if<std::string>(&rhs.src);
            if (srclSrc && srcrSrc) {
                return *srclSrc == *srcrSrc;
            }

            return false;
        }
        case LINEARGRADIENT:
            return lhs.colors == rhs.colors && lhs.stops == rhs.stops && lhs.size == rhs.size && lhs.direction == rhs.direction && lhs.angle == rhs.angle;
        case RADIALGRADIENT:
            return lhs.colors == rhs.colors && lhs.stops == rhs.stops && lhs.size == rhs.size;
    }
}

inline bool operator!=(const BackgroundImageItem &lhs, const BackgroundImageItem &rhs) {
    return !(lhs == rhs);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet