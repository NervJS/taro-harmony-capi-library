/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "animation_property.h"

#include "animation_property/base_property.h"
#include "engine/react_common/systrace_section.h"
#include "helper/StringUtils.h"
#include "helper/TaroLog.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/width.h"
#include "runtime/render.h"
#include "yoga/YGNodeLayout.h"

namespace TaroRuntime {
namespace TaroAnimate {
    TaroAnimationProps* TaroAnimationProps::instance() {
        static TaroAnimationProps* props = nullptr;
        if (props == nullptr) {
            props = new TaroAnimationProps;
        }
        return props;
    }

    int TaroAnimationProps::registerProp(CSSProperty::Type prop_type,
                                         AnimationPropBase* prop_handler) {
        if (prop_handler == nullptr || anim_props_.count(prop_type) > 0) {
            TARO_LOG_DEBUG("TaroAnimation", "prop:%{public}d has been register",
                           prop_type);
            return -1;
        }
        anim_props_[prop_type] = prop_handler;
        return 0;
    }

    const AnimationPropBase* TaroAnimationProps::getAnimationProp(
        CSSProperty::Type prop_type) const {
        const auto iter = anim_props_.find(prop_type);
        if (iter != anim_props_.end()) {
            return iter->second;
        }
        return nullptr;
    }

    bool TaroAnimationProps::checkPropertyType(
        CSSProperty::Type prop_type, const TaroAnimationPropValue& prop_value) {
        const auto prop_handler =
            TaroAnimationProps::instance()->getAnimationProp(prop_type);
        if (prop_handler == nullptr) {
            return false;
        }
        return prop_handler->checkPropertyType(prop_type, prop_value);
    }

    bool TaroAnimationProps::getSystemPropValue(
        CSSProperty::Type prop_type, std::shared_ptr<TaroDOM::TaroRenderNode> node,
        TaroAnimationPropValue& prop_value) {
        if (node == nullptr) {
            return false;
        }

        const auto prop_handler =
            TaroAnimationProps::instance()->getAnimationProp(prop_type);
        if (prop_handler == nullptr) {
            return false;
        }
        return prop_handler->getSystemPropValue(node, prop_type,
                                                prop_value);
    }

    bool TaroAnimationProps::getAnimationPropValue(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        const TaroCSSOM::TaroStylesheet::KeyframeValue& n_val,
        const TaroAnimationPropValue& sys_value,
        TaroAnimationPropValue& prop_value) {
        const auto prop_handler =
            TaroAnimationProps::instance()->getAnimationProp(prop_type);
        if (prop_handler == nullptr) {
            return false;
        }
        return prop_handler->getAnimationPropValue(node, prop_type, n_val, sys_value,
                                                   prop_value);
    }

    void TaroAnimationProps::setNodeProperty(
        std::weak_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type, const TaroAnimationPropsInfo& prop_value,
        uint64_t version) {
        const auto prop_handler =
            TaroAnimationProps::instance()->getAnimationProp(prop_type);
        if (prop_handler == nullptr) {
            return;
        }

        std::shared_ptr<TaroThread::TaskExecutor> runner = Render::GetInstance()->GetTaskRunner();
        runner->runTask(TaroThread::TaskThread::MAIN, [prop_handler, node, prop_type, prop_value, version]() {
            auto share_node = node.lock();
            if (share_node == nullptr || share_node->animation_ == nullptr) {
                return;
            }
            bool valid = share_node->animation_->isValid(prop_value.animate_type, prop_value.animator_id_, version);
            if (!valid) {
                TARO_LOG_DEBUG("TaroAnimation", "animator:%{public}d not valid",
                               prop_value.animator_id_);
                return;
            }
            prop_handler->setNodeProperty(share_node, prop_type, prop_value.prop_value_);
        });
        return;
    }

    // 设置动画初始值到节点
    void TaroAnimationProps::setKeyframeToNode(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        const TaroCSSOM::TaroStylesheet::KeyframeValue& keyframe) {
        const auto prop_handler = TaroAnimationProps::instance()->getAnimationProp(prop_type);
        if (prop_handler == nullptr) {
            return;
        }
        prop_handler->setKeyframeToNode(node, prop_type, keyframe);
    }

    // 设置style值到node节点
    void TaroAnimationProps::resetStyleSheetProp(
        std::weak_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type) {
        const auto prop_handler = TaroAnimationProps::instance()->getAnimationProp(prop_type);
        if (prop_handler == nullptr) {
            return;
        }

        auto update_fun = [prop_handler, node, prop_type]() {
            auto shared_node = node.lock();
            if (shared_node && shared_node->style_ref_) {
                prop_handler->resetStyleSheetProp(shared_node, prop_type, shared_node->style_ref_);
            }
        };

        std::shared_ptr<TaroThread::TaskExecutor> runner = Render::GetInstance()->GetTaskRunner();
        if (runner->isOnTaskThread(TaroThread::TaskThread::MAIN)) {
            update_fun();
        } else {
            runner->runTask(TaroThread::TaskThread::MAIN, update_fun);
        }
    }

    std::shared_ptr<TaroEvaluator> TaroAnimationProps::getEvaluator(
        CSSProperty::Type prop_type) {
        const auto prop_handler =
            TaroAnimationProps::instance()->getAnimationProp(prop_type);
        if (prop_handler == nullptr) {
            return TaroEvaluators::getEvaluator(TaroEvaluatorType::Base);
        }
        return prop_handler->getEvaluator();
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
