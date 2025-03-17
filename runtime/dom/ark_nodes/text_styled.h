//
// Created on 2024/9/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_HARMONY_CPP_TEXT_STYLED_H
#define TARO_HARMONY_CPP_TEXT_STYLED_H
#include <arkui/styled_string.h>
#include <native_drawing/drawing_text_declaration.h>
#include <vector>
#include "runtime/dom/ark_nodes/arkui_node.h"
#include "runtime/dom/element/image.h"
namespace TaroRuntime {
namespace TaroDOM {
struct ImageInfo {
    std::string src = "";
    int index = -1;
    float width = -1;
    float height = -1;
    float oriWidth = -1;
    float oriHeight = -1;
    float offsetTop = 0;
    float offsetLeft = 0;
    int32_t nid = 0;
    ImageMode mode = ScaleToFill;

    void ResetOffset() {
        offsetTop = 0;
        offsetLeft = 0;
    }
};
static const std::unordered_map<ArkUI_FontWeight, OH_Drawing_FontWeight> FONT_WEIGHT_MAP = {
    {ARKUI_FONT_WEIGHT_W100, FONT_WEIGHT_100},
    {ARKUI_FONT_WEIGHT_W200, FONT_WEIGHT_200},
    {ARKUI_FONT_WEIGHT_W300, FONT_WEIGHT_300},
    {ARKUI_FONT_WEIGHT_W400, FONT_WEIGHT_400},
    {ARKUI_FONT_WEIGHT_W500, FONT_WEIGHT_500},
    {ARKUI_FONT_WEIGHT_W600, FONT_WEIGHT_600},
    {ARKUI_FONT_WEIGHT_W700, FONT_WEIGHT_700},
    {ARKUI_FONT_WEIGHT_W800, FONT_WEIGHT_800},
    {ARKUI_FONT_WEIGHT_W900, FONT_WEIGHT_900},
    {ARKUI_FONT_WEIGHT_NORMAL, FONT_WEIGHT_400},
    {ARKUI_FONT_WEIGHT_BOLD, FONT_WEIGHT_700},
    {ARKUI_FONT_WEIGHT_MEDIUM, FONT_WEIGHT_500},
    {ARKUI_FONT_WEIGHT_REGULAR, FONT_WEIGHT_400},
};
class TextStyled {
private:
    OH_Drawing_Typography *m_Typography = nullptr;
    std::vector<OH_Drawing_Typography *> m_OldTypography;
    ArkUI_StyledString *m_StyledString = nullptr;
    std::vector<ArkUI_StyledString *> m_OldStyledString;
    OH_Drawing_TypographyStyle *m_TypographyStyle = nullptr;
    bool m_HasBeenLayout = false;
    
public:
    TextStyled();
    ~TextStyled();
    void InitStyledString(const StylesheetRef& nodeStyle, const StylesheetRef& textStyle, const DimensionContextRef&);
    void InitTypography();
    void TextTypographyLayout(OH_Drawing_Typography *, const double);
    ArkUI_StyledString* GetStyledString();
    OH_Drawing_Typography* GetTypography();
    void DestroyStyle();
    OH_Drawing_TextStyle* GetTextStyle(const StylesheetRef& style, const DimensionContextRef& dimensionContext);
    void SetTypographyStyle(OH_Drawing_TypographyStyle* typographyStyle, const StylesheetRef& style, const StylesheetRef& style_ref, OH_Drawing_TextStyle* textStyle);
    void SetSingleTextStyle(const StylesheetRef&, const StylesheetRef&, const std::string&, const DimensionContextRef&);
    void SetSingleImageStyle(const StylesheetRef&, std::shared_ptr<ImageInfo>&, const DimensionContextRef&);
    void ComputeImagePlaceHolderPosition(const StylesheetRef&, OH_Drawing_PlaceholderSpan&, std::shared_ptr<ImageInfo>&, const DimensionContextRef&);
    void SetHashBeenLayout(const bool hasBeenLayout);
    bool GetHasBeenLayout();
    void DestroyOldTypography();
    void DestroyOldStyledString();
    void Destroy();
};
}
}

#endif //TARO_HARMONY_CPP_TEXT_STYLED_H
