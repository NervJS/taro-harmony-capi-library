//
// Created on 2024/7/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "radio.h"
namespace TaroRuntime {
namespace TaroDOM {
    TaroRadioNode::TaroRadioNode(const TaroElementRef element)
        : TaroRenderNode(element) {}
    TaroRadioNode::~TaroRadioNode() {}
    void TaroRadioNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_RADIO));
        setZeroMargin();
    }
    void TaroRadioNode::setValue(const char *value) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.string = value};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_RADIO_VALUE, &item);
    }

    void TaroRadioNode::setZeroMargin() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue numberValue[1] = {};
        numberValue->f32 = 0;
        ArkUI_AttributeItem item = {numberValue, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_MARGIN, &item);
    }
    void TaroRadioNode::setNodeGroup(const char *groupName) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.string = groupName};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_RADIO_GROUP, &item);
    }
    void TaroRadioNode::setChecked(bool checked) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue numberValue[1] = {};
        int32_t isChecked = checked ? 1 : 0;
        numberValue[0].i32 = isChecked;
        ArkUI_AttributeItem item = {numberValue, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_RADIO_CHECKED, &item);
    }

    void TaroRadioNode::ChangeChecked() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        const ArkUI_AttributeItem *item = nativeNodeApi->getAttribute(GetArkUINodeHandle(), NODE_RADIO_CHECKED);
        if (!item->value[0].i32) {
            setChecked(!item->value[0].i32);
        }
    }

    //  只需要设置底板颜色，其他是默认颜色
    void TaroRadioNode::setColor(uint32_t color) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue numberValue[3] = {};
        numberValue[0].u32 = color;
        numberValue[1].u32 = 0xFF182431; // 默认颜色
        numberValue[2].u32 = 0xFFFFFFFF; // 默认颜色
        ArkUI_AttributeItem item = {numberValue, 3};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_RADIO_STYLE, &item);
    }
} // namespace TaroDOM
} // namespace TaroRuntime