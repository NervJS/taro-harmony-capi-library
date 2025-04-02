/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once
#include "base_property.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class AnimationPropComputedStyle : public AnimationPropBase {
        public:
        explicit AnimationPropComputedStyle(CSSProperty::Type prop_type);

        // 从computed_style上获取system信息
        bool getSystemPropValue(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                CSSProperty::Type prop_type,
                                TaroAnimationPropValue& prop_value) const override;

        // 获取动画值
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

        private:
        virtual std::string getComputedName(CSSProperty::Type prop_type) const;

        void setLengthValue(CSSProperty::Type prop_type,
                            std::shared_ptr<TaroDOM::TaroRenderNode>& node,
                            const Dimension& length_value) const;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
