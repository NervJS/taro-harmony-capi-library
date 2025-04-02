/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "picker.h"

#include <js_native_api_types.h>

#include "arkjs/ArkJS.h"
#include "arkjs/Scope.h"
#include "runtime/NapiSetter.h"
#include "runtime/dom/ark_nodes/picker.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/event/event_helper.h"
#include "runtime/dom/event/event_hm/event_types/event_picker.h"
#include "runtime/dom/event/event_user/user_event.h"
#include "utilities.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroPicker::TaroPicker(napi_value node)
        : FormWidget(node),
          attributes_(std::make_unique<TaroPickerCommonAttributes>()) {
    }

    TaroPicker::~TaroPicker() {}

    void TaroPicker::GetNodeAttributes() {
        FormWidget::GetNodeAttributes();

        ArkJS arkJs(NativeNodeApi::env);
        auto modeValue = GetAttributeNodeValue("mode");
        mode = modeValue != nullptr ? arkJs.getString(modeValue) : "selector";
        auto picker = std::static_pointer_cast<TaroPicker>(shared_from_this());
        if (mode == "multiSelector") {
            delegate = std::make_shared<TaroMultiSelectorPickerDelegate>(picker);
        } else if (mode == "time") {
            delegate = std::make_shared<TaroTimePickerDelegate>(picker);
        } else if (mode == "date") {
            delegate = std::make_shared<TaroDatePickerDelegate>(picker);
        } else if (mode == "region") {
            delegate = std::make_shared<TaroRegionPickerDelegate>(picker);
        } else {
            // default is selector
            delegate = std::make_shared<TaroSelectorPickerDelegate>(picker);
        }
        // delegate default is TaroSelectorPickerDelegate
        delegate->onGetNodeAttributes();
    }

    void TaroPicker::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            std::shared_ptr<TaroPickerNode> node;
            if (mode == "multiSelector") {
                node = std::make_shared<TaroMultiSelectorPickerNode>(element);
            } else if (mode == "time") {
                node = std::make_shared<TaroTimePickerNode>(element);
            } else if (mode == "date") {
                node = std::make_shared<TaroDatePickerNode>(element);
            } else if (mode == "region") {
                node = std::make_shared<TaroRegionPickerNode>(element);
            } else {
                node = std::make_shared<TaroSelectorPickerNode>(element);
            }
            SetRenderNode(node);
            GetHeadRenderNode()->Build();
            RegisterEvents();
        }
    }

    void TaroPicker::RegisterEvents() {
        auto click = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            auto pickerNode = std::static_pointer_cast<TaroPickerNode>(GetHeadRenderNode());
            pickerNode->Prepare();
            delegate->onSetAttributesToRenderNode(pickerNode);
            // 显示弹窗
            pickerNode->Show();
            // 各个子类注册不同的事件种类
            delegate->onRegisterEvent(event_emitter_, pickerNode->GetPickerHandle());
            // 公共按钮区域
            auto cancelClick = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
                auto pickerNode = std::static_pointer_cast<TaroPickerNode>(GetHeadRenderNode());
                NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
                delegate->onCancel(GetNodeValue());
                pickerNode->onCancel();
                return 0;
            };
            auto confirmClick = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
                auto pickerNode = std::static_pointer_cast<TaroPickerNode>(GetHeadRenderNode());
                NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
                pickerNode->onConfirm();
                delegate->onConfirm(GetHeadRenderNode(), GetNodeValue());
                return 0;
            };
            event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_CLICK, "click", cancelClick,
                                                     pickerNode->GetCancelHandle());
            event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_CLICK, "click", confirmClick,
                                                     pickerNode->GetConfirmHandle());
            return 0;
        };
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_CLICK, "click", click, GetNodeHandle());
    }

    void TaroPicker::SetAttributesToRenderNode() {
        FormWidget::SetAttributesToRenderNode();
        GetHeadRenderNode()->SetStyle(style_);
    }

    void TaroPicker::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        FormWidget::SetAttribute(renderNode, name, value);

        // 各picker delegate就不用判断了
        if (!is_init_ || GetHeadRenderNode() == nullptr) {
            return;
        }
        auto pickerNode = std::static_pointer_cast<TaroPickerNode>(renderNode);
        NapiGetter getter(value);
        // 公共属性设置
        switch (name) {
            case ATTRIBUTE_NAME::OKText: {
                pickerNode->SetOKText(getter.String().value());
                break;
            }
            case ATTRIBUTE_NAME::CANCELText: {
                pickerNode->SetCancelText(getter.String().value());
                break;
            }
            default:
                break;
        }

        // 各个picker进行设置，公共部分上面处理
        delegate->SetAttribute(renderNode, name, value);
    }

    bool TaroPicker::bindListenEvent(const std::string& event_name) {
        if (event_name == "cancel") {
            event_emitter_->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, event_name);
        } else if (event_name == "change") {
            event_emitter_->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, event_name);
        } else if (event_name == "columnChange") {
            event_emitter_->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, event_name);
        } else {
            return false;
        }
        return true;
    }

    void TaroPickerDelegate::SetCommonAttributes() {
        ArkJS arkJs = ArkJS(NativeNodeApi::env);
        auto okText = element_ref_->GetAttributeNodeValue("okText");
        if (okText != nullptr) {
            GetAttrs()->okText.set(arkJs.getString(okText));
        }
        auto cancelText = element_ref_->GetAttributeNodeValue("cancelText");
        if (cancelText != nullptr) {
            GetAttrs()->cancelText.set(arkJs.getString(cancelText));
        }
    }

    void TaroPickerDelegate::SetCommonAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) {
        auto pickerNode = std::static_pointer_cast<TaroPickerNode>(renderNode);
        if (GetAttrs()->okText.has_value()) {
            pickerNode->SetOKText(GetAttrs()->okText.value());
        }
        if (GetAttrs()->cancelText.has_value()) {
            pickerNode->SetCancelText(GetAttrs()->cancelText.value());
        }
    }

    void TaroPickerDelegate::SetCommonAttributesChanged(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name,
                                                        napi_value value) {
        auto pickerNode = std::static_pointer_cast<TaroPickerNode>(renderNode);
        NapiGetter getter(value);
        switch (name) {
            case ATTRIBUTE_NAME::OKText: {
                if (getter.String().has_value()) {
                    pickerNode->SetOKText(getter.String().value());
                }
                break;
            }
            case ATTRIBUTE_NAME::CANCELText: {
                if (getter.String().has_value()) {
                    pickerNode->SetCancelText(getter.String().value());
                }
                break;
            }
            default:
                break;
        }
    }

    // selector
    void TaroSelectorPickerDelegate::onGetNodeAttributes() {
        ArkJS arkJs(NativeNodeApi::env);
        auto rangeKey = element_ref_->GetAttributeNodeValue("rangeKey");
        SetRangeKeyAttributes(arkJs.getString(rangeKey), false);

        NapiGetter range(element_ref_->GetAttributeNodeValue("range"));
        SetRangeAttributes(range);
        if (isObjArray) {
            SetObjectRangeAttributes(range);
        }
        SetValueAttributes(NapiGetter(element_ref_->GetAttributeNodeValue("value")));
        SetCommonAttributes();
    }

    void TaroSelectorPickerDelegate::SetRangeAttributes(NapiGetter rangeNapi) {
        // string[] or number[] or obj[]
        Optional<std::vector<NapiGetter>> rangeArray = rangeNapi.Vector();
        if (!rangeArray.has_value()) {
            TARO_LOG_ERROR("TaroPicker", "selector picker range is empty.");
            return;
        }
        columnChanged.set(0);
        attributes_->value.set(0);
        // create arkui format range info
        std::string range = "";
        for (auto it = rangeArray.value().begin(); it != rangeArray.value().end(); it++) {
            // handle type
            napi_valuetype type = it->GetType();
            if (type == napi_string) {
                range += it->String().value().c_str();
            } else if (type == napi_number) {
                range += std::to_string(it->GetValue().UInt32().value());
            } else if (type == napi_object) {
                // if type napi_object maybe range element is []
                isObjArray = true;
                break;
            }
            if (it != rangeArray.value().end() - 1) {
                range += ";";
            }
        }
        if (!range.empty()) {
            attributes_->range.set(range);
        }
    }

    void TaroSelectorPickerDelegate::SetObjectRangeAttributes(NapiGetter objRangeNapi) {
        // 是否有rangeKey
        if (!attributes_->rangeKey.has_value()) {
            TARO_LOG_ERROR("TaroPicker", "selector rangeKey is empty.");
            return;
        }
        auto objsNapi = objRangeNapi.Vector();
        if (!objsNapi.has_value()) {
            TARO_LOG_ERROR("TaroPicker", "selector range is empty.");
            return;
        }

        origin.clear();

        for (int i = 0; i < objsNapi.value().size(); i++) {
            NapiGetter item = objsNapi.value().at(i);
            std::vector<NapiGetter> names = item.GetAllPropertyNames();
            if (names.size() == 0 || !names[0].String().has_value()) {
                continue;
            }
            SelectorSource source;
            source.key = names[0].String().value();
            napi_valuetype type = item.GetProperty(source.key.c_str()).GetType();
            if (type == napi_string) {
                source.value = item.GetProperty(source.key.c_str()).String().value();
            } else if (type == napi_number) {
                source.value = std::to_string(item.GetProperty(source.key.c_str()).Int32().value());
            }
            // position
            source.position = i;
            if (origin.find(source.key) == origin.end()) {
                std::vector<SelectorSource> sourceByKey;
                sourceByKey.push_back(source);
                origin.insert(std::pair(source.key, sourceByKey));
            } else {
                origin.find(source.key)->second.push_back(source);
            }
        }
        auto iteratorByRangeKey = origin.find(attributes_->rangeKey.value());
        if (iteratorByRangeKey == origin.end()) {
            return;
        }
        std::vector<SelectorSource> sources = iteratorByRangeKey->second;
        std::string range = "";
        for (auto it = sources.begin(); it != sources.end(); it++) {
            range += it->value;
            if (it != sources.end() - 1) {
                range += ";";
            }
        }
        if (!range.empty()) {
            attributes_->range.set(range);
        }
    }

    void TaroSelectorPickerDelegate::onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) {
        auto pickerNode = std::static_pointer_cast<TaroSelectorPickerNode>(renderNode);
        if (!attributes_->range.has_value()) {
            return;
        }
        pickerNode->SetSelectorRange(attributes_->range);
        if (attributes_->value.has_value()) {
            pickerNode->SetSelect(attributes_->value);
        }
        SetCommonAttributesToRenderNode(renderNode);
    }

    void TaroSelectorPickerDelegate::onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter,
                                                     ArkUI_NodeHandle handle) {
        auto change = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            auto ui_event = std::static_pointer_cast<TaroEvent::TaroPickerChangeEvent>(event);
            columnChanged.set(ui_event->comp_event_.data[0].i32);
            return 0;
        };
        emitter->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_TEXT_PICKER_ON_CHANGE, "change", change, handle);
    }

    void TaroSelectorPickerDelegate::onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) {
        if (!columnChanged.has_value()) {
            return;
        }
        attributes_->value = columnChanged.value();
        auto event = std::make_shared<TaroEvent::UserEventBase>("change");
        // send event
        napi_value jsObj = event->detail();
        if (isObjArray) {
            // 当前rangekey上的元素, 在JS层postion
            if (!attributes_->rangeKey.has_value() ||
                origin.find(attributes_->rangeKey.value()) == origin.end()) {
                return;
            }
            int position = (origin.find(attributes_->rangeKey.value())->second).at(columnChanged.value()).position;
            NapiSetter::SetProperty(jsObj, "value", position);
        } else {
            NapiSetter::SetProperty(jsObj, "value", columnChanged.value());
        }
        element_ref_->getEventEmitter()->triggerEvents(event);
    }

    void TaroSelectorPickerDelegate::onCancel(napi_value picker) {
        auto event = std::make_shared<TaroEvent::UserEventBase>("cancel");
        element_ref_->getEventEmitter()->triggerEvents(event);
    }

    void TaroSelectorPickerDelegate::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        ArkJS arkJs(NativeNodeApi::env);
        auto pickerNode = std::static_pointer_cast<TaroSelectorPickerNode>(renderNode);
        NapiGetter getter(value);
        switch (name) {
            case ATTRIBUTE_NAME::VALUE: {
                SetValueAttributes(getter);
                if (attributes_->value.has_value()) {
                    pickerNode->SetSelect(attributes_->value.value());
                }
                break;
            }
            case ATTRIBUTE_NAME::RANGE: {
                SetRangeAttributes(getter);
                if (isObjArray) {
                    SetObjectRangeAttributes(getter);
                }
                // arkui 更新 range
                if (attributes_->range.has_value()) {
                    TARO_LOG_DEBUG("TaroPicker", "xxxx %{public}s", attributes_->range.value().c_str());
                    pickerNode->SetSelectorRange(attributes_->range);
                }
                break;
            }
            case ATTRIBUTE_NAME::RANGE_KEY: {
                SetRangeKeyAttributes(arkJs.getString(value), true);
                // range key变了 同步更新arkui的range
                if (attributes_->range.has_value()) {
                    pickerNode->SetSelectorRange(attributes_->range.value());
                }
                break;
            }
            default:
                break;
        }
        // handle common attributes
        SetCommonAttributesChanged(renderNode, name, value);
    }

    void TaroSelectorPickerDelegate::SetValueAttributes(NapiGetter valueNapi) {
        if (!isObjArray || !valueNapi.UInt32().has_value()) {
            attributes_->value.set(valueNapi.UInt32().has_value() ? valueNapi.UInt32().value() : 0);
        } else {
            // 如果是obj[]的模式, value是数组中的索引，但是通过category过滤之后，
            // 设置的Arkui的索引是过滤之后的第几个
            if (!attributes_->rangeKey.has_value()) {
                return;
            }
            auto it = origin.find(attributes_->rangeKey.value());
            if (it == origin.end()) {
                return;
            }
            for (int i = 0; i < it->second.size(); i++) {
                if (it->second.at(i).position == valueNapi.UInt32().value()) {
                    TARO_LOG_DEBUG("TaroPicker", "selector obj[] position is %{public}d", it->second.at(i).position);
                    // 当前的obj的索引
                    attributes_->value.set(i);
                    return;
                }
            }
            attributes_->value.set(0); // default is 0
        }
    }

    void TaroSelectorPickerDelegate::SetRangeKeyAttributes(std::string rangeKey, bool isUpdateRange) {
        attributes_->rangeKey.set(rangeKey);
        // 是否需要更新 range
        if (!isUpdateRange) {
            return;
        }
        // 如果不是obj array 不处理range key
        if (!isObjArray) {
            return;
        }

        columnChanged.set(0);

        if (origin.size() == 0) {
            return;
        }
        auto it = origin.find(rangeKey);
        if (it == origin.end()) {
            return;
        }
        std::string range = "";
        for (int i = 0; i < it->second.size(); i++) {
            range += it->second.at(i).value;
            if (i != it->second.size() - 1) {
                range += ";";
            }
        }
        // 关联关系 更新了range key要同步更新 range
        if (!range.empty()) {
            attributes_->range.set(range);
        }
    }

    // Multi Selector
    void TaroMultiSelectorPickerDelegate::onGetNodeAttributes() {
        SetRangeKeyAttribute(NapiGetter(element_ref_->GetAttributeNodeValue("rangeKey")));
        SetRangeAttribute(NapiGetter(element_ref_->GetAttributeNodeValue("range")));
        SetValueAttribute(NapiGetter(element_ref_->GetAttributeNodeValue("value")));
        SetCommonAttributes();
    }

    void TaroMultiSelectorPickerDelegate::SetRangeKeyAttribute(NapiGetter rangeKeyNapi) {
        if (!rangeKeyNapi.String().has_value()) {
            return;
        }
        attributes_->rangeKey.set(rangeKeyNapi.String().value());
    }

    void TaroMultiSelectorPickerDelegate::SetRangeAttribute(NapiGetter rangeNapi) {
        if (!rangeNapi.Vector().has_value()) {
            TARO_LOG_ERROR("TaroPicker", "Set Range Value.");
            return;
        }

        std::vector<NapiGetter> allColumns = rangeNapi.Vector().value();
        if (allColumns.empty()) {
            return;
        }
        std::vector<NapiGetter> firstColumn = allColumns.at(0).Vector().value();
        if (firstColumn.at(0).GetType() == napi_object) {
            TARO_LOG_DEBUG("TaroPicker", "multi selector type is obj array[].");
            isObjArray = true;
        }

        if (!isObjArray) {
            std::string range = ParseRange(rangeNapi.Vector().value());
            attributes_->range.set(range);
            columnCount = rangeNapi.Vector().value().size();
        } else {
            if (!attributes_->rangeKey.has_value()) {
                return;
            }
            std::string rangeByKey = ParseRangeByRangeKey(attributes_->rangeKey.value(), rangeNapi.Vector().value());
            attributes_->range.set(rangeByKey);
        }
    }

    void TaroMultiSelectorPickerDelegate::SetValueAttribute(NapiGetter valueNapi) {
        if (!valueNapi.Vector().has_value()) {
            return;
        }
        std::vector<uint32_t> values;
        for (int i = 0; i < valueNapi.Vector().value().size(); i++) {
            int32_t temp = valueNapi.Vector().value().at(i).UInt32().value();
            values.push_back(temp);
        }
        attributes_->value.set(values);
    }

    std::string TaroMultiSelectorPickerDelegate::ParseRange(std::vector<NapiGetter> napiValues) {
        std::string result = "";
        // 遍历多维数组
        for (auto it = napiValues.begin(); it != napiValues.end(); it++) {
            if (it->GetType() == napi_object) {
                // 递归
                if (it->Vector().has_value()) {
                    result += ParseRange(it->Vector().value());
                }
                continue;
            }
            // 具体元素
            if (it->GetType() == napi_string) {
                // string[]
                result += it->String().value().c_str();
            } else if (it->GetType() == napi_number) {
                // number[]
                result += std::to_string(it->Int32().value());
            }
            // 按照arkui组成字符串
            result += (it != napiValues.end() - 1) ? "," : ";";
        }
        return result;
    }

    std::string TaroMultiSelectorPickerDelegate::ParseRangeByRangeKey(std::string rangeKey, std::vector<NapiGetter> rangeNapi) {
        sources.clear();
        for (int i = 0; i < rangeNapi.size(); i++) {
            std::vector<NapiGetter> array = rangeNapi.at(i).Vector().value();
            std::string key = array.at(0).GetAllPropertyNames().at(0).String().value();
            auto it = sources.find(key);
            if (it == sources.end()) {
                std::vector<std::vector<SelectorSource>> items;
                sources.insert(std::pair(key, items));
            }
            std::vector<SelectorSource> sourceArray;
            for (int j = 0; j < array.size(); j++) {
                SelectorSource source;
                source.key = key;
                NapiGetter value = array.at(j).GetProperty(key.c_str());
                if (value.GetType() == napi_string) {
                    source.value = value.String().value();
                } else if (value.GetType() == napi_number) {
                    source.value = std::to_string(value.Int32().value());
                }
                source.position = j;
                sourceArray.push_back(source);
            }
            sources.find(key)->second.push_back(sourceArray);
        }

        std::string range = "";
        auto it = sources.find(rangeKey);
        if (it == sources.end()) {
            return range;
        }
        columnCount = it->second.size();
        for (int i = 0; i < it->second.size(); i++) {
            std::vector<SelectorSource> array = it->second.at(i);
            for (int j = 0; j < array.size(); j++) {
                range += array.at(j).value;
                range += (j != array.size() - 1) ? "," : ";";
            }
        }
        return range;
    }

    void TaroMultiSelectorPickerDelegate::onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) {
        auto pickerNode = std::static_pointer_cast<TaroMultiSelectorPickerNode>(renderNode);
        if (attributes_->range.has_value()) {
            pickerNode->SetMultiSelectorRange(attributes_->range.value());
        }
        if (attributes_->value.has_value()) {
            pickerNode->SetMultiSelectorValues(attributes_->value.value());
        }
        SetCommonAttributesToRenderNode(renderNode);
    }

    void TaroMultiSelectorPickerDelegate::onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter, ArkUI_NodeHandle handle) {
        auto change = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            changed.clear();
            auto ui_event = std::static_pointer_cast<TaroEvent::TaroPickerChangeEvent>(event);
            for (int i = 0; i < columnCount; i++) {
                changed.push_back(ui_event->comp_event_.data[i].i32);
            }
            return 0;
        };
        emitter->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_TEXT_PICKER_ON_CHANGE, "change", change, handle);
    }

    void TaroMultiSelectorPickerDelegate::onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) {
        SendColumnChangedEvent(picker);

        ArkJS arkJs(NativeNodeApi::env);
        auto event = std::make_shared<TaroEvent::UserEventBase>("change");
        napi_value jsObj = event->detail();
        napi_value result = arkJs.createArray();

        if (changed.size() == 0) {
            return;
        }
        for (int i = 0; i < changed.size(); i++) {
            napi_value item = arkJs.createInt(changed.at(i));
            napi_set_element(NativeNodeApi::env, result, i, item);
        }

        NapiSetter::SetProperty(jsObj, "value", result);
        element_ref_->getEventEmitter()->triggerEvents(event);
    }

    void TaroMultiSelectorPickerDelegate::SendColumnChangedEvent(napi_value picker) {
        if (!attributes_->value.has_value()) {
            return;
        }
        if (changed.size() == 0 || changed.size() != columnCount) {
            return;
        }
        for (int i = 0; i < columnCount; i++) {
            int current = attributes_->value.value().at(i);
            int change = changed.at(i);
            if (current != change) {
                auto event = std::make_shared<TaroEvent::UserEventBase>("columnChange");
                napi_value columnChangeValue = event->detail();
                NapiSetter::SetProperty(columnChangeValue, "column", i);
                NapiSetter::SetProperty(columnChangeValue, "value", change);
                element_ref_->getEventEmitter()->triggerEvents(event);
            }
        }
    }

    void TaroMultiSelectorPickerDelegate::onCancel(napi_value picker) {
        auto event = std::make_shared<TaroEvent::UserEventBase>("cancel");
        element_ref_->getEventEmitter()->triggerEvents(event);
    }

    void TaroMultiSelectorPickerDelegate::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name,
                                                       napi_value value) {
        auto pickerNode = std::static_pointer_cast<TaroMultiSelectorPickerNode>(renderNode);
        NapiGetter getter(value);
        switch (name) {
            case ATTRIBUTE_NAME::RANGE: {
                SetRangeAttribute(getter);
                pickerNode->SetMultiSelectorRange(attributes_->range);
                break;
            }
            case ATTRIBUTE_NAME::RANGE_KEY: {
                SetRangeKeyAttribute(getter);
                SetRangeAttribute(getter);
                // 关联修改range
                pickerNode->SetMultiSelectorRange(attributes_->range);
                break;
            }
            case ATTRIBUTE_NAME::VALUE:
                SetValueAttribute(getter);
                break;
            default:
                break;
        }
        SetCommonAttributesChanged(renderNode, name, value);
    }
    //////////////////////////// Time Picker //////////////////////////////////////
    void TaroTimePickerDelegate::onGetNodeAttributes() {
        ArkJS arkJs(NativeNodeApi::env);
        auto timeValue = element_ref_->GetAttributeNodeValue("value");
        if (timeValue != nullptr) {
            attributes_->value.set(arkJs.getString(timeValue));
        }
    }

    void TaroTimePickerDelegate::onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) {
        auto pickerNode = std::static_pointer_cast<TaroTimePickerNode>(renderNode);
        if (attributes_->value.has_value()) {
            pickerNode->SetTime(attributes_->value.value());
        }
    }

    void TaroTimePickerDelegate::onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter, ArkUI_NodeHandle handle) {
        auto change = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            auto ui_event = std::static_pointer_cast<TaroEvent::TaroPickerChangeEvent>(event);
            std::string current = std::to_string(ui_event->comp_event_.data[0].i32) + ":" + std::to_string(ui_event->comp_event_.data[1].i32);
            changed.set(current);
            return 0;
        };
        emitter->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_TIME_PICKER_ON_CHANGE, "change", change, handle);
    }

    void TaroTimePickerDelegate::onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) {
        if (!changed.has_value()) {
            return;
        }
        auto event = std::make_shared<TaroEvent::UserEventBase>("change");
        napi_value jsObj = event->detail();
        NapiSetter::SetProperty(jsObj, "value", changed.value());
        element_ref_->getEventEmitter()->triggerEvents(event);
    }
    void TaroTimePickerDelegate::onCancel(napi_value picker) {
        auto event = std::make_shared<TaroEvent::UserEventBase>("cancel");
        element_ref_->getEventEmitter()->triggerEvents(event);
    }

    void TaroTimePickerDelegate::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        auto pickerNode = std::static_pointer_cast<TaroTimePickerNode>(renderNode);
        NapiGetter getter(value);
        switch (name) {
            case ATTRIBUTE_NAME::VALUE:
                if (getter.String().has_value()) {
                    attributes_->value.set(getter.String().value());
                    pickerNode->SetTime(getter.String().value());
                }
                break;
            default:
                break;
        }
    }

    /////////////////////////////////////date picker//////////////////////////////
    void TaroDatePickerDelegate::onGetNodeAttributes() {
        ArkJS arkJs(NativeNodeApi::env);
        auto dataValue = element_ref_->GetAttributeNodeValue("value");
        if (dataValue != nullptr) {
            attributes_->value.set(arkJs.getString(dataValue));
        }
        auto dataStart = element_ref_->GetAttributeNodeValue("start");
        if (dataStart != nullptr) {
            attributes_->start.set(arkJs.getString(dataStart));
        }
        auto dataEnd = element_ref_->GetAttributeNodeValue("end");
        if (dataEnd != nullptr) {
            attributes_->end.set(arkJs.getString(dataEnd));
        }
    }

    void TaroDatePickerDelegate::onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) {
        auto pickerNode = std::static_pointer_cast<TaroDatePickerNode>(renderNode);
        if (attributes_->value.has_value()) {
            pickerNode->SetDate(attributes_->value.value());
        }
        if (attributes_->start.has_value()) {
            pickerNode->SetStart(attributes_->start.value());
        }
        if (attributes_->end.has_value()) {
            pickerNode->SetEnd(attributes_->end.value());
        }
    }

    void TaroDatePickerDelegate::onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter, ArkUI_NodeHandle handle) {
        auto change = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            auto ui_event = std::static_pointer_cast<TaroEvent::TaroPickerChangeEvent>(event);
            std::string current = std::to_string(ui_event->comp_event_.data[0].i32) + "-" +
                                  std::to_string(ui_event->comp_event_.data[1].i32 + 1) + "-" +
                                  std::to_string(ui_event->comp_event_.data[2].i32);
            changed.set(current);
            return 0;
        };
        emitter->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_DATE_PICKER_ON_CHANGE, "change", change, handle);
    }

    void TaroDatePickerDelegate::onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) {
        if (!changed.has_value()) {
            return;
        }

        auto event = std::make_shared<TaroEvent::UserEventBase>("change");
        napi_value jsObj = event->detail();
        NapiSetter::SetProperty(jsObj, "value", changed.value());
        element_ref_->getEventEmitter()->triggerEvents(event);
    }

    void TaroDatePickerDelegate::onCancel(napi_value picker) {
        auto event = std::make_shared<TaroEvent::UserEventBase>("cancel");
        element_ref_->getEventEmitter()->triggerEvents(event);
    }

    void TaroDatePickerDelegate::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        auto pickerNode = std::static_pointer_cast<TaroDatePickerNode>(renderNode);
        NapiGetter getter(value);
        switch (name) {
            case ATTRIBUTE_NAME::VALUE: {
                if (!getter.String().has_value()) {
                    return;
                }
                attributes_->value.set(getter.String().value());
                pickerNode->SetDate(getter.String().value());
                break;
            }
            case ATTRIBUTE_NAME::START: {
                if (!getter.String().has_value()) {
                    return;
                }
                attributes_->start.set(getter.String().value());
                pickerNode->SetStart(attributes_->start.value());
                break;
            }
            case ATTRIBUTE_NAME::END: {
                if (!getter.String().has_value()) {
                    return;
                }
                attributes_->end.set(getter.String().value());
                pickerNode->SetEnd(attributes_->end.value());
                break;
            }
            default:
                break;
        }
    }
    ////////////////////////// Region Picker base on Multi Selector //////////////////////////
    void TaroRegionPickerDelegate::onGetNodeAttributes() {
        ArkJS arkJs(NativeNodeApi::env);
        auto regionDataValue = element_ref_->GetAttributeNodeValue("regionData");
        if (regionDataValue != nullptr) {
            attributes_->regionData.set(ParseRegionData(NapiGetter(regionDataValue).Vector().value()));
        } else {
            TARO_LOG_ERROR("TaroPicker", "region picker data is empty.");
            return;
        }

        auto valueValue = element_ref_->GetAttributeNodeValue("value");
        if (valueValue != nullptr) {
            SetValue(NapiGetter(valueValue).Vector().value());
        }

        auto levelValue = element_ref_->GetAttributeNodeValue("level");
        if (levelValue != nullptr) {
            SetLevel(arkJs.getString(levelValue));
        } else {
            attributes_->level.set("region");
        }

        auto customItemValue = element_ref_->GetAttributeNodeValue("customItem");
        if (customItemValue != nullptr) {
            SetCustomItem(arkJs.getString(customItemValue));
        }
    }

    std::vector<TaroRegionData> TaroRegionPickerDelegate::ParseRegionData(std::vector<NapiGetter> regionData) {
        std::vector<TaroRegionData> result;
        for (NapiGetter getter : regionData) {
            TaroRegionData resultData;
            NapiGetter name = getter.GetProperty("value");
            if (name.String().has_value()) {
                resultData.value.set(name.String().value());
            }
            NapiGetter code = getter.GetProperty("code");
            if (code.String().has_value()) {
                resultData.code.set(code.String().value());
            }
            NapiGetter postCode = getter.GetProperty("postcode");
            if (postCode.String().has_value()) {
                resultData.postcode.set(postCode.String().value());
            }
            NapiGetter children = getter.GetProperty("children");
            if (children.Vector().has_value()) {
                std::vector<TaroRegionData> childrenData = ParseRegionData(children.Vector().value());
                resultData.children.set(childrenData);
            }
            result.push_back(resultData);
        }
        return result;
    }

    void TaroRegionPickerDelegate::ParseRegionInfos() {
        if (!attributes_->regionData.has_value()) {
            return;
        }
        std::vector<TaroRegionData> data = attributes_->regionData.value();
        for (std::vector<TaroRegionData>::iterator it = data.begin(); it != data.end(); it++) {
            std::string provinceName = it->value.value(); // 省名称
            std::vector<std::string> cities;
            for (auto cityIt = it->children.value().begin(); cityIt != it->children.value().end(); cityIt++) {
                std::string cityName = cityIt->value.value();
                cities.push_back(cityName);
                std::vector<std::string> regions;
                for (auto regionIt = cityIt->children.value().begin(); regionIt != cityIt->children.value().end(); regionIt++) {
                    std::string regionName = regionIt->value.value();
                    regions.push_back(regionName);
                }
                regionMap.insert(std::pair<std::string, std::vector<std::string>>(cityName, regions));
            }
            cityMap.insert(std::pair<std::string, std::vector<std::string>>(provinceName, cities));
        }
    }

    std::string TaroRegionPickerDelegate::CreateRegionRange(bool isUsingDefault) {
        // 清理当前picker状态值
        provinces.clear();
        city.clear();
        region.clear();
        // 如果数据重置清零
        if (isUsingDefault) {
            provinceIndex = 0;
            cityIndex = 0;
            regionIndex = 0;
        }

        bool isCustomExist = attributes_->customItem.has_value();
        std::vector<TaroRegionData> data = attributes_->regionData.value();

        // 处理 province 的Arkui字符串拼接
        for (auto iterator = data.begin(); iterator != data.end(); iterator++) {
            provinces.push_back(iterator->value.value());
        }

        if (isCustomExist) {
            provinces.insert(provinces.begin(), attributes_->customItem.value());
        }

        std::string provincesNames = "";
        for (int i = 0; i < provinces.size(); i++) {
            if (isUsingDefault && attributes_->value.has_value() && provinces.at(i) == attributes_->value.value().at(0)) {
                provinceIndex = i;
            }
            provincesNames += provinces.at(i);
            provincesNames += (i == (provinces.size() - 1)) ? ";" : ",";
        }
        // level is province
        if (attributes_->level.value() == "province") {
            return provincesNames;
        }

        // city
        std::string cityNames = "";
        std::string regionNames = "";

        auto it = cityMap.find(provinces.at(provinceIndex));
        if (it != cityMap.end()) {
            for (int i = 0; i < it->second.size(); i++) {
                city.push_back(it->second.at(i));
            }
            if (isCustomExist) {
                city.insert(city.begin(), attributes_->customItem.value());
            }

            for (int i = 0; i < city.size(); i++) {
                if (isUsingDefault && attributes_->value.has_value() && city.at(i) == attributes_->value.value().at(1)) {
                    cityIndex = i;
                }
                cityNames += city.at(i);
                cityNames += (i == (city.size() - 1)) ? ";" : ",";
            }
            // level is city
            if (attributes_->level.value() == "city") {
                return provincesNames + cityNames;
            }

            // regionNames
            auto regionIt = regionMap.find(city.at(cityIndex)); // 根据省找城市
            if (regionIt != regionMap.end()) {
                for (int i = 0; i < regionIt->second.size(); i++) {
                    region.push_back(regionIt->second.at(i));
                }

                if (isCustomExist) {
                    region.insert(region.begin(), attributes_->customItem.value());
                }

                for (int i = 0; i < region.size(); i++) {
                    if (isUsingDefault && attributes_->value.has_value() && region.at(i) == attributes_->value.value().at(2)) {
                        regionIndex = i;
                    }
                    regionNames += region.at(i);
                    regionNames += (i == (region.size() - 1)) ? ";" : ",";
                }
            } else if (isCustomExist && cityIndex == 0) {
                region.push_back(attributes_->customItem.value());
                regionIndex = 0;
                regionNames = attributes_->customItem.value() + ";";
            }
        } else if (isCustomExist && provinceIndex == 0) {
            city.push_back(attributes_->customItem.value());
            cityIndex = 0;
            cityNames = attributes_->customItem.value() + ";";
            region.push_back(attributes_->customItem.value());
            regionIndex = 0;
            regionNames = attributes_->customItem.value() + ";";
        }
        return provincesNames + cityNames + regionNames;
    }

    void TaroRegionPickerDelegate::onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) {
        ParseRegionInfos();
        m_renderNode = renderNode;
        auto pickerNode = std::static_pointer_cast<TaroRegionPickerNode>(renderNode);
        if (attributes_->regionData.has_value()) {
            pickerNode->SetRegionData(CreateRegionRange(true), provinceIndex, cityIndex, regionIndex);
        }
        SetCommonAttributesToRenderNode(renderNode);
    }

    void TaroRegionPickerDelegate::onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter, ArkUI_NodeHandle handle) {
        auto change = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            auto ui_event = std::static_pointer_cast<TaroEvent::TaroPickerChangeEvent>(event);
            TARO_LOG_DEBUG("TaroPicker", "TaroRegionPickerDelegate changed, %{public}d, %{public}d, %{public}d.",
                           ui_event->comp_event_.data[0].i32, ui_event->comp_event_.data[1].i32, ui_event->comp_event_.data[2].i32);
            HandleColumnChanged(ui_event->comp_event_.data[0].i32,
                                ui_event->comp_event_.data[1].i32, ui_event->comp_event_.data[2].i32);
            return 0;
        };
        emitter->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_TEXT_PICKER_ON_CHANGE, "change", change, handle);
    }

    void TaroRegionPickerDelegate::HandleColumnChanged(int provinceChanged, int cityChanged, int regionChanged) {
        auto pickerNode = std::static_pointer_cast<TaroRegionPickerNode>(m_renderNode);
        if (provinceIndex != provinceChanged) { // province变化了，city以及region要重置，记录当前provice滚动索引
            provinceIndex = provinceChanged;
            cityIndex = 0;
            regionIndex = 0;
        } else if (cityChanged != cityIndex) { // city变化了，region要重置，province不处理，记录当前city滚动索引
            cityIndex = cityChanged;
            regionIndex = 0;
        } else if (regionIndex != regionChanged) { // 区变化了，记录区的索引
            // do nothing if region is changed
            regionIndex = regionChanged;
            return;
        }
        // 切换省对应的Range
        std::string range = CreateRegionRange(false);
        TARO_LOG_DEBUG("TaroPicker", "%{public}d, %{public}d, %{public}d, range is %{public}s",
                       provinceIndex, cityIndex, regionIndex, range.c_str());
        // 设置Range并且设置当前默认位置
        pickerNode->SetRegionData(range, provinceIndex, cityIndex, regionIndex);
    }

    void TaroRegionPickerDelegate::onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) {
        if (!attributes_->regionData.has_value()) {
            return;
        }
        ArkJS arkJs(NativeNodeApi::env);
        auto event = std::make_shared<TaroEvent::UserEventBase>("change");
        napi_value jsObj = event->detail();

        // [provice, city, region]
        napi_value result = arkJs.createArray();
        // province
        napi_value provinceNapi = arkJs.createString(provinces.at(provinceIndex));
        napi_set_element(NativeNodeApi::env, result, 0, provinceNapi);
        // city
        if (attributes_->level.value() != "province") {
            napi_value cityNapi = arkJs.createString(city.at(cityIndex));
            napi_set_element(NativeNodeApi::env, result, 1, cityNapi);
        }
        // region
        if (attributes_->level.value() != "province" && attributes_->level.value() != "city") {
            napi_value regionNapi = arkJs.createString(region.at(regionIndex));
            napi_set_element(NativeNodeApi::env, result, 2, regionNapi);
        }
        NapiSetter::SetProperty(jsObj, "value", result);
        element_ref_->getEventEmitter()->triggerEvents(event);
    }

    void TaroRegionPickerDelegate::onCancel(napi_value picker) {
        auto event = std::make_shared<TaroEvent::UserEventBase>("cancel");
        element_ref_->getEventEmitter()->triggerEvents(event);
    }

    void TaroRegionPickerDelegate::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        ArkJS arkJs(NativeNodeApi::env);
        auto pickerNode = std::static_pointer_cast<TaroRegionPickerNode>(renderNode);
        NapiGetter getter(value);
        switch (name) {
            case ATTRIBUTE_NAME::VALUE: {
                SetValue(getter.Vector().value());
                break;
            }
            case ATTRIBUTE_NAME::LEVEL: {
                SetLevel(arkJs.getString(value));
                pickerNode->SetRegionData(CreateRegionRange(true), provinceIndex, cityIndex, regionIndex);
                break;
            }
            default:
                break;
        }
    }

    void TaroRegionPickerDelegate::SetValue(std::vector<NapiGetter> valueNapi) {
        std::vector<std::string> defaultValue;
        for (int i = 0; i < valueNapi.size(); i++) {
            defaultValue.push_back(valueNapi.at(i).String().value());
        }
        attributes_->value.set(defaultValue);
    }

    void TaroRegionPickerDelegate::SetLevel(std::string levelNapi) {
        attributes_->level.set(levelNapi);
    }

    void TaroRegionPickerDelegate::SetCustomItem(std::string customItem) {
        attributes_->customItem.set(customItem);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
