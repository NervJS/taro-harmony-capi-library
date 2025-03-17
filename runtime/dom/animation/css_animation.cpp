#include "css_animation.h"

#include "animation_config.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/animation/animator/animation.h"
#include "runtime/dom/animation/animator/keyframe_animation.h"

namespace TaroRuntime {
namespace TaroAnimate {
    TaroCSSAnimation::TaroCSSAnimation(
        std::shared_ptr<TaroAnimationPropsController> props_controller,
        std::weak_ptr<TaroDOM::TaroRenderNode> node)
        : TaroAnimate(node, TaroAnimateType::CSS_ANIMATION, props_controller) {}

    int TaroCSSAnimation::bindAnimation(const TaroCSSOM::TaroStylesheet::AnimationOption& option, const std::string& router) {
        // 相同名称的事件不一起执行
        if (name_to_anim_.count(option.name) > 0) {
            return 0;
        }

        // 获取keyframes
        auto keyframes = TaroCSSOM::CSSStyleSheet::GetInstance()->getAnimKeyframes(option.name, router);
        if (keyframes == nullptr || keyframes->keyframes_.empty()) {
            TARO_LOG_ERROR("TaroAnimation", "animation name:%{public}p keyframe is null",
                           option.name.c_str());
            return -1;
        }

        std::shared_ptr<TaroAnimator> animator = std::make_shared<TaroAnimator>();
        animator->setStopCallBack(
            std::bind(&TaroCSSAnimation::stopCallBack, this, animator->id()));
        animator->setIterCallBack(
            std::bind(&TaroCSSAnimation::iterCallBack, this, animator->id(), std::placeholders::_1));
        animator->setName(option.name);
        animator->setFillMode(TaroAnimationConfig::parseFillMode(option.fill_mode));
        animator->setDirection(TaroAnimationConfig::parseDirection(option.direction));
        animator->setDuration(option.duration);
        auto curve = TaroCurves::getCurve(option.timingFunction);
        animator->setDelay(option.delay);
        animator->setIteration(option.interationCount);

        fillKeyframes(animator, curve, keyframes->keyframes_);
        addAnimator(animator);
        name_to_anim_[option.name] = animator;
        animator->play();
        return 0;
    }

    int TaroCSSAnimation::refreshAnimation(const TaroCSSOM::TaroStylesheet::AnimationOption& option, TaroDOM::StylesheetRef style, const std::string& router) {
        auto iter = name_to_anim_.find(option.name);
        if (iter == name_to_anim_.end() || iter->second == nullptr) {
            TARO_LOG_ERROR("TaroAnimation", "anim:%{public}s is not found", option.name.c_str());
            return 0;
        }
        auto animator = iter->second;
        // 清理之前要将旧的style重刷
        const auto& prop_types = animator->getProps();
        for (const auto& prop_type : prop_types) {
            TaroAnimationProps::resetStyleSheetProp(node_owner_, prop_type);
        }
        animator->clearAnimation();

        // 获取keyframes
        auto keyframes = TaroCSSOM::CSSStyleSheet::GetInstance()->getAnimKeyframes(option.name, router);
        if (keyframes == nullptr || keyframes->keyframes_.empty()) {
            TARO_LOG_ERROR("TaroAnimation", "animation name:%{public}p keyframe is null",
                           option.name.c_str());
            return -1;
        }

        animator->setFillMode(TaroAnimationConfig::parseFillMode(option.fill_mode));
        animator->setDirection(TaroAnimationConfig::parseDirection(option.direction));
        animator->setDuration(option.duration);
        auto curve = TaroCurves::getCurve(option.timingFunction);
        animator->setDelay(option.delay);
        animator->setIteration(option.interationCount);
        fillKeyframes(animator, curve, keyframes->keyframes_);
        animator->refresh();

        // 已结束且为forwards的要重置为终点值

        return 0;
    }

    void TaroCSSAnimation::fillKeyframes(TaroAnimatorPtr animator,
                                         TaroCurvePtr curve,
                                         const std::vector<TaroCSSOM::TaroStylesheet::KeyFrame>& keyframes) {
        // 获取系统值
        std::unordered_map<CSSProperty::Type, TaroAnimationPropValue> prop_system_values;
        auto get_sys_value =
            [&](CSSProperty::Type prop_type) -> const TaroAnimationPropValue* {
            const auto iter = prop_system_values.find(prop_type);
            if (iter != prop_system_values.end()) {
                return &iter->second;
            }
            TaroAnimationPropValue system_value;
            bool success = TaroAnimationProps::getSystemPropValue(
                prop_type, node_owner_.lock(), system_value);
            if (!success) {
                TARO_LOG_ERROR("TaroTransition",
                               "prop:%{public}d system value is not found", prop_type);
                return nullptr;
            }
            prop_system_values[prop_type] = std::move(system_value);
            return &prop_system_values[prop_type];
        };

        std::unordered_map<CSSProperty::Type, std::shared_ptr<TaroAnimation>> prop_to_animations;
        for (const auto& keyframe : keyframes) {
            for (const auto& prop : keyframe.params) {
                if (prop.first == CSSProperty::Type::Invalid) {
                    TARO_LOG_ERROR("TaroAnimation", "invalid prop type");
                    continue;
                }
                auto prop_type = prop.first;
                // 获取系统值
                auto system_value = get_sys_value(prop.first);
                if (system_value == nullptr) {
                    TARO_LOG_ERROR("TaroAnimation", "get prop:%{public}d sys failed", prop.first);
                    continue;
                }

                // 获取prop值
                TaroAnimationPropValue prop_value;
                bool success = TaroAnimationProps::getAnimationPropValue(node_owner_.lock(),
                                                                         prop_type, prop.second, *system_value, prop_value);
                if (!success) {
                    TARO_LOG_ERROR("TaroAnimation",
                                   "get prop:%{public}d value failed", prop_type);
                    continue;
                }

                std::shared_ptr<TaroAnimation> animation = nullptr;
                auto anim_iter = prop_to_animations.find(prop_type);
                if (anim_iter == prop_to_animations.end()) {
                    // 创建新animation
                    auto fun = std::bind(&TaroAnimationPropsController::setProp, props_controller_,
                                         animate_type_, prop_type, animator->id(), std::placeholders::_1);
                    animation = createKeyframeAnimation(prop_type, *system_value, fun, curve);
                    if (animation != nullptr) {
                        prop_to_animations[prop_type] = animation;
                        animator->addAnimation(animation);
                        animator->addPropType(prop_type);
                    }
                } else {
                    animation = anim_iter->second;
                }
                addKeyframe(animation, keyframe.percent, prop_value, curve);
            }
        }
    }

    void TaroCSSAnimation::stopCallBack(int32_t anim_id) {
        TARO_LOG_DEBUG("TaroAnimation", "stop anim_id:%{public}d", anim_id);
        auto animator = getAnimatorById(anim_id);
        if (animator->getFillMode() == TaroAnimationFillMode::None || animator->getFillMode() == TaroAnimationFillMode::Backwards) {
            const auto& props = animator->getProps();
            for (const auto& elem : props) {
                resetStyleProp(elem);
            }
        }
    }

    void TaroCSSAnimation::iterCallBack(int32_t anim_id, int32_t iter) {
        TARO_LOG_DEBUG("TaroAnimation", "anim_id:%{public}d iter:%{public}d", anim_id,
                       iter);
    }

    void TaroCSSAnimation::clearAnimId(CSSProperty::Type prop_type,
                                       int32_t anim_id) {
        props_controller_->clearAnimId(prop_type, anim_id);
    }

    void TaroCSSAnimation::cancelAnimation(const std::string& anim_name) {
        auto iter = name_to_anim_.find(anim_name);
        if (iter == name_to_anim_.end()) {
            return;
        }
        // iter->second->stop();
        const auto& props = iter->second->getProps();
        for (const auto& elem : props) {
            resetStyleProp(elem);
        }
        deleteAnimator(iter->second->id());
        name_to_anim_.erase(iter);
    }

    bool TaroCSSAnimation::hasAnimation(const std::string& anim_name) {
        return name_to_anim_.find(anim_name) != name_to_anim_.end();
    }

    void TaroCSSAnimation::getNames(std::vector<std::string>& names) {
        for (const auto& elem : name_to_anim_) {
            names.push_back(elem.first);
        }
    }

    bool TaroCSSAnimation::isActivePropType(CSSProperty::Type prop_type) {
        for (const auto& elem : name_to_anim_) {
            if (elem.second->isActivePropType(prop_type)) {
                return true;
            }
        }
        return false;
    }

    std::shared_ptr<TaroAnimation> TaroCSSAnimation::createKeyframeAnimation(
        CSSProperty::Type prop_type, const TaroAnimationPropValue& sys_value,
        const TaroAnimationPropSetFun& set_fun, std::shared_ptr<TaroCurve> curve) {
        // check prop类型
        bool b_ok = TaroAnimationProps::checkPropertyType(prop_type, sys_value);
        if (!b_ok) {
            TARO_LOG_ERROR("TaroAnimation", "invalid prop type");
            return nullptr;
        }

        auto evaluator = TaroAnimationProps::getEvaluator(prop_type);
// create animation
// float type
#define CREATE_KEYFRAME_ANIMATION(PROP_TYPE)                                     \
    {                                                                            \
        const PROP_TYPE* f_sys_value = std::get_if<PROP_TYPE>(&sys_value);       \
        if (f_sys_value != nullptr) {                                            \
            auto animation = std::make_shared<TaroKeyframeAnimation<PROP_TYPE>>( \
                *f_sys_value, set_fun);                                          \
            animation->setEvaluator(evaluator);                                  \
            auto begin =                                                         \
                std::make_shared<TaroKeyframe<PROP_TYPE>>(0.0f, *f_sys_value);   \
            begin->setCurve(curve);                                              \
            animation->addKeyframe(begin);                                       \
            auto end =                                                           \
                std::make_shared<TaroKeyframe<PROP_TYPE>>(1.0f, *f_sys_value);   \
            end->setCurve(curve);                                                \
            animation->addKeyframe(end);                                         \
            return animation;                                                    \
        }                                                                        \
    }
        CREATE_KEYFRAME_ANIMATION(double);
        CREATE_KEYFRAME_ANIMATION(
            std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>);
        CREATE_KEYFRAME_ANIMATION(Dimension);
        return nullptr;
    }

    void TaroCSSAnimation::addKeyframe(std::shared_ptr<TaroAnimation> animation,
                                       float percent,
                                       const TaroAnimationPropValue& keyframe_value,
                                       std::shared_ptr<TaroCurve> curve) {
#define CREATE_KEYFRAME(PROP_TYPE)                                            \
    {                                                                         \
        const PROP_TYPE* f_value = std::get_if<PROP_TYPE>(&keyframe_value);   \
        if (f_value != nullptr) {                                             \
            auto keyframe_animation =                                         \
                std::dynamic_pointer_cast<TaroKeyframeAnimation<PROP_TYPE>>(  \
                    animation);                                               \
            if (keyframe_animation == nullptr) {                              \
                TARO_LOG_ERROR("TaroAnimation",                               \
                               "dynamic_pointer_cast float point failed");    \
                return;                                                       \
            }                                                                 \
            auto keyframe =                                                   \
                std::make_shared<TaroKeyframe<PROP_TYPE>>(percent, *f_value); \
            keyframe->setCurve(curve);                                        \
            keyframe_animation->addKeyframe(keyframe);                        \
            return;                                                           \
        }                                                                     \
    }
        CREATE_KEYFRAME(double);
        CREATE_KEYFRAME(std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>);
        CREATE_KEYFRAME(Dimension);
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
