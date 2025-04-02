/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/animation/animation_property.h"
#include "runtime/dom/animation/animator/evaluators.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class AnimationPropBase {
        public:
        explicit AnimationPropBase(CSSProperty::Type prop_type,
                                   const std::string &anim_type_name);

        // check类型是否匹配
        bool checkPropertyType(CSSProperty::Type prop_type,
                               const TaroAnimationPropValue &prop_value) const;

        // 从yoga或者style上获取TaroAnimationPropValue信息
        virtual bool getSystemPropValue(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                        CSSProperty::Type prop_type,
                                        TaroAnimationPropValue &prop_value) const = 0;

        // 从KeyframeValue上获取格式化的属性信息-单值
        virtual bool getAnimationPropValue(
            std::shared_ptr<TaroDOM::TaroRenderNode> node,
            CSSProperty::Type prop_type,
            const TaroCSSOM::TaroStylesheet::KeyframeValue &n_val,
            const TaroAnimationPropValue &sys_value,
            TaroAnimationPropValue &prop_value) const;

        // 设置属性值到对应节点
        virtual void setNodeProperty(
            std::shared_ptr<TaroDOM::TaroRenderNode> node,
            CSSProperty::Type prop_type,
            const TaroAnimationPropValue &prop_value) const = 0;

        // 设置动画初始值到节点
        virtual void setKeyframeToNode(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                       CSSProperty::Type prop_type,
                                       const TaroCSSOM::TaroStylesheet::KeyframeValue &keyframe) const {};

        // 设置style值到node节点
        virtual void resetStyleSheetProp(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                         CSSProperty::Type prop_type,
                                         TaroDOM::StylesheetRef style) const {};

        std::shared_ptr<TaroEvaluator> getEvaluator() const;

        protected:
        // 获取percent值
        bool getFloatPercentValue(const double percent,
                                  const TaroAnimationPropValue &sys_value,
                                  TaroAnimationPropValue &prop_value) const;

        bool getFloatPercentValue(const double percent, double sys_value,
                                  double &prop_value) const;

        bool getFloatPropValue(CSSProperty::Type prop_type,
                               const Dimension &n_val,
                               const double sys_value, double &prop_value) const;

        CSSProperty::Type prop_type_;
        std::string anim_type_name_; // 对应TaroAnimationPropValue里面对应的类型名称

        std::shared_ptr<TaroEvaluator> evaluator_ = nullptr;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
