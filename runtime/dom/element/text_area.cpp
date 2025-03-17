//
// Created on 2024/8/12.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "text_area.h"

#include <boost/algorithm/string.hpp>

#include "arkjs/ArkJS.h"
#include "helper/TaroLog.h"
#include "runtime/dom/ark_nodes/text_area.h"
#include "runtime/dom/event/event_hm/event_types/event_change.h"
#include "runtime/dom/event/event_visible.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroTextArea::TaroTextArea(napi_value node)
        : TaroInput(node) {
        attributes_ = std::make_unique<TaroTextAreaAttributes>();
    }

    void TaroTextArea::Build() {
        if (!is_init_) {
            // create render node
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_textarea = std::make_shared<TaroTextAreaNode>(element);
            render_textarea->Build();

            SetRenderNode(render_textarea);
        }
    }

    void TaroTextArea::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        switch (name) {
            case ATTRIBUTE_NAME::AUTO_HEIGHT: {
                GetAutoHeightAttribute(value);
                if (is_init_) {
                    SetAutoHeightAttribute();
                }
                break;
            }
            default:
                TaroInput::SetAttribute(renderNode, name, value);
                break;
        }
    }

    void TaroTextArea::SetAutoHeightAttribute() {
        std::shared_ptr<TaroTextAreaNode> render_textarea = std::static_pointer_cast<TaroTextAreaNode>(GetHeadRenderNode());
        if (attributes_->autoHeight.has_value()) {
            render_textarea->SetAutoHeight(attributes_->autoHeight.value());
        }
    }

    void TaroTextArea::GetAutoHeightAttribute(const napi_value& value) {
        ArkJS arkJs(NativeNodeApi::env);
        if (value && arkJs.getType(value) == napi_boolean) {
            attributes_->autoHeight.set(arkJs.getBoolean(value));
        }
    }

    void TaroTextArea::GetNodeAttributes() {
        TaroInput::GetNodeAttributes();

        GetAutoHeightAttribute(GetAttributeNodeValue("autoHeight"));
    }

    void TaroTextArea::SetAttributesToRenderNode() {
        FormWidget::SetAttributesToRenderNode();

        std::shared_ptr<TaroTextInputNode> render_textarea = std::static_pointer_cast<TaroTextInputNode>(GetHeadRenderNode());

        SetPasswordAttribute();
        SetTypeAttribute();
        SetConfirmTypeAttribute();
        SetMaxLengthTypeAttribute();
        SetFocusTypeAttribute();
        SetPlaceholderAttribute();
        SetPlaceholderStyleAttribute();
        SetPlaceholderTextColorAttribute();
        SetAutoHeightAttribute();

        auto overrideStyle = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
        overrideStyle->paddingTop.set(style_->paddingTop.value_or(0));
        overrideStyle->paddingRight.set(style_->paddingRight.value_or(0));
        overrideStyle->paddingBottom.set(style_->paddingBottom.value_or(0));
        overrideStyle->paddingLeft.set(style_->paddingLeft.value_or(0));
        overrideStyle->borderTopLeftRadius.set(style_->borderTopLeftRadius.value_or(0));
        overrideStyle->borderTopRightRadius.set(style_->borderTopRightRadius.value_or(0));
        overrideStyle->borderBottomRightRadius.set(style_->borderBottomRightRadius.value_or(0));
        overrideStyle->borderBottomLeftRadius.set(style_->borderBottomLeftRadius.value_or(0));

        // 默认值
        overrideStyle->width.set(style_->width.value_or(Dimension(1, DimensionUnit::PERCENT)));
        overrideStyle->height.set(style_->height.value_or(Dimension(40, DimensionUnit::VP)));
        overrideStyle->color.set(disabled_ ? 0xFF545454 : style_->color.value_or(0xFF000000));

        render_textarea->SetStyle(overrideStyle);
    }

    int TaroTextArea::GetInputType(const std::string& type) {
        if (type == "number" || type == "numberpad") {
            return ArkUI_TextAreaType::ARKUI_TEXTAREA_TYPE_NUMBER;
        }
        return ArkUI_TextAreaType::ARKUI_TEXTAREA_TYPE_NORMAL;
    }
} // namespace TaroDOM
} // namespace TaroRuntime
