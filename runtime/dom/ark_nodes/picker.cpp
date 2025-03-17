//
// Created on 2024/6/27.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "picker.h"
#include <arkui/native_dialog.h>
#include <cstdint>

#include "runtime/NapiDialogNodeApi.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/dimension/context.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroPickerNode::TaroPickerNode(const TaroElementRef element)
        : TaroRenderNode(element) {}

    TaroPickerNode::~TaroPickerNode() {}

    void TaroPickerNode::Build() {
        SetArkUINodeHandle(NativeNodeApi::getInstance()->createNode(ARKUI_NODE_CUSTOM));
    }

    void TaroPickerNode::Prepare() {
        // 各个子类按需创建自己的样式
        CreatePickerContent();
        CreateDialogContent();
        isPickerCreated = true;
    }

    void TaroPickerNode::Show() {
        ArkUI_NativeDialogAPI_1* api = NapiDialogNodeApi::GetInstance()->GetAPI();
        dialogHandle = api->create();
        api->setModalMode(dialogHandle, true);
        api->setContentAlignment(dialogHandle, static_cast<int32_t>(ARKUI_ALIGNMENT_BOTTOM), 0, 0);
        api->enableCustomStyle(dialogHandle, true);
        api->enableCustomAnimation(dialogHandle, true);
        api->setMask(dialogHandle, 0, nullptr);
        api->setBackgroundColor(dialogHandle, 0x00000000);
        api->setContent(dialogHandle, CreateDialogContent());
        api->show(dialogHandle, false);
    }

    ArkUI_NodeHandle TaroPickerNode::CreateDialogContent() {
        ArkUI_NodeHandle content = NativeNodeApi::getInstance()->createNode(ARKUI_NODE_COLUMN);
        ArkUI_NumberValue backgroundValue[] = {{.u32 = 0xFFF5F5F5}};
        ArkUI_AttributeItem backgroundItem = {backgroundValue, 1};
        NativeNodeApi::getInstance()->setAttribute(content, NODE_BACKGROUND_COLOR, &backgroundItem);

        ArkUI_NodeHandle btnRow = NativeNodeApi::getInstance()->createNode(ARKUI_NODE_ROW);
        ArkUI_NumberValue percentValue[] = {{.f32 = 1}};
        ArkUI_AttributeItem percentValueItem = {percentValue, 1};
        NativeNodeApi::getInstance()->setAttribute(btnRow, NODE_WIDTH_PERCENT, &percentValueItem);

        cancelHandle = NativeNodeApi::getInstance()->createNode(ARKUI_NODE_TEXT);
        ArkUI_AttributeItem cancelText = {.string = this->cancelText.c_str()};
        NativeNodeApi::getInstance()->setAttribute(cancelHandle, NODE_TEXT_CONTENT, &cancelText);

        confirmHandle = NativeNodeApi::getInstance()->createNode(ARKUI_NODE_TEXT);
        ArkUI_AttributeItem confirmText = {.string = this->okText.c_str()};
        NativeNodeApi::getInstance()->setAttribute(confirmHandle, NODE_TEXT_CONTENT, &confirmText);

        ArkUI_NumberValue fontColor[] = {{.u32 = 0xFF1aad19}};
        ArkUI_AttributeItem fontColorItem = {fontColor, 1};
        NativeNodeApi::getInstance()->setAttribute(confirmHandle, NODE_FONT_COLOR, &fontColorItem);

        percentValue[0].f32 = 0.5;
        NativeNodeApi::getInstance()->setAttribute(confirmHandle, NODE_WIDTH_PERCENT, &percentValueItem);
        NativeNodeApi::getInstance()->setAttribute(cancelHandle, NODE_WIDTH_PERCENT, &percentValueItem);

        ArkUI_NumberValue fontSize[] = {{.f32 = 16}};
        ArkUI_AttributeItem fontSizeItem = {fontSize, 1};
        NativeNodeApi::getInstance()->setAttribute(confirmHandle, NODE_FONT_SIZE, &fontSizeItem);
        NativeNodeApi::getInstance()->setAttribute(cancelHandle, NODE_FONT_SIZE, &fontSizeItem);

        ArkUI_NumberValue confirmAlign[] = {{.i32 = ARKUI_TEXT_ALIGNMENT_END}};
        ArkUI_AttributeItem confirmAlignItem = {confirmAlign, 1};
        NativeNodeApi::getInstance()->setAttribute(confirmHandle, NODE_TEXT_ALIGN, &confirmAlignItem);

        ArkUI_NumberValue padding[] = {{.f32 = 20}};
        ArkUI_AttributeItem paddingItem = {padding, 1};
        NativeNodeApi::getInstance()->setAttribute(confirmHandle, NODE_PADDING, &paddingItem);
        NativeNodeApi::getInstance()->setAttribute(cancelHandle, NODE_PADDING, &paddingItem);
        // add child
        NativeNodeApi::getInstance()->addChild(btnRow, cancelHandle);
        NativeNodeApi::getInstance()->addChild(btnRow, confirmHandle);
        NativeNodeApi::getInstance()->addChild(content, btnRow);
        NativeNodeApi::getInstance()->addChild(content, pickerHandle);
        return content;
    }

    void TaroPickerNode::SetOKText(std::string okText) {
        this->okText = okText;
    }

    void TaroPickerNode::SetCancelText(std::string cancelText) {
        this->cancelText = cancelText;
    }
    ////////////////////////// Selector ///////////////////////////
    TaroSelectorPickerNode::TaroSelectorPickerNode(const TaroElementRef element)
        : TaroPickerNode(element) {}

    ArkUI_NodeHandle TaroSelectorPickerNode::CreatePickerContent() {
        pickerHandle = NativeNodeApi::getInstance()->createNode(ARKUI_NODE_TEXT_PICKER);

        ArkUI_NumberValue pickerSetting[] = {{.f32 = DimensionContext::GetInstance()->device_width_ - 10}};
        ArkUI_AttributeItem settingItem = {.value = pickerSetting, 1};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_WIDTH, &settingItem);

        pickerSetting[0] = {.i32 = false};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_CAN_LOOP, &settingItem);

        ArkUI_AttributeItem selectedTextStyle = {.string = selectTextStyle.c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_SELECTED_TEXT_STYLE, &selectedTextStyle);
        return pickerHandle;
    }

    void TaroSelectorPickerNode::SetSelectorRange(Optional<std::string> values) {
        if(!values.has_value() || !isPickerCreated) {
            return;
        }
        ArkUI_NumberValue pickerSetting[] = {{.i32 = ARKUI_TEXTPICKER_RANGETYPE_SINGLE}};
        ArkUI_AttributeItem settingItem = {pickerSetting, 1, values.value().c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_OPTION_RANGE, &settingItem);
    }

    void TaroSelectorPickerNode::SetSelect(Optional<uint32_t> select) {
        if(!isPickerCreated || !select.has_value()) {
            return;
        }
        ArkUI_NumberValue pickerSetting[] = {{.u32 = select.value()}};
        ArkUI_AttributeItem settingItem = {
            pickerSetting, sizeof(pickerSetting) / sizeof(ArkUI_NumberValue)};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_OPTION_SELECTED, &settingItem);
    }

    void TaroSelectorPickerNode::onConfirm() {
        NapiDialogNodeApi::GetInstance()->GetAPI()->close(dialogHandle);
    }

    void TaroSelectorPickerNode::onCancel() {
        NapiDialogNodeApi::GetInstance()->GetAPI()->close(dialogHandle);
    }

    //////////////////////////// Multi Picker ///////////////////////
    TaroMultiSelectorPickerNode::TaroMultiSelectorPickerNode(const TaroElementRef element)
        : TaroPickerNode(element) {}

    ArkUI_NodeHandle TaroMultiSelectorPickerNode::CreatePickerContent() {
        pickerHandle = NativeNodeApi::getInstance()->createNode(ARKUI_NODE_TEXT_PICKER);
        ArkUI_NumberValue pickerSetting[] = {{.f32 = DimensionContext::GetInstance()->device_width_ - 10}};
        ArkUI_AttributeItem settingItem = {.value = pickerSetting, 1};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_WIDTH, &settingItem);

        pickerSetting[0] = {.i32 = false};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_CAN_LOOP, &settingItem);

        ArkUI_AttributeItem selectedTextStyle = {.string = selectTextStyle.c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_SELECTED_TEXT_STYLE, &selectedTextStyle);

        return pickerHandle;
    }

    void TaroMultiSelectorPickerNode::SetMultiSelectorRange(Optional<std::string> values) {
        if (!isPickerCreated || !values.has_value() || values.value().empty()) {
            return;
        }
        ArkUI_NumberValue pickType[] = {{.i32 = ARKUI_TEXTPICKER_RANGETYPE_MULTI}};
        ArkUI_AttributeItem item = {
            pickType,
            sizeof(pickType) / sizeof(ArkUI_NumberValue),
            values.value().c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_OPTION_RANGE, &item);
    }

    void TaroMultiSelectorPickerNode::SetMultiSelectorValues(std::vector<uint32_t> index) {
        if(!isPickerCreated || index.size() == 0) {
            return;
        }
        ArkUI_NumberValue indexArray[index.size()];
        for (int i = 0; i < index.size(); i++) {
            indexArray[i] = {.u32 = index.at(i)};
        }
        ArkUI_AttributeItem valueItem = {indexArray, static_cast<int32_t>(index.size())};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_OPTION_SELECTED, &valueItem);
    }

    void TaroMultiSelectorPickerNode::onConfirm() {
        NapiDialogNodeApi::GetInstance()->GetAPI()->close(dialogHandle);
    }

    void TaroMultiSelectorPickerNode::onCancel() {
        NapiDialogNodeApi::GetInstance()->GetAPI()->close(dialogHandle);
    }

    ////////////////////////// TIME PICKER ///////////////////////////////////
    TaroTimePickerNode::TaroTimePickerNode(const TaroElementRef element)
        : TaroPickerNode(element) {}

    ArkUI_NodeHandle TaroTimePickerNode::CreatePickerContent() {
        pickerHandle = NativeNodeApi::getInstance()->createNode(ARKUI_NODE_TIME_PICKER);
        ArkUI_NumberValue widthValue[] = {{.f32 = DimensionContext::GetInstance()->device_width_ - 10}};
        ArkUI_AttributeItem widthItem = {.value = widthValue, 1};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_WIDTH, &widthItem);

        ArkUI_AttributeItem selectedTextStyle = {.string = selectTextStyle.c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TIME_PICKER_SELECTED_TEXT_STYLE, &selectedTextStyle);

        ArkUI_NumberValue militaryTime[] = {{.i32 = true}};
        ArkUI_AttributeItem militaryTimeItem = {.value = militaryTime, 1};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TIME_PICKER_USE_MILITARY_TIME, &militaryTimeItem);

        return pickerHandle;
    }

    void TaroTimePickerNode::onConfirm() {
        NapiDialogNodeApi::GetInstance()->GetAPI()->close(dialogHandle);
    }

    void TaroTimePickerNode::onCancel() {
        NapiDialogNodeApi::GetInstance()->GetAPI()->close(dialogHandle);
    }

    void TaroTimePickerNode::SetTime(std::string time) {
        if(!isPickerCreated) {
            return;
        }
        size_t pos = time.find(":");
        if(pos == std::string::npos) {
            return;
        }
        ArkUI_AttributeItem item = {.string = time.replace(pos, 1, "-").c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TIME_PICKER_SELECTED, &item);
    }

    /////////////////////////// Date Picker //////////////////////////////
    TaroDatePickerNode::TaroDatePickerNode(const TaroElementRef element)
        : TaroPickerNode(element) {}

    ArkUI_NodeHandle TaroDatePickerNode::CreatePickerContent() {
        pickerHandle = NativeNodeApi::getInstance()->createNode(ARKUI_NODE_DATE_PICKER);

        ArkUI_AttributeItem selectedTextStyle = {.string = selectTextStyle.c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_DATE_PICKER_SELECTED_TEXT_STYLE, &selectedTextStyle);
        return pickerHandle;
    }

    void TaroDatePickerNode::SetDate(std::string date) {
        if(!isPickerCreated || date.empty()) {
            return;
        }
        ArkUI_AttributeItem item = {.string = date.c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_DATE_PICKER_SELECTED, &item);
    }

    void TaroDatePickerNode::SetStart(std::string start) {
        if(!isPickerCreated || start.empty()) {
            return;
        }
        ArkUI_AttributeItem item = {.string = start.c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_DATE_PICKER_START, &item);
    }

    void TaroDatePickerNode::SetEnd(std::string end) {
        if(!isPickerCreated || end.empty()) {
            return;
        }
        ArkUI_AttributeItem item = {.string = end.c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_DATE_PICKER_END, &item);
    }

    void TaroDatePickerNode::onConfirm() {
        NapiDialogNodeApi::GetInstance()->GetAPI()->close(dialogHandle);
    }

    void TaroDatePickerNode::onCancel() {
        NapiDialogNodeApi::GetInstance()->GetAPI()->close(dialogHandle);
    }

    /////////////////////////// Region Picker //////////////////////////////
    TaroRegionPickerNode::TaroRegionPickerNode(const TaroElementRef element)
        : TaroPickerNode(element) {}

    ArkUI_NodeHandle TaroRegionPickerNode::CreatePickerContent() {
        pickerHandle = NativeNodeApi::getInstance()->createNode(ARKUI_NODE_TEXT_PICKER);
        ArkUI_NumberValue pickerSetting[] = {{.f32 = DimensionContext::GetInstance()->device_width_ - 10}};
        ArkUI_AttributeItem settingItem = {.value = pickerSetting, 1};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_WIDTH, &settingItem);
        pickerSetting[0] = {.i32 = false};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_CAN_LOOP, &settingItem);
        ArkUI_AttributeItem selectedTextStyle = {.string = "#ff000000;18;normal;Arial;normal"};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_SELECTED_TEXT_STYLE, &selectedTextStyle);
        return pickerHandle;
    }

    void TaroRegionPickerNode::onConfirm() {
        NapiDialogNodeApi::GetInstance()->GetAPI()->close(dialogHandle);
    }

    void TaroRegionPickerNode::onCancel() {
        NapiDialogNodeApi::GetInstance()->GetAPI()->close(dialogHandle);
    }

    void TaroRegionPickerNode::SetRegionData(std::string regionData, uint32_t province, uint32_t city, uint32_t region) {
        if(!isPickerCreated) {
            return;
        }
        // set range
        ArkUI_NumberValue pickType[] = {{.i32 = ARKUI_TEXTPICKER_RANGETYPE_MULTI}};
        ArkUI_AttributeItem item = {
            pickType,
            sizeof(pickType) / sizeof(ArkUI_NumberValue),
            regionData.c_str()};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_OPTION_RANGE, &item);
        // set current position
        ArkUI_NumberValue indexArray[3];
        indexArray[0] = {.u32 = province};
        indexArray[1] = {.u32 = city};
        indexArray[2] = {.u32 = region};
        ArkUI_AttributeItem valueItem = {indexArray, static_cast<int32_t>(3)};
        NativeNodeApi::getInstance()->setAttribute(pickerHandle, NODE_TEXT_PICKER_OPTION_SELECTED, &valueItem);
    }

} // namespace TaroDOM
} // namespace TaroRuntime
