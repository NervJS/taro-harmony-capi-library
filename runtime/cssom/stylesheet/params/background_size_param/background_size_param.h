//
// Created on 2024/6/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "helper/Optional.h"
#include "runtime/cssom/dimension/dimension.h"
#include "runtime/cssom/stylesheet/common.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
enum BackgroundSizeType {
    SIZEOPTIONS,
    IMAGESIZE
};

struct BackgroundSizeParam {
    BackgroundSizeType type;
    TaroHelper::Optional<ArkUI_ImageSize> imageSize;
    TaroHelper::Optional<Dimension> width;
    TaroHelper::Optional<Dimension> height;
};

inline bool operator==(const BackgroundSizeParam &lhs, const BackgroundSizeParam &rhs) {
    if (lhs.type != rhs.type) return false;
    switch (lhs.type) {
        case IMAGESIZE:
            return lhs.imageSize == rhs.imageSize;
        case SIZEOPTIONS:
            return lhs.width == rhs.width && lhs.height == rhs.height;
    }
}
inline bool operator!=(const BackgroundSizeParam &lhs, const BackgroundSizeParam &rhs) {
    return !(lhs == rhs);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet