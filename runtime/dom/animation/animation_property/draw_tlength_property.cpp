#include "draw_tlength_property.h"
#include "runtime/cssom/stylesheet/css_property.h"

namespace TaroRuntime {
namespace TaroAnimate {

    AnimationPropDrawTLength::AnimationPropDrawTLength(CSSProperty::Type prop_type)
        : AnimationPropBase(prop_type, typeid(Dimension).name()) {
    }

    bool AnimationPropDrawTLength::getSystemPropValue(std::shared_ptr<TaroDOM::TaroRenderNode> node,
            CSSProperty::Type prop_type,
            TaroAnimationPropValue &prop_value) const {
        Optional<Dimension> optional_value;
        switch(prop_type) {
        case CSSProperty::Type::BorderTopLeftRadius:
            optional_value = node->GetBorderRadius(TaroEdge::TaroEdgeTop);
            break;
        case CSSProperty::Type::BorderTopRightRadius:
            optional_value = node->GetBorderRadius(TaroEdge::TaroEdgeRight);
            break;
        case CSSProperty::Type::BorderBottomLeftRadius:
            optional_value = node->GetBorderRadius(TaroEdge::TaroEdgeLeft);
            break;
        case CSSProperty::Type::BorderBottomRightRadius:
            optional_value = node->GetBorderRadius(TaroEdge::TaroEdgeBottom);
            break;
        case CSSProperty::Type::BackgroundPositionX:
            optional_value = node->GetBackgroundPositionX();
            break;
        case CSSProperty::Type::BackgroundPositionY:
            optional_value = node->GetBackgroundPositionY();
            break;
        default:
            return false;
        }

        if (!optional_value.has_value()) {
            return false;
        }
        prop_value = optional_value.value();
        return true;
    }

    bool AnimationPropDrawTLength::getAnimationPropValue(
            std::shared_ptr<TaroDOM::TaroRenderNode> node,
            CSSProperty::Type prop_type,
            const TaroCSSOM::TaroStylesheet::KeyframeValue &n_val,
            const TaroAnimationPropValue &sys_value,
            TaroAnimationPropValue &prop_value) const {
        if (auto val = std::get_if<Dimension>(&n_val)) {
            prop_value = *val;
            return true;
        }
        return false;
    }

    void AnimationPropDrawTLength::setNodeProperty(std::shared_ptr<TaroDOM::TaroRenderNode> node,
         CSSProperty::Type prop_type,
         const TaroAnimationPropValue &prop_value) const {
        const Dimension *value
            = std::get_if<Dimension>(&prop_value);
        if (value == nullptr) {
            return;
        }
    
        TARO_LOG_DEBUG("AnimationPropDrawTLength", "prop:%{public}d tlenght(%{public}d, %{public}f)",
            prop_type, value->Unit(), value->Value());
    
        setTLengthValue(prop_type, node, *value);
    }

    // 设置动画初始值到节点
    void AnimationPropDrawTLength::setKeyframeToNode(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        const TaroCSSOM::TaroStylesheet::KeyframeValue& keyframe) const {
        auto tlength_value = std::get_if<Dimension>(&keyframe);
        if (tlength_value == nullptr) {
            return;
        }
        setTLengthValue(prop_type, node, *tlength_value);
    }       

    // 设置style值到node节点
    void AnimationPropDrawTLength::resetStyleSheetProp(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        TaroDOM::StylesheetRef style) const {
        switch(prop_type) {
        case CSSProperty::Type::BorderTopLeftRadius:
            node->ResetBorderColor(TaroEdge::TaroEdgeTop);
            break;
        case CSSProperty::Type::BorderTopRightRadius:
            node->ResetBorderColor(TaroEdge::TaroEdgeRight);
            break;
        case CSSProperty::Type::BorderBottomLeftRadius:
            node->ResetBorderColor(TaroEdge::TaroEdgeLeft);
            break;
        case CSSProperty::Type::BorderBottomRightRadius:
            node->ResetBorderColor(TaroEdge::TaroEdgeBottom);
            break;
        case CSSProperty::Type::BackgroundPositionX:
            node->ResetBackgroundPositionX();
            break;
        case CSSProperty::Type::BackgroundPositionY:
            node->ResetBackgroundPositionY();
            break; 
        }
    }

    void AnimationPropDrawTLength::setTLengthValue(CSSProperty::Type prop_type,
        std::shared_ptr<TaroDOM::TaroRenderNode>& node,
        const Dimension& tlength_value) const {
        switch(prop_type) {
        case CSSProperty::Type::BorderTopLeftRadius:
            node->SetBorderRadius(tlength_value, TaroEdge::TaroEdgeTop);
            break;
        case CSSProperty::Type::BorderTopRightRadius:
            node->SetBorderRadius(tlength_value, TaroEdge::TaroEdgeRight);
            break;
        case CSSProperty::Type::BorderBottomLeftRadius:
            node->SetBorderRadius(tlength_value, TaroEdge::TaroEdgeLeft);
            break;
        case CSSProperty::Type::BorderBottomRightRadius:
            node->SetBorderRadius(tlength_value, TaroEdge::TaroEdgeBottom);
            break;
        case CSSProperty::Type::BackgroundPositionX:
            node->SetBackgroundPositionX(tlength_value);
            break;
        case CSSProperty::Type::BackgroundPositionY:
            node->SetBackgroundPositionY(tlength_value);
            break;
        }
    }

    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::BorderTopLeftRadius,
                                  AnimationPropDrawTLength, 0);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::BorderTopRightRadius,
                                  AnimationPropDrawTLength, 1);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::BorderBottomLeftRadius,
                                  AnimationPropDrawTLength, 2);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::BorderBottomRightRadius,
                                  AnimationPropDrawTLength, 3);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::BackgroundPositionX,
                                  AnimationPropDrawTLength, 4);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::BackgroundPositionY,
                                  AnimationPropDrawTLength, 5);
} // namespace TaroAnimate
} // namespace TaroRuntime
