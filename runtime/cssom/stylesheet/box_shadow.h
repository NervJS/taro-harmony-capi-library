/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_BOXSHADOW_H
#define TARO_CAPI_HARMONY_DEMO_BOXSHADOW_H

#include <string_view>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "attribute_base.h"
#include "helper/Optional.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

struct BoxShadowItem {
    TaroHelper::Optional<Dimension> radius;
    TaroHelper::Optional<Dimension> offsetX;
    TaroHelper::Optional<Dimension> offsetY;
    TaroHelper::Optional<uint32_t> color;
    TaroHelper::Optional<int32_t> fill;

    //     radius: number
    //     color?: string
    //     offsetX?: number
    //     offsetY?: number
    //     fill?: boolean
    //      * .value[0].f32: blur radius of the shadow, in vp.\n
    //      * .value[1].i32: whether to enable the coloring strategy. \n
    //      * .value[2].f32: offset of the shadow along the x-axis, in vp.\n
    //      * .value[3].f32: offset of the shadow along the y-axis, in vp.\n
    //      * .value[4].i32: shadow type {@link ArkUI_ShadowType}. The default
    //      value is <b>ARKUI_SHADOW_TYPE_COLOR</b>.\n
    //      * .value[5].u32: shadow color, in 0xARGB format. For example,
    //      0xFFFF0000 indicates red.\n
    //      * .value[6].u32: whether to fill the shadow. The value <b>1</b> means
    //      to fill the shadow, and <b>0</b>
};

inline bool operator==(const BoxShadowItem& lhs, const BoxShadowItem& rhs) {
    return lhs.radius == rhs.radius &&
           lhs.color == rhs.color &&
           lhs.fill == rhs.fill &&
           lhs.offsetX == rhs.offsetX &&
           lhs.offsetY == rhs.offsetY;
}
inline bool operator!=(const BoxShadowItem& lhs, const BoxShadowItem& rhs) {
    return !(lhs == rhs);
}

class BoxShadow : public AttributeBase<BoxShadowItem> {
    public:
    BoxShadow() = default;
    BoxShadow(const napi_value& value) {
        setValueFromNapi(value);
    }

    void setValueFromStringView(std::string_view str) override;

    void setValueFromNapi(const napi_value&) override;

    private:
    BoxShadowItem boxShadowItem;
};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // TARO_CAPI_HARMONY_DEMO_BOXSHADOW_H
