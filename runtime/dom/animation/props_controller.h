#pragma once

#include <any>
#include <unordered_map>
#include <unordered_set>

#include "helper/life_statistic.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/animation/animation_property.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroAnimate {
    // 枚举值也代表了setprop的优先级，不可随意更改
    enum class TaroAnimateType {
        INVALID = 0,
        CSS_TRANSITION = 1,
        CSS_ANIMATION = 2,
        JS_ANIMATION = 3
    };

    struct TaroAnimationPropsInfo {
        TaroAnimateType animate_type = TaroAnimateType::INVALID;
        int32_t animator_id_ = 0;
        TaroAnimationPropValue prop_value_;
    };

    class TaroAnimationPropsController final
        : public TaroHelper::TaroClassLife<TaroAnimationPropsController> {
        public:
        explicit TaroAnimationPropsController(
            std::weak_ptr<TaroDOM::TaroRenderNode> node);
        ~TaroAnimationPropsController() = default;

        void setProp(TaroAnimateType animate_type, CSSProperty::Type prop_type,
                     int32_t anim_id, const TaroAnimationPropValue& value);

        void clearAnimId(CSSProperty::Type prop_type, int32_t anim_id);

        TaroAnimationPropsInfo* getStyle(CSSProperty::Type prop_type);

        void clearAll();

        void updatePropsToNode(uint64_t version);

        private:
        // 存储不同CSS属性对应该的动画属性信息(优先级，动画值，id)
        std::unordered_map<CSSProperty::Type, TaroAnimationPropsInfo> props_;
        std::weak_ptr<TaroDOM::TaroRenderNode> node_owner_;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
