/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include <gtest/gtest.h>

#define private public
#define protected public

#include "runtime/dom/animation/animator/utils.h"
#include "runtime/dom/animation/node_animations.h"
#include "runtime/dom/animation/trigger/frame_trigger_master.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroAnimate {

    // TEST(TaroAnimation, css_animation) {
    //     TARO_LOG_DEBUG("TaroAnimation", "start testCSSAnimation");
    //     napi_value node = nullptr;
    //     auto tar_node = std::make_shared<TaroDOM::TaroElement>(node);
    //     auto a = new std::shared_ptr<TaroDOM::TaroElement>();
    //     *a = tar_node;
    //     std::shared_ptr<TaroNodeAnimations> node_animation_ =
    //     std::make_shared<TaroNodeAnimations>(tar_node); TaroCSSAnimationParams
    //     animation_params; animation_params.name_ = "test_css_animation";
    //     animation_params.delay_ = 1000;
    //     animation_params.duration_ = 10000;
    //     animation_params.direction_ = "normal";
    //     animation_params.fill_mode_ = "both";
    //     animation_params.iteration_count_ = 1;
    //     TaroCSSAnimationParam_KeyFrame keyframe;
    //     keyframe.percent_ = 0.1f;
    //     keyframe.percent_params_.emplace_back(std::make_pair(CSSProperty::Type::Height,
    //     18.0f));
    //     keyframe.percent_params_.emplace_back(std::make_pair(CSSProperty::Type::Width,
    //     18.0f)); animation_params.key_frames_.emplace_back(keyframe);
    //     keyframe.percent_params_.clear();
    //     keyframe.percent_ = 0.5f;
    //     keyframe.percent_params_.emplace_back(std::make_pair(CSSProperty::Type::Height,
    //     38.0f)); animation_params.key_frames_.emplace_back(keyframe);
    //     keyframe.percent_ = 1.0f;
    //     keyframe.percent_params_.emplace_back(std::make_pair(CSSProperty::Type::Height,
    //     68.0f)); animation_params.key_frames_.emplace_back(keyframe);
    //     animation_params.sys_params_[CSSProperty::Type::Height] = 5.0f;
    //     animation_params.sys_params_[CSSProperty::Type::Width] = 5.0f;
    //     node_animation_->bindAnimation(animation_params);
    //     EXPECT_NE(node_animation_->css_animation_, nullptr);
    //     EXPECT_EQ(node_animation_->css_animation_->animators_.size(), 1);
    //     TARO_LOG_DEBUG("TaroAnimation", "bind animation ok");
    //     uint64_t current_time = TaroUtils::getCurrentMsTime();
    //     node_animation_->tick(current_time+500);
    //     EXPECT_EQ(node_animation_->props_controller_->props_.size(), 2);
    //
    //     TaroAnimationPropsInfo height_props =
    //     node_animation_->props_controller_->props_[CSSProperty::Type::Height];
    //     EXPECT_EQ(height_props.animate_type, TaroAnimateType::CSS_ANIMATION);
    //     EXPECT_NE(height_props.animator_id_, 0);
    //     float height_val = *std::get_if<float>(&height_props.prop_value_);
    //     EXPECT_FLOAT_EQ(height_val, 5.0f);
    //
    //     node_animation_->tick(current_time + 1500);
    //
    //     height_props =
    //     node_animation_->props_controller_->props_[CSSProperty::Type::Height];
    //     EXPECT_EQ(height_props.animate_type, TaroAnimateType::CSS_ANIMATION);
    //     EXPECT_NE(height_props.animator_id_, 0);
    //     height_val = *std::get_if<float>(&height_props.prop_value_);
    //     EXPECT_GT(height_val, 5.0f);
    //
    //     node_animation_->tick(current_time + 15000);
    //     height_props =
    //     node_animation_->props_controller_->props_[CSSProperty::Type::Height];
    //     EXPECT_EQ(height_props.animate_type, TaroAnimateType::CSS_ANIMATION);
    //     EXPECT_NE(height_props.animator_id_, 0);
    //     height_val = *std::get_if<float>(&height_props.prop_value_);
    //     EXPECT_FLOAT_EQ(height_val, 68.0f);
    //     TARO_LOG_DEBUG("TaroAnimation", "height_props.prop_value_:%{public}f",
    //     height_val); node_animation_->tick(current_time + 16000);
    //     EXPECT_EQ(node_animation_->props_controller_->props_.size(), 0);
    //     TARO_LOG_DEBUG("TaroAnimation", "height_props.prop_value_:%{public}lu",
    //     node_animation_->props_controller_->props_.size());
    //
    //     TARO_LOG_DEBUG("TaroAnimation", "end testCSSAnimation");
    // }
} // namespace TaroAnimate
} // namespace TaroRuntime
