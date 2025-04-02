/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "animate.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/animation/animation_property.h"
#include "runtime/dom/animation/animator/animation.h"
#include "runtime/dom/animation/animator/animator.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroCSSTransition final : public TaroAnimate, public std::enable_shared_from_this<TaroCSSTransition> {
        public:
        explicit TaroCSSTransition(
            std::shared_ptr<TaroAnimationPropsController> props_controller,
            std::weak_ptr<TaroDOM::TaroRenderNode> node);
        ~TaroCSSTransition() = default;

        int bindTransition(const TaroCSSOM::TaroStylesheet::TransitionParam& param);

        void clear(CSSProperty::Type prop_type);

        void getRunningProp(std::vector<CSSProperty::Type>& props);

        void cancelTransition(CSSProperty::Type);

        void refreshAnimation();

        private:
        void clearAnimId(CSSProperty::Type prop_type, int32_t anim_id);
        void stopCallBack(int32_t anim_id);
        std::shared_ptr<TaroAnimation> createCurveAnimation(
            CSSProperty::Type prop_type, const TaroAnimationPropValue& begin_value,
            const TaroAnimationPropValue& end_value, std::shared_ptr<TaroCurve> curve,
            const TaroAnimationPropSetFun& set_fun);

        private:
        std::list<std::pair<CSSProperty::Type, std::shared_ptr<TaroAnimator>>> type_to_animator_;
    };

} // namespace TaroAnimate
} // namespace TaroRuntime
