//
// Created on 2024/9/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "text_styled.h"
#include <native_drawing/drawing_brush.h>
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/font/FontFamilyManager.h"

namespace TaroRuntime {
namespace TaroDOM {
    TextStyled::TextStyled() {}
    
    TextStyled::~TextStyled() {}

    void TextStyled::InitStyledString(const StylesheetRef& nodeStyle, const StylesheetRef& textStyle, const DimensionContextRef& dimensionContext) {
//         if (!m_StyledString) {
            m_StyledString && m_OldStyledString.emplace_back(m_StyledString);
            m_TypographyStyle = OH_Drawing_CreateTypographyStyle();
            auto fontCollection = TaroCSSOM::FontFamilyManager::GetInstance()->GetFontCollection();
            auto globalTextStyle = GetTextStyle(textStyle, dimensionContext);
            SetTypographyStyle(m_TypographyStyle, textStyle, nodeStyle, globalTextStyle);
            m_StyledString = OH_ArkUI_StyledString_Create(m_TypographyStyle, fontCollection);
            if (globalTextStyle) {
                OH_Drawing_DestroyTextStyle(globalTextStyle);
            }
//         }
    }

    ArkUI_StyledString* TextStyled::GetStyledString() {
        return m_StyledString;
    }

    void TextStyled::InitTypography() {
        if (m_StyledString) {
            m_OldTypography.emplace_back(m_Typography);
            m_HasBeenLayout = false;
            m_Typography = OH_ArkUI_StyledString_CreateTypography(m_StyledString);
        }
    }

    void TextStyled::TextTypographyLayout(OH_Drawing_Typography * typography, const double width) {
        if (!typography || width <= 0 || std::isnan(width)) return;
        OH_Drawing_TypographyLayout(typography, width);
        m_HasBeenLayout = true;
    }

    OH_Drawing_Typography* TextStyled::GetTypography() {
        return m_Typography;
    }

    void TextStyled::SetTypographyStyle(OH_Drawing_TypographyStyle* typographyStyle, const StylesheetRef& style, const StylesheetRef& style_ref, OH_Drawing_TextStyle* textStyle) {
        if (!style) return;
        if (style->wordBreak.has_value()) {
            auto wordBreak = style->wordBreak.value();
            OH_Drawing_SetTypographyTextWordBreakType(typographyStyle, wordBreak);
        } else {
            OH_Drawing_SetTypographyTextWordBreakType(typographyStyle, WORD_BREAK_TYPE_NORMAL);
        }

        if (style_ref->overflow.has_value() && style_ref->overflow.value() == PropertyType::Overflow::Hidden) {
            auto lines = style->webkitLineClamp.has_value() ? style->webkitLineClamp.value() : 1;
            bool needEllipsis = false;
            if (lines == 1) {
                if (style->textOverflow.has_value() && style->textOverflow.value() == ArkUI_TextOverflow::ARKUI_TEXT_OVERFLOW_ELLIPSIS) {
                    needEllipsis = style->whiteSpace.has_value() && style->whiteSpace.value() == PropertyType::WhiteSpace::NoWrap;
                }
            } else {
                if (style_ref->display.has_value() && style_ref->display.value() == PropertyType::Display::Box && style_ref->boxOrient.has_value() && style_ref->boxOrient.value() == PropertyType::BoxOrient::Vertical) {
                    needEllipsis = true;
                }
            }
            if (needEllipsis) {
                OH_Drawing_SetTypographyTextEllipsis(typographyStyle, "...");
                OH_Drawing_SetTypographyTextEllipsisModal(typographyStyle, ELLIPSIS_MODAL_TAIL);
            }
        }

        OH_Drawing_SetTypographyTextSplitRatio(typographyStyle, 0.0);
        // 未来用于国际化
        //         OH_Drawing_SetTypographyTextLocale(typographyStyle, "zh-CN");

        // TODO 需要结合 direction 来设置
        OH_Drawing_SetTypographyTextDirection(typographyStyle, TEXT_DIRECTION_LTR);
        if (style->webkitLineClamp.has_value()) {
            auto lineLamp = style->webkitLineClamp.value();
            OH_Drawing_SetTypographyTextMaxLines(typographyStyle, lineLamp);
        } else if (style->whiteSpace.has_value() && style->whiteSpace.value() == PropertyType::WhiteSpace::NoWrap) {
            OH_Drawing_SetTypographyTextMaxLines(typographyStyle, 1);
        }
        OH_Drawing_SetTypographyTextStyle(typographyStyle, textStyle);
    }

    OH_Drawing_TextStyle* TextStyled::GetTextStyle(const StylesheetRef& style, const DimensionContextRef& dimensionContext) {
        OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
        if (style) {
            if (style->fontSize.has_value()) {
                auto fontSize = style->fontSize.value();
                if (
                    fontSize.Unit() == DimensionUnit::PX ||
                    fontSize.Unit() == DimensionUnit::VP ||
                    fontSize.Unit() == DimensionUnit::DESIGN_PX) {
                    OH_Drawing_SetTextStyleFontSize(textStyle, fontSize.ConvertToPx(dimensionContext));
                }
            } else {
                OH_Drawing_SetTextStyleFontSize(textStyle, vp2Px(14));
            }

            OH_Drawing_FontStyleStruct fontStyleStruct;
            if (style->fontWeight.has_value()) {
                auto fontWeight = style->fontWeight.value();
                if (fontWeight == ARKUI_FONT_WEIGHT_BOLDER) {
                    // TODO
                } else if (fontWeight == ARKUI_FONT_WEIGHT_LIGHTER) {
                    // TODO
                } else {
                    auto it = FONT_WEIGHT_MAP.find(fontWeight);
                    if (it != FONT_WEIGHT_MAP.end()) {
                        fontStyleStruct.weight = it->second;
                    } else {
                        fontStyleStruct.weight = FONT_WEIGHT_400;
                    }
                }
            } else {
                fontStyleStruct.weight = FONT_WEIGHT_400;
            }

            if (style->fontStyle.has_value()) {
                auto fontStyle = style->fontStyle.value();
                if (fontStyle == ARKUI_FONT_STYLE_ITALIC) {
                    fontStyleStruct.slant = FONT_STYLE_ITALIC;
                } else if (fontStyle == ARKUI_FONT_STYLE_NORMAL) {
                    fontStyleStruct.slant = FONT_STYLE_NORMAL;
                }
            } else {
                fontStyleStruct.slant = FONT_STYLE_NORMAL;
            }
            // TODO 可以支持 font-stretch
            fontStyleStruct.width = FONT_WIDTH_NORMAL;
            OH_Drawing_SetTextStyleFontStyleStruct(textStyle, fontStyleStruct);

            if (style->fontFamily.has_value()) {
                auto fontFamily = style->fontFamily.value();
                auto fontFamilyVec = TaroHelper::string::split(fontFamily, ",");
                std::vector<std::string> validFontFamilies;
                for (const auto& familyView : fontFamilyVec) {
                    std::string family(familyView);
                    auto fontFamilyManager = TaroCSSOM::FontFamilyManager::GetInstance();
                    if (fontFamilyManager->HasFont(family)) {
                        validFontFamilies.push_back(std::move(family));
                    }
                }
                if (validFontFamilies.empty()) {
                    const char* defaultFontFamily[] = {"HarmonyOS Sans"};
                    OH_Drawing_SetTextStyleFontFamilies(textStyle, 1, defaultFontFamily);
                } else {
                    std::vector<const char*> fontFamilyCStrings;
                    for (const auto& family : validFontFamilies) {
                        fontFamilyCStrings.push_back(family.c_str());
                    }
                    OH_Drawing_SetTextStyleFontFamilies(textStyle, fontFamilyCStrings.size(), fontFamilyCStrings.data());
                }
            } else {
                const char* fontFamilies[] = {"HarmonyOS Sans"};
                OH_Drawing_SetTextStyleFontFamilies(textStyle, 1, fontFamilies);
            }
            auto textColor = 0xff000000;
            if (style->color.has_value()) {
                auto color = style->color.value();
                textColor = color;
                OH_Drawing_SetTextStyleColor(textStyle, color);
            } else {
                OH_Drawing_SetTextStyleColor(textStyle, textColor);
            }

            if (style->textDecorationLine.has_value()) {
                OH_Drawing_SetTextStyleDecoration(textStyle, style->textDecorationLine.value());
                if (style->textDecorationStyle.has_value()) {
                    OH_Drawing_SetTextStyleDecorationStyle(textStyle, style->textDecorationStyle.value());
                }

                if (style->textDecorationColor.has_value()) {
                    OH_Drawing_SetTextStyleDecorationColor(textStyle, style->textDecorationColor.value());
                } else {
                    OH_Drawing_SetTextStyleDecorationColor(textStyle, textColor);
                }
            } else {
                OH_Drawing_SetTextStyleDecoration(textStyle, TEXT_DECORATION_NONE);
            }

            if (style->lineHeight.has_value()) {
                auto lineHeight = style->lineHeight.value();
                switch (lineHeight.Unit()) {
                    case DimensionUnit::VP:
                    case DimensionUnit::PX:
                    case DimensionUnit::DESIGN_PX: {
                        auto fontSize = OH_Drawing_TextStyleGetFontSize(textStyle);
                        OH_Drawing_SetTextStyleFontHeight(textStyle, lineHeight.ConvertToPx(dimensionContext) / fontSize);
                    } break;
                    case DimensionUnit::PERCENT:
                        OH_Drawing_SetTextStyleFontHeight(textStyle, lineHeight.Value());
                        break;
                    default:
                        break;
                }
            }

            if (style->backgroundColor.has_value()) {
                auto brush = OH_Drawing_BrushCreate();
                auto backgroundColor = style->backgroundColor.value();
                OH_Drawing_BrushReset(brush);
                OH_Drawing_BrushSetColor(brush, backgroundColor);
                OH_Drawing_SetTextStyleBackgroundBrush(textStyle, brush);
                OH_Drawing_BrushDestroy(brush);
            }

            if (style->letterSpacing.has_value()) {
                auto letterSpacing = style->letterSpacing.value();
                OH_Drawing_SetTextStyleLetterSpacing(textStyle, letterSpacing);
            }

            //             OH_Drawing_SetTextStyleLocale(textStyle, "zh-CN");
            OH_Drawing_SetTextStyleHalfLeading(textStyle, true);
        }
        return textStyle;
    }

    void TextStyled::SetSingleTextStyle(const StylesheetRef& styleRef, const StylesheetRef& textItemStyleRef, const std::string& textContent, const DimensionContextRef& dimensionContext) {
        auto combinedStyleSheet = TaroCSSOM::TaroStylesheet::Stylesheet::assign(styleRef, textItemStyleRef);
        if (!textItemStyleRef->backgroundColor.has_value()) {
            // 防止背景色被设置到文字背景上
            combinedStyleSheet->backgroundColor.reset();
        }
        auto singleTextStyle = GetTextStyle(combinedStyleSheet, dimensionContext);
        if (!textContent.empty()) {
            OH_ArkUI_StyledString_PushTextStyle(m_StyledString, singleTextStyle);
            OH_ArkUI_StyledString_AddText(m_StyledString, textContent.c_str());
            // OH_ArkUI_StyledString_PopTextStyle(m_StyledString);
        }
        if (singleTextStyle) {
            OH_Drawing_DestroyTextStyle(singleTextStyle);
        }
    }

    void TextStyled::SetSingleImageStyle(const StylesheetRef& styleRef, std::shared_ptr<ImageInfo>& imageInfo, const DimensionContextRef& dimensionContext) {
        OH_Drawing_PlaceholderSpan placeholder;
        placeholder.baseline = TEXT_BASELINE_ALPHABETIC;
        placeholder.baselineOffset = 0;
        imageInfo->ResetOffset();
        if (styleRef) {
            if (styleRef->width.has_value()) {
                auto widthValue = styleRef->width.value().ParseToVp(dimensionContext);
                if (widthValue.has_value()) {
                    imageInfo->width = widthValue.value();
                }
            }
            if (styleRef->height.has_value()) {
                auto heightValue = styleRef->height.value().ParseToVp(dimensionContext);
                if (heightValue.has_value()) {
                    imageInfo->height = heightValue.value();
                }
            }
        }

        float width = -1;
        float height = -1;
        switch (imageInfo->mode) {
            case ScaleToFill: {
                width = imageInfo->width != -1 ? imageInfo->width : imageInfo->oriWidth;
                height = imageInfo->height != -1 ? imageInfo->height : imageInfo->oriHeight;
                break;
            };
            case WidthFix: {
                if (imageInfo->width != -1) {
                    width = imageInfo->width;
                    if (imageInfo->oriWidth != -1) {
                        auto ratio = width / imageInfo->oriWidth;
                        height = imageInfo->oriHeight * ratio;
                    } else {
                        height = imageInfo->height;
                    }
                }
                break;
            };
            case HeightFix: {
                if (imageInfo->height != -1) {
                    height = imageInfo->height;
                    if (imageInfo->oriHeight != -1) {
                        auto ratio = height / imageInfo->oriHeight;
                        width = imageInfo->oriWidth * ratio;
                    } else {
                        width = imageInfo->width;
                    }
                }
                break;
            }
            default:
                break;
        }
        if (width == -1 || height == -1) return;
        imageInfo->width = width;
        imageInfo->height = height;
        placeholder.width = vp2Px(imageInfo->width);
        placeholder.height = vp2Px(imageInfo->height);
        ComputeImagePlaceHolderPosition(styleRef, placeholder, imageInfo, dimensionContext);
        if (styleRef) {
            if (styleRef->verticalAlign.has_value()) {
                auto verticalAlign = styleRef->verticalAlign.value();
                placeholder.alignment = verticalAlign;
            } else {
                placeholder.alignment = ALIGNMENT_OFFSET_AT_BASELINE;
            }
        } else {
            placeholder.alignment = ALIGNMENT_OFFSET_AT_BASELINE;
        }
        OH_ArkUI_StyledString_AddPlaceholder(m_StyledString, &placeholder);
    }

    void TextStyled::ComputeImagePlaceHolderPosition(const StylesheetRef& styleRef, OH_Drawing_PlaceholderSpan& placeholder, std::shared_ptr<ImageInfo>& imageInfo, const DimensionContextRef& dimensionContext) {
        if (!styleRef) return;
        if (styleRef->marginRight.has_value()) {
            auto marginRight = styleRef->marginRight.value().ParseToVp(dimensionContext);
            if (marginRight.has_value()) {
                placeholder.width += vp2Px(marginRight.value());
            }
        }
        if (styleRef->marginBottom.has_value()) {
            auto marginBottomValue = styleRef->marginBottom.value().ParseToVp(dimensionContext);
            if (marginBottomValue.has_value()) {
                placeholder.height += vp2Px(marginBottomValue.value());
            }
        }
        if (styleRef->marginLeft.has_value()) {
            auto marginLeftValue = styleRef->marginLeft.value().ParseToVp(dimensionContext);
            if (marginLeftValue.has_value()) {
                imageInfo->offsetLeft = marginLeftValue.value();
                placeholder.width += vp2Px(imageInfo->offsetLeft);
            }
        }
        if (styleRef->marginTop.has_value()) {
            auto marginTopValue = styleRef->marginTop.value().ParseToVp(dimensionContext);
            if (marginTopValue.has_value()) {
                imageInfo->offsetTop = marginTopValue.value();
                placeholder.height += vp2Px(imageInfo->offsetTop);
            }
        }

        if (styleRef->paddingLeft.has_value()) {
            auto paddingLeftValue = styleRef->paddingLeft.value().ParseToVp(dimensionContext);
            if (paddingLeftValue.has_value()) {
                auto vp = paddingLeftValue.value();
                placeholder.width += vp2Px(vp);
                imageInfo->width += vp;
            }
        }
        if (styleRef->paddingRight.has_value()) {
            auto paddingRightValue = styleRef->paddingRight.value().ParseToVp(dimensionContext);
            if (paddingRightValue.has_value()) {
                auto vp = paddingRightValue.value();
                placeholder.width += vp2Px(vp);
                imageInfo->width += vp;
            }
        }
        if (styleRef->paddingTop.has_value()) {
            auto paddingTopValue = styleRef->paddingTop.value().ParseToVp(dimensionContext);
            if (paddingTopValue.has_value()) {
                auto vp = paddingTopValue.value();
                placeholder.height += vp2Px(vp);
                imageInfo->height += vp;
            }
        }
        if (styleRef->paddingBottom.has_value()) {
            auto paddingBottomValue = styleRef->paddingBottom.value().ParseToVp(dimensionContext);
            if (paddingBottomValue.has_value()) {
                auto vp = paddingBottomValue.value();
                placeholder.height += vp2Px(vp);
                imageInfo->height += vp;
            }
        }
        if (styleRef->position.has_value()) {
            auto position = styleRef->position.value();
            if (position == PropertyType::Position::Relative) {
                if (styleRef->top.has_value()) {
                    if (auto topValue = styleRef->top.value().ParseToVp(dimensionContext); topValue.has_value()) {
                        imageInfo->offsetTop += topValue.value();
                    }
                }
                if (styleRef->left.has_value()) {
                    if (auto leftValue = styleRef->left.value().ParseToVp(dimensionContext); leftValue.has_value()) {
                        imageInfo->offsetLeft += leftValue.value();
                    }
                }
            }
        }
    }

    void TextStyled::DestroyStyle() {
        if (m_TypographyStyle) {
            OH_Drawing_DestroyTypographyStyle(m_TypographyStyle);
            m_TypographyStyle = nullptr;
        }
    }

    void TextStyled::DestroyOldTypography() {
        if (m_OldTypography.size() > 0) {
            for (auto typography : m_OldTypography) {
                if (typography) {
                    OH_Drawing_DestroyTypography(typography);
                }
            }
            m_OldTypography.clear();
        }
    }

    void TextStyled::DestroyOldStyledString() {
        if (m_OldStyledString.size() > 0) {
            for (auto styledString : m_OldStyledString) {
                if (styledString) {
                    OH_ArkUI_StyledString_Destroy(styledString);
                }
            }
            m_OldStyledString.clear();
        }
    }

    void TextStyled::SetHashBeenLayout(const bool hasBeenLayout) {
        m_HasBeenLayout = hasBeenLayout;
    }
    bool TextStyled::GetHasBeenLayout() {
        return m_HasBeenLayout;
    }

    void TextStyled::Destroy() {
        if (m_StyledString) {
            OH_ArkUI_StyledString_Destroy(m_StyledString);
            m_StyledString = nullptr;
        }
        if (m_Typography) {
            OH_Drawing_DestroyTypography(m_Typography);
            m_Typography = nullptr;
        }
        DestroyOldStyledString();
        DestroyOldTypography();
    }
}
}