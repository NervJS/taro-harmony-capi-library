#include "js_animation.h"

#include "quickjs.h"

namespace TaroRuntime {
namespace TaroAnimate {
    int TaroJSAnimation::bindJsAnimation(std::shared_ptr<TaroJsAnimationOption> option) {
        TaroJsAnimatorPtr animator = std::make_shared<TaroJsAnimator>(node_owner_, shared_from_this(), props_controller_);
        int ret = animator->setAnimation(option);
        if (ret != 0) {
            TARO_LOG_ERROR("TaroAnimation", "bindJsAnimation failed, ret=%{public}d", ret);
            return ret;
        }

        if (cur_animator_ != nullptr) {
            js_animators_.push_back(cur_animator_);
        }
        cur_animator_ = animator;
        for (const auto& elem : js_animators_) {
            elem->jumpNextStep();
        }
        return 0;
    }

    void TaroJSAnimation::stopCallBack(uint32_t id) {
        // 移除结束的动画
        if (cur_animator_ == nullptr || cur_animator_->id() != id) {
            TARO_LOG_ERROR("TaroAnimation", "current animator is not %{public}u", id);
            return;
        }

        // todo: 触发stop结束事件
        cur_animator_ = nullptr;
        // 选一个动画恢复step
        while (!js_animators_.empty()) {
            cur_animator_ = js_animators_.back();
            js_animators_.pop_back();
            // 跳入下一个step
            // todo: 触发 js iter事件
            int ret = cur_animator_->switchNextStep();
            if (ret == 0) {
                break;
            }

            // step失败
            TARO_LOG_ERROR("TaroAnimation", "switchNextStep failed， ret=%{public}d", ret);
            cur_animator_ = nullptr;
        }

        idleIterNext();
    }

    void TaroJSAnimation::stepCallBack(uint32_t id) {
        cur_animator_->switchNextStep();
        idleIterNext();
    }

    void TaroJSAnimation::idleIterNext() {
        for (auto iter = js_animators_.begin(); iter != js_animators_.end();) {
            (*iter)->jumpNextStep();
            if ((*iter)->state() == TaroAnimationState::STOPPED) {
                // todo: 触发 stop结束事件
                iter == js_animators_.erase(iter);
            } else {
                // todo: 触发 js iter事件
                ++iter;
            }
        }
    }

    void TaroJSAnimation::tick(uint64_t current_time) {
        if (cur_animator_ == nullptr) {
            return;
        }

        cur_animator_->tick(current_time);
    }

    void TaroJSAnimation::pause() {
        if (cur_animator_ == nullptr) {
            return;
        }
        cur_animator_->pause();
    }

    void TaroJSAnimation::resume() {
        if (cur_animator_ == nullptr) {
            return;
        }
        cur_animator_->resume();
    }

    bool TaroJSAnimation::isValid(uint32_t animator_id) {
        for (const auto& elem : js_animators_) {
            if (elem->id() == animator_id) {
                return true;
            }
        }
        return false;
    }
} // namespace TaroAnimate
} // namespace TaroRuntime
