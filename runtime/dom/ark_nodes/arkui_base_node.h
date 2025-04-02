/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_ARKUI_BASE_NODE_H
#define HARMONY_LIBRARY_ARKUI_BASE_NODE_H

#include <bitset>
#include <cstdint>
#include <arkui/native_node.h>

#include "runtime/constant.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/cssom/stylesheet/background_image.h"
#include "runtime/cssom/stylesheet/background_size.h"
#include "runtime/cssom/stylesheet/box_shadow.h"
#include "runtime/cssom/stylesheet/common.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/params/transform_param//transform_param.h"
#include "runtime/cssom/stylesheet/transform_origin.h"
#include "runtime/cssom/stylesheet/utils.h"
#include "runtime/dom/ark_nodes/differ/paint_differ.h"
#include "runtime/dom/ark_nodes/differ/layout_differ.h"
#include "yoga/YGConfig.h"

#define DECLARE_DRAW_STYLE_WITH_IDX_FUNCTIONS(NAME, TYPE) \
void Set##NAME(const TYPE &val, const TaroEdge &edge); \
void Reset##NAME(const TaroEdge &edge); \
const Optional<TYPE>& Get##NAME(const TaroEdge &edge) const;

#define DECLARE_DRAW_STYLE_FUNCTIONS(NAME, TYPE) \
void Set##NAME(const TYPE &val); \
void Reset##NAME(); \
const Optional<TYPE>& Get##NAME() const;

namespace TaroRuntime {
namespace TaroDOM {
    class BaseRenderNode : public std::enable_shared_from_this<BaseRenderNode> {
        public:
        // yoga 节点
        BaseRenderNode();
        virtual ~BaseRenderNode();

        YGNodeRef ygNodeRef;
        PaintDiffer paintDiffer_;
        // Layout 样式控制器
        LayoutDiffer layoutDiffer_;

        void SetNodeLayoutStyle(
            const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &style,
            const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &oldStyleRef,
            const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &parentStyleRef);

        void SetNodeDrawStyle(
            const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &style,
            const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &oldStyleRef);

        void SetFontDrawStyle(
            const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &style,
            const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &oldStyleRef);

        void SetLayoutDirty(const bool &val);
        void SetDrawDirty(const bool &val);
        bool GetLayoutDirty();
        bool GetDrawDirty();
        void RegistryLayoutTaskOnNextVsync();
        void RegistryDrawTaskOnNextVsync();
        void CheckIfYGDirty();
        // 钩子方法，每个值设置时，会经过该方法，返回true则设置，返回false则不设置
        virtual bool OnSetPropertyIntoNode(const CSSProperty::Type &property,
                                           const TaroChange &changeType,
                                           const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &style);

        // Layout Properties
        void SetDisplay(const PropertyType::Display &val, const PropertyType::Display &oldVal = PropertyType::Display::UnKnown);
        void SetPosition(const PropertyType::Position &val);
        void SetOverflow(const PropertyType::Overflow &val);
        void SetAlignContent(const ArkUI_FlexAlignment &val);
        void SetAlignSelf(const ArkUI_ItemAlignment &val);
        void SetFlexGrow(const float &val);
        void SetFlexBasis(const Dimension &val);
        void SetFlexShrink(const float &val);
        void SetFlexDirection(const ArkUI_FlexDirection &val);
        void SetAlignItems(const ArkUI_ItemAlignment &val);
        void SetJustifyContent(const ArkUI_FlexAlignment &val);
        void SetFlexWrap(const ArkUI_FlexWrap &val);
        void SetMinHeight(const Dimension &val);
        void SetMinWidth(const Dimension &val);
        void SetMaxHeight(const Dimension &val);
        void SetMaxWidth(const Dimension &val);
        void SetHeight(const Dimension &val, const Dimension &oldVal = Dimension{0, DimensionUnit::NONE});
        void SetWidth(const Dimension &val);
        void SetPadding(const Dimension &val, const TaroEdge &edge);
        void SetBorder(const Dimension &val, const TaroEdge &edge);
        void SetMargin(const Dimension &val, const TaroEdge &edge);
        void SetPosition(const Dimension &val, const TaroEdge &edge);

        // Draw Properties
        DECLARE_DRAW_STYLE_WITH_IDX_FUNCTIONS(BorderStyle, ArkUI_BorderStyle)
        DECLARE_DRAW_STYLE_WITH_IDX_FUNCTIONS(BorderRadius, Dimension)
        DECLARE_DRAW_STYLE_WITH_IDX_FUNCTIONS(BorderColor, uint32_t)
        DECLARE_DRAW_STYLE_FUNCTIONS(BackgroundColor, uint32_t)
        DECLARE_DRAW_STYLE_FUNCTIONS(BackgroundImage, TaroCSSOM::TaroStylesheet::BackgroundImageItem)
        DECLARE_DRAW_STYLE_FUNCTIONS(BackgroundRepeat, ArkUI_ImageRepeat)
        DECLARE_DRAW_STYLE_FUNCTIONS(BackgroundSize, TaroCSSOM::TaroStylesheet::BackgroundSizeParam)
        DECLARE_DRAW_STYLE_FUNCTIONS(BackgroundPositionX, Dimension)
        DECLARE_DRAW_STYLE_FUNCTIONS(BackgroundPositionY, Dimension)
        DECLARE_DRAW_STYLE_FUNCTIONS(Opacity, float)
        DECLARE_DRAW_STYLE_FUNCTIONS(FixedVisibility, ArkUI_Visibility)
        DECLARE_DRAW_STYLE_FUNCTIONS(Visibility, ArkUI_Visibility)
        DECLARE_DRAW_STYLE_FUNCTIONS(Transform, TaroCSSOM::TaroStylesheet::TransformParam)
        DECLARE_DRAW_STYLE_FUNCTIONS(TransformOrigin, TaroCSSOM::TaroStylesheet::TransformOriginData)
        DECLARE_DRAW_STYLE_FUNCTIONS(ZIndex, int32_t)
        DECLARE_DRAW_STYLE_FUNCTIONS(BoxShadow, TaroCSSOM::TaroStylesheet::BoxShadowItem)
        DECLARE_DRAW_STYLE_FUNCTIONS(Color, uint32_t)
        DECLARE_DRAW_STYLE_FUNCTIONS(FontSize, Dimension)
        DECLARE_DRAW_STYLE_FUNCTIONS(FontWeight, ArkUI_FontWeight)
        DECLARE_DRAW_STYLE_FUNCTIONS(FontStyle, ArkUI_FontStyle)
        DECLARE_DRAW_STYLE_FUNCTIONS(FontFamily, std::string)
        DECLARE_DRAW_STYLE_FUNCTIONS(LineHeight, Dimension)
        DECLARE_DRAW_STYLE_FUNCTIONS(LetterSpacing, float)
        DECLARE_DRAW_STYLE_FUNCTIONS(WordBreak, OH_Drawing_WordBreakType)
        DECLARE_DRAW_STYLE_FUNCTIONS(TextAlign, ArkUI_TextAlignment)
        DECLARE_DRAW_STYLE_FUNCTIONS(WebkitLineClamp, int32_t)
        DECLARE_DRAW_STYLE_FUNCTIONS(WhiteSpace, PropertyType::WhiteSpace)
        DECLARE_DRAW_STYLE_FUNCTIONS(TextDecorationLine, OH_Drawing_TextDecoration)
        DECLARE_DRAW_STYLE_FUNCTIONS(TextDecorationLineStyle, OH_Drawing_TextDecorationStyle)
        DECLARE_DRAW_STYLE_FUNCTIONS(TextOverflow, ArkUI_TextOverflow)
        DECLARE_DRAW_STYLE_FUNCTIONS(PointerEvents, PropertyType::PointerEvents)

        bool CheckAndSetFontStyle(const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &);
        bool CheckAndSetFontBgColorStyle(const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> &);
        virtual void OnDisplayChange(const PropertyType::Display &val, const PropertyType::Display &oldVal) {};

        DimensionContextRef GetDimensionContext();

        protected:
        bool is_layout_dirty_ = false;
        bool is_draw_dirty_ = false;

        template <typename T>
        void SetDrawPropertyDirty(DrawProperty<T> &drawProperty) {
            drawProperty.force_update = true;
            SetDrawDirty(true);
        }
    };

} // namespace TaroDOM

} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_ARKUI_BASE_NODE_H
