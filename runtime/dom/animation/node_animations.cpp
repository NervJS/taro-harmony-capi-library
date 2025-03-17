#include "node_animations.h"

#include "animator/utils.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/animation/animation_lock.h"
#include "runtime/dom/element/page_container.h"

namespace TaroRuntime {
namespace TaroAnimate {
    TaroNodeAnimations::TaroNodeAnimations(
        std::shared_ptr<TaroDOM::TaroRenderNode> node)
        : node_owner_(node) {
        props_controller_ = std::make_shared<TaroAnimationPropsController>(node);
        css_animation_ = std::make_shared<TaroCSSAnimation>(props_controller_, node);
        css_transition_ = std::make_shared<TaroCSSTransition>(props_controller_, node);
        js_animation_ = std::make_shared<TaroJSAnimation>(props_controller_, node);
    }

    TaroNodeAnimations::~TaroNodeAnimations() {
        disable();
    }

    void TaroNodeAnimations::tick(uint64_t current_time) {
        props_controller_->clearAll();

        css_animation_->tick(current_time);
        bool css_animation_active = css_animation_->isActive();

        css_transition_->tick(current_time);
        bool css_transition_active = css_transition_->isActive();

        js_animation_->tick(current_time);
        bool js_animation_active = js_animation_->isActive();

        props_controller_->updatePropsToNode(anim_version_);
        if (!css_animation_active && !css_transition_active && !js_animation_active) {
            disable();
        }
    }

    void TaroNodeAnimations::clearCSSAnimation() {
        css_transition_->clearAnimator();
    }

    void TaroNodeAnimations::clearCSSTransition() {
        css_transition_->clearAnimator();
    }

    void TaroNodeAnimations::enableTrigger() {
        auto node = node_owner_.lock();
        if (node == nullptr) {
            return;
        }

        if (const auto elementRef = node->element_ref_.lock()) {
            if (auto root = std::dynamic_pointer_cast<TaroDOM::TaroPageContainer>(
                    elementRef->getPageRoot())) {
                enable(root->nid_);
            }
        }
    }

    void TaroNodeAnimations::pause() {
        css_animation_->pause();
        css_transition_->pause();
        js_animation_->pause();
    }

    void TaroNodeAnimations::resume() {
        css_animation_->resume();
        css_transition_->resume();
        js_animation_->resume();
    }

    int TaroNodeAnimations::bindAnimation(
        const TaroCSSOM::TaroStylesheet::AnimationOption& optional) {
        TARO_LOG_DEBUG("TaroNodeAnimations", "bindAnimation: %{public}s",
                       optional.name.c_str());
        auto node = node_owner_.lock();
        if (node == nullptr) {
            TARO_LOG_ERROR("TaroAnimation", "node has been freed");
            return -1;
        }

        auto elem = node->element_ref_.lock();
        if (elem == nullptr || elem->context_ == nullptr) {
            TARO_LOG_ERROR("TaroAnimation", "elem is %{public}s null", elem == nullptr ? "" : "not");
            return -2;
        }

        int ret = css_animation_->bindAnimation(optional, elem->context_->page_path_);
        enableTrigger();
        return ret;
    }

    int TaroNodeAnimations::bindTransition(const TaroCSSOM::TaroStylesheet::TransitionParam& optional) {
        TARO_LOG_DEBUG("TaroTransition", "bindTransition:%{public}d", optional.prop_type_);
        auto node = node_owner_.lock();
        if (node == nullptr) {
            TARO_LOG_ERROR("TaroTransition", "node has been freed");
            return -1;
        }

        int ret = css_transition_->bindTransition(optional);
        enableTrigger();
        return ret;
    }

    void TaroNodeAnimations::cancelAnimation(const std::string& name) {
        if (name.empty()) {
            return;
        }
        css_animation_->cancelAnimation(name);
    }

    void TaroNodeAnimations::cancelTransition(CSSProperty::Type prop_type) {
        if (prop_type == CSSProperty::Type::Invalid) {
            return;
        }
        css_transition_->cancelTransition(prop_type);
    }

    void TaroNodeAnimations::clearStyleCSSAnimations() {
        auto node = node_owner_.lock();
        auto old_style = node->old_style_ref_;
        auto new_style = node->style_ref_;

        if (old_style == nullptr) {
            return;
        }

        std::lock_guard lock(AnimationLock::getAnimationLock());

        if (old_style->animationMulti.has_value()) {
            const auto& old_names = old_style->animationMulti.value().names_;
            for (const auto& elem_name : old_names) {
                if (needDisplay(new_style->display) && new_style->animationMulti.has_value()) {
                    const auto& new_names = new_style->animationMulti.value().names_;
                    // 继续存在，不清理
                    if (std::find(new_names.begin(), new_names.end(), elem_name) != new_names.end()) {
                        continue;
                    }
                }
                // 清理不再存在的动画
                cancelAnimation(elem_name);
            }
        }

        // clear CSSTransition
        std::vector<CSSProperty::Type> trans_props;
        css_transition_->getRunningProp(trans_props);
        for (const auto& elem : trans_props) {
            // 清空操作
            if (!new_style->transition.isActivePropType(elem) || !needDisplay(new_style->display)) {
                cancelTransition(elem);
            }
        }
    }

    void TaroNodeAnimations::reloadAnimation(TaroDOM::StylesheetRef style) {
        auto node = node_owner_.lock();
        auto elem = node->element_ref_.lock();
        if (elem == nullptr || elem->context_ == nullptr) {
            TARO_LOG_ERROR("TaroAnimation", "elem is %{public}s null", elem == nullptr ? "" : "not");
            return;
        }
        props_controller_->clearAll();
        // refresh Animation
        std::vector<std::string> names;
        css_animation_->getNames(names);
        for (const auto name : names) {
            auto option = style->animationMulti.getAnimation(name);
            if (option.has_value() && needDisplay(style->display)) {
                css_animation_->refreshAnimation(option.value(), style, elem->context_->page_path_);
            } else {
                cancelAnimation(name);
            }
        }

        // 写值
        props_controller_->updatePropsToNode(anim_version_);
    }

    void TaroNodeAnimations::setCSSAnimationZeroValue() {
        auto node = node_owner_.lock();
        auto elem = node->element_ref_.lock();
        if (elem == nullptr || elem->context_ == nullptr) {
            TARO_LOG_ERROR("TaroAnimation", "elem is %{public}s null", elem == nullptr ? "" : "not");
            return;
        }

        auto old_style = node->old_style_ref_;
        auto new_style = node->style_ref_;
        // 新style为空
        if (new_style == nullptr || !new_style->animationMulti.has_value() || !needDisplay(new_style->display)) {
            return;
        }

        // 动画未发生变化
        if (old_style != nullptr && needDisplay(old_style->display) && old_style->animationMulti.has_value() && old_style->animationMulti.value().names_ == new_style->animationMulti.value().names_) {
            return;
        }

        const auto& new_names = node->style_ref_->animationMulti.value().names_;
        for (size_t idx = 0; idx < new_names.size(); idx++) {
            // 防止delay有值的时候，强制设置了第一帧
            auto delays = node->style_ref_->animationMulti.value().delays_;
            if (delays.size() >= idx + 1) {
              if (delays[idx] > 0) {
                  continue;
              }
            }
            if (old_style != nullptr && needDisplay(old_style->display) && old_style->animationMulti.has_value()) {
                const auto& old_names = old_style->animationMulti.value().names_;
                const auto iter = std::find(old_names.begin(), old_names.end(), new_names[idx]);
                // 旧style已存在，不需要设置zero
                if (iter != old_names.end()) {
                    continue;
                }
            }

            auto keyframes = TaroCSSOM::CSSStyleSheet::GetInstance()->getAnimKeyframes(new_names[idx], elem->context_->page_path_);
            if (keyframes == nullptr || keyframes->keyframes_.empty()) {
                TARO_LOG_ERROR("TaroAnimation", "animation name:%{public}p keyframe is null",
                               new_names[idx].c_str());
                continue;
            }

            const auto& new_keyframes = keyframes->keyframes_;
            // 非 percent = 0
            for (const auto& keyframe : new_keyframes) {
                if (!TaroUtils::nearEqual(keyframe.percent, 0)) {
                    continue;
                }
                // 布局类要提前设置到yoga
                for (const auto& elem : keyframe.params) {
                    TaroAnimationProps::setKeyframeToNode(node, elem.first, elem.second);
                }
            }
        }
    }

    int32_t TaroNodeAnimations::startCSSAnimation() {
        auto node = node_owner_.lock();
        auto elem = node->element_ref_.lock();
        if (elem == nullptr || elem->context_ == nullptr) {
            TARO_LOG_ERROR("TaroAnimation", "elem is %{public}s null", elem == nullptr ? "" : "not");
            return 0;
        }
        uint64_t new_version = TaroCSSOM::CSSStyleSheet::getAnimKeyframesPtr(elem->context_->page_path_)->getVersion();
        if (!node->style_ref_->animationMulti.has_value() || !needDisplay(node->style_ref_->display)) {
            anim_version_ = new_version;
            return 0;
        }

        std::lock_guard lock(AnimationLock::getAnimationLock());
        // 当折叠时，重新加载动画
        if (new_version != anim_version_) {
            anim_version_ = new_version;
            reloadAnimation(node->style_ref_);
        }

        auto& animation = node->style_ref_->animationMulti.value();
        const auto& names = node->style_ref_->animationMulti.value().names_;
        for (size_t idx = 0; idx < names.size(); idx++) {
            // check 是否是旧动画
            if (css_animation_->hasAnimation(names[idx])) {
                continue;
            }
            // 启动新动画
            auto animation_option = node->style_ref_->animationMulti.getAnimation(idx);
            if (animation_option.has_value()) {
                bindAnimation(animation_option.value());
            }
        }
        return 0;
    }

    int32_t TaroNodeAnimations::bindJSAnimation(const napi_value& napi_val) {
        if (napi_val == nullptr) {
            return false;
        }
        auto option = std::make_shared<TaroJsAnimationOption>();
        int ret = option->setFromNode(napi_val);
        if (ret != 0) {
            TARO_LOG_ERROR("TaroAnimation", "set TaroJsAnimationOption failed, ret=%{public}d", ret);
            return ret;
        }
        return bindJSAnimation(option);
    }

    int32_t TaroNodeAnimations::bindJSAnimation(const TaroJsAnimationOptionRef& option) {
        auto node = node_owner_.lock();
        if (option == nullptr || node == nullptr ||
            (node->style_ref_ != nullptr && !needDisplay(node->style_ref_->display))) {
            return -1;
        }
        std::lock_guard lock(AnimationLock::getAnimationLock());
        int ret = js_animation_->bindJsAnimation(option);
        if (ret != 0) {
            TARO_LOG_ERROR("TaroAnimation", "bindJsAnimation failed, ret=%{public}d", ret);
            return ret;
        }
        enableTrigger();
        return 0;
    }

    bool TaroNodeAnimations::onSetPropertyIntoNode(const CSSProperty::Type& property,
                                                   const TaroChange& changeType) {
        auto node = node_owner_.lock();
        auto elem = node->element_ref_.lock();
        if (elem == nullptr || elem->context_ == nullptr) {
            TARO_LOG_ERROR("TaroAnimation", "elem is %{public}s null", elem == nullptr ? "" : "not");
            return true;
        }
        if (node->style_ref_ != nullptr && !needDisplay(node->style_ref_->display)) {
            return true;
        }
        std::lock_guard lock(AnimationLock::getAnimationLock());
        // 版本不一致时清空transition，且不进行动画
        auto version = TaroCSSOM::CSSStyleSheet::getAnimKeyframesPtr(elem->context_->page_path_)->getVersion();
        if (version != anim_version_ && anim_version_ != 0) {
            css_transition_->refreshAnimation();
            return true;
        }

        // check 是否有animation动画作用于该属性
        if (css_animation_ != nullptr) {
            bool ret = css_animation_->isActivePropType(property);
            if (ret) {
                TARO_LOG_DEBUG("TaroNodeAnimations", "prop:%{public}d is active", property);
                return false;
            }
        }
        // check Transition
        if (changeType == TaroChange::Modified) {
            auto style = node->style_ref_;
            auto transition_param = style->transition.getTransitionParam(property, *style);
            if (transition_param != nullptr) {
                int ret = bindTransition(*transition_param);
                if (ret == 0) {
                    TARO_LOG_DEBUG("TaroRenderNode", "attach transition success");
                    return false;
                }
            }
        }
        return true;
    }

    //     bool TaroNodeAnimations::isValidVersion() const {
    //         uint64_t cur_version = TaroCSSOM::CSSStyleSheet::getAnimKeyframesPtr()->getVersion();
    //         return cur_version == anim_version_;
    //     }

    bool TaroNodeAnimations::isValid(TaroAnimateType anim_type, int anim_id, uint64_t version) {
        if (version != anim_version_) {
            return false;
        }
        std::lock_guard lock(AnimationLock::getAnimationLock());
        if (anim_type == TaroAnimateType::CSS_ANIMATION) {
            if (css_animation_ != nullptr) {
                return css_animation_->isValid(anim_id);
            }
        } else if (anim_type == TaroAnimateType::CSS_TRANSITION) {
            if (css_transition_ != nullptr) {
                return css_transition_->isValid(anim_id);
            }
        } else if (anim_type == TaroAnimateType::JS_ANIMATION) {
            if (js_animation_ != nullptr) {
                return js_animation_->isValid(anim_id);
            }
        }
        return false;
    }

    bool TaroNodeAnimations::needDisplay(const TaroCSSOM::TaroStylesheet::Display& display) const {
        if (display.has_value() && display.value() == PropertyType::Display::None) {
            return false;
        }
        return true;
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
