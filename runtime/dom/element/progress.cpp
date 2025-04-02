/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "progress.h"

#include <cstdint>

#include "arkjs/ArkJS.h"
#include "runtime/dom/ark_nodes/stack.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroProgress::TaroProgress(napi_value node)
        : TaroElement(node),
          attributes_(std::make_unique<TaroProgressAttributes>()) {
    }

    TaroProgress::~TaroProgress() {}

    void TaroProgress::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            progress_ = std::make_shared<TaroProgressNode>(element);
            progress_->Build();
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
            render_stack->AppendChild(progress_);
        }
    }

    void TaroProgress::GetNodeAttributes() {
        TaroElement::GetNodeAttributes();

        ArkJS arkJs(NativeNodeApi::env);
        attributes_ = std::make_unique<TaroProgressAttributes>();
        GetPercentAttribute(GetAttributeNodeValue("percent"));
        GetShowInfoAttribute(GetAttributeNodeValue("showInfo"));
        GetBorderRadiusAttribute(GetAttributeNodeValue("borderRadius"));
        GetFontSizeAttribute(GetAttributeNodeValue("fontSize"));
        GetStrokeWidthAttribute(GetAttributeNodeValue("strokeWidth"));
        GetActiveColorAttribute(GetAttributeNodeValue("activeColor"));
        if (!attributes_->activeColor.has_value()) {
            if (auto active_color = GetAttributeNodeValue("color")) {
                GetActiveColorAttribute(active_color);
            }
        }
        GetBackgroundColorAttribute(GetAttributeNodeValue("backgroundColor"));
        GetActiveAttribute(GetAttributeNodeValue("active"));
        GetActiveModeAttribute(GetAttributeNodeValue("activeMode"));
    }

    void TaroProgress::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();

        SetPercentAttribute();
        SetShowInfoAttribute();
        SetBorderRadiusAttribute();
        SetFontSizeAttribute();
        SetStrokeWidthAttribute();
        SetActiveColorAttribute();
        SetBackgroundColorAttribute();

        if (!style_->height.has_value()) {
            style_->height.set(Dimension(40, DimensionUnit::VP));
        }
        if (!style_->width.has_value()) {
            style_->width.set(Dimension(1, DimensionUnit::PERCENT));
        }
        GetHeadRenderNode()->SetStyle(style_);
    }

    void TaroProgress::GetPercentAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value != nullptr && arkJs.getType(value) == napi_number) {
            attributes_->percent.set(arkJs.getInteger(value));
        }
    }

    void TaroProgress::GetShowInfoAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value != nullptr && arkJs.getType(value) == napi_boolean) {
            attributes_->showInfo.set(arkJs.getBoolean(value));
        }
    }

    void TaroProgress::GetBorderRadiusAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value != nullptr && arkJs.getType(value) == napi_number) {
            attributes_->borderRadius.set(arkJs.getInteger(value));
        }
    }

    void TaroProgress::GetFontSizeAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value != nullptr && arkJs.getType(value) == napi_number) {
            attributes_->fontSize.set(arkJs.getInteger(value));
        }
    }

    void TaroProgress::GetStrokeWidthAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value != nullptr && arkJs.getType(value) == napi_number) {
            attributes_->strokeWidth.set(arkJs.getInteger(value));
        }
    }

    void TaroProgress::GetActiveColorAttribute(const napi_value& value) {
        if (value != nullptr) {
            NapiGetter getter(value);
            attributes_->activeColor.set(TaroHelper::ColorUtils::getInt32FromNapiGetter(getter));
        }
    }

    void TaroProgress::GetBackgroundColorAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value != nullptr && arkJs.getType(value) == napi_number) {
            NapiGetter getter(value);
            attributes_->backgroundColor.set(TaroHelper::ColorUtils::getInt32FromNapiGetter(getter));
        }
    }

    void TaroProgress::GetActiveAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value != nullptr && arkJs.getType(value) == napi_boolean) {
            attributes_->active.set(arkJs.getBoolean(value));
        }
    }

    void TaroProgress::GetActiveModeAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value != nullptr && arkJs.getType(value) == napi_string) {
            attributes_->activeMode.set(arkJs.getString(value));
        }
    }

    void TaroProgress::SetPercentAttribute() {
        if (attributes_->percent.has_value()) {
            progress_->SetPercent(attributes_->percent.value());
            if (span_) {
                std::string val = std::to_string(attributes_->percent.value()) + "%";
                span_->SetContent(std::move(val));
            }
        }
    }

    void TaroProgress::SetShowInfoAttribute() {
        auto progress_style = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
        if (attributes_->showInfo.has_value() && attributes_->showInfo.value()) {
            progress_style->width.set(Dimension(0.9, DimensionUnit::PERCENT));
            if (span_ == nullptr) {
                auto span_style = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
                auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
                span_ = std::make_shared<TaroTextSpanNode>(element);
                span_->Build();
                if (attributes_->percent.has_value()) {
                    std::string val = std::to_string(attributes_->percent.value()) + "%";
                    span_->SetContent(std::move(val));
                }
                span_style->height.set(Dimension(1, DimensionUnit::PERCENT));
                span_style->width.set(Dimension(0.1, DimensionUnit::PERCENT));
                span_->SetStyle(span_style);
                GetHeadRenderNode()->AppendChild(span_);
            }
        } else {
            progress_style->width.set(Dimension(1, DimensionUnit::PERCENT));
            if (span_ != nullptr) {
                GetHeadRenderNode()->RemoveChild(span_);
                span_ = nullptr;
            }
        }

        progress_style->height.set(Dimension(1, DimensionUnit::PERCENT));
        progress_->SetStyle(progress_style);
    }

    void TaroProgress::SetBorderRadiusAttribute() {
        if (attributes_->borderRadius.has_value()) {
            progress_->SetStrokeRadius(attributes_->borderRadius.value());
        }
    }

    void TaroProgress::SetFontSizeAttribute() {
        if (attributes_->fontSize.has_value()) {
            if (span_) {
                auto span_style = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
                span_style->fontSize.set(Dimension(attributes_->fontSize.value(), DimensionUnit::VP));
                span_->SetStyle(span_style);
            }
        }
    }

    void TaroProgress::SetStrokeWidthAttribute() {
        if (attributes_->strokeWidth.has_value()) {
            progress_->SetStrokeWidth(attributes_->strokeWidth.value());
        }
    }

    void TaroProgress::SetActiveColorAttribute() {
        progress_->SetActiveColor(attributes_->activeColor.has_value() ? attributes_->activeColor.value() : 0xFF09BB07);
    }

    void TaroProgress::SetBackgroundColorAttribute() {
        progress_->SetBackgroundColor(attributes_->backgroundColor.has_value() ? attributes_->backgroundColor.value() : 0xFFEBEBEB);
    }

    void TaroProgress::SetActiveAttribute() {
        if (attributes_->active.has_value()) {
            TARO_LOG_WARN("TaroProgress", "active is not supported: 鸿蒙 CAPI 暂不支持 enableSmoothEffect 属性");
        }
    }

    void TaroProgress::SetActiveModeAttribute() {
        if (attributes_->activeMode.has_value()) {
            TARO_LOG_ERROR("TaroProgress", "activeMode is not supported");
        }
    }

    void TaroProgress::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);
        std::shared_ptr<TaroProgressNode> progressNode =
            std::static_pointer_cast<TaroProgressNode>(GetHeadRenderNode());
        switch (name) {
            case ATTRIBUTE_NAME::PERCENT: {
                GetPercentAttribute(value);
                if (is_init_) {
                    SetPercentAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::SHOW_INFO: {
                GetShowInfoAttribute(value);
                if (is_init_) {
                    SetShowInfoAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::BORDER_RADIUS: {
                GetBorderRadiusAttribute(value);
                if (is_init_) {
                    SetBorderRadiusAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::FONT_SIZE: {
                GetFontSizeAttribute(value);
                if (is_init_) {
                    SetFontSizeAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::STROKE_WIDTH: {
                GetStrokeWidthAttribute(value);
                if (is_init_) {
                    SetStrokeWidthAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::COLOR:
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
            case ATTRIBUTE_NAME::ACTIVE: {
                GetActiveAttribute(value);
                if (is_init_) {
                    SetActiveAttribute();
                }
                break;
            }
            case ATTRIBUTE_NAME::ACTIVE_MODE: {
                GetActiveModeAttribute(value);
                if (is_init_) {
                    SetActiveModeAttribute();
                }
                break;
            }
            default:
                break;
        }
    }
} // namespace TaroDOM
} // namespace TaroRuntime
