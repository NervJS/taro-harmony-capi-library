#include "props_controller.h"

#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/animation/animate.h"

namespace TaroRuntime {
namespace TaroAnimate {

    TaroAnimationPropsController::TaroAnimationPropsController(
        std::weak_ptr<TaroDOM::TaroRenderNode> node)
        : node_owner_(node) {}

    void TaroAnimationPropsController::setProp(
        TaroAnimateType animate_type, CSSProperty::Type prop_type, int32_t anim_id,
        const TaroAnimationPropValue& value) {
        // 如果动画类型无效或者CSS属性类型无效,直接返回
        if (animate_type == TaroAnimateType::INVALID ||
            prop_type == CSSProperty::Type::Invalid) {
            return;
        }

        // 查找对应的CSS属性是否已经存在动画属性信息
        auto iter = props_.find(prop_type);

        // 如果不存在,则创建一个新的动画属性信息并插入到unordered_map中
        if (iter == props_.end()) {
            TaroAnimationPropsInfo prop_info;
            prop_info.animate_type = animate_type;
            prop_info.animator_id_ = anim_id;
            prop_info.prop_value_ = value;
            props_[prop_type] = std::move(prop_info);
            return;
        }

        // 如果已经有了就有他对应的值的引用拿出来
        auto& prop_info = iter->second;

        // 比较新旧动画的优先级
        // 如果新动画的优先级更高,则更新动画属性信息
        if (prop_info.animate_type < animate_type) {
            prop_info.animate_type = animate_type;
            prop_info.animator_id_ = anim_id;
            prop_info.prop_value_ = value;
        } // 如果新旧动画的优先级相同,则比较动画ID
          // 如果新动画的ID更大或相等,则更新动画属性信息
        else if (prop_info.animate_type == animate_type) {
            if (prop_info.animator_id_ <= anim_id) {
                prop_info.animator_id_ = anim_id;
                prop_info.prop_value_ = value;
            }
        }

        return;
    }

    void TaroAnimationPropsController::clearAnimId(CSSProperty::Type prop_type,
                                                   int32_t anim_id) {
        auto iter = props_.find(prop_type);
        if (iter == props_.end()) {
            return;
        }
        iter->second.animator_id_ = 0;
    }

    void TaroAnimationPropsController::clearAll() {
        props_.clear();
    }

    TaroAnimationPropsInfo* TaroAnimationPropsController::getStyle(
        CSSProperty::Type prop_type) {
        auto iter = props_.find(prop_type);
        if (iter == props_.end()) {
            return nullptr;
        }
        return &iter->second;
    }

    void TaroAnimationPropsController::updatePropsToNode(uint64_t version) {
        if (props_.empty()) {
            return;
        }

        for (auto& elem : props_) {
            TaroAnimationProps::setNodeProperty(node_owner_, elem.first,
                                                elem.second, version);
        }
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
