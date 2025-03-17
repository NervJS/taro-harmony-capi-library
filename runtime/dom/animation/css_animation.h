#pragma once

#include "runtime/cssom/stylesheet/css_keyframes.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/animation/animate.h"
#include "runtime/dom/animation/animation_property.h"
#include "runtime/dom/animation/animator/animation.h"
#include "runtime/dom/animation/animator/animator.h"
#include "runtime/dom/animation/props_controller.h"

namespace TaroRuntime {
namespace TaroAnimate {

    class TaroCSSAnimation final : public TaroAnimate {
        public:
        explicit TaroCSSAnimation(
            std::shared_ptr<TaroAnimationPropsController> props_controller,
            std::weak_ptr<TaroDOM::TaroRenderNode> node);
        ~TaroCSSAnimation() = default;

        int bindAnimation(const TaroCSSOM::TaroStylesheet::AnimationOption& option, const std::string& router);

        // 重新刷新动画内容
        int refreshAnimation(const TaroCSSOM::TaroStylesheet::AnimationOption& option, TaroDOM::StylesheetRef style, const std::string& router);

        void cancelAnimation(const std::string& anim_name);

        bool hasAnimation(const std::string& anim_name);

        void getNames(std::vector<std::string>& names);

        bool isActivePropType(CSSProperty::Type prop_type);

        private:
        void clearAnimId(CSSProperty::Type prop_type, int32_t anim_id);
        void stopCallBack(int32_t anim_id);
        void iterCallBack(int32_t anim_id, int32_t iter);

        // 填充animator相关桢
        void fillKeyframes(TaroAnimatorPtr animator,
                           TaroCurvePtr curve,
                           const std::vector<TaroCSSOM::TaroStylesheet::KeyFrame>& keyframes);

        // 创建桢动画
        std::shared_ptr<TaroAnimation> createKeyframeAnimation(
            CSSProperty::Type prop_type, const TaroAnimationPropValue& sys_value,
            const TaroAnimationPropSetFun& set_fun, std::shared_ptr<TaroCurve> curve);

        // 创建动画的关键帧信息
        void addKeyframe(std::shared_ptr<TaroAnimation> animation, float percent,
                         const TaroAnimationPropValue& keyframe_value,
                         std::shared_ptr<TaroCurve> curve);

        std::unordered_map<std::string, std::shared_ptr<TaroAnimator> > name_to_anim_;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
