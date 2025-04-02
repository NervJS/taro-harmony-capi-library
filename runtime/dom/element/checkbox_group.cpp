/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "checkbox_group.h"

#include "runtime/NapiSetter.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/dom/ark_nodes/flex.h"
#include "runtime/dom/element/checkbox.h"
#include "runtime/dom/event/event_helper.h"
#include "runtime/dom/event/event_user/user_event.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroCheckboxGroup::TaroCheckboxGroup(napi_value node)
        : FormWidget(node),
          attributes_(std::make_unique<TaroFormAttributes>()) {
    }

    void TaroCheckboxGroup::callEventHandler(std::string value, bool selected) {
        if (selected) {
            list_.emplace(value);
        } else {
            list_.erase(value);
        }

        auto event = std::make_shared<TaroEvent::UserEventBase>("change");
        napi_value& js_detail = event->detail();
        NapiSetter::SetProperty(js_detail, "value", list_);
        getEventEmitter()->triggerEvents(event);
    }
    bool TaroCheckboxGroup::bindListenEvent(const std::string& event_name) {
        if (event_name == "change") {
            event_emitter_->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, event_name);
        } else {
            return false;
        }
        return true;
    }
    void TaroCheckboxGroup::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_checkbox_group = std::make_shared<TaroFlexNode>(element);
            render_checkbox_group->Build();
            SetRenderNode(render_checkbox_group);
        }
    }

    void TaroCheckboxGroup::SetAttributesToRenderNode() {
        FormWidget::SetAttributesToRenderNode();
        auto new_style =
            std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
        std::shared_ptr<TaroFlexNode> render_checkbox =
            std::static_pointer_cast<TaroFlexNode>(GetHeadRenderNode());

        if (new_style->display.has_value()) {
            new_style->display.set(PropertyType::Display::Flex);
        }

        new_style->alignItems.set(ArkUI_ItemAlignment::ARKUI_ITEM_ALIGNMENT_CENTER);
        render_checkbox->SetStyle(new_style);
    }
    void TaroCheckboxGroup::onAppendChild(std::shared_ptr<TaroNode> child) {
        FormWidget::onAppendChild(child);
        auto element = std::static_pointer_cast<TaroElement>(child);
        if (element->tag_name_ == TAG_NAME::CHECKBOX) {
            auto checkbox = std::static_pointer_cast<TaroCheckbox>(child);
            checkbox->SetParent(shared_from_this());
        }
    }

    void TaroCheckboxGroup::onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node) {
        FormWidget::onRemoveChild(child);
        auto element = std::static_pointer_cast<TaroElement>(child);
        if (element->tag_name_ == TAG_NAME::CHECKBOX) {
            auto checkbox = std::static_pointer_cast<TaroCheckbox>(child);
            checkbox->SetParent(nullptr);
        }
    }
} // namespace TaroDOM
} // namespace TaroRuntime
