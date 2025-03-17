#include "computed_style_property.h"

#include "../animation_property.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/render.h"
#include "yoga/YGNodeLayout.h"

namespace TaroRuntime {
namespace TaroAnimate {

    AnimationPropComputedStyle::AnimationPropComputedStyle(CSSProperty::Type prop_type)
        : AnimationPropBase(prop_type, typeid(static_cast<double>(0)).name()) {}

    std::string AnimationPropComputedStyle::getComputedName(CSSProperty::Type prop_type) const {
        static const std::unordered_map<CSSProperty::Type, std::string> name_map = {
            {CSSProperty::Type::Width, "width"},
            {CSSProperty::Type::Height, "height"},
            {CSSProperty::Type::PaddingTop, "paddingTop"},
            {CSSProperty::Type::PaddingRight, "paddingRight"},
            {CSSProperty::Type::PaddingBottom, "paddingBottom"},
            {CSSProperty::Type::PaddingLeft, "paddingLeft"},
            {CSSProperty::Type::BorderTopWidth, "borderTop"},
            {CSSProperty::Type::BorderRightWidth, "borderRight"},
            {CSSProperty::Type::BorderBottomWidth, "borderBottom"},
            {CSSProperty::Type::BorderLeftWidth, "borderLeft"},
            {CSSProperty::Type::MarginTop, "marginTop"},
            {CSSProperty::Type::MarginRight, "marginRight"},
            {CSSProperty::Type::MarginBottom, "marginBottom"},
            {CSSProperty::Type::MarginLeft, "marginLeft"},
            {CSSProperty::Type::Top, "top"},
            {CSSProperty::Type::Left, "left"},
            {CSSProperty::Type::Right, "right"},
            {CSSProperty::Type::Bottom, "bottom"},
        };
        auto iter = name_map.find(prop_type);
        if (iter != name_map.end()) {
            return iter->second;
        }
        return "";
    }

    bool AnimationPropComputedStyle::getSystemPropValue(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        TaroAnimationPropValue &prop_value) const {
        std::string name = getComputedName(prop_type);
        if (name.empty()) {
            return false;
        }
        double computed_value = node->GetComputedStyle(name.c_str());
        prop_value = computed_value;
        TARO_LOG_DEBUG("AnimationPropComputedStyle", "system prop_type:%{public}d value:%{public}f",
                       prop_type, computed_value);
        return true;
    }

    bool AnimationPropComputedStyle::getAnimationPropValue(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        const TaroCSSOM::TaroStylesheet::KeyframeValue &n_val,
        const TaroAnimationPropValue &sys_value,
        TaroAnimationPropValue &prop_value) const {
        if (auto val = std::get_if<Dimension>(&n_val)) {
            if (auto vpVal = val->ParseToVp(node->GetDimensionContext()); vpVal.has_value()) {
                prop_value = vpVal.value();
                return true;
            } else if (val->Unit() == DimensionUnit::PERCENT) {
                if (val->Value() < 0.0f) return false;
                auto parent_node = node->parent_ref_.lock();
                if (parent_node == nullptr) return false;

                std::string name = getComputedName(prop_type);
                if (name.empty()) return false;
                switch (prop_type) {
                    // 参考父节点的height计算百分比
                    case CSSProperty::Type::Height: {
                        prop_value = (double)parent_node->GetComputedStyle("height") * val->Value();
                        break;
                    }
                    case CSSProperty::Type::Top: 
                    case CSSProperty::Type::Bottom: {
                        auto positionType = node->style_ref_->position.value_or(PropertyType::Position::Static);
                        if (positionType== PropertyType::Position::Relative) {
                            prop_value = (double)node->GetComputedStyle("height") * val->Value();
                        } else if (positionType == PropertyType::Position::Absolute || positionType == PropertyType::Position::Fixed){
                            // 最近的已定位元素
                            auto positionParent = parent_node;
                            while (positionParent) {
                                if (positionParent->style_ref_->position.value_or(PropertyType::Position::Static) == PropertyType::Position::Static) {
                                    positionParent = positionParent->parent_ref_.lock();
                                } else {
                                    prop_value = (double)positionParent->GetComputedStyle("height") * val->Value();
                                    break;
                                }
                            }
                        } else {
                            return false;
                        }
                        break;
                    }
                    // 参考父节点的width计算百分比
                    case CSSProperty::Type::Width:
                    case CSSProperty::Type::MarginTop:
                    case CSSProperty::Type::MarginLeft:
                    case CSSProperty::Type::MarginBottom:
                    case CSSProperty::Type::MarginRight:
                    case CSSProperty::Type::PaddingTop:
                    case CSSProperty::Type::PaddingLeft:
                    case CSSProperty::Type::PaddingBottom:
                    case CSSProperty::Type::PaddingRight:{
                        prop_value = (double)parent_node->GetComputedStyle("width") * val->Value();
                        break;
                    }
                    case CSSProperty::Type::Left: 
                    case CSSProperty::Type::Right: {
                        auto positionType = node->style_ref_->position.value_or(PropertyType::Position::Static);
                        if (positionType== PropertyType::Position::Relative) {
                            prop_value = (double)node->GetComputedStyle("width") * val->Value();
                        } else if (positionType == PropertyType::Position::Absolute || positionType == PropertyType::Position::Fixed){
                            // 最近的已定位元素
                            auto positionParent = parent_node;
                            while (positionParent) {
                                if (positionParent->style_ref_->position.value_or(PropertyType::Position::Static) == PropertyType::Position::Static) {
                                    positionParent = positionParent->parent_ref_.lock();
                                } else {
                                    prop_value = (double)positionParent->GetComputedStyle("width") * val->Value();
                                    break;
                                }
                            }
                        } else {
                            return false;
                        }
                        break;
                    }
                    default: {
                        prop_value = (double)parent_node->GetComputedStyle(name.c_str()) * val->Value();
                        break;
                    }
                }
                return true;
            }
        }
        return false;
    }

    void AnimationPropComputedStyle::setNodeProperty(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        const TaroAnimationPropValue &prop_value) const {
        if (!node) {
            return;
        }
        const double *double_value = std::get_if<double>(&prop_value);
        if (double_value == nullptr) {
            return;
        }
        TARO_LOG_DEBUG("AnimationPropComputedStyle", "prop_type:%{public}d value:%{public}f",
                       prop_type, (*double_value));

        setLengthValue(prop_type, node, Dimension{*double_value, DimensionUnit::VP});
    }

    void AnimationPropComputedStyle::setKeyframeToNode(std::shared_ptr<TaroDOM::TaroRenderNode> node,
                                                       CSSProperty::Type prop_type,
                                                       const TaroCSSOM::TaroStylesheet::KeyframeValue &keyframe) const {
        if (!node) {
            return;
        }

        auto length_value = std::get_if<Dimension>(&keyframe);
        if (length_value == nullptr) {
            return;
        }

        setLengthValue(prop_type, node, *length_value);
    }

    void AnimationPropComputedStyle::setLengthValue(CSSProperty::Type prop_type,
                                                    std::shared_ptr<TaroDOM::TaroRenderNode> &node,
                                                    const Dimension &length_value) const {
        switch (prop_type) {
            case CSSProperty::Type::Width:
                node->SetWidth(length_value);
                break;
            case CSSProperty::Type::Height:
                node->SetHeight(length_value);
                break;
            case CSSProperty::Type::PaddingTop:
                node->SetPadding(length_value, TaroEdge::TaroEdgeTop);
                break;
            case CSSProperty::Type::PaddingRight:
                node->SetPadding(length_value, TaroEdge::TaroEdgeRight);
                break;
            case CSSProperty::Type::PaddingBottom:
                node->SetPadding(length_value, TaroEdge::TaroEdgeBottom);
                break;
            case CSSProperty::Type::PaddingLeft:
                node->SetPadding(length_value, TaroEdge::TaroEdgeLeft);
                break;
            case CSSProperty::Type::BorderTopWidth:
                node->SetBorder(length_value, TaroEdge::TaroEdgeTop);
                break;
            case CSSProperty::Type::BorderRightWidth:
                node->SetBorder(length_value, TaroEdge::TaroEdgeRight);
                break;
            case CSSProperty::Type::BorderBottomWidth:
                node->SetBorder(length_value, TaroEdge::TaroEdgeBottom);
                break;
            case CSSProperty::Type::BorderLeftWidth:
                node->SetBorder(length_value, TaroEdge::TaroEdgeLeft);
                break;
            case CSSProperty::Type::MarginTop:
                node->SetMargin(length_value, TaroEdge::TaroEdgeTop);
                break;
            case CSSProperty::Type::MarginRight:
                node->SetMargin(length_value, TaroEdge::TaroEdgeRight);
                break;
            case CSSProperty::Type::MarginBottom:
                node->SetMargin(length_value, TaroEdge::TaroEdgeBottom);
                break;
            case CSSProperty::Type::MarginLeft:
                node->SetMargin(length_value, TaroEdge::TaroEdgeLeft);
                break;
            case CSSProperty::Type::Top:
                node->SetPosition(length_value, TaroEdge::TaroEdgeTop);
                break;
            case CSSProperty::Type::Left:
                node->SetPosition(length_value, TaroEdge::TaroEdgeLeft);
                break;
            case CSSProperty::Type::Right:
                node->SetPosition(length_value, TaroEdge::TaroEdgeRight);
                break;
            case CSSProperty::Type::Bottom:
                node->SetPosition(length_value, TaroEdge::TaroEdgeBottom);
                break;
            default:
                break;
        }
    }

    // 设置style值到node节点
    void AnimationPropComputedStyle::resetStyleSheetProp(
        std::shared_ptr<TaroDOM::TaroRenderNode> node,
        CSSProperty::Type prop_type,
        TaroDOM::StylesheetRef style) const {
        switch (prop_type) {
            case CSSProperty::Type::Width:
                if (style->width.has_value()) {
                    node->SetWidth(style->width.value());
                }
                break;
            case CSSProperty::Type::Height:
                if (style->height.has_value()) {
                    node->SetHeight(style->height.value());
                }
                break;
            case CSSProperty::Type::PaddingTop:
                if (style->paddingTop.has_value()) {
                    node->SetPadding(style->paddingTop.value(), TaroEdge::TaroEdgeTop);
                }
                break;
            case CSSProperty::Type::PaddingRight:
                if (style->paddingRight.has_value()) {
                    node->SetPadding(style->paddingRight.value(), TaroEdge::TaroEdgeRight);
                }
                break;
            case CSSProperty::Type::PaddingBottom:
                if (style->paddingBottom.has_value()) {
                    node->SetPadding(style->paddingBottom.value(), TaroEdge::TaroEdgeBottom);
                    break;
                    case CSSProperty::Type::PaddingLeft:
                        if (style->paddingLeft.has_value()) {
                            node->SetPadding(style->paddingLeft.value(), TaroEdge::TaroEdgeLeft);
                        }
                        break;
                    case CSSProperty::Type::BorderTopWidth:
                        if (style->borderTopWidth.has_value()) {
                            node->SetBorder(style->borderTopWidth.value(), TaroEdge::TaroEdgeTop);
                        }
                        break;
                    case CSSProperty::Type::BorderRightWidth:
                        if (style->borderRightWidth.has_value()) {
                            node->SetBorder(style->borderRightWidth.value(), TaroEdge::TaroEdgeRight);
                        }
                        break;
                    case CSSProperty::Type::BorderBottomWidth:
                        if (style->borderBottomWidth.has_value())
                            node->SetBorder(style->borderBottomWidth.value(), TaroEdge::TaroEdgeBottom);
                }
                break;
            case CSSProperty::Type::BorderLeftWidth:
                if (style->borderLeftWidth.has_value()) {
                    node->SetBorder(style->borderLeftWidth.value(), TaroEdge::TaroEdgeLeft);
                }
                break;
            case CSSProperty::Type::MarginTop:
                if (style->marginTop.has_value()) {
                    node->SetMargin(style->marginTop.value(), TaroEdge::TaroEdgeTop);
                }
                break;
            case CSSProperty::Type::MarginRight:
                if (style->marginRight.has_value()) {
                    node->SetMargin(style->marginRight.value(), TaroEdge::TaroEdgeRight);
                }
                break;
            case CSSProperty::Type::MarginBottom:
                if (style->marginBottom.has_value()) {
                    node->SetMargin(style->marginBottom.value(), TaroEdge::TaroEdgeBottom);
                }
                break;
            case CSSProperty::Type::MarginLeft:
                if (style->marginLeft.has_value()) {
                    node->SetMargin(style->marginLeft.value(), TaroEdge::TaroEdgeLeft);
                }
                break;
            case CSSProperty::Type::Top:
                if (style->top.has_value()) {
                    node->SetPosition(style->top.value(), TaroEdge::TaroEdgeTop);
                }
                break;
            case CSSProperty::Type::Left:
                if (style->left.has_value()) {
                    node->SetPosition(style->left.value(), TaroEdge::TaroEdgeLeft);
                }
                break;
            case CSSProperty::Type::Right:
                if (style->right.has_value()) {
                    node->SetPosition(style->right.value(), TaroEdge::TaroEdgeRight);
                }
                break;
            case CSSProperty::Type::Bottom:
                if (style->bottom.has_value()) {
                    node->SetPosition(style->bottom.value(), TaroEdge::TaroEdgeBottom);
                }
                break;
            default:
                break;
        }
    }

    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::Width, AnimationPropComputedStyle, 1);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::Height, AnimationPropComputedStyle, 2);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::PaddingTop, AnimationPropComputedStyle, 3);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::PaddingRight, AnimationPropComputedStyle, 4);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::PaddingBottom, AnimationPropComputedStyle, 5);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::PaddingLeft, AnimationPropComputedStyle, 6);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::BorderTopWidth, AnimationPropComputedStyle, 7);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::BorderRightWidth, AnimationPropComputedStyle, 8);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::BorderBottomWidth, AnimationPropComputedStyle, 9);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::BorderLeftWidth, AnimationPropComputedStyle, 10);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::MarginTop, AnimationPropComputedStyle, 11);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::MarginRight, AnimationPropComputedStyle, 12);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::MarginBottom, AnimationPropComputedStyle, 13);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::MarginLeft, AnimationPropComputedStyle, 14);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::Top, AnimationPropComputedStyle, 15);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::Left, AnimationPropComputedStyle, 16);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::Right, AnimationPropComputedStyle, 17);
    ANIMATION_PROP_REGISTER_INDEX(CSSProperty::Type::Bottom, AnimationPropComputedStyle, 18);
} // namespace TaroAnimate
} // namespace TaroRuntime
