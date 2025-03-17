#include <gtest/gtest.h>

#define private public
#define protected public

#include "runtime/dom/animation/animator/utils.h"
#include "runtime/dom/animation/node_animations.h"
// #include "runtime/dom/animation/trigger/frame_trigger_master.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroAnimate {
    //
    // TEST(TaroAnimation, css_transition) {
    //     TARO_LOG_DEBUG("TaroAnimation", "start testCSSTransition");
    //     napi_value node = nullptr;
    //     auto tar_node = std::make_shared<TaroDOM::TaroElement>(node);
    //     auto a = new std::shared_ptr<TaroDOM::TaroElement>();
    //     *a = tar_node;
    //     std::shared_ptr<TaroNodeAnimations> node_animation_ =
    //     std::make_shared<TaroNodeAnimations>(tar_node); TaroCSSTransitionParams
    //     transition_param; TaroCSSTransitionParams_Property prop_param;
    //     prop_param.prop_type_ = CSSProperty::Type::Height;
    //     prop_param.start_value_ = 5.0f;
    //     prop_param.end_value_ = 100.f;
    //     prop_param.delay_ = 1000;
    //     prop_param.duration_ = 10000;
    //     prop_param.timing_function_ = "ease-in";
    //     transition_param.properties_.push_back(prop_param);
    //
    //     prop_param.prop_type_ = CSSProperty::Type::Width;
    //     prop_param.start_value_ = 5.0f;
    //     prop_param.end_value_ = 100.f;
    //     prop_param.delay_ = 1000;
    //     prop_param.duration_ = 10000;
    //     prop_param.timing_function_ = "ease-in-out";
    //     transition_param.properties_.push_back(prop_param);
    //
    //     node_animation_->bindTransition(transition_param);
    //     EXPECT_NE(node_animation_->css_transition_, nullptr);
    //
    //     EXPECT_EQ(node_animation_->css_transition_->animators_.size(), 2);
    //     TARO_LOG_DEBUG("TaroAnimation", "bind transition ok");
    //     uint64_t current_time = TaroUtils::getCurrentMsTime();
    //     //TaroFrameTriggerMaster::instance()->onFrame(current_time + 500);
    //     node_animation_->tick(current_time + 500);
    //     EXPECT_EQ(node_animation_->props_controller_->props_.size(), 0);
    //
    //     //TaroFrameTriggerMaster::instance()->onFrame(current_time + 1500);
    //     node_animation_->tick(current_time + 1500);
    //     TARO_LOG_DEBUG("TaroAnimation", "props size:%{public}lu",
    //     node_animation_->props_controller_->props_.size()); auto height_props =
    //     node_animation_->props_controller_->props_[CSSProperty::Type::Height];
    //     EXPECT_EQ(height_props.animate_type, TaroAnimateType::CSS_TRANSITION);
    //     EXPECT_NE(height_props.animator_id_, 0);
    //     float height_val = *std::get_if<float>(&height_props.prop_value_);
    //     TARO_LOG_DEBUG("TaroAnimation", "prop_value:%{public}f", height_val);
    //
    //     EXPECT_GT(height_val, 5.0f);
    //     TARO_LOG_DEBUG("TaroAnimation", "transition size:%{public}lu",
    //     node_animation_->props_controller_->props_.size());
    //
    //      EXPECT_EQ(node_animation_->props_controller_->props_.size(), 2);
    //
    //     node_animation_->tick(current_time + 15000);
    //     //TaroFrameTriggerMaster::instance()->onFrame(current_time + 15000);
    //     height_props =
    //     node_animation_->props_controller_->props_[CSSProperty::Type::Height];
    //     EXPECT_EQ(height_props.animate_type, TaroAnimateType::CSS_TRANSITION);
    //     EXPECT_NE(height_props.animator_id_, 0);
    //     height_val = *std::get_if<float>(&height_props.prop_value_);
    //     EXPECT_FLOAT_EQ(height_val, 100.0f);
    //     TARO_LOG_DEBUG("TaroAnimation", "height_props.prop_value_:%{public}f",
    //     height_val);
    //     // TaroFrameTriggerMaster::instance()->onFrame(current_time + 16000);
    //     node_animation_->tick(current_time + 16000);
    //     EXPECT_EQ(node_animation_->props_controller_->props_.size(), 0);
    //     TARO_LOG_DEBUG("TaroAnimation", "height_props.prop_value_:%{public}lu",
    //                    node_animation_->props_controller_->props_.size());
    //
    //     TARO_LOG_DEBUG("TaroAnimation", "end testCSSTransition");
    // }
} // namespace TaroAnimate
} // namespace TaroRuntime
