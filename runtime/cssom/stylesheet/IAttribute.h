/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_IATRIBUTE_H
#define TARO_CAPI_HARMONY_DEMO_IATRIBUTE_H

#include "./align_content.h"
#include "./align_items.h"
#include "./align_self.h"
#include "./animation.h"
#include "./aspect_ratio.h"
#include "./background_color.h"
#include "./background_image.h"
#include "./background_position_x.h"
#include "./background_position_y.h"
#include "./background_repeat.h"
#include "./background_size.h"
#include "./border_bottom_style.h"
#include "./border_bottom_width.h"
#include "./border_left_style.h"
#include "./border_left_width.h"
#include "./border_right_style.h"
#include "./border_right_width.h"
#include "./border_top_style.h"
#include "./border_top_width.h"
#include "./box_shadow.h"
#include "./color.h"
#include "./css_property.h"
#include "./display.h"
#include "./flex_basis.h"
#include "./flex_direction.h"
#include "./flex_grow.h"
#include "./flex_shrink.h"
#include "./flex_wrap.h"
#include "./font_family.h"
#include "./font_size.h"
#include "./font_style.h"
#include "./font_weight.h"
#include "./height.h"
#include "./justify_content.h"
#include "./letter_spacing.h"
#include "./line_height.h"
#include "./margin_bottom.h"
#include "./margin_left.h"
#include "./margin_right.h"
#include "./margin_top.h"
#include "./opacity.h"
#include "./overflow.h"
#include "./padding_bottom.h"
#include "./padding_left.h"
#include "./padding_right.h"
#include "./padding_top.h"
#include "./position.h"
#include "./text_align.h"
#include "./text_decoration_color.h"
#include "./text_decoration_line.h"
#include "./text_decoration_style.h"
#include "./text_overflow.h"
#include "./transform.h"
#include "./transform_origin.h"
#include "./transition.h"
#include "./vertical_align.h"
#include "./visibility.h"
#include "./webkit_line_clamp.h"
#include "./width.h"
#include "./word_break.h"
#include "./z_index.h"
#include "border_bottom_color.h"
#include "border_bottom_left_radius.h"
#include "border_bottom_right_radius.h"
#include "border_left_color.h"
#include "border_right_color.h"
#include "border_top_color.h"
#include "border_top_left_radius.h"
#include "border_top_right_radius.h"
#include "pointer_events.h"
#include "runtime/cssom/stylesheet/animation_multi.h"
#include "runtime/cssom/stylesheet/bottom.h"
#include "runtime/cssom/stylesheet/box_orient.h"
#include "runtime/cssom/stylesheet/column_gap.h"
#include "runtime/cssom/stylesheet/left.h"
#include "runtime/cssom/stylesheet/max_height.h"
#include "runtime/cssom/stylesheet/max_width.h"
#include "runtime/cssom/stylesheet/min_height.h"
#include "runtime/cssom/stylesheet/min_width.h"
#include "runtime/cssom/stylesheet/right.h"
#include "runtime/cssom/stylesheet/row_gap.h"
#include "runtime/cssom/stylesheet/top.h"
#include "runtime/cssom/stylesheet/white_space.h"
#include "yoga/YGConfig.h"
namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {

        enum class ChangeType { Added,
                                Modified,
                                Removed,
                                NotChange };
        struct Change {
            CSSProperty::Type attribute;
            ChangeType type;
        };

        class Stylesheet {
            public:
            // 通用属性
            Display display;
            Width width;
            MinWidth minWidth;
            MaxWidth maxWidth;
            Height height;
            MinHeight minHeight;
            MaxHeight maxHeight;
            FlexDirection flexDirection;
            FlexWrap flexWrap;
            AlignContent alignContent;
            JustifyContent justifyContent;
            AlignItems alignItems;
            AlignSelf alignSelf;
            FlexGrow flexGrow;
            FlexShrink flexShrink;
            FlexBasis flexBasis;

            MarginBottom marginBottom;
            MarginTop marginTop;
            MarginLeft marginLeft;
            MarginRight marginRight;

            PaddingBottom paddingBottom;
            PaddingTop paddingTop;
            PaddingLeft paddingLeft;
            PaddingRight paddingRight;

            ColumnGap columnGap;
            RowGap rowGap;

            BackgroundColor backgroundColor;
            BackgroundImage backgroundImage;

            BorderBottomLeftRadius borderBottomLeftRadius;
            BorderBottomRightRadius borderBottomRightRadius;
            BorderTopLeftRadius borderTopLeftRadius;
            BorderTopRightRadius borderTopRightRadius;

            Opacity opacity;

            // 文本属性
            FontSize fontSize;
            FontWeight fontWeight;
            FontStyle fontStyle;
            Color color;
            PointerEvents pointerEvents;
            TextDecorationStyle textDecorationStyle;
            TextDecorationLine textDecorationLine;
            TextDecorationColor textDecorationColor;
            FontFamily fontFamily;

            Visibility visibility;
            Overflow overflow;
            BoxShadow boxShadow;

            BorderTopWidth borderTopWidth;
            BorderRightWidth borderRightWidth;
            BorderBottomWidth borderBottomWidth;
            BorderLeftWidth borderLeftWidth;

            BorderBottomColor borderBottomColor;
            BorderTopColor borderTopColor;
            BorderLeftColor borderLeftColor;
            BorderRightColor borderRightColor;

            BorderTopStyle borderTopStyle;
            BorderRightStyle borderRightStyle;
            BorderBottomStyle borderBottomStyle;
            BorderLeftStyle borderLeftStyle;

            BackgroundRepeat backgroundRepeat;

            Position position;
            Top top;
            Left left;
            Right right;
            Bottom bottom;
            BackgroundSize backgroundSize;
            BackgroundPositionX backgroundPositionX;
            BackgroundPositionY backgroundPositionY;
            TransformOrigin transformOrigin;
            Transform transform;

            TextAlign textAlign;
            LetterSpacing letterSpacing;
            LineHeight lineHeight;
            WhiteSpace whiteSpace;

            WordBreak wordBreak;
            TextOverflow textOverflow;

            VerticalAlign verticalAlign;
            WebkitLineClamp webkitLineClamp;
            ZIndex zIndex;

            AnimationMulti animationMulti;
            Transition transition;

            BoxOrient boxOrient;
    
            AspectRatio aspectRatio;

            Stylesheet();

            static std::shared_ptr<Stylesheet> assign(
                const std::shared_ptr<Stylesheet>& s1,
                const std::shared_ptr<Stylesheet>& s2);

            static bool compare(
                const std::shared_ptr<Stylesheet>& s1,
                const std::shared_ptr<Stylesheet>& s2);
            /**
             * 属性键值对解析
             * 注：方法内部做去除字符串两端空格的处理
             */
            void parseCssProperty(std::string_view name, std::string_view value);

            /**
             * 根据节点的 style 属性生成自身
             */
            static std::shared_ptr<Stylesheet> makeFromInlineStyle(napi_value node);

            private:
            template <typename T>
            static void compareAttributes(const CSSProperty::Type& name, const T& attr1,
                                          const T& attr2, std::vector<Change>& changes);
        };

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_IATRIBUTE_H
