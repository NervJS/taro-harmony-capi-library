//
// Created on 2024/6/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
void HarmonyStyleSetter::setBackgroundImage(
    const ArkUI_NodeHandle& node,
    const std::shared_ptr<
        TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
        stylesheet) {
    auto backgroundImage = stylesheet->backgroundImage;
    auto backgroundRepeat = stylesheet->backgroundRepeat;
    setBackgroundImage(node, backgroundImage, backgroundRepeat);
}
void HarmonyStyleSetter::setBackgroundImage(
    const ArkUI_NodeHandle& node,
    const Optional<BackgroundImageItem>& val,
    const Optional<ArkUI_ImageRepeat>& repeatVal) {
    if (!val.has_value()) return;
    auto value = val.value();
    auto repeatVale = repeatVal.has_value() ? repeatVal.value() : ARKUI_IMAGE_REPEAT_NONE;
    if (value.type == PIC) {
        ArkUI_NumberValue arkUI_NumberValue[1] = {};
        ArkUI_AttributeItem item;
        // 普通字符串的url
        if (auto url = std::get_if<std::string>(&value.src)) {
            item = {.value = arkUI_NumberValue, .size = 1, .string = (*url).c_str()};
        }
        arkUI_NumberValue[0].i32 = repeatVale;
        TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, NODE_BACKGROUND_IMAGE, &item);
        NativeNodeApi::getInstance()->resetAttribute(node, NODE_LINEAR_GRADIENT);
        NativeNodeApi::getInstance()->resetAttribute(node, NODE_RADIAL_GRADIENT);
    } else {
        ArkUI_ColorStop colors = {.colors = value.colors.data(),
                                  .stops = value.stops.data(),
                                  .size = value.size};
        if (value.type == RADIALGRADIENT) {
            ArkUI_NumberValue arkUI_NumberValue[4] = {};
            ArkUI_AttributeItem item = {.value = arkUI_NumberValue,
                                        .size = 4,
                                        .object = &colors};
            // 径向渐变的中心点，即相对于当前组件左上角的坐标,X轴坐标
            arkUI_NumberValue[0].f32 = value.centerX;
            // 径向渐变的中心点，即相对于当前组件左上角的坐标,Y轴坐标
            arkUI_NumberValue[1].f32 = value.centerY;
            // 径向渐变的半径，默认值0。
            // 这个半径如果设置成0的话是直接没有渐变的，不过如果设置成非常大的话，会取容器的最大值，
            arkUI_NumberValue[2].f32 = 1000000000;
            // 为渐变的颜色重复着色，0表示不重复着色，1表示重复着色
            arkUI_NumberValue[3].i32 = repeatVale;
            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(
                node, NODE_RADIAL_GRADIENT, &item);
//             setBackgroundImage(node, BackgroundImageItem::emptyImg);
//             NativeNodeApi::getInstance()->resetAttribute(node, NODE_LINEAR_GRADIENT);
        } else if (value.type == LINEARGRADIENT) {
            ArkUI_NumberValue arkUI_NumberValue[3] = {};
            ArkUI_AttributeItem item = {.value = arkUI_NumberValue,
                                        .size = 3,
                                        .object = &colors};

            if (value.direction.has_value()) {
                // 线性渐变的方向，设置angle后不生效。数据类型ArkUI_LinearGradientDirection
                arkUI_NumberValue[1].i32 = value.direction.value();
            } else {
                // 线性渐变的起始角度。0点方向顺时针旋转为正向角度，默认值：180；
                arkUI_NumberValue[0].f32 = value.angle;
                arkUI_NumberValue[1].i32 = ARKUI_LINEAR_GRADIENT_DIRECTION_CUSTOM;
            }
            // 为渐变的颜色重复着色，0表示不重复着色，1表示重复着色
            arkUI_NumberValue[2].i32 = repeatVale;

            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, NODE_LINEAR_GRADIENT, &item);
        }
    }
}
void HarmonyStyleSetter::setBackgroundImage(const ArkUI_NodeHandle& node) {
    setBackgroundImage(node, BackgroundImageItem::emptyImg);
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_LINEAR_GRADIENT);
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_RADIAL_GRADIENT);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet