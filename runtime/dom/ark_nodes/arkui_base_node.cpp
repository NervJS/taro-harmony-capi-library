//
// Created on 2024/6/24.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "./arkui_base_node.h"

#include <stack>

#include "./differ/differ_macro.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dirty_vsync_task.h"
#include "runtime/dom/ark_nodes/stack.h"
#include "runtime/dom/ark_nodes/text.h"

namespace TaroRuntime {
namespace TaroDOM {

    static const std::unordered_map<TaroEdge, YGEdge> EDGE_MAPPING = {
        {TaroEdge::TaroEdgeAll, YGEdge::YGEdgeAll},
        {TaroEdge::TaroEdgeTop, YGEdge::YGEdgeTop},
        {TaroEdge::TaroEdgeLeft, YGEdge::YGEdgeLeft},
        {TaroEdge::TaroEdgeRight, YGEdge::YGEdgeRight},
        {TaroEdge::TaroEdgeBottom, YGEdge::YGEdgeBottom}};

    static YGEdge TaroEdge2YgEdgeMapping(const TaroEdge &edge) {
        auto iter = EDGE_MAPPING.find(edge);
        if (iter != EDGE_MAPPING.end()) {
            return iter->second;
        }
        return YGEdgeAll;
    }

    static constexpr auto to_underlying(TaroEdge e) noexcept {
        return static_cast<std::underlying_type_t<decltype(e)>>(e);
    }

    BaseRenderNode::BaseRenderNode()
        : ygNodeRef(TaroYogaApi::getInstance()->createYogaNodeRef()), paintDiffer_(this), layoutDiffer_(this) {}

    BaseRenderNode::~BaseRenderNode() {
        if (ygNodeRef) {
            YGNodeFree(ygNodeRef);
        }
    }

#define DIFF_STYLE_AND_SET(PROPERTY_ENUM, PROPERTY, SET_EXPR, SET_DEFAULT_EXPR)                                                     \
    {                                                                                                                               \
        if (oldStyleRef && oldStyleRef->PROPERTY.has_value()) {                                                                     \
            if (style->PROPERTY.has_value()) {                                                                                      \
                if (oldStyleRef->PROPERTY.value() != style->PROPERTY.value()) {                                                     \
                    if (OnSetPropertyIntoNode(PROPERTY_ENUM, TaroChange::Modified, style)) SET_EXPR;                                \
                }                                                                                                                   \
            } else {                                                                                                                \
                if (OnSetPropertyIntoNode(PROPERTY_ENUM, TaroChange::Modified, style)) SET_DEFAULT_EXPR;                            \
            }                                                                                                                       \
        } else {                                                                                                                    \
            if (style->PROPERTY.has_value()) {                                                                                      \
                if (OnSetPropertyIntoNode(PROPERTY_ENUM, !oldStyleRef ? TaroChange::Added : TaroChange::Modified, style)) SET_EXPR; \
            } else {                                                                                                                \
                SET_DEFAULT_EXPR;                                                                                                   \
            }                                                                                                                       \
        }                                                                                                                           \
    }

    void BaseRenderNode::SetNodeLayoutStyle(
        const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &style,
        const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &oldStyleRef,
        const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &parentStyleRef) {
        using namespace TaroCSSOM::TaroStylesheet;
        if (!style) return;
        // 设置yoga属性
        bool isFlex = false;

        DIFF_STYLE_AND_SET(
            CSSProperty::Display,
            display,
            SetDisplay(style->display.value(), oldStyleRef && oldStyleRef->display.has_value() ? oldStyleRef->display.value() : PropertyType::Display::UnKnown),
            SetDisplay(PropertyType::Display::Flex, oldStyleRef && oldStyleRef->display.has_value() ? oldStyleRef->display.value() : PropertyType::Display::UnKnown));
        if (style->display.has_value() && style->display.value() == PropertyType::Display::Flex) {
            isFlex = true;
            SetFlexDirection(style->flexDirection.value_or(ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_ROW));
            SetAlignItems(style->alignItems.value_or(ArkUI_ItemAlignment::ARKUI_ITEM_ALIGNMENT_START));
            SetJustifyContent(style->justifyContent.value_or(ArkUI_FlexAlignment::ARKUI_FLEX_ALIGNMENT_START));
            SetAlignContent(style->alignContent.value_or(ArkUI_FlexAlignment::ARKUI_FLEX_ALIGNMENT_START));
            SetFlexWrap(style->flexWrap.value_or(ArkUI_FlexWrap::ARKUI_FLEX_WRAP_NO_WRAP));
        } else {
            SetFlexDirection(ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_COLUMN);
            SetAlignItems(ArkUI_ItemAlignment::ARKUI_ITEM_ALIGNMENT_START);
            SetJustifyContent(ArkUI_FlexAlignment::ARKUI_FLEX_ALIGNMENT_START);
            SetAlignContent(ArkUI_FlexAlignment::ARKUI_FLEX_ALIGNMENT_START);
            SetFlexWrap(ArkUI_FlexWrap::ARKUI_FLEX_WRAP_NO_WRAP);
        }
        if (parentStyleRef) {
            const auto &parentDisplay = parentStyleRef->display;
            bool parentIsFlex = parentDisplay.has_value() && parentDisplay.value() == PropertyType::Display::Flex;

            // 模拟block拉满的逻辑
            ArkUI_ItemAlignment alignSelf = ARKUI_ITEM_ALIGNMENT_AUTO;
            if (parentIsFlex) {
                if (parentStyleRef->alignItems.has_value()) {
                    alignSelf = ARKUI_ITEM_ALIGNMENT_AUTO;
                } else if (parentStyleRef->flexWrap.has_value() && parentStyleRef->flexWrap.value() != ArkUI_FlexWrap::ARKUI_FLEX_WRAP_NO_WRAP) {
                    alignSelf = ARKUI_ITEM_ALIGNMENT_STRETCH;
                } else {
                    if (!std::dynamic_pointer_cast<TaroTextNode>(shared_from_this())) {
                        alignSelf = ARKUI_ITEM_ALIGNMENT_STRETCH;
                    }
                }
            } else {
                //                 if (std::dynamic_pointer_cast<TaroStackNode>(shared_from_this())) {
                if (!style->width.has_value() || style->width.value().Unit() == DimensionUnit::AUTO) {
                    alignSelf = ARKUI_ITEM_ALIGNMENT_STRETCH;
                }
                //                 }
            }

            bool LeaveDocFlow = false;
            if (style->position.has_value() && (style->position.value() == PropertyType::Position::Absolute || style->position.value() == PropertyType::Position::Fixed)) {
                LeaveDocFlow = true;
            }

            DIFF_STYLE_AND_SET(
                CSSProperty::AlignSelf,
                alignSelf,
                parentIsFlex && !LeaveDocFlow ? SetAlignSelf(style->alignSelf.value()) : SetAlignSelf(ARKUI_ITEM_ALIGNMENT_AUTO),
                SetAlignSelf(alignSelf));

            DIFF_STYLE_AND_SET(
                CSSProperty::FlexGrow,
                flexGrow,
                parentIsFlex && !LeaveDocFlow ? SetFlexGrow(style->flexGrow.value()) : SetFlexGrow(0),
                SetFlexGrow(0));
            DIFF_STYLE_AND_SET(
                CSSProperty::FlexBasis,
                flexBasis,
                parentIsFlex && !LeaveDocFlow ? SetFlexBasis(style->flexBasis.value()) : SetFlexBasis(Dimension{0, DimensionUnit::AUTO}),
                SetFlexBasis(Dimension{0, DimensionUnit::AUTO}));
            DIFF_STYLE_AND_SET(
                CSSProperty::FlexShrink,
                flexShrink,
                parentIsFlex && !LeaveDocFlow ? SetFlexShrink(style->flexShrink.value()) : SetFlexShrink(0),
                SetFlexShrink(0));
        }

        // verticalAlign会影响文本的对齐方式，这里采用align-items来处理；
        DIFF_STYLE_AND_SET(CSSProperty::VerticalAlign, verticalAlign,
                           {
                               if (
                                   style->verticalAlign.has_value() &&
                                   !style->alignItems.has_value() &&
                                   YGNodeStyleGetFlexDirection(ygNodeRef) == YGFlexDirection::YGFlexDirectionRow) {
                                   switch (style->verticalAlign.value()) {
                                       case ALIGNMENT_OFFSET_AT_BASELINE: {
                                           SetAlignItems(ARKUI_ITEM_ALIGNMENT_BASELINE);
                                           break;
                                       }
                                       case ALIGNMENT_ABOVE_BASELINE: {
                                           SetAlignItems(ARKUI_ITEM_ALIGNMENT_START);
                                           break;
                                       }
                                       case ALIGNMENT_BELOW_BASELINE: {
                                           SetAlignItems(ARKUI_ITEM_ALIGNMENT_BASELINE);
                                           break;
                                       }
                                       case ALIGNMENT_CENTER_OF_ROW_BOX: {
                                           SetAlignItems(ARKUI_ITEM_ALIGNMENT_CENTER);
                                           break;
                                       }
                                       case ALIGNMENT_TOP_OF_ROW_BOX: {
                                           SetAlignItems(ARKUI_ITEM_ALIGNMENT_START);
                                           break;
                                       }
                                       case ALIGNMENT_BOTTOM_OF_ROW_BOX: {
                                           SetAlignItems(ARKUI_ITEM_ALIGNMENT_END);
                                           break;
                                       }
                                       default: {
                                       }
                                   }
                               }
                           },
                           {});

        if (style->position.has_value()) {
            DIFF_STYLE_AND_SET(
                CSSProperty::Position,
                position,
                SetPosition(style->position.value()),
                SetPosition(PropertyType::Position::Static));
            SetPosition(style->top.value_or(style->bottom.has_value() ? YGUndefined : 0.0), TaroEdge::TaroEdgeTop);
            SetPosition(style->right.value_or(YGUndefined), TaroEdge::TaroEdgeRight);
            SetPosition(style->bottom.value_or(YGUndefined), TaroEdge::TaroEdgeBottom);
            SetPosition(style->left.value_or(style->right.has_value() ? YGUndefined : 0.0), TaroEdge::TaroEdgeLeft);
        } else {
            DIFF_STYLE_AND_SET(
                CSSProperty::Position,
                position,
                SetPosition(PropertyType::Position::Static),
                SetPosition(PropertyType::Position::Static));
        }

        DIFF_STYLE_AND_SET(
            CSSProperty::MarginTop,
            marginTop,
            SetMargin(style->marginTop.value(), TaroEdge::TaroEdgeTop),
            SetMargin(Dimension{YGUndefined}, TaroEdge::TaroEdgeTop));
        DIFF_STYLE_AND_SET(
            CSSProperty::MarginRight,
            marginRight,
            SetMargin(style->marginRight.value(), TaroEdge::TaroEdgeRight),
            SetMargin(Dimension{YGUndefined}, TaroEdge::TaroEdgeRight));
        DIFF_STYLE_AND_SET(
            CSSProperty::MarginBottom,
            marginBottom,
            SetMargin(style->marginBottom.value(), TaroEdge::TaroEdgeBottom),
            SetMargin(Dimension{YGUndefined}, TaroEdge::TaroEdgeBottom));
        DIFF_STYLE_AND_SET(
            CSSProperty::MarginLeft,
            marginLeft,
            SetMargin(style->marginLeft.value(), TaroEdge::TaroEdgeLeft),
            SetMargin(Dimension{YGUndefined}, TaroEdge::TaroEdgeLeft));

        DIFF_STYLE_AND_SET(
            CSSProperty::PaddingTop,
            paddingTop,
            SetPadding(style->paddingTop.value(), TaroEdge::TaroEdgeTop),
            SetPadding(Dimension{YGUndefined}, TaroEdge::TaroEdgeTop));
        DIFF_STYLE_AND_SET(
            CSSProperty::PaddingRight,
            paddingRight,
            SetPadding(style->paddingRight.value(), TaroEdge::TaroEdgeRight),
            SetPadding(Dimension{YGUndefined}, TaroEdge::TaroEdgeRight));
        DIFF_STYLE_AND_SET(
            CSSProperty::PaddingBottom,
            paddingBottom,
            SetPadding(style->paddingBottom.value(), TaroEdge::TaroEdgeBottom),
            SetPadding(Dimension{YGUndefined}, TaroEdge::TaroEdgeBottom));
        DIFF_STYLE_AND_SET(
            CSSProperty::PaddingLeft,
            paddingLeft,
            SetPadding(style->paddingLeft.value(), TaroEdge::TaroEdgeLeft),
            SetPadding(Dimension{YGUndefined}, TaroEdge::TaroEdgeLeft));

        DIFF_STYLE_AND_SET(
            CSSProperty::BorderTopWidth,
            borderTopWidth,
            SetBorder(style->borderTopWidth.value(), TaroEdge::TaroEdgeTop),
            SetBorder(Dimension{YGUndefined}, TaroEdge::TaroEdgeTop));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderRightWidth,
            borderRightWidth,
            SetBorder(style->borderRightWidth.value(), TaroEdge::TaroEdgeRight),
            SetBorder(Dimension{YGUndefined}, TaroEdge::TaroEdgeRight));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderBottomWidth,
            borderBottomWidth,
            SetBorder(style->borderBottomWidth.value(), TaroEdge::TaroEdgeBottom),
            SetBorder(Dimension{YGUndefined}, TaroEdge::TaroEdgeBottom));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderLeftWidth,
            borderLeftWidth,
            SetBorder(style->borderLeftWidth.value(), TaroEdge::TaroEdgeLeft),
            SetBorder(Dimension{YGUndefined}, TaroEdge::TaroEdgeLeft));

        DIFF_STYLE_AND_SET(
            CSSProperty::Width,
            width,
            SetWidth(style->width.value()),
            SetWidth(Dimension{0, DimensionUnit::AUTO}));
        DIFF_STYLE_AND_SET(
            CSSProperty::Height,
            height,
            SetHeight(style->height.value(), oldStyleRef && oldStyleRef->height.has_value() ? oldStyleRef->height.value() : Dimension{0, DimensionUnit::NONE}),
            SetHeight(Dimension{0, DimensionUnit::AUTO}, oldStyleRef && oldStyleRef->height.has_value() ? oldStyleRef->height.value() : Dimension{0, DimensionUnit::NONE}));
        DIFF_STYLE_AND_SET(
            CSSProperty::MinWidth,
            minWidth,
            SetMinWidth(style->minWidth.value()),
            SetMinWidth(Dimension{YGUndefined}));
        DIFF_STYLE_AND_SET(
            CSSProperty::MaxWidth,
            maxWidth,
            SetMaxWidth(style->maxWidth.value()),
            SetMaxWidth(Dimension{YGUndefined}));
        DIFF_STYLE_AND_SET(
            CSSProperty::MinHeight,
            minHeight,
            SetMinHeight(style->minHeight.value()),
            SetMinHeight(Dimension{YGUndefined}));
        DIFF_STYLE_AND_SET(
            CSSProperty::MaxHeight,
            maxHeight,
            SetMaxHeight(style->maxHeight.value()),
            SetMaxHeight(Dimension{YGUndefined}));

        DIFF_STYLE_AND_SET(
            CSSProperty::Overflow,
            overflow,
            SetOverflow(style->overflow.value()),
            SetOverflow(PropertyType::Overflow::Visible));

        YGNodeStyleSetDirection(ygNodeRef, YGDirectionLTR);
    }

    void BaseRenderNode::SetNodeDrawStyle(
        const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &style,
        const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &oldStyleRef) {
        // 绘制样式
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderTopStyle,
            borderTopStyle,
            SetBorderStyle(style->borderTopStyle.value(), TaroEdge::TaroEdgeTop),
            ResetBorderStyle(TaroEdge::TaroEdgeTop));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderRightStyle,
            borderRightStyle,
            SetBorderStyle(style->borderRightStyle.value(), TaroEdge::TaroEdgeRight),
            ResetBorderStyle(TaroEdge::TaroEdgeRight));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderBottomStyle,
            borderBottomStyle,
            SetBorderStyle(style->borderBottomStyle.value(), TaroEdge::TaroEdgeBottom),
            ResetBorderStyle(TaroEdge::TaroEdgeBottom));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderLeftStyle,
            borderLeftStyle,
            SetBorderStyle(style->borderLeftStyle.value(), TaroEdge::TaroEdgeLeft),
            ResetBorderStyle(TaroEdge::TaroEdgeLeft));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderTopLeftRadius,
            borderTopLeftRadius,
            SetBorderRadius(style->borderTopLeftRadius.value(), TaroEdge::TaroEdgeTop),
            ResetBorderRadius(TaroEdge::TaroEdgeTop));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderTopRightRadius,
            borderTopRightRadius,
            SetBorderRadius(style->borderTopRightRadius.value(), TaroEdge::TaroEdgeRight),
            ResetBorderRadius(TaroEdge::TaroEdgeRight));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderBottomRightRadius,
            borderBottomRightRadius,
            SetBorderRadius(style->borderBottomRightRadius.value(), TaroEdge::TaroEdgeBottom),
            ResetBorderRadius(TaroEdge::TaroEdgeBottom));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderBottomLeftRadius,
            borderBottomLeftRadius,
            SetBorderRadius(style->borderBottomLeftRadius.value(), TaroEdge::TaroEdgeLeft),
            ResetBorderRadius(TaroEdge::TaroEdgeLeft));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderTopColor,
            borderTopColor,
            SetBorderColor(style->borderTopColor.value(), TaroEdge::TaroEdgeTop),
            ResetBorderColor(TaroEdge::TaroEdgeTop));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderRightColor,
            borderRightColor,
            SetBorderColor(style->borderRightColor.value(), TaroEdge::TaroEdgeRight),
            ResetBorderColor(TaroEdge::TaroEdgeRight));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderBottomColor,
            borderBottomColor,
            SetBorderColor(style->borderBottomColor.value(), TaroEdge::TaroEdgeBottom),
            ResetBorderColor(TaroEdge::TaroEdgeBottom));
        DIFF_STYLE_AND_SET(
            CSSProperty::BorderLeftColor,
            borderLeftColor,
            SetBorderColor(style->borderLeftColor.value(), TaroEdge::TaroEdgeLeft),
            ResetBorderColor(TaroEdge::TaroEdgeLeft));
        DIFF_STYLE_AND_SET(
            CSSProperty::BackgroundColor,
            backgroundColor,
            SetBackgroundColor(style->backgroundColor.value()),
            ResetBackgroundColor());
        DIFF_STYLE_AND_SET(
            CSSProperty::BackgroundRepeat,
            backgroundRepeat,
            SetBackgroundRepeat(style->backgroundRepeat.value()),
            ResetBackgroundRepeat());

        DIFF_STYLE_AND_SET(
            CSSProperty::BackgroundImage,
            backgroundImage,
            SetBackgroundImage(style->backgroundImage.value()),
            ResetBackgroundImage());
        DIFF_STYLE_AND_SET(
            CSSProperty::BackgroundSize,
            backgroundSize,
            SetBackgroundSize(style->backgroundSize.value()),
            ResetBackgroundSize());
        DIFF_STYLE_AND_SET(
            CSSProperty::BackgroundPositionX,
            backgroundPositionX,
            SetBackgroundPositionX(style->backgroundPositionX.value()),
            ResetBackgroundPositionX());
        DIFF_STYLE_AND_SET(
            CSSProperty::BackgroundPositionY,
            backgroundPositionY,
            SetBackgroundPositionY(style->backgroundPositionY.value()),
            ResetBackgroundPositionY());
        DIFF_STYLE_AND_SET(
            CSSProperty::Opacity,
            opacity,
            SetOpacity(style->opacity.value()),
            ResetOpacity());
        DIFF_STYLE_AND_SET(
            CSSProperty::Visibility,
            visibility,
            SetVisibility(style->visibility.value()),
            ResetVisibility());
        DIFF_STYLE_AND_SET(
            CSSProperty::Transform,
            transform,
            SetTransform(style->transform.value()),
            ResetTransform());
        DIFF_STYLE_AND_SET(
            CSSProperty::TransformOrigin,
            transformOrigin,
            SetTransformOrigin(style->transformOrigin.value()),
            ResetTransformOrigin());
        DIFF_STYLE_AND_SET(
            CSSProperty::PointerEvents,
            pointerEvents,
            SetPointerEvents(style->pointerEvents.value()),
            ResetPointerEvents());
        DIFF_STYLE_AND_SET(
            CSSProperty::ZIndex,
            zIndex,
            SetZIndex(style->zIndex.value()),
            ResetZIndex());
        DIFF_STYLE_AND_SET(
            CSSProperty::BoxShadow,
            boxShadow,
            SetBoxShadow(style->boxShadow.value()),
            ResetBoxShadow());
    }

    void BaseRenderNode::SetFontDrawStyle(
        const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &style,
        const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &oldStyleRef) {
        DIFF_STYLE_AND_SET(
            CSSProperty::Color,
            color,
            SetColor(style->color.value()),
            ResetColor());
        DIFF_STYLE_AND_SET(
            CSSProperty::FontSize,
            fontSize,
            SetFontSize(style->fontSize.value()),
            ResetFontSize());
        DIFF_STYLE_AND_SET(
            CSSProperty::FontWeight,
            fontWeight,
            SetFontWeight(style->fontWeight.value()),
            ResetFontWeight());
        DIFF_STYLE_AND_SET(
            CSSProperty::FontStyle,
            fontStyle,
            SetFontStyle(style->fontStyle.value()),
            ResetFontStyle());
        DIFF_STYLE_AND_SET(
            CSSProperty::FontFamily,
            fontFamily,
            SetFontFamily(style->fontFamily.value()),
            ResetFontFamily());
        DIFF_STYLE_AND_SET(
            CSSProperty::LineHeight,
            lineHeight,
            SetLineHeight(style->lineHeight.value()),
            ResetLineHeight());
        DIFF_STYLE_AND_SET(
            CSSProperty::LetterSpacing,
            letterSpacing,
            SetLetterSpacing(style->letterSpacing.value()),
            ResetLetterSpacing());
        DIFF_STYLE_AND_SET(
            CSSProperty::WordBreak,
            wordBreak,
            SetWordBreak(style->wordBreak.value()),
            ResetWordBreak());
        DIFF_STYLE_AND_SET(
            CSSProperty::TextAlign,
            textAlign,
            SetTextAlign(style->textAlign.value()),
            ResetTextAlign());
        DIFF_STYLE_AND_SET(
            CSSProperty::WebkitLineClamp,
            webkitLineClamp,
            SetWebkitLineClamp(style->webkitLineClamp.value()),
            ResetWebkitLineClamp());
        DIFF_STYLE_AND_SET(
            CSSProperty::WhiteSpace,
            whiteSpace,
            SetWhiteSpace(style->whiteSpace.value()),
            ResetWhiteSpace());
        DIFF_STYLE_AND_SET(
            CSSProperty::TextDecorationLine,
            textDecorationLine,
            SetTextDecorationLine(style->textDecorationLine.value()),
            ResetTextDecorationLine());
        DIFF_STYLE_AND_SET(
            CSSProperty::TextDecorationStyle,
            textDecorationStyle,
            SetTextDecorationLineStyle(style->textDecorationStyle.value()),
            ResetTextDecorationLineStyle());
        DIFF_STYLE_AND_SET(
            CSSProperty::TextOverflow,
            textOverflow,
            SetTextOverflow(style->textOverflow.value()),
            ResetTextOverflow());
    }

    void BaseRenderNode::SetLayoutDirty(const bool &val) {
        if (is_layout_dirty_ == val) return;
        is_layout_dirty_ = val;
        // 启动监听下一个vsync时，进行排版布局
        if (is_layout_dirty_) {
            RegistryLayoutTaskOnNextVsync();
        }
    }
    void BaseRenderNode::SetDrawDirty(const bool &val) {
        if (is_draw_dirty_ == val) return;
        is_draw_dirty_ = val;
        // 启动监听下一个vsync，进行绘制
        if (is_draw_dirty_) {
            RegistryDrawTaskOnNextVsync();
        }
    }
    bool BaseRenderNode::GetLayoutDirty() {
        return is_layout_dirty_;
    }

    bool BaseRenderNode::GetDrawDirty() {
        return is_draw_dirty_;
    }

    void BaseRenderNode::RegistryLayoutTaskOnNextVsync() {
        if (auto self = shared_from_this()) {
            auto render_node = std::static_pointer_cast<TaroRenderNode>(self);
#if IS_DEBUG
            auto el = render_node->element_ref_.lock();
            TARO_LOG_DEBUG("dirty", "nid %{public}d nodeName:%{public}s %{public}s",
                           render_node->element_nid_, render_node->element_node_name_.c_str(),
                           el ? el->class_name_.c_str() : "<el-released>");
#endif
            if (auto element = render_node->element_ref_.lock()) {
                if (auto measureRoot = element->context_->measure_root_.lock()) {
                    DirtyTaskPipeline::GetInstance()->AddDirtyLayoutNode(measureRoot->GetHeadRenderNode());
                }
            }
        }
    }

    void BaseRenderNode::RegistryDrawTaskOnNextVsync() {
        if (auto self = shared_from_this()) {
            DirtyTaskPipeline::GetInstance()->AddDirtyPaintNode(std::static_pointer_cast<TaroRenderNode>(self));
        }
    }

    void BaseRenderNode::CheckIfYGDirty() {
        if (YGNodeIsDirty(ygNodeRef)) {
            //             SetLayoutPropertyDirty(propertyName);
            SetLayoutDirty(true);
        }
    }

    bool BaseRenderNode::OnSetPropertyIntoNode(const CSSProperty::Type &property,
                                               const TaroChange &changeType,
                                               const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &style) {
        // true: 设值到render node上
        return true;
    }

    void BaseRenderNode::SetDisplay(const PropertyType::Display &val, const PropertyType::Display &oldVal) {
        if (val == PropertyType::Display::None) {
            YGNodeStyleSetDisplay(ygNodeRef, YGDisplayNone);
        } else {
            YGNodeStyleSetDisplay(ygNodeRef, YGDisplayFlex);
        }
        OnDisplayChange(val, oldVal);
        // 从 none 切到非 none，需要找到带有自定义测量函数的子节点标脏
        if (oldVal == PropertyType::Display::None && val != PropertyType::Display::None) {
            TaroYogaApi::getInstance()->markDirtyNodesWithCustomMeasureFunction(ygNodeRef);
        }
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetPosition(const PropertyType::Position &val) {
        switch (val) {
            case PropertyType::Position::Absolute:
            case PropertyType::Position::Sticky:
            case PropertyType::Position::Fixed: {
                YGNodeStyleSetPositionType(ygNodeRef, YGPositionTypeAbsolute);
                break;
            }
            case PropertyType::Position::Relative: {
                YGNodeStyleSetPositionType(ygNodeRef, YGPositionTypeRelative);
                break;
            }
            default: {
                YGNodeStyleSetPositionType(ygNodeRef, YGPositionTypeStatic);
                break;
            }
        }
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetOverflow(const PropertyType::Overflow &val) {
        switch (val) {
            case PropertyType::Overflow::Hidden:
                YGNodeStyleSetOverflow(ygNodeRef, YGOverflowHidden);
                break;
            case PropertyType::Overflow::Visible:
                YGNodeStyleSetOverflow(ygNodeRef, YGOverflowVisible);
                break;
            case PropertyType::Overflow::Scroll:
                YGNodeStyleSetOverflow(ygNodeRef, YGOverflowScroll);
                break;
            default:
                YGNodeStyleSetOverflow(ygNodeRef, YGOverflowVisible);
                break;
        }
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetAlignContent(const ArkUI_FlexAlignment &val) {
        switch (val) {
            case ARKUI_FLEX_ALIGNMENT_START:
                YGNodeStyleSetAlignContent(ygNodeRef, YGAlignFlexStart);
                break;
            case ARKUI_FLEX_ALIGNMENT_CENTER:
                YGNodeStyleSetAlignContent(ygNodeRef, YGAlignCenter);
                break;
            case ARKUI_FLEX_ALIGNMENT_END:
                YGNodeStyleSetAlignContent(ygNodeRef, YGAlignFlexEnd);
                break;
            case ARKUI_FLEX_ALIGNMENT_SPACE_BETWEEN:
                YGNodeStyleSetAlignContent(ygNodeRef, YGAlignSpaceBetween);
                break;
            case ARKUI_FLEX_ALIGNMENT_SPACE_AROUND:
                YGNodeStyleSetAlignContent(ygNodeRef, YGAlignSpaceAround);
                break;
            case ARKUI_FLEX_ALIGNMENT_SPACE_EVENLY:
                YGNodeStyleSetAlignContent(ygNodeRef, YGAlignSpaceEvenly);
                break;
            default:
                YGNodeStyleSetAlignContent(ygNodeRef, YGAlignFlexStart);
                break;
        }
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetAlignSelf(const ArkUI_ItemAlignment &val) {
        switch (val) {
            case ARKUI_ITEM_ALIGNMENT_START:
                YGNodeStyleSetAlignSelf(ygNodeRef, YGAlign::YGAlignFlexStart);
                break;
            case ARKUI_ITEM_ALIGNMENT_CENTER:
                YGNodeStyleSetAlignSelf(ygNodeRef, YGAlign::YGAlignCenter);
                break;
            case ARKUI_ITEM_ALIGNMENT_END:
                YGNodeStyleSetAlignSelf(ygNodeRef, YGAlign::YGAlignFlexEnd);
                break;
            case ARKUI_ITEM_ALIGNMENT_STRETCH:
                YGNodeStyleSetAlignSelf(ygNodeRef, YGAlign::YGAlignStretch);
                break;
            case ARKUI_ITEM_ALIGNMENT_BASELINE:
                YGNodeStyleSetAlignSelf(ygNodeRef, YGAlign::YGAlignBaseline);
                break;
            default:
                YGNodeStyleSetAlignSelf(ygNodeRef, YGAlign::YGAlignAuto);
                break;
        }
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetFlexGrow(const float &val) {
        YGNodeStyleSetFlexGrow(ygNodeRef, val);
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetFlexBasis(const Dimension &val) {
        switch (val.Unit()) {
            case DimensionUnit::VP: {
                if (val.Value() == 0.0f) {
                    YGNodeStyleSetFlexBasis(ygNodeRef, YGUndefined);
                } else {
                    YGNodeStyleSetFlexBasis(ygNodeRef, val.Value());
                }
                break;
            }
            case DimensionUnit::PX:
            case DimensionUnit::SAFE_AREA:
            case DimensionUnit::DESIGN_PX: {
                YGNodeStyleSetFlexBasis(ygNodeRef, val.ConvertToVp());
                break;
            }
            case DimensionUnit::PERCENT: {
                if (val.Value() == 0.0f) {
                    YGNodeStyleSetFlexBasis(ygNodeRef, YGUndefined);
                } else {
                    YGNodeStyleSetFlexBasisPercent(ygNodeRef, val.Value() * 100);
                }
                break;
            }
            default: {
                YGNodeStyleSetFlexBasisAuto(ygNodeRef);
                break;
            }
        }
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetFlexShrink(const float &val) {
        YGNodeStyleSetFlexShrink(ygNodeRef, val);
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetFlexDirection(const ArkUI_FlexDirection &val) {
        switch (val) {
            case ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_COLUMN:
                YGNodeStyleSetFlexDirection(ygNodeRef, YGFlexDirectionColumn);
                break;
            case ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_ROW_REVERSE:
                YGNodeStyleSetFlexDirection(ygNodeRef,
                                            YGFlexDirectionRowReverse);
                break;
            case ArkUI_FlexDirection::ARKUI_FLEX_DIRECTION_COLUMN_REVERSE:
                YGNodeStyleSetFlexDirection(ygNodeRef,
                                            YGFlexDirectionColumnReverse);
                break;
            default:
                YGNodeStyleSetFlexDirection(ygNodeRef, YGFlexDirectionRow);
                break;
        }
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetAlignItems(const ArkUI_ItemAlignment &val) {
        switch (val) {
            case ARKUI_ITEM_ALIGNMENT_AUTO:
                YGNodeStyleSetAlignItems(ygNodeRef, YGAlign::YGAlignAuto);
                break;
            case ARKUI_ITEM_ALIGNMENT_START:
                YGNodeStyleSetAlignItems(ygNodeRef, YGAlign::YGAlignFlexStart);
                break;
            case ARKUI_ITEM_ALIGNMENT_CENTER:
                YGNodeStyleSetAlignItems(ygNodeRef, YGAlign::YGAlignCenter);
                break;
            case ARKUI_ITEM_ALIGNMENT_END:
                YGNodeStyleSetAlignItems(ygNodeRef, YGAlign::YGAlignFlexEnd);
                break;
            case ARKUI_ITEM_ALIGNMENT_STRETCH:
                YGNodeStyleSetAlignItems(ygNodeRef, YGAlign::YGAlignStretch);
                break;
            case ARKUI_ITEM_ALIGNMENT_BASELINE:
                YGNodeStyleSetAlignItems(ygNodeRef, YGAlign::YGAlignBaseline);
                break;
            default:
                YGNodeStyleSetAlignItems(ygNodeRef, YGAlign::YGAlignFlexStart);
                break;
        }
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetJustifyContent(const ArkUI_FlexAlignment &val) {
        switch (val) {
            case ARKUI_FLEX_ALIGNMENT_START:
                YGNodeStyleSetJustifyContent(ygNodeRef, YGJustifyFlexStart);
                break;
            case ARKUI_FLEX_ALIGNMENT_CENTER:
                YGNodeStyleSetJustifyContent(ygNodeRef, YGJustifyCenter);
                break;
            case ARKUI_FLEX_ALIGNMENT_END:
                YGNodeStyleSetJustifyContent(ygNodeRef, YGJustifyFlexEnd);
                break;
            case ARKUI_FLEX_ALIGNMENT_SPACE_BETWEEN:
                YGNodeStyleSetJustifyContent(ygNodeRef, YGJustifySpaceBetween);
                break;
            case ARKUI_FLEX_ALIGNMENT_SPACE_AROUND:
                YGNodeStyleSetJustifyContent(ygNodeRef, YGJustifySpaceAround);
                break;
            case ARKUI_FLEX_ALIGNMENT_SPACE_EVENLY:
                YGNodeStyleSetJustifyContent(ygNodeRef, YGJustifySpaceEvenly);
                break;
            default:
                YGNodeStyleSetJustifyContent(ygNodeRef, YGJustifyFlexStart);
                break;
        }
        CheckIfYGDirty();
    }

    void BaseRenderNode::SetFlexWrap(const ArkUI_FlexWrap &val) {
        switch (val) {
            case ARKUI_FLEX_WRAP_NO_WRAP:
                YGNodeStyleSetFlexWrap(ygNodeRef, YGWrapNoWrap);
                break;
            case ARKUI_FLEX_WRAP_WRAP:
                YGNodeStyleSetFlexWrap(ygNodeRef, YGWrapWrap);
                break;
            case ARKUI_FLEX_WRAP_WRAP_REVERSE:
                YGNodeStyleSetFlexWrap(ygNodeRef, YGWrapWrapReverse);
                break;
            default:
                YGNodeStyleSetFlexWrap(ygNodeRef, YGWrapNoWrap);
                break;
        }
        CheckIfYGDirty();
    }

#define SET_LENGTH_VALUE_PROPERTY(PX_FUNCTION, PERCENT_FUNCTION, CALC_FUNCTION)                                                             \
    {                                                                                                                                       \
        switch (val.Unit()) {                                                                                                               \
            case DimensionUnit::VP:                                                                                                         \
            case DimensionUnit::PX:                                                                                                         \
            case DimensionUnit::VW:                                                                                                         \
            case DimensionUnit::VH:                                                                                                         \
            case DimensionUnit::SAFE_AREA:                                                                                                  \
            case DimensionUnit::DESIGN_PX: {                                                                                                \
                PX_FUNCTION(ygNodeRef, val.ConvertToVp());                                                                                  \
                break;                                                                                                                      \
            }                                                                                                                               \
            case DimensionUnit::PERCENT: {                                                                                                  \
                PERCENT_FUNCTION(ygNodeRef, val.Value() * 100);                                                                             \
                break;                                                                                                                      \
            }                                                                                                                               \
            case DimensionUnit::CALC: {                                                                                                     \
                auto context = GetDimensionContext();                                                                                       \
                \ 
                CALC_FUNCTION(ygNodeRef, val.GetCalcExpression(), context->design_ratio_, context->device_width_, context->device_height_); \
                break;                                                                                                                      \
            }                                                                                                                               \
            default: {                                                                                                                      \
                PX_FUNCTION(ygNodeRef, YGUndefined);                                                                                        \
                break;                                                                                                                      \
            }                                                                                                                               \
        }                                                                                                                                   \
        CheckIfYGDirty();                                                                                                                   \
    }

    void BaseRenderNode::SetMinHeight(const Dimension &val) {
        SET_LENGTH_VALUE_PROPERTY(YGNodeStyleSetMinHeight, YGNodeStyleSetMinHeightPercent, YGNodeStyleSetMinHeightCalc);
    }

    void BaseRenderNode::SetMaxHeight(const Dimension &val) {
        SET_LENGTH_VALUE_PROPERTY(YGNodeStyleSetMaxHeight, YGNodeStyleSetMaxHeightPercent, YGNodeStyleSetMaxHeightCalc);
    }

    void BaseRenderNode::SetMinWidth(const Dimension &val) {
        SET_LENGTH_VALUE_PROPERTY(YGNodeStyleSetMinWidth, YGNodeStyleSetMinWidthPercent, YGNodeStyleSetMinWidthCalc);
    }

    void BaseRenderNode::SetMaxWidth(const Dimension &val) {
        SET_LENGTH_VALUE_PROPERTY(YGNodeStyleSetMaxWidth, YGNodeStyleSetMaxWidthPercent, YGNodeStyleSetMaxWidthCalc);
    }

#define SET_LENGTH_VALUE_PROPERTY_WITH_AUTO(PX_FUNCTION, PERCENT_FUNCTION, CALC_FUCNTION, AUTO_FUNCTION)                                    \
    {                                                                                                                                       \
        switch (val.Unit()) {                                                                                                               \
            case DimensionUnit::VP:                                                                                                         \
            case DimensionUnit::PX:                                                                                                         \
            case DimensionUnit::VW:                                                                                                         \
            case DimensionUnit::VH:                                                                                                         \
            case DimensionUnit::SAFE_AREA:                                                                                                  \
            case DimensionUnit::DESIGN_PX: {                                                                                                \
                PX_FUNCTION(ygNodeRef, val.ConvertToVp());                                                                                  \
                break;                                                                                                                      \
            }                                                                                                                               \
            case DimensionUnit::PERCENT: {                                                                                                  \
                PERCENT_FUNCTION(ygNodeRef, val.Value() * 100);                                                                             \
                break;                                                                                                                      \
            }                                                                                                                               \
            case DimensionUnit::CALC: {                                                                                                     \
                auto context = GetDimensionContext();                                                                                       \
                \ 
                CALC_FUCNTION(ygNodeRef, val.GetCalcExpression(), context->design_ratio_, context->device_width_, context->device_height_); \
                break;                                                                                                                      \
            }                                                                                                                               \
            default: {                                                                                                                      \
                AUTO_FUNCTION(ygNodeRef);                                                                                                   \
                break;                                                                                                                      \
            }                                                                                                                               \
        }                                                                                                                                   \
        CheckIfYGDirty();                                                                                                                   \
    }

    void BaseRenderNode::SetHeight(const Dimension &val, const Dimension &oldVal) {
        // 高度从 0 切到非 0，需要找到带有自定义测量函数的子节点标脏
        if (oldVal.IsValid() && val.IsValid()) {
            if (oldVal.IsNonPositive() && val.IsNonZero()) {
                TaroYogaApi::getInstance()->markDirtyNodesWithCustomMeasureFunction(ygNodeRef);
            }
        }
        SET_LENGTH_VALUE_PROPERTY_WITH_AUTO(YGNodeStyleSetHeight, YGNodeStyleSetHeightPercent, YGNodeStyleSetHeightCalc, YGNodeStyleSetHeightAuto);
    }

    void BaseRenderNode::SetWidth(const Dimension &val) {
        SET_LENGTH_VALUE_PROPERTY_WITH_AUTO(YGNodeStyleSetWidth, YGNodeStyleSetWidthPercent, YGNodeStyleSetWidthCalc, YGNodeStyleSetWidthAuto);
    }

#define SET_LENGTH_VALUE_PROPERTY_WITH_EDGE(PX_FUNCTION, PERCENT_FUNCTION, CALC_FUNCTION, EDGE)                                                   \
    {                                                                                                                                             \
        switch (val.Unit()) {                                                                                                                     \
            case DimensionUnit::VP:                                                                                                               \
            case DimensionUnit::PX:                                                                                                               \
            case DimensionUnit::VW:                                                                                                               \
            case DimensionUnit::VH:                                                                                                               \
            case DimensionUnit::SAFE_AREA:                                                                                                        \
            case DimensionUnit::DESIGN_PX: {                                                                                                      \
                PX_FUNCTION(ygNodeRef, EDGE, val.ConvertToVp());                                                                                  \
                break;                                                                                                                            \
            }                                                                                                                                     \
            case DimensionUnit::PERCENT: {                                                                                                        \
                PERCENT_FUNCTION(ygNodeRef, EDGE, val.Value() * 100);                                                                             \
                break;                                                                                                                            \
            }                                                                                                                                     \
            case DimensionUnit::CALC: {                                                                                                           \
                auto context = GetDimensionContext();                                                                                             \
                \ 
                CALC_FUNCTION(ygNodeRef, EDGE, val.GetCalcExpression(), context->design_ratio_, context->device_width_, context->device_height_); \
                break;                                                                                                                            \
            }                                                                                                                                     \
            default: {                                                                                                                            \
                PX_FUNCTION(ygNodeRef, EDGE, YGUndefined);                                                                                        \
                break;                                                                                                                            \
            }                                                                                                                                     \
        }                                                                                                                                         \
        CheckIfYGDirty();                                                                                                                         \
    }

    void BaseRenderNode::SetPadding(const Dimension &val, const TaroEdge &edge) {
        SET_LENGTH_VALUE_PROPERTY_WITH_EDGE(YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent, YGNodeStyleSetPaddingCalc, TaroEdge2YgEdgeMapping(edge));
    }

#define SET_LENGTH_VALUE_PROPERTY_WITH_BORDER_EDGE(PX_FUNCTION, EDGE) \
    {                                                                 \
        switch (val.Unit()) {                                         \
            case DimensionUnit::VP:                                   \
            case DimensionUnit::PX:                                   \
            case DimensionUnit::VW:                                   \
            case DimensionUnit::VH:                                   \
            case DimensionUnit::SAFE_AREA:                            \
            case DimensionUnit::DESIGN_PX: {                          \
                PX_FUNCTION(ygNodeRef, EDGE, val.ConvertToVp());      \
                break;                                                \
            }                                                         \
            default: {                                                \
                PX_FUNCTION(ygNodeRef, EDGE, YGUndefined);            \
                break;                                                \
            }                                                         \
        }                                                             \
        CheckIfYGDirty();                                             \
    }

    void BaseRenderNode::SetBorder(const Dimension &val, const TaroEdge &edge) {
        SET_LENGTH_VALUE_PROPERTY_WITH_BORDER_EDGE(YGNodeStyleSetBorder, TaroEdge2YgEdgeMapping(edge));
    }

#define SET_LENGTH_VALUE_PROPERTY_WITH_AUTO_EDGE(                                                                                                 \
    PX_FUNCTION, PERCENT_FUNCTION, AUTO_FUNCTION, CALC_FUNCTION, EDGE)                                                                            \
    {                                                                                                                                             \
        switch (val.Unit()) {                                                                                                                     \
            case DimensionUnit::VP:                                                                                                               \
            case DimensionUnit::PX:                                                                                                               \
            case DimensionUnit::VW:                                                                                                               \
            case DimensionUnit::VH:                                                                                                               \
            case DimensionUnit::SAFE_AREA:                                                                                                        \
            case DimensionUnit::DESIGN_PX: {                                                                                                      \
                PX_FUNCTION(ygNodeRef, EDGE, val.ConvertToVp());                                                                                  \
                break;                                                                                                                            \
            }                                                                                                                                     \
            case DimensionUnit::PERCENT: {                                                                                                        \
                PERCENT_FUNCTION(ygNodeRef, EDGE, val.Value() * 100);                                                                             \
                break;                                                                                                                            \
            }                                                                                                                                     \
            case DimensionUnit::CALC: {                                                                                                           \
                auto context = GetDimensionContext();                                                                                             \
                \ 
                CALC_FUNCTION(ygNodeRef, EDGE, val.GetCalcExpression(), context->design_ratio_, context->device_width_, context->device_height_); \
                break;                                                                                                                            \
            }                                                                                                                                     \
            default: {                                                                                                                            \
                AUTO_FUNCTION(ygNodeRef, EDGE);                                                                                                   \
                break;                                                                                                                            \
            }                                                                                                                                     \
        }                                                                                                                                         \
        CheckIfYGDirty();                                                                                                                         \
        \   
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
    }

    void BaseRenderNode::SetMargin(const Dimension &val, const TaroEdge &edge) {
        SET_LENGTH_VALUE_PROPERTY_WITH_AUTO_EDGE(
            YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent,
            YGNodeStyleSetMarginAuto, YGNodeStyleSetMarginCalc, TaroEdge2YgEdgeMapping(edge));
    }

    void BaseRenderNode::SetPosition(const Dimension &val, const TaroEdge &edge) {
        SET_LENGTH_VALUE_PROPERTY_WITH_EDGE(YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent, YGNodeStyleSetPositionCalc, TaroEdge2YgEdgeMapping(edge));
    }

    void BaseRenderNode::SetBackgroundImage(const TaroCSSOM::TaroStylesheet::BackgroundImageItem &val) {
        SET_DRAW_STYLE(background_image_);

        // backgroundSize 和 backgroundPosition 为百分比的时候，值会根据image的尺寸更新，但是百分比这个值本身没变
        // 所以手动标脏
        // TODO: 其实有一些情况是不用手动标脏的，所有可以改成详细判断
        if (paintDiffer_.paint_style_->background_size_.value.has_value()) {
            SetDrawPropertyDirty(paintDiffer_.paint_style_->background_size_);
        }
        if (auto positionVal = paintDiffer_.paint_style_->background_position_.value; positionVal[0].has_value() && positionVal[1].has_value()) {
            SetDrawPropertyDirty(paintDiffer_.paint_style_->background_position_);
        }
    }
    void BaseRenderNode::ResetBackgroundImage() {
        RESET_DRAW_STYLE(background_image_);
    }
    const Optional<TaroCSSOM::TaroStylesheet::BackgroundImageItem> &BaseRenderNode::GetBackgroundImage() const {
        GET_DRAW_STYLE(background_image_);
    }

    const Optional<Dimension> &BaseRenderNode::GetBackgroundPositionX() const {
        return paintDiffer_.paint_style_->background_position_.value[0];
    }

    void BaseRenderNode::SetBackgroundPositionX(const Dimension &val) {
        SET_DRAW_STYLE_WITH_IDX(background_position_, 0);
    }
    void BaseRenderNode::ResetBackgroundPositionX() {
        RESET_DRAW_STYLE_WITH_IDX(background_position_, 0);
    }

    const Optional<Dimension> &BaseRenderNode::GetBackgroundPositionY() const {
        return paintDiffer_.paint_style_->background_position_.value[1];
    }

    void BaseRenderNode::SetBackgroundPositionY(const Dimension &val) {
        SET_DRAW_STYLE_WITH_IDX(background_position_, 1);
    }
    void BaseRenderNode::ResetBackgroundPositionY() {
        RESET_DRAW_STYLE_WITH_IDX(background_position_, 1);
    }

    GENERATE_DRAW_STYLE_WITH_IDX_FUNCTIONS(BorderStyle, ArkUI_BorderStyle, border_style_)
    GENERATE_DRAW_STYLE_WITH_IDX_FUNCTIONS(BorderRadius, Dimension, border_radius_)
    GENERATE_DRAW_STYLE_WITH_IDX_FUNCTIONS(BorderColor, uint32_t, border_color_)
    GENERATE_DRAW_STYLE_FUNCTIONS(BackgroundColor, uint32_t, background_color_)
    GENERATE_DRAW_STYLE_FUNCTIONS(BoxShadow, TaroCSSOM::TaroStylesheet::BoxShadowItem, boxShadow_)
    GENERATE_DRAW_STYLE_FUNCTIONS(BackgroundRepeat, ArkUI_ImageRepeat, background_repeat_)
    GENERATE_DRAW_STYLE_FUNCTIONS(BackgroundSize, TaroCSSOM::TaroStylesheet::BackgroundSizeParam, background_size_)
    GENERATE_DRAW_STYLE_FUNCTIONS(Opacity, float, opacity_)
    GENERATE_DRAW_STYLE_FUNCTIONS(Visibility, ArkUI_Visibility, visibility_)
    GENERATE_DRAW_STYLE_FUNCTIONS(FixedVisibility, ArkUI_Visibility, fixed_visibility_)
    GENERATE_DRAW_STYLE_FUNCTIONS(Transform, TaroCSSOM::TaroStylesheet::TransformParam, transform_)
    GENERATE_DRAW_STYLE_FUNCTIONS(TransformOrigin, TaroCSSOM::TaroStylesheet::TransformOriginData, transform_origin_)
    GENERATE_DRAW_STYLE_FUNCTIONS(ZIndex, int32_t, zIndex_)
    GENERATE_DRAW_STYLE_FUNCTIONS(PointerEvents, PropertyType::PointerEvents, pointerEvents_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(Color, uint32_t, color_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(FontSize, Dimension, fontSize_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(FontWeight, ArkUI_FontWeight, fontWeight_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(FontStyle, ArkUI_FontStyle, fontStyle_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(FontFamily, std::string, fontFamily_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(LineHeight, Dimension, lineHeight_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(LetterSpacing, float, letterSpacing_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(WordBreak, OH_Drawing_WordBreakType, wordBreak_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(TextAlign, ArkUI_TextAlignment, textAlign_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(WebkitLineClamp, int32_t, webkitLineClamp_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(WhiteSpace, PropertyType::WhiteSpace, whiteSpace_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(TextDecorationLine, OH_Drawing_TextDecoration, textDecorationLine_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(TextDecorationLineStyle, OH_Drawing_TextDecorationStyle, textDecorationStyle_)
    GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(TextOverflow, ArkUI_TextOverflow, textOverflow_)

    bool BaseRenderNode::CheckAndSetFontStyle(const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &style) {
        bool isChanged = false;
        std::optional<uint32_t> defaultColor = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->color, color_, defaultColor);
        std::optional<double> defaultFontSize = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->fontSize, fontSize_, defaultFontSize);
        std::optional<ArkUI_FontWeight> defaultFontWeight = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->fontWeight, fontWeight_, defaultFontWeight);
        std::optional<ArkUI_FontStyle> defaultFontStyle = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->fontStyle, fontStyle_, defaultFontStyle);
        std::optional<std::string> defaultFontFamily = std::string{""};
        CHECK_AND_SET_FONT_STYLE(style->fontFamily, fontFamily_, defaultFontFamily);
        std::optional<float> defaultLineHeight = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->lineHeight, lineHeight_, defaultLineHeight);
        std::optional<float> defaultLetterSpacing = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->letterSpacing, letterSpacing_, defaultLetterSpacing);
        std::optional<OH_Drawing_WordBreakType> defaultWordBreak = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->wordBreak, wordBreak_, defaultWordBreak);
        std::optional<ArkUI_TextAlignment> defaultTextAlign = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->textAlign, textAlign_, defaultTextAlign);
        std::optional<int> defaultWebkitLineClamp = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->webkitLineClamp, webkitLineClamp_, defaultWebkitLineClamp);
        std::optional<PropertyType::WhiteSpace> defaultWhiteSpace = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->whiteSpace, whiteSpace_, defaultWhiteSpace);
        std::optional<OH_Drawing_TextDecoration> defaultTextDecorationLine = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->textDecorationLine, textDecorationLine_, defaultTextDecorationLine);
        std::optional<OH_Drawing_TextDecorationStyle> defaultTextDecorationStyle_ = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->textDecorationStyle, textDecorationStyle_, defaultTextDecorationStyle_);
        std::optional<ArkUI_TextOverflow> defaultTextOverflow_ = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->textOverflow, textOverflow_, defaultTextOverflow_);
        return isChanged;
    }

    bool BaseRenderNode::CheckAndSetFontBgColorStyle(const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &style) {
        bool isChanged = false;
        std::optional<uint32_t> defaultBackgroundColor = std::nullopt;
        CHECK_AND_SET_FONT_STYLE(style->backgroundColor, background_color_, defaultBackgroundColor);
        return isChanged;
    }

    DimensionContextRef BaseRenderNode::GetDimensionContext() {
        DimensionContextRef context = DimensionContext::GetInstance();
        auto render_node = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
        if (auto element = render_node->element_ref_.lock()) {
            context = TaroCSSOM::CSSStyleSheet::GetInstance()->GetCurrentPageDimension(element->context_->page_path_);
        }
        return context;
    }
} // namespace TaroDOM
} // namespace TaroRuntime