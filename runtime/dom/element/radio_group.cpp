/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "radio_group.h"

#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "radio.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/dom/ark_nodes/flex.h"
#include "runtime/dom/element/radio.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroRadioGroup::TaroRadioGroup(napi_value node)
        : FormWidget(node),
          attributes_(std::make_unique<TaroFormAttributes>()) {}
    void TaroRadioGroup::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_radio_group = std::make_shared<TaroFlexNode>(element);
            render_radio_group->Build();
            SetRenderNode(render_radio_group);
        }
    }

    void TaroRadioGroup::SetAttributesToRenderNode() {
        FormWidget::SetAttributesToRenderNode();
        auto new_style =
            std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);
        std::shared_ptr<TaroFlexNode> render_radio_group =
            std::static_pointer_cast<TaroFlexNode>(GetHeadRenderNode());
        if (new_style->display.has_value()) {
            new_style->display.set(PropertyType::Display::Flex);
        }
        render_radio_group->SetStyle(new_style);
    }

    void TaroRadioGroup::onAppendChild(std::shared_ptr<TaroNode> child) {
        FormWidget::onAppendChild(child);
        auto element = std::static_pointer_cast<TaroElement>(child);
        if (element->tag_name_ == TAG_NAME::RADIO) {
            auto radio = std::static_pointer_cast<TaroRadio>(child);
            radio->SetParent(shared_from_this());
        }
    }

    void TaroRadioGroup::onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node) {
        FormWidget::onRemoveChild(child);
        auto element = std::static_pointer_cast<TaroElement>(child);
        if (element->tag_name_ == TAG_NAME::RADIO) {
            auto radio = std::static_pointer_cast<TaroRadio>(child);
            radio->SetParent(nullptr);
        }
    }

    bool TaroRadioGroup::bindListenEvent(const std::string& event_name) {
        if (event_name == "change") {
            event_emitter_->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, event_name);
        } else {
            return false;
        }
        return true;
    }

} // namespace TaroDOM
} // namespace TaroRuntime
