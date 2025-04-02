/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "helper/Optional.h"
#include "runtime/cssom/dimension/dimension.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/params/background_size_param/background_size_param.h"
#include "runtime/cssom/stylesheet/params/transform_param/transform_param.h"
#include "runtime/cssom/stylesheet/background_image.h"
#include "runtime/cssom/stylesheet/background_size.h"
#include "runtime/cssom/stylesheet/box_shadow.h"
#include "runtime/cssom/stylesheet/common.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/transform_origin.h"
#include "runtime/cssom/stylesheet/utils.h"
#include <arkui/native_type.h>
#include <native_drawing/drawing_text_typography.h>

namespace TaroRuntime {
namespace TaroDOM {

class BaseRenderNode;
template <typename T>
struct DrawProperty {
    T value;
    bool force_update = false;
};
struct PaintStyle {
    DrawProperty<std::array<TaroHelper::Optional<ArkUI_BorderStyle>, 4>> border_style_;
    DrawProperty<std::array<TaroHelper::Optional<Dimension>, 4>> border_radius_;
    DrawProperty<std::array<TaroHelper::Optional<uint32_t>, 4>> border_color_;
    DrawProperty<TaroHelper::Optional<uint32_t>> background_color_;
    DrawProperty<TaroHelper::Optional<ArkUI_ImageRepeat>> background_repeat_;
    DrawProperty<TaroHelper::Optional<TaroCSSOM::TaroStylesheet::BackgroundImageItem>> background_image_;
    DrawProperty<TaroHelper::Optional<TaroCSSOM::TaroStylesheet::BackgroundSizeParam>> background_size_;
    DrawProperty<std::array<TaroHelper::Optional<Dimension>, 2>> background_position_;
    DrawProperty<TaroHelper::Optional<float>> opacity_;
    DrawProperty<TaroHelper::Optional<ArkUI_Visibility>> visibility_;
    DrawProperty<TaroHelper::Optional<ArkUI_Visibility>> fixed_visibility_;
    DrawProperty<TaroHelper::Optional<TaroCSSOM::TaroStylesheet::TransformParam>> transform_;
    DrawProperty<TaroHelper::Optional<TaroCSSOM::TaroStylesheet::TransformOriginData>> transform_origin_;
    DrawProperty<TaroHelper::Optional<int32_t>> zIndex_;
    DrawProperty<TaroHelper::Optional<PropertyType::PointerEvents>> pointerEvents_;

    DrawProperty<TaroHelper::Optional<uint32_t>> color_;
    DrawProperty<TaroHelper::Optional<Dimension>> fontSize_;
    DrawProperty<TaroHelper::Optional<ArkUI_FontWeight>> fontWeight_;
    DrawProperty<TaroHelper::Optional<ArkUI_FontStyle>> fontStyle_;
    DrawProperty<TaroHelper::Optional<std::string>> fontFamily_;
    DrawProperty<TaroHelper::Optional<Dimension>> lineHeight_;
    DrawProperty<TaroHelper::Optional<float>> letterSpacing_;
    DrawProperty<TaroHelper::Optional<OH_Drawing_WordBreakType>> wordBreak_;
    DrawProperty<TaroHelper::Optional<ArkUI_TextAlignment>> textAlign_;
    DrawProperty<TaroHelper::Optional<int32_t>> webkitLineClamp_;
    DrawProperty<TaroHelper::Optional<PropertyType::WhiteSpace>> whiteSpace_;
    DrawProperty<TaroHelper::Optional<OH_Drawing_TextDecoration>> textDecorationLine_;
    DrawProperty<TaroHelper::Optional<OH_Drawing_TextDecorationStyle>> textDecorationStyle_;
    DrawProperty<TaroHelper::Optional<ArkUI_TextOverflow>> textOverflow_;
    DrawProperty<TaroHelper::Optional<TaroCSSOM::TaroStylesheet::BoxShadowItem>> boxShadow_;

    PaintStyle &operator=(const PaintStyle &paintStyle) {
        if (this == &paintStyle) {
            return *this;
        }
        border_style_ = paintStyle.border_style_;
        border_radius_ = paintStyle.border_radius_;
        border_color_ = paintStyle.border_color_;
        background_color_ = paintStyle.background_color_;
        background_repeat_ = paintStyle.background_repeat_;
        background_image_ = paintStyle.background_image_;
        background_size_ = paintStyle.background_size_;
        background_position_ = paintStyle.background_position_;
        opacity_ = paintStyle.opacity_;
        visibility_= paintStyle.visibility_;
        fixed_visibility_ = paintStyle.fixed_visibility_;
        transform_ = paintStyle.transform_;
        transform_origin_= paintStyle.transform_origin_;
        zIndex_ = paintStyle.zIndex_;
        pointerEvents_ = paintStyle.pointerEvents_;

        color_= paintStyle.color_;
        fontSize_ = paintStyle.fontSize_;
        fontWeight_ = paintStyle.fontWeight_;
        fontStyle_ = paintStyle.fontStyle_;
        fontFamily_ = paintStyle.fontFamily_;
        lineHeight_ = paintStyle.lineHeight_;
        letterSpacing_ = paintStyle.letterSpacing_;
        wordBreak_ = paintStyle.wordBreak_;
        textAlign_ = paintStyle.textAlign_;
        webkitLineClamp_ = paintStyle.webkitLineClamp_;
        whiteSpace_ = paintStyle.whiteSpace_;
        textDecorationLine_ = paintStyle.textDecorationLine_;
        textDecorationStyle_ = paintStyle.textDecorationStyle_;
        textOverflow_ = paintStyle.textOverflow_;
        boxShadow_ = paintStyle.boxShadow_;

        return *this;
    }
};

class PaintDiffer {
    public:
    PaintDiffer(BaseRenderNode* render_node): render_node_(render_node) {
        paint_style_ = std::make_shared<PaintStyle>();
        old_paint_style_ = std::make_shared<PaintStyle>();
    };

    ~PaintDiffer() {}

    std::shared_ptr<PaintStyle> paint_style_;

    std::shared_ptr<PaintStyle> old_paint_style_;

    void ClearForceUpdate();
    void ClearOldStyle();

    static void DiffAndSetStyle(PaintStyle& paint_style_, PaintStyle& old_paint_style_, BaseRenderNode* base_render_node);

    private:
    BaseRenderNode* render_node_;
};

}
}
