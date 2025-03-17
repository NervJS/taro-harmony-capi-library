//
// Created on 2024/8/7.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "slider.h"

#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroSliderNode::TaroSliderNode(const TaroElementRef element)
        : TaroRenderNode(element) {}
    TaroSliderNode::~TaroSliderNode() {}

    void TaroSliderNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();

        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_SLIDER));
    }

    void TaroSliderNode::SetStep(int32_t step) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[1] = {0};
        value[0].f32 = static_cast<float>(step);
        ArkUI_AttributeItem item = {value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SLIDER_STEP, &item);
    }

    void TaroSliderNode::SetValue(int32_t value) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue sliderValue = {.f32 = static_cast<float>(value)};
        ArkUI_AttributeItem item = {&sliderValue, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SLIDER_VALUE, &item);
    }

    void TaroSliderNode::SetMinValue(int32_t minValue) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue min[1] = {0};
        min[0].f32 = static_cast<float>(minValue);
        ArkUI_AttributeItem item = {min, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SLIDER_MIN_VALUE, &item);
    }

    void TaroSliderNode::SetMaxValue(int32_t maxValue) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue max[1] = {0};
        max[0].f32 = static_cast<float>(maxValue);
        ArkUI_AttributeItem item = {max, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SLIDER_MAX_VALUE, &item);
        return;
    }

    void TaroSliderNode::SetActiveColor(uint32_t color) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[] = {0};
        ArkUI_AttributeItem item = {value, 1};
        value[0].u32 = color;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SLIDER_SELECTED_COLOR, &item);
    }
    void TaroSliderNode::SetBackgroundColor(uint32_t color) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.u32 = color};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SLIDER_TRACK_COLOR, &item);
    }
    void TaroSliderNode::SetBlockColor(uint32_t color) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value = {.u32 = color};
        ArkUI_AttributeItem item = {&value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SLIDER_BLOCK_COLOR, &item);
    }
    void TaroSliderNode::SetBlockSize(int32_t size) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[4] = {{.i32 = ARKUI_SLIDER_BLOCK_STYLE_SHAPE},
                                      {.i32 = ARKUI_SHAPE_TYPE_CIRCLE},
                                      {.f32 = static_cast<float>(size)},
                                      {.f32 = static_cast<float>(size)}};
        ArkUI_AttributeItem item = {value, 4};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_SLIDER_BLOCK_STYLE, &item);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
