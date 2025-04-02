/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once
#include "base_property.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class AnimationPropFontSize : public AnimationPropBase {
        public:
        explicit AnimationPropFontSize(CSSProperty::Type prop_type);

        bool getSystemPropValue(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                CSSProperty::Type prop_type,
                                TaroAnimationPropValue& prop_value) const override;

        // 设置属性值到对应节点
        void setNodeProperty(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                             CSSProperty::Type prop_type,
                             const TaroAnimationPropValue& prop_value) const override;
    };

} // namespace TaroAnimate
} // namespace TaroRuntime
