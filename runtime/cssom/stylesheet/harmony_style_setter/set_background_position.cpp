//
// Created on 2024/6/26.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "harmony_style_setter.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
void HarmonyStyleSetter::setBackgroundPosition(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) {
    auto x = stylesheet->backgroundPositionX;
    auto y = stylesheet->backgroundPositionY;
    setBackgroundPosition(node, x, y);
}
void HarmonyStyleSetter::setBackgroundPosition(const ArkUI_NodeHandle& node, const double& x, const double y) {
    //     setFloat(node, NODE_BACKGROUND_IMAGE_POSITION, x, y);
}
void HarmonyStyleSetter::setBackgroundPosition(const ArkUI_NodeHandle& node, const Optional<Dimension>& x, const Optional<Dimension>& y) {
    //     setFloat(node, NODE_BACKGROUND_IMAGE_POSITION, x, y);
}
void HarmonyStyleSetter::setBackgroundPosition(const ArkUI_NodeHandle& node, const Optional<Dimension>& x, const Optional<Dimension>& y, float widthBase, float heightBase, const Optional<TaroCSSOM::TaroStylesheet::BackgroundSizeParam>& backgroundSize) {
    float newX = 0;
    float newY = 0;

    float imgWidth = 0.0f;
    float imgHeight = 0.0f;

    if (
        backgroundSize.has_value() &&
        backgroundSize.value().type == SIZEOPTIONS &&
        backgroundSize.value().width.has_value() &&
        backgroundSize.value().height.has_value()) {
        if (auto val = backgroundSize.value().width.value().ParseToVp(widthBase); val.has_value()) {
            imgWidth = val.value();
        }
        if (auto val = backgroundSize.value().height.value().ParseToVp(heightBase); val.has_value()) {
            imgHeight = val.value();
        }

        if (x.has_value()) {
            if (auto val = x.value().ParseToVp(widthBase - imgWidth); val.has_value()) {
                newX = val.value();
            }
        }

        if (y.has_value()) {
            if (auto val = y.value().ParseToVp(heightBase - imgHeight); val.has_value()) {
                newY = val.value();
            }
        }

        setFloat(node, NODE_BACKGROUND_IMAGE_POSITION, vp2Px(newX), vp2Px(newY));
    }
}
void HarmonyStyleSetter::setBackgroundPosition(const ArkUI_NodeHandle& node) {
    NativeNodeApi::getInstance()->resetAttribute(node, NODE_BACKGROUND_IMAGE_POSITION);
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet