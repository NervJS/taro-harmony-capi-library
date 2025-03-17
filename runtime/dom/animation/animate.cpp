#include "animate.h"

namespace TaroRuntime {
namespace TaroAnimate {
    void TaroAnimate::asynDeleteAnimator(uint32_t animator_id) {
        // std::lock_guard lock(change_lock_);
        delete_queue_.push_back(animator_id);
    }

    void TaroAnimate::deleteAnimator(uint32_t animator_id) {
        for (auto iter = animators_.begin(); iter != animators_.end(); iter++) {
            if (animator_id == (*iter)->id()) {
                animators_.erase(iter);
                break;
            }
        }
    }

    void TaroAnimate::executeDelete() {
        // std::lock_guard lock(change_lock_);
        // std::lock_guard animator_lock(animators_lock_);
        for (const auto& elem : delete_queue_) {
            for (auto iter = animators_.begin();
                 iter != animators_.end(); iter++) {
                if (elem == (*iter)->id()) {
                    animators_.erase(iter);
                    break;
                }
            }
        }
        delete_queue_.clear();
    }

    bool TaroAnimate::isActive() {
        // std::lock_guard animator_lock(animators_lock_);
        return !animators_.empty();
    }

    void TaroAnimate::clearAnimator() {
        // std::lock_guard animator_lock(animators_lock_);
        animators_.clear();
        delete_queue_.clear();
        set_system_props_.clear();
    }

    std::shared_ptr<TaroAnimator> TaroAnimate::getAnimatorById(uint32_t animator_id) {
        for (auto& elem : animators_) {
            if (elem->id() == animator_id) {
                return elem;
            }
        }
        return nullptr;
    }

    void TaroAnimate::pause() {
        // std::lock_guard animator_lock(animators_lock_);
        for (auto& elem : animators_) {
            elem->pause();
        }
    }

    void TaroAnimate::resume() {
        // std::lock_guard animator_lock(animators_lock_);
        for (auto& elem : animators_) {
            elem->resume();
        }
    }

    void TaroAnimate::tick(uint64_t current_time) {
        executeDelete();
        resetStyleSheetProp();
        {
            // std::lock_guard animator_lock(animators_lock_);
            for (auto animator : animators_) {
                animator->onFrame(current_time);
            }
        }
    }

    void TaroAnimate::addAnimator(std::shared_ptr<TaroAnimator> animator) {
        // std::lock_guard animator_lock(animators_lock_);
        animators_.push_back(animator);
    }

    void TaroAnimate::resetStyleProp(CSSProperty::Type prop_type) {
        set_system_props_.insert(prop_type);
    }

    void TaroAnimate::resetStyleSheetProp() {
        for (auto elem : set_system_props_) {
            TaroAnimationProps::resetStyleSheetProp(node_owner_, elem);
        }
        set_system_props_.clear();
    }
    bool TaroAnimate::isValid(uint32_t animator_id) {
        for (const auto& elem : animators_) {
            if (elem->id() == animator_id) {
                return true;
            }
        }
        return false;
    }
} // namespace TaroAnimate
} // namespace TaroRuntime
