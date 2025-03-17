//
// Created on 2024/8/7.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "slider.h"

#include "arkjs/ArkJS.h"
#include "helper/TaroLog.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/event/event_hm/event_types/event_slider.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroSlider::TaroSlider(napi_value node)
        : FormWidget(node) {
        attributes_ = std::make_unique<TaroSliderAttributes>();
    }

    bool TaroSlider::bindListenEvent(const std::string& event_name) {
        if (event_name == "click") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else if (event_name == "focus") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_FOCUS, event_name);
        } else if (event_name == "blur") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_BLUR, event_name);
        } else if (event_name == "change") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_SLIDER_ON_CHANGE, event_name, nullptr, slider_ ? slider_->GetArkUINodeHandle() : nullptr);
        } else if (event_name == "changing") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_SLIDER_ON_CHANGING, event_name, nullptr, slider_ ? slider_->GetArkUINodeHandle() : nullptr);
        } else {
            return false;
        }
        return true;
    }

    void TaroSlider::bindSelfEventHandle() {
        FormWidget::bindSelfEventHandle();
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_SLIDER_ON_CHANGE, "change", [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            if (auto change = std::dynamic_pointer_cast<TaroEvent::TaroSliderEventChange>(event)) {
                float value = change->getValue();
                auto attr = GetAttrs();
                attr->value.set(value);
                SetValueAttribute();
            }
            return 0; }, slider_->GetArkUINodeHandle());
    }

    void TaroSlider::Build() {
        if (!is_init_) {
            // 构建slider render node
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            slider_ = std::make_shared<TaroSliderNode>(element);
            slider_->Build();
            auto render_stack = std::make_shared<TaroStackNode>(element);
            render_stack->Build();

            SetRenderNode(render_stack);

            if (!style_->display.has_value()) {
                style_->display.set(PropertyType::Display::Flex);
            }
            if (!style_->flexDirection.has_value()) {
                style_->flexDirection.set(ARKUI_FLEX_DIRECTION_ROW);
            }
            if (!style_->height.has_value()) {
                style_->height.set(Dimension(40, DimensionUnit::VP));
            }
            if (!style_->width.has_value()) {
                style_->width.set(Dimension(1, DimensionUnit::PERCENT));
            }
            GetHeadRenderNode()->SetStyle(style_);
            render_stack->AppendChild(slider_);
        }
    }

    void TaroSlider::GetNodeAttributes() {
        FormWidget::GetNodeAttributes();

        GetStepAttribute(GetAttributeNodeValue("step"));
        GetMinValueAttribute(GetAttributeNodeValue("min"));
        GetMaxValueAttribute(GetAttributeNodeValue("max"));
        GetShowValueAttribute(GetAttributeNodeValue("showValue"));
        GetBlockSizeAttribute(GetAttributeNodeValue("blockSize"));
        GetActiveColorAttribute(GetAttributeNodeValue("activeColor"));
        GetBackgroundColorAttribute(GetAttributeNodeValue("backgroundColor"));
        GetBlockColorAttribute(GetAttributeNodeValue("blockColor"));
    }

    void TaroSlider::GetStepAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value && arkJs.getType(value) == napi_number) {
            attributes_->step.set(arkJs.getInteger(value));
        }
    }

    void TaroSlider::GetValueAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value && arkJs.getType(value) == napi_number) {
            attributes_->value.set(arkJs.getInteger(value));
        }
    }

    void TaroSlider::GetMinValueAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value && arkJs.getType(value) == napi_number) {
            attributes_->minValue.set(arkJs.getInteger(value));
        }
    }

    void TaroSlider::GetMaxValueAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value && arkJs.getType(value) == napi_number) {
            attributes_->maxValue.set(arkJs.getInteger(value));
        }
    }

    void TaroSlider::GetShowValueAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value && arkJs.getType(value) == napi_boolean) {
            attributes_->showValue.set(arkJs.getBoolean(value));
        }
    }

    void TaroSlider::GetBlockSizeAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value && arkJs.getType(value) == napi_number) {
            attributes_->blockSize.set(arkJs.getInteger(value));
        }
    }

    void TaroSlider::GetActiveColorAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value && arkJs.getType(value) == napi_string) {
            attributes_->activeColor.set(arkJs.getString(value));
        }
    }

    void TaroSlider::GetBackgroundColorAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value && arkJs.getType(value) == napi_string) {
            attributes_->backgroundColor.set(arkJs.getString(value));
        }
    }

    void TaroSlider::GetBlockColorAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value && arkJs.getType(value) == napi_string) {
            attributes_->blockColor.set(arkJs.getString(value));
        }
    }

    void TaroSlider::SetStepAttribute() {
        if (attributes_->step.has_value()) {
            auto slider = std::static_pointer_cast<TaroSliderNode>(slider_);
            slider->SetStep(attributes_->step.value());
        }
    }

    void TaroSlider::SetValueAttribute() {
        if (attributes_->value.has_value()) {
            auto slider = std::static_pointer_cast<TaroSliderNode>(slider_);
            slider->SetValue(attributes_->value.value());
            if (span_) {
                std::string val = std::to_string(attributes_->value.value());
                span_->SetContent(std::move(val));
            }
        }
    }

    void TaroSlider::SetMinValueAttribute() {
        if (attributes_->minValue.has_value()) {
            auto slider = std::static_pointer_cast<TaroSliderNode>(slider_);
            slider->SetMinValue(attributes_->minValue.value());
        }
    }

    void TaroSlider::SetMaxValueAttribute() {
        if (attributes_->maxValue.has_value()) {
            auto slider = std::static_pointer_cast<TaroSliderNode>(slider_);
            slider->SetMaxValue(attributes_->maxValue.value());
        }
    }

    void TaroSlider::SetShowValueAttribute() {
        auto slider_style = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
        if (attributes_->showValue.has_value() && attributes_->showValue.value()) {
            slider_style->width.set(Dimension(0.9, DimensionUnit::PERCENT));
            if (span_ == nullptr) {
                auto span_style = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
                auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
                span_ = std::make_shared<TaroTextSpanNode>(element);
                span_->Build();
                if (attributes_->value.has_value()) {
                    std::string val = std::to_string(attributes_->value.value());
                    span_->SetContent(std::move(val));
                }
                span_style->height.set(Dimension(1, DimensionUnit::PERCENT));
                span_style->width.set(Dimension(0.1, DimensionUnit::PERCENT));
                span_->SetStyle(span_style);
                GetHeadRenderNode()->AppendChild(span_);
            }
        } else {
            slider_style->width.set(style_->width.value_or(Dimension(1, DimensionUnit::PERCENT)));
            if (span_ != nullptr) {
                GetHeadRenderNode()->RemoveChild(span_);
                span_ = nullptr;
            }
        }
        slider_->SetStyle(slider_style);
    }

    void TaroSlider::SetBlockSizeAttribute() {
        if (attributes_->blockSize.has_value()) {
            auto slider = std::static_pointer_cast<TaroSliderNode>(slider_);
            slider->SetBlockSize(attributes_->blockSize.value());
        }
    }

    void TaroSlider::SetActiveColorAttribute() {
        if (attributes_->activeColor.has_value()) {
            std::optional<uint32_t> colorValue = TaroHelper::StringUtils::parseColor(attributes_->activeColor.value());
            auto slider = std::static_pointer_cast<TaroSliderNode>(slider_);
            slider->SetActiveColor(colorValue.value());
        }
    }

    void TaroSlider::SetBackgroundColorAttribute() {
        if (attributes_->backgroundColor.has_value()) {
            std::optional<uint32_t> colorValue = TaroHelper::StringUtils::parseColor(attributes_->backgroundColor.value());
            auto slider = std::static_pointer_cast<TaroSliderNode>(slider_);
            slider->SetBackgroundColor(colorValue.value());
        }
    }

    void TaroSlider::SetBlockColorAttribute() {
        if (attributes_->blockColor.has_value()) {
            std::optional<uint32_t> colorValue = TaroHelper::StringUtils::parseColor(attributes_->blockColor.value());
            auto slider = std::static_pointer_cast<TaroSliderNode>(slider_);
            slider->SetBlockColor(colorValue.value());
        }
    }

    void TaroSlider::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        switch (name) {
            case ATTRIBUTE_NAME::STEP: {
                GetStepAttribute(value);
                if (is_init_) {
                    SetStepAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::VALUE: {
                GetValueAttribute(value);
                if (is_init_) {
                    SetValueAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::MIN: {
                GetMinValueAttribute(value);
                if (is_init_) {
                    SetMinValueAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::MAX: {
                GetMaxValueAttribute(value);
                if (is_init_) {
                    SetMaxValueAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::SHOW_VALUE: {
                GetShowValueAttribute(value);
                if (is_init_) {
                    SetShowValueAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::BLOCK_SIZE: {
                GetBlockSizeAttribute(value);
                if (is_init_) {
                    SetBlockSizeAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::ACTIVE_COLOR: {
                GetActiveColorAttribute(value);
                if (is_init_) {
                    SetActiveColorAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::BACKGROUND_COLOR: {
                GetBackgroundColorAttribute(value);
                if (is_init_) {
                    SetBackgroundColorAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::BLOCK_COLOR: {
                GetBlockColorAttribute(value);
                if (is_init_) {
                    SetBlockColorAttribute();
                }
                break;
            }
            default:
                FormWidget::SetAttribute(renderNode, name, value);
                break;
        }
    }

    void TaroSlider::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();

        SetStepAttribute();
        SetValueAttribute();
        SetMinValueAttribute();
        SetMaxValueAttribute();
        SetShowValueAttribute();
        SetBlockSizeAttribute();
        SetActiveColorAttribute();
        SetBackgroundColorAttribute();
        SetBlockColorAttribute();
    }
} // namespace TaroDOM
} // namespace TaroRuntime
