#pragma once

#include <string>
#include <unordered_set>

#include "animation_prop_types.h"
#include "runtime/cssom/stylesheet/IAttribute.h"
#include "runtime/cssom/stylesheet/animation.h"
#include "runtime/cssom/stylesheet/color.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/transform.h"
#include "runtime/dom/animation/animator/evaluator.h"
#include "runtime/dom/ark_nodes/arkui_node.h"
#include "thread/NapiTaskRunner.h"
#include "yoga/YGConfig.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class AnimationPropBase;
    class TaroAnimationPropsInfo;
    using TaroAnimationPropValue =
        std::variant<double,  Dimension,
                     std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>>;

    using TaroAnimationPropSetFun =
        std::function<void(const TaroAnimationPropValue &t)>;

    class TaroAnimationProps {
        public:
        static bool checkPropertyType(CSSProperty::Type prop_type,
                                      const TaroAnimationPropValue &prop_value);

        // 获取系统值
        static bool getSystemPropValue(
            CSSProperty::Type prop_type, std::shared_ptr<TaroDOM::TaroRenderNode> node,
            TaroAnimationPropValue &prop_value);

        // 获取动画配置关键帧值
        static bool getAnimationPropValue(
            std::shared_ptr<TaroDOM::TaroRenderNode> node,
            CSSProperty::Type prop_type,
            const TaroCSSOM::TaroStylesheet::KeyframeValue &n_val,
            const TaroAnimationPropValue &sys_value,
            TaroAnimationPropValue &prop_value);

        // 设置节点属性信息
        static void setNodeProperty(std::weak_ptr<TaroDOM::TaroRenderNode> node,
                                    CSSProperty::Type prop_type,
                                    const TaroAnimationPropsInfo &prop_value,
                                    uint64_t version);

        // 设置动画初始值到节点
        static void setKeyframeToNode(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                      CSSProperty::Type prop_type,
                                      const TaroCSSOM::TaroStylesheet::KeyframeValue &keyframe);

        // 设置style值到node节点
        static void resetStyleSheetProp(std::weak_ptr<TaroDOM::TaroRenderNode> node,
                                        CSSProperty::Type prop_type);

        static std::shared_ptr<TaroEvaluator> getEvaluator(
            CSSProperty::Type prop_type);

        public:
        // 注册动画处理类
        int registerProp(CSSProperty::Type prop_type,
                         AnimationPropBase *prop_handler);

        // 获取动画处理类
        const AnimationPropBase *getAnimationProp(CSSProperty::Type prop_type) const;

        static TaroAnimationProps *instance();

        private:
        // 动画类型->对应的处理类
        std::unordered_map<CSSProperty::Type, AnimationPropBase *> anim_props_;
    };

    class TaroAnimationPropsRegister {
        public:
        explicit TaroAnimationPropsRegister(CSSProperty::Type prop_type,
                                            AnimationPropBase *prop_handler) {
            TaroAnimationProps::instance()->registerProp(prop_type, prop_handler);
        }
    };

#define ANIMATION_PROP_REGISTER(prop_type, PROP_CLASS)             \
    static TaroAnimationPropsRegister PROP_CLASS##register_helper( \
        prop_type, new PROP_CLASS(prop_type));
#define ANIMATION_PROP_REGISTER_INDEX(prop_type, PROP_CLASS, index)       \
    static TaroAnimationPropsRegister PROP_CLASS##register_helper##index( \
        prop_type, new PROP_CLASS(prop_type));
} // namespace TaroAnimate
} // namespace TaroRuntime
