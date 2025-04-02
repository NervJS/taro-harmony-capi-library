/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "movable_view.h"

#include <boost/algorithm/string.hpp>

#include "runtime/NapiSetter.h"
#include "runtime/NativeGestureApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/event/event_helper.h"
#include "runtime/dom/event/event_user/user_event.h"
#include "runtime/dom/event/gesture/gesture_event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    static void MovableEventForPan(ArkUI_GestureEvent *event, void *data) {
        auto movable_view = static_cast<TaroMovableView *>(data);
        if (movable_view == nullptr) {
            TARO_LOG_ERROR("MovableEventForPan", "movable_view is nullptr");
            return;
        }
        ArkUI_GestureEventActionType type =
            OH_ArkUI_GestureEvent_GetActionType(event);
        switch (type) {
            case GESTURE_EVENT_ACTION_ACCEPT:
                movable_view->onPanStart(event);
                break;
            case GESTURE_EVENT_ACTION_UPDATE:
                movable_view->onPanUpdate(event);
                break;
            case GESTURE_EVENT_ACTION_END:
                movable_view->onPanEnd(event);
                break;
            default:
                break;
        }
    }

    static void MovableEventForPinch(ArkUI_GestureEvent *event, void *data) {
        auto movable_view = static_cast<TaroMovableView *>(data);
        if (movable_view == nullptr) {
            TARO_LOG_ERROR("MovableEventForPinch", "movable_view is nullptr");
            return;
        }
        ArkUI_GestureEventActionType type =
            OH_ArkUI_GestureEvent_GetActionType(event);
        switch (type) {
            case GESTURE_EVENT_ACTION_ACCEPT:
                movable_view->onPinchStart(event);
                break;
            case GESTURE_EVENT_ACTION_UPDATE:
                movable_view->onPinchUpdate(event);
                break;
            case GESTURE_EVENT_ACTION_END:
                movable_view->onPinchEnd(event);
                break;
            default:
                break;
        }
    }

    TaroMovableView::TaroMovableView(napi_value node)
        : TaroElement(node), attributes_(std::make_unique<TaroMovableViewAttributes>()), position_x(0), position_y(0), out_of_bounds(0), scale_value(1) {}

    TaroMovableView::~TaroMovableView() {
    }

    void TaroMovableView::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_movable_view = std::make_shared<TaroStackNode>(element);
            render_movable_view->Build();
            SetRenderNode(render_movable_view);
            bindSelfEventHandle();
            updatePanGesture(attributes_->direction.value_or("none"));
        }
    }

    void TaroMovableView::bindSelfEventHandle() {
        event_emitter_->registerEvent_NoCallBack(TaroEvent::EventGeneratorType::Gesture, (int)TaroEvent::TaroGestureType::Pan, "pan",
                                                 [this](TaroEvent::TaroEventPtr event) {
                                                     auto gesture_event = std::dynamic_pointer_cast<TaroEvent::GestureEventBase>(event);
                                                     MovableEventForPan(gesture_event->event_, this);
                                                     return 0;
                                                 });
        event_emitter_->registerEvent_NoCallBack(TaroEvent::EventGeneratorType::Gesture, (int)TaroEvent::TaroGestureType::Pinch, "pinch",
                                                 [this](TaroEvent::TaroEventPtr event) {
                                                     auto gesture_event = std::dynamic_pointer_cast<TaroEvent::GestureEventBase>(event);
                                                     MovableEventForPinch(gesture_event->event_, this);
                                                     return 0;
                                                 });
    }

    void TaroMovableView::updatePanGesture(std::string direction) {
        event_emitter_->unRegisterEventByName_NoCallBack("pan");
        // 根据入参的方向重新生成子手势
        ArkUI_GestureDirection dir = GESTURE_DIRECTION_ALL;
        if (direction == "horizontal") {
            dir = GESTURE_DIRECTION_HORIZONTAL;
        } else if (direction == "vertical") {
            dir = GESTURE_DIRECTION_VERTICAL;
        } else if (direction == "disabled") {
            dir = GESTURE_DIRECTION_NONE;
        }

        TaroEvent::GestureParam_Pan pan_param;
        pan_param.directions = dir;
        event_emitter_->registerEvent_NoCallBack(TaroEvent::EventGeneratorType::Gesture, (int)TaroEvent::TaroGestureType::Pan, "pan", [this](TaroEvent::TaroEventPtr event) {
                                                 auto gesture_event = std::dynamic_pointer_cast<TaroEvent::GestureEventBase>(event);
                                                 MovableEventForPan(gesture_event->event_, this);
                                                 return 0; }, nullptr, &pan_param);
    }

    bool TaroMovableView::bindListenEvent(const std::string &event_name) {
        if (event_name == "touchstart") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_START, event_name);
        } else if (event_name == "touchmove") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_MOVE, event_name);
        } else if (event_name == "touchend") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_END, event_name);
        } else {
            static std::set<std::string> s_listen_keys = {"dragstart", "htouchmove", "vtouchmove", "change", "changeend", "dragend", "scale"};
            if (s_listen_keys.count(event_name) > 0) {
                event_emitter_->registerEvent(TaroEvent::EventGeneratorType::UserDefine, -1, event_name);
                return true;
            }
            return false;
        }
        return true;
    }

    void TaroMovableView::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode,
                                       ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);
        NapiGetter getter(value);
        std::shared_ptr<TaroStackNode> render_node =
            std::static_pointer_cast<TaroStackNode>(GetHeadRenderNode());
        switch (name) {
            case ATTRIBUTE_NAME::DIRECTION: {
                attributes_->direction = getter.String();
                if (!attributes_->direction.has_value()) {
                    attributes_->direction.set("none");
                }
                updatePanGesture(attributes_->direction.value());
                break;
            }
            case ATTRIBUTE_NAME::OUT_OF_BOUNDS: {
                attributes_->outOfBounds = getter.BoolNull();
                if (!attributes_->outOfBounds.has_value()) {
                    attributes_->outOfBounds.set(false);
                }
                break;
            }
            case ATTRIBUTE_NAME::POSITION_X: {
                auto x = TaroCSSOM::TaroStylesheet::getFloat(getter);
                if (x.has_value()) {
                    position_x = x.value();
                }
                if (is_init_) {
                    render_node->SetTranslate(position_x, 0, 0);
                }
                break;
            }
            case ATTRIBUTE_NAME::POSITION_Y: {
                auto y = TaroCSSOM::TaroStylesheet::getFloat(getter);
                if (y.has_value()) {
                    position_y = y.value();
                }
                if (is_init_) {
                    render_node->SetTranslate(0, position_y, 0);
                }
                break;
            }
            case ATTRIBUTE_NAME::SCALE: {
                attributes_->scale = getter.BoolNull();
                if (!attributes_->scale.has_value()) {
                    attributes_->scale.set(false);
                }
                break;
            }
            case ATTRIBUTE_NAME::SCALE_MIN: {
                attributes_->scaleMin = TaroCSSOM::TaroStylesheet::getFloat(getter);
                if (!attributes_->scaleMin.has_value()) {
                    attributes_->scaleMin.set(0.5);
                }
                break;
            }
            case ATTRIBUTE_NAME::SCALE_MAX: {
                attributes_->scaleMax = TaroCSSOM::TaroStylesheet::getFloat(getter);
                if (!attributes_->scaleMax.has_value()) {
                    attributes_->scaleMax.set(10);
                }
                break;
            }
            case ATTRIBUTE_NAME::SCALE_VALUE: {
                auto scaleValue = TaroCSSOM::TaroStylesheet::getFloat(getter);
                if (scaleValue.has_value()) {
                    scale_value = scaleValue.value();
                }
                if (is_init_ && attributes_->scale.value()) {
                    render_node->SetScale(scale_value, scale_value);
                }
                break;
            }
            default:
                break;
        }
    }

    void TaroMovableView::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();

        std::shared_ptr<TaroStackNode> render_node =
            std::static_pointer_cast<TaroStackNode>(GetHeadRenderNode());
        render_node->SetIsInline(true);
        render_node->SetStyle(style_);
        render_node->SetTranslate(position_x, position_y, 0);
        render_node->SetScale(scale_value, scale_value);
    }

    float TaroMovableView::calcPosition(float position, float start, float end) {
        if (position <= end && position >= start) {
            return position;
        }
        if (position < start) {
            return start;
        }
        return end;
    }

    void TaroMovableView::checkPositionBoundary() {
        auto areaWidthEnd = area_width - self_width * scale_value;
        auto areaHeightEnd = area_height - self_height * scale_value;

        auto incrementWidth = (scale_value - 1) * self_width;
        auto incrementHeight = (scale_value - 1) * self_height;

        position_x = calcPosition(position_x, incrementWidth * 0.5,
                                  areaWidthEnd + incrementWidth * 0.5);
        position_y = calcPosition(position_y, incrementHeight * 0.5,
                                  areaHeightEnd + incrementHeight * 0.5);
    }

    void TaroMovableView::onPanStart(ArkUI_GestureEvent *event) {
        if (disabled_) {
            return;
        }
        position_temp_x = position_x;
        position_temp_y = position_y;

        // Get the calculated properties of the parent component in Yoga.
        std::shared_ptr<TaroStackNode> parentNode =
            std::static_pointer_cast<TaroStackNode>(this->GetParentNode()->GetHeadRenderNode());
        area_width = parentNode->layoutDiffer_.computed_style_.width;
        area_height = parentNode->layoutDiffer_.computed_style_.height;

        // Get the calculated properties of itself in Yoga.
        std::shared_ptr<TaroStackNode> viewNode =
            std::static_pointer_cast<TaroStackNode>(GetHeadRenderNode());
        self_width = viewNode->layoutDiffer_.computed_style_.width;
        self_height = viewNode->layoutDiffer_.computed_style_.height;
        if (attributes_->outOfBounds.value()) {
            out_of_bounds = self_width / 3.0;
        }
        callJSFunc("dragstart");
        std::string direction = attributes_->direction.value();
        if (direction == "horizontal") {
            callJSFunc("htouchmove");
        } else if (direction == "vertical") {
            callJSFunc("vtouchmove");
        }
    }

    void TaroMovableView::onPanUpdate(ArkUI_GestureEvent *event) {
        std::string direction = attributes_->direction.value();
        if (disabled_ || direction == "none") {
            return;
        }
        auto offsetX = OH_ArkUI_PanGesture_GetOffsetX(event); // unit:px
        auto offsetY = OH_ArkUI_PanGesture_GetOffsetY(event); // unit:px
        offsetX = px2Vp(offsetX);
        offsetY = px2Vp(offsetY);

        auto areaWidthEnd = area_width - self_width * scale_value;
        auto areaHeightEnd = area_height - self_height * scale_value;

        auto incrementWidth = (scale_value - 1) * self_width;
        auto incrementHeight = (scale_value - 1) * self_height;

        auto x = position_temp_x;
        auto y = position_temp_y;
        if (direction == "all" || direction == "horizontal") {
            auto nextX = position_temp_x + offsetX * scale_value;
            x = calcPosition(
                nextX,
                incrementWidth * 0.5 - out_of_bounds,
                areaWidthEnd + incrementWidth * 0.5 + out_of_bounds);
        }
        if (direction == "all" || direction == "vertical") {
            auto nextY = position_temp_y + offsetY * scale_value;
            y = calcPosition(
                nextY,
                incrementHeight * 0.5 - out_of_bounds,
                areaHeightEnd + incrementHeight * 0.5 + out_of_bounds);
        }
        std::shared_ptr<TaroStackNode> render_node =
            std::static_pointer_cast<TaroStackNode>(GetHeadRenderNode());
        render_node->SetTranslate(x, y, 0);
        position_x = x;
        position_y = y;
        callJSFunc("change");
    }

    void TaroMovableView::onPanEnd(ArkUI_GestureEvent *event) {
        if (disabled_) {
            return;
        }
        checkPositionBoundary();
        callJSFunc("changeend");
        callJSFunc("dragend");
    }

    void TaroMovableView::onPinchStart(ArkUI_GestureEvent *event) {
        scale_value_temp = scale_value;
    }

    bool TaroMovableView::checkScaleValueInBounds(float currentScale) {
        if (currentScale >= attributes_->scaleMin.value() && currentScale <= attributes_->scaleMax.value()) {
            return true;
        }
        return false;
    }

    void TaroMovableView::onPinchUpdate(ArkUI_GestureEvent *event) {
        // TODO 双指缩放时缩放范围有问题，需要调整
        if (disabled_ || !attributes_->scale.value()) {
            return;
        }
        auto pinchValue = OH_ArkUI_PinchGesture_GetScale(event);
        if (checkScaleValueInBounds(pinchValue)) {
            scale_value = scale_value_temp * pinchValue;
            std::shared_ptr<TaroStackNode> render_node =
                std::static_pointer_cast<TaroStackNode>(GetHeadRenderNode());
            render_node->SetScale(scale_value, scale_value);
            callJSFunc("scale");
        }
    }

    void TaroMovableView::onPinchEnd(ArkUI_GestureEvent *event) {}

    void TaroMovableView::serializeForPan(napi_value &ret_obj) {
        NapiSetter::SetProperty(ret_obj, "x", position_x);
        NapiSetter::SetProperty(ret_obj, "y", position_y);
        NapiSetter::SetProperty(ret_obj, "source", "touch");
    }

    void TaroMovableView::serializeForPinch(napi_value &ret_obj) {
        NapiSetter::SetProperty(ret_obj, "x", position_x);
        NapiSetter::SetProperty(ret_obj, "y", position_y);
        NapiSetter::SetProperty(ret_obj, "scale", scale_value);
    }

    void TaroMovableView::callJSFunc(std::string eventType) {
        auto event = std::make_shared<TaroEvent::UserEventBase>(eventType);
        // detail 填充
        napi_value js_detail = event->detail();
        if (eventType == "change" || eventType == "changeend") {
            serializeForPan(js_detail);
        } else if (eventType == "scale") {
            serializeForPinch(js_detail);
        }
        getEventEmitter()->triggerEvents(event);
    }

    void TaroMovableView::Build(std::shared_ptr<TaroElement> &reuse_element) {
        std::shared_ptr<TaroStackNode> new_node = std::dynamic_pointer_cast<TaroStackNode>(GetHeadRenderNode());
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        auto ark_handle = reuse_element->GetNodeHandle();
        if (new_node == nullptr && ark_handle == nullptr) {
            is_init_ = false;
            Build();
            return;
        }
        if (new_node == nullptr) {
            new_node = std::make_shared<TaroStackNode>(element);
            new_node->SetArkUINodeHandle(ark_handle);
            SetRenderNode(new_node);
            new_node->UpdateDifferOldStyleFromElement(reuse_element);
            reuse_element->event_emitter_->clearNodeEvent(ark_handle);
            reuse_element->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
            // bindSelfEventHandle();
            updatePanGesture(attributes_->direction.value_or("none"));
            return;
        }
        if (ark_handle == nullptr) {
            auto parent = new_node->parent_ref_.lock();
            if (parent) {
                new_node->Build();
                parent->UpdateChild(new_node);
                // bindSelfEventHandle();
                updatePanGesture(attributes_->direction.value_or("none"));
                new_node->ClearDifferOldStyleFromElement();
            }
            return;
        }
        new_node->SetArkUINodeHandle(ark_handle);
        new_node->UpdateDifferOldStyleFromElement(reuse_element);
        reuse_element->event_emitter_->clearNodeEvent(ark_handle);
        reuse_element->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
        // bindSelfEventHandle();
        updatePanGesture(attributes_->direction.value_or("none"));
    }

} // namespace TaroDOM
} // namespace TaroRuntime