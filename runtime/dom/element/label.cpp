/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "label.h"

#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/dom/ark_nodes/flex.h"
#include "runtime/dom/element/button.h"
#include "runtime/dom/element/checkbox.h"
#include "runtime/dom/element/form/group_manager.h"
#include "runtime/dom/element/radio.h"
#include "runtime/dom/element/text.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroLabel::TaroLabel(napi_value node)
        : TaroElement(node) {}
    void TaroLabel::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_label = std::make_shared<TaroFlexNode>(element);
            render_label->Build();
            SetRenderNode(render_label);
        }
        HandleEvents();
    }

    void TaroLabel::HandleEvents() {
        auto onClick = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value &) -> int {
            if (callback_) {
                callback_(nullptr);
            }
            return 0;
        };

        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_CLICK, "click", onClick, GetHeadRenderNode()->GetArkUINodeHandle());
    }

    void TaroLabel::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        auto new_style =
            std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
        std::shared_ptr<TaroFlexNode> render_checkbox =
            std::static_pointer_cast<TaroFlexNode>(GetHeadRenderNode());
        if (!new_style->display.has_value()) {
            new_style->display.set(PropertyType::Display::Flex);
        }
        render_checkbox->SetStyle(new_style);

        // 处理无for属性的label
        if (!hasFor) {
            auto node = GetFirstNode();
            if (node != nullptr) {
                auto element = std::static_pointer_cast<TaroElement>(node);
                LinkElement(element);
            }
        }
    }

    void TaroLabel::LinkElement(std::shared_ptr<TaroElement> forNode) {
        auto render_label =
            std::static_pointer_cast<TaroFlexNode>(GetHeadRenderNode());
        switch (forNode->tag_name_) {
            case TAG_NAME::CHECKBOX: {
                auto checkbox = std::static_pointer_cast<TaroCheckbox>(forNode);
                RegisterEventCallback(TAG_NAME::CHECKBOX, [checkbox](std::shared_ptr<void>) {
                    checkbox->ChangeSelectState();
                });
                break;
            }
            case TAG_NAME::RADIO: {
                auto radio = std::static_pointer_cast<TaroRadio>(forNode);
                RegisterEventCallback(TAG_NAME::RADIO, [radio](std::shared_ptr<void>) {
                    radio->ChangeSelectState();
                });
                break;
            }
            default:
                break;
        }
    }

    std::shared_ptr<TaroNode> TaroLabel::GetElementById(const std::string &id) {
        if (context_) {
            if (context_->group_manger_) {
                return context_->group_manger_->GetNodeById(id);
            }
        }
        return nullptr;
    }

    std::shared_ptr<TaroNode> TaroLabel::GetFirstNode() {
        for (auto &iter : child_nodes_) {
            auto temp = std::static_pointer_cast<TaroElement>(iter);
            if (temp->tag_name_ == TAG_NAME::RADIO || temp->tag_name_ == TAG_NAME::CHECKBOX || temp->tag_name_ == TAG_NAME::BUTTON) {
                return temp;
            }
        }
        return nullptr;
    }

    void TaroLabel::ProcessForNode(napi_value value) {
        NapiGetter getter(value);
        TaroHelper::Optional<std::string> id = getter.String();
        auto forNode = GetElementById(id.value());
        // 查找到节点才处理，查找不到再查询是否存在内部节点
        if (forNode != nullptr) {
            hasFor = true;
            auto element = std::static_pointer_cast<TaroElement>(forNode);
            LinkElement(element);
        }
    }

    void TaroLabel::RegisterEventCallback(TAG_NAME nodeName, std::function<void(std::shared_ptr<void>)> callback) {
        callback_ = callback;
    }

    void TaroLabel::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroElement::SetAttribute(renderNode, name, value);

        switch (name) {
            case ATTRIBUTE_NAME::LABEL_FOR:
                ProcessForNode(value);
                break;
            default:
                break;
        }
    }
} // namespace TaroDOM
} // namespace TaroRuntime
