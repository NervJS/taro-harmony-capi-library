//
// Created on 2024/4/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "ets_component.h"

#include "runtime/NativeNodeApi.h"
#include "runtime/dom/ark_nodes/custom.h"
#include "runtime/dom/ark_nodes/ets.h"
#include "runtime/dom/event/gesture/gesture_param.h"
#include "runtime/render.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroEtsComponent::TaroEtsComponent(napi_value node)
        : TaroElement(node) {
    }

    void TaroEtsComponent::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto custom_render_node = std::make_shared<TaroCustomNode>(element);
            auto ets_render_node = std::make_shared<TaroEtsNode>(element);
            custom_render_node->Build();
            ets_render_node->Build();
            custom_render_node->AppendChild(ets_render_node);
            SetHeadRenderNode(custom_render_node);
            SetFooterRenderNode(ets_render_node);
        }
    }

    void TaroEtsComponent::PostBuild() {
        auto parent = GetParentNode();
        if (!is_init_) {
            if (parent != nullptr && parent->is_init_) {
                // Note: 确定节点在父节点中的位置，挂载到父节点的RenderNode上
                AppendOrInsertIntoParent();
            }
            is_init_ = true;
        }
        SetAttributesToRenderNode();
    }

    void TaroEtsComponent::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        GetHeadRenderNode()->SetStyle(style_);
    }

    void TaroEtsComponent::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);

        auto footer_render = std::static_pointer_cast<TaroEtsNode>(GetFooterRenderNode());
        if (is_init_ && footer_render) {
            footer_render->Update();
        }
    }

    bool TaroEtsComponent::bindListenEvent(const std::string& event_name) {
        if (event_name == "click") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else if (event_name == "touchstart") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_START, event_name);
        } else if (event_name == "touchmove") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_MOVE, event_name);
        } else if (event_name == "touchend") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_END, event_name);
        } else if (event_name == "touchcancel") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_CANCEL, event_name);
        } else if (event_name == "longpress") {
            event_emitter_->registerEvent(TaroEvent::EventGeneratorType::Gesture, int(TaroEvent::TaroGestureType::LongPress), event_name);
            // event_emitter_->gesture()->registerLongPress(event_name);
        } else {
            return false;
        }
        return true;
    }
} // namespace TaroDOM
} // namespace TaroRuntime
