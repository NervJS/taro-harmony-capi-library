/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./TEnum.h"

#include <typeindex>
#include <typeinfo>
#include <native_drawing/drawing_text_typography.h>

#include "runtime/cssom/stylesheet/css_property.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

std::unordered_map<std::type_index, std::unordered_map<std::string_view, int>>
    TEnum::enumMappings = {
        {std::type_index(typeid(OH_Drawing_TextDecoration)),
         {{"none", TEXT_DECORATION_NONE},
          {"underline", TEXT_DECORATION_UNDERLINE},
          {"overline", TEXT_DECORATION_OVERLINE},
          {"line-through", TEXT_DECORATION_LINE_THROUGH}}},
        {std::type_index(typeid(OH_Drawing_TextDecorationStyle)),
         {{"solid", TEXT_DECORATION_STYLE_SOLID},
          {"double", TEXT_DECORATION_STYLE_DOUBLE},
          {"dotted", TEXT_DECORATION_STYLE_DOTTED},
          {"dashed", TEXT_DECORATION_STYLE_DASHED},
          {"wavy", TEXT_DECORATION_STYLE_WAVY}}},
        {std::type_index(typeid(ArkUI_ItemAlignment)),
         {{"normal", ARKUI_ITEM_ALIGNMENT_AUTO},
          {"auto", ARKUI_ITEM_ALIGNMENT_AUTO},
          {"start", ARKUI_ITEM_ALIGNMENT_START},
          {"flex-start", ARKUI_ITEM_ALIGNMENT_START},
          {"center", ARKUI_ITEM_ALIGNMENT_CENTER},
          {"end", ARKUI_ITEM_ALIGNMENT_END},
          {"flex-end", ARKUI_ITEM_ALIGNMENT_END},
          {"stretch", ARKUI_ITEM_ALIGNMENT_STRETCH},
          {"baseline", ARKUI_ITEM_ALIGNMENT_BASELINE}}},
        {std::type_index(typeid(PropertyType::Display)),
         {{"none", static_cast<int>(PropertyType::Display::None)},
          {"block", static_cast<int>(PropertyType::Display::Block)},
          {"flex", static_cast<int>(PropertyType::Display::Flex)},
          {"box", static_cast<int>(PropertyType::Display::Box)},
          {"-webkit-box", static_cast<int>(PropertyType::Display::Box)}}},
        {std::type_index(typeid(ArkUI_FontStyle)),
         {
             {"normal", ARKUI_FONT_STYLE_NORMAL},
             {"italic", ARKUI_FONT_STYLE_ITALIC},
         }},
        {std::type_index(typeid(ArkUI_TextAlignment)),
         {
             {"left", ARKUI_TEXT_ALIGNMENT_START},
             {"center", ARKUI_TEXT_ALIGNMENT_CENTER},
             {"right", ARKUI_TEXT_ALIGNMENT_END},
             {"justify", ARKUI_TEXT_ALIGNMENT_JUSTIFY},
         }},
        {std::type_index(typeid(ArkUI_TextOverflow)),
         {
             {"none", ARKUI_TEXT_OVERFLOW_NONE},
             {"clip", ARKUI_TEXT_OVERFLOW_CLIP},
             {"ellipsis", ARKUI_TEXT_OVERFLOW_ELLIPSIS},
         }},
        {std::type_index(typeid(OH_Drawing_PlaceholderVerticalAlignment)),
         {{"baseline", ALIGNMENT_OFFSET_AT_BASELINE},
          {"text-top", ALIGNMENT_ABOVE_BASELINE},
          {"text-bottom", ALIGNMENT_BELOW_BASELINE},
          {"top", ALIGNMENT_TOP_OF_ROW_BOX},
          {"bottom", ALIGNMENT_BOTTOM_OF_ROW_BOX},
          {"middle", ALIGNMENT_CENTER_OF_ROW_BOX}}},
        {std::type_index(typeid(PropertyType::LengthUnit)),
         {
             {"px", static_cast<int>(PropertyType::LengthUnit::PX)},
             {"vw", static_cast<int>(PropertyType::LengthUnit::VW)},
             {"vh", static_cast<int>(PropertyType::LengthUnit::VH)},
             {"%", static_cast<int>(PropertyType::LengthUnit::PERCENT)},
         }},

        {std::type_index(typeid(ArkUI_Visibility)),
         {{"visible", ARKUI_VISIBILITY_VISIBLE},
          {"hidden", ARKUI_VISIBILITY_HIDDEN}}},

        {std::type_index(typeid(ArkUI_FlexAlignment)),
         {{"start", ARKUI_FLEX_ALIGNMENT_START},
          {"flex-start", ARKUI_FLEX_ALIGNMENT_START},
          {"center", ARKUI_FLEX_ALIGNMENT_CENTER},
          {"end", ARKUI_FLEX_ALIGNMENT_END},
          {"flex-end", ARKUI_FLEX_ALIGNMENT_END},
          {"space-between", ARKUI_FLEX_ALIGNMENT_SPACE_BETWEEN},
          {"space-around", ARKUI_FLEX_ALIGNMENT_SPACE_AROUND},
          {"space-evenly", ARKUI_FLEX_ALIGNMENT_SPACE_EVENLY}

         }},
        {std::type_index(typeid(ArkUI_ImageRepeat)),
         {
             {"no-repeat", ARKUI_IMAGE_REPEAT_NONE},
             {"repeat-x", ARKUI_IMAGE_REPEAT_X},
             {"repeat-y", ARKUI_IMAGE_REPEAT_Y},
             {"repeat", ARKUI_IMAGE_REPEAT_XY},
         }},
        {std::type_index(typeid(ArkUI_BorderStyle)),
         {{"solid",
           ARKUI_BORDER_STYLE_SOLID},
          {"dashed",
           ARKUI_BORDER_STYLE_DASHED},
          {"dotted",
           ARKUI_BORDER_STYLE_DOTTED}

         }},
        {std::type_index(typeid(ArkUI_FlexDirection)),
         {{"row",
           ARKUI_FLEX_DIRECTION_ROW},
          {"column",
           ARKUI_FLEX_DIRECTION_COLUMN},
          {"row-reverse",
           ARKUI_FLEX_DIRECTION_ROW_REVERSE},
          {"column-reverse",
           ARKUI_FLEX_DIRECTION_COLUMN_REVERSE}

         }},
        {std::type_index(typeid(ArkUI_FlexWrap)),
         {{"nowrap",
           ARKUI_FLEX_WRAP_NO_WRAP},
          {"wrap",
           ARKUI_FLEX_WRAP_WRAP},
          {"wrap-reverse",
           ARKUI_FLEX_WRAP_WRAP_REVERSE}

         }},
        {std::type_index(typeid(ArkUI_FontWeight)),
         {{"100", ARKUI_FONT_WEIGHT_W100},
          {"200", ARKUI_FONT_WEIGHT_W200},
          {"300", ARKUI_FONT_WEIGHT_W300},
          {"400", ARKUI_FONT_WEIGHT_W400},
          {"500", ARKUI_FONT_WEIGHT_W500},
          {"600", ARKUI_FONT_WEIGHT_W600},
          {"700", ARKUI_FONT_WEIGHT_W700},
          {"800", ARKUI_FONT_WEIGHT_W800},
          {"900", ARKUI_FONT_WEIGHT_W900},
          {"bold", ARKUI_FONT_WEIGHT_BOLD},
          {"normal", ARKUI_FONT_WEIGHT_NORMAL},
          {"bolder", ARKUI_FONT_WEIGHT_BOLDER},
          {"lighter", ARKUI_FONT_WEIGHT_LIGHTER},
          {"medium", ARKUI_FONT_WEIGHT_MEDIUM},
          {"regular", ARKUI_FONT_WEIGHT_REGULAR}

         }},
        {std::type_index(typeid(PropertyType::Overflow)), {
                                                              {"hidden", static_cast<int>(PropertyType::Overflow::Hidden)},
                                                              {"visible", static_cast<int>(PropertyType::Overflow::Visible)},
                                                              {"scroll", static_cast<int>(PropertyType::Overflow::Scroll)},
                                                          }},
        {std::type_index(typeid(PropertyType::WhiteSpace)), {
                                                                {"normal", static_cast<int>(PropertyType::WhiteSpace::Wrap)},
                                                                {"nowrap", static_cast<int>(PropertyType::WhiteSpace::NoWrap)},
                                                            }},
        {std::type_index(typeid(PropertyType::Position)), {{"static", static_cast<int>(PropertyType::Position::Static)}, {"relative", static_cast<int>(PropertyType::Position::Relative)}, {"absolute", static_cast<int>(PropertyType::Position::Absolute)}, {"fixed", static_cast<int>(PropertyType::Position::Fixed)}, {"sticky", static_cast<int>(PropertyType::Position::Sticky)}}},

        {std::type_index(typeid(PropertyType::BoxOrient)), {{"horizontal", static_cast<int>(PropertyType::BoxOrient::Horizontal)}, {"vertical", static_cast<int>(PropertyType::BoxOrient::Vertical)}, {"inline-axis", static_cast<int>(PropertyType::BoxOrient::InlineAxis)}, {"block-axis", static_cast<int>(PropertyType::BoxOrient::BlockAxis)}}},

};
std::unordered_map<std::type_index, std::unordered_map<int, int>> TEnum::intEnumMappings = {
    {std::type_index(typeid(OH_Drawing_TextDecoration)),
     {{ARKUI_TEXT_DECORATION_TYPE_UNDERLINE, TEXT_DECORATION_UNDERLINE},
      {ARKUI_TEXT_DECORATION_TYPE_LINE_THROUGH, TEXT_DECORATION_LINE_THROUGH},
      {ARKUI_TEXT_DECORATION_TYPE_OVERLINE, TEXT_DECORATION_OVERLINE},
      {ARKUI_TEXT_DECORATION_TYPE_NONE, TEXT_DECORATION_NONE}}},
    {std::type_index(typeid(OH_Drawing_TextDecorationStyle)),
     {{ARKUI_TEXT_DECORATION_STYLE_DOTTED, TEXT_DECORATION_STYLE_DOTTED},
      {ARKUI_TEXT_DECORATION_STYLE_DOUBLE, TEXT_DECORATION_STYLE_DOUBLE},
      {ARKUI_TEXT_DECORATION_STYLE_DASHED, TEXT_DECORATION_STYLE_DASHED},
      {ARKUI_TEXT_DECORATION_STYLE_WAVY, TEXT_DECORATION_STYLE_WAVY}}},
    {std::type_index(typeid(OH_Drawing_WordBreakType)),
     {{ARKUI_WORD_BREAK_NORMAL, WORD_BREAK_TYPE_NORMAL},
      {ARKUI_WORD_BREAK_BREAK_ALL, WORD_BREAK_TYPE_BREAK_ALL},
      {ARKUI_WORD_BREAK_BREAK_WORD, WORD_BREAK_TYPE_BREAK_WORD}}}};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
