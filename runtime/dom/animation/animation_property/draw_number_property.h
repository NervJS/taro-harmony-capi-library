/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once
#include "base_property.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class AnimationPropDrawNumber : public AnimationPropBase {
        public:
        explicit AnimationPropDrawNumber(CSSProperty::Type prop_type);

        // 从computed_style上获取sytem信息
        bool getSystemPropValue(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                CSSProperty::Type prop_type,
                                TaroAnimationPropValue& prop_value) const override;

        bool getAnimationPropValue(
            std::shared_ptr<TaroDOM::TaroRenderNode> node,
            CSSProperty::Type prop_type,
            const TaroCSSOM::TaroStylesheet::KeyframeValue& n_val,
            const TaroAnimationPropValue& sys_value,
            TaroAnimationPropValue& prop_value) const override;

        // 设置属性值到对应节点
        void setNodeProperty(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                             CSSProperty::Type prop_type,
                             const TaroAnimationPropValue& prop_value) const override;

        // 设置动画初始值到节点
        void setKeyframeToNode(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                               CSSProperty::Type prop_type,
                               const TaroCSSOM::TaroStylesheet::KeyframeValue& keyframe) const override;

        // 设置style值到node节点
        void resetStyleSheetProp(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                 CSSProperty::Type prop_type,
                                 TaroDOM::StylesheetRef style) const override;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
