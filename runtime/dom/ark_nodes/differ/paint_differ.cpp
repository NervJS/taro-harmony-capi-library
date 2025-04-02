/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "paint_differ.h"

#include "differ_macro.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {

    void PaintDiffer::DiffAndSetStyle(PaintStyle& paint_style_, PaintStyle& old_paint_style_, BaseRenderNode* base_render_node) {
        auto render_node = dynamic_cast<TaroRenderNode*>(base_render_node);
        if (!render_node)
            return;
        bool is_border_style_changed = IS_PROPERTY_WITH_IDX_CHANGED(border_style_);
        bool is_border_style_removed = IS_PROPERTY_WITH_IDX_REMOVED(border_style_);
        bool is_border_radius_changed = IS_PROPERTY_WITH_IDX_CHANGED(border_radius_);
        bool is_border_radius_removed = IS_PROPERTY_WITH_IDX_REMOVED(border_radius_);
        bool is_border_color_changed = IS_PROPERTY_WITH_IDX_CHANGED(border_color_);
        bool is_border_color_removed = IS_PROPERTY_WITH_IDX_REMOVED(border_color_);

        if (is_border_style_changed) {
            if (is_border_style_removed) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBorderStyle(render_node->ark_node_);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBorderStyle(
                    render_node->ark_node_, paint_style_.border_style_.value[0],
                    paint_style_.border_style_.value[1],
                    paint_style_.border_style_.value[2],
                    paint_style_.border_style_.value[3]);
            }
        }
        if (is_border_radius_changed) {
            if (is_border_radius_removed) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBorderRadius(render_node->ark_node_);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBorderRadius(
                    render_node->ark_node_, paint_style_.border_radius_.value[0],
                    paint_style_.border_radius_.value[1],
                    paint_style_.border_radius_.value[2],
                    paint_style_.border_radius_.value[3],
                    render_node->layoutDiffer_.computed_style_.width);
            }
        }
        if (is_border_color_changed) {
            if (is_border_color_removed) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBorderColor(render_node->ark_node_);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBorderColor(
                    render_node->ark_node_,
                    paint_style_.border_color_.value[0],
                    paint_style_.border_color_.value[1],
                    paint_style_.border_color_.value[2],
                    paint_style_.border_color_.value[3]);
            }
        }
        if (IS_PROPERTY_CHANGED(background_color_)) {
            if (IS_PROPERTY_REMOVED(background_color_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBackgroundColor(render_node->ark_node_);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBackgroundColor(render_node->ark_node_, paint_style_.background_color_.value);
            }
        }
        if (IS_PROPERTY_CHANGED(boxShadow_)) {
            if (IS_PROPERTY_REMOVED(boxShadow_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBoxShadow(render_node->ark_node_);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBoxShadow(render_node->ark_node_, paint_style_.boxShadow_.value);
            }
        }
        if (IS_PROPERTY_CHANGED(opacity_)) {
            if (IS_PROPERTY_REMOVED(opacity_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setOpacity(render_node->ark_node_);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setOpacity(render_node->ark_node_, paint_style_.opacity_.value);
            }
        }

        if (IS_PROPERTY_CHANGED(fixed_visibility_)) {
            if (paint_style_.fixed_visibility_.value.value_or(ARKUI_VISIBILITY_VISIBLE) == ARKUI_VISIBILITY_HIDDEN) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setVisibility(render_node->ark_node_, ARKUI_VISIBILITY_HIDDEN);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setVisibility(render_node->ark_node_, paint_style_.visibility_.value.value_or(ARKUI_VISIBILITY_VISIBLE));
            }
        } else if (paint_style_.fixed_visibility_.value.value_or(ARKUI_VISIBILITY_VISIBLE) != ARKUI_VISIBILITY_HIDDEN) {
            if (IS_PROPERTY_CHANGED(visibility_)) {
                if (IS_PROPERTY_REMOVED(visibility_)) {
                    TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setVisibility(render_node->ark_node_);
                } else {
                    TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setVisibility(render_node->ark_node_, paint_style_.visibility_.value);
                }
            }
        } else {
            TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setVisibility(render_node->ark_node_, ARKUI_VISIBILITY_HIDDEN);
        }

        bool is_background_image_reset = false;
        bool is_background_position_reset = false;
        bool is_background_size_reset = false;
        // 背景图三剑客（image，size，position）任一个变了都调setBackgroundImageAndPositionAndSize把三个都更新，但reset各自reset
        // 而且reset要先设，setBackgroundImageAndPositionAndSize的结果是一定准的，如果reset后面会把必准的覆盖掉
        if (
            (old_paint_style_.background_position_.value[0].has_value() && !paint_style_.background_position_.value[0].has_value()) ||
            (old_paint_style_.background_position_.value[1].has_value() && !paint_style_.background_position_.value[1].has_value())) {
            TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBackgroundPosition(render_node->ark_node_);
            is_background_position_reset = true;
        }
        if (IS_PROPERTY_REMOVED(background_image_)) {
            TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBackgroundImage(render_node->ark_node_);
            is_background_image_reset = true;
        }
        if (IS_PROPERTY_REMOVED(background_size_)) {
            TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBackgroundSize(render_node->ark_node_);
            is_background_size_reset = true;
        }
        if (
            (((old_paint_style_.background_position_.value[0].has_value() != paint_style_.background_position_.value[0].has_value()) ||
              (old_paint_style_.background_position_.value[1].has_value() != paint_style_.background_position_.value[1].has_value()) ||
              (old_paint_style_.background_position_.value[0].has_value() && paint_style_.background_position_.value[0].has_value() &&
               old_paint_style_.background_position_.value[0].value() != paint_style_.background_position_.value[0].value()) ||
              (old_paint_style_.background_position_.value[1].has_value() && paint_style_.background_position_.value[1].has_value() &&
               old_paint_style_.background_position_.value[1].value() != paint_style_.background_position_.value[1].value())) &&
             !is_background_position_reset) ||
            (IS_PROPERTY_CHANGED(background_image_) && !is_background_image_reset) ||
            (IS_PROPERTY_CHANGED(background_size_) && !is_background_size_reset)) {
            render_node->setBackgroundImageAndPositionAndSize();
        }

        if (IS_PROPERTY_CHANGED(transform_)) {
            if (IS_PROPERTY_REMOVED(transform_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setTransform(render_node->ark_node_);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setTransform(
                    render_node->ark_node_,
                    paint_style_.transform_.value,
                    render_node->layoutDiffer_.computed_style_.width,
                    render_node->layoutDiffer_.computed_style_.height);
            }
        }
        if (IS_PROPERTY_CHANGED(transform_origin_)) {
            if (IS_PROPERTY_REMOVED(transform_origin_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setTransformOrigin(render_node->ark_node_);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setTransformOrigin(render_node->ark_node_, paint_style_.transform_origin_.value);
            }
        }
        if (IS_PROPERTY_CHANGED(zIndex_)) {
            if (IS_PROPERTY_REMOVED(zIndex_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setZIndex(render_node->ark_node_);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setZIndex(render_node->ark_node_, paint_style_.zIndex_.value);
            }
        }

        if (IS_PROPERTY_CHANGED(pointerEvents_)) {
            if (IS_PROPERTY_REMOVED(pointerEvents_)) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setPointerEvents(render_node->ark_node_);
            } else {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setPointerEvents(render_node->ark_node_, paint_style_.pointerEvents_.value);
            }
        }
    }

    void PaintDiffer::ClearForceUpdate() {
        paint_style_->border_style_.force_update = false;
        paint_style_->border_radius_.force_update = false;
        paint_style_->border_color_.force_update = false;
        paint_style_->background_color_.force_update = false;
        paint_style_->background_repeat_.force_update = false;
        paint_style_->background_image_.force_update = false;
        paint_style_->background_size_.force_update = false;
        paint_style_->background_position_.force_update = false;
        paint_style_->opacity_.force_update = false;
        paint_style_->visibility_.force_update = false;
        paint_style_->fixed_visibility_.force_update = false;
        paint_style_->transform_.force_update = false;
        paint_style_->transform_origin_.force_update = false;
        paint_style_->zIndex_.force_update = false;
        paint_style_->pointerEvents_.force_update = false;

        paint_style_->color_.force_update = false;
        paint_style_->fontSize_.force_update = false;
        paint_style_->fontWeight_.force_update = false;
        paint_style_->fontStyle_.force_update = false;
        paint_style_->fontFamily_.force_update = false;
        paint_style_->lineHeight_.force_update = false;
        paint_style_->letterSpacing_.force_update = false;
        paint_style_->wordBreak_.force_update = false;
        paint_style_->textAlign_.force_update = false;
        paint_style_->webkitLineClamp_.force_update = false;
        paint_style_->whiteSpace_.force_update = false;
        paint_style_->textDecorationLine_.force_update = false;
        paint_style_->textDecorationStyle_.force_update = false;
        paint_style_->textOverflow_.force_update = false;
        paint_style_->boxShadow_.force_update = false;
    }

    void PaintDiffer::ClearOldStyle() {
        old_paint_style_->border_style_ = DrawProperty<std::array<TaroHelper::Optional<ArkUI_BorderStyle>, 4>>();
        old_paint_style_->border_radius_ = DrawProperty<std::array<TaroHelper::Optional<Dimension>, 4>>();
        old_paint_style_->border_color_ = DrawProperty<std::array<TaroHelper::Optional<uint32_t>, 4>>();
        old_paint_style_->background_color_ = DrawProperty<TaroHelper::Optional<uint32_t>>();
        old_paint_style_->background_repeat_ = DrawProperty<TaroHelper::Optional<ArkUI_ImageRepeat>>();
        old_paint_style_->background_image_ = DrawProperty<TaroHelper::Optional<TaroCSSOM::TaroStylesheet::BackgroundImageItem>>();
        old_paint_style_->background_size_ = DrawProperty<TaroHelper::Optional<TaroCSSOM::TaroStylesheet::BackgroundSizeParam>>();
        old_paint_style_->background_position_ = DrawProperty<std::array<TaroHelper::Optional<Dimension>, 2>>();
        old_paint_style_->opacity_ = DrawProperty<TaroHelper::Optional<float>>();
        old_paint_style_->visibility_ = DrawProperty<TaroHelper::Optional<ArkUI_Visibility>>();
        old_paint_style_->fixed_visibility_ = DrawProperty<TaroHelper::Optional<ArkUI_Visibility>>();
        old_paint_style_->transform_ = DrawProperty<TaroHelper::Optional<TaroCSSOM::TaroStylesheet::TransformParam>>();
        old_paint_style_->transform_origin_ = DrawProperty<TaroHelper::Optional<TaroCSSOM::TaroStylesheet::TransformOriginData>>();
        old_paint_style_->zIndex_ = DrawProperty<TaroHelper::Optional<int32_t>>();
        old_paint_style_->pointerEvents_ = DrawProperty<TaroHelper::Optional<PropertyType::PointerEvents>>();

        old_paint_style_->color_ = DrawProperty<TaroHelper::Optional<uint32_t>>();
        old_paint_style_->fontSize_ = DrawProperty<TaroHelper::Optional<Dimension>>();
        old_paint_style_->fontWeight_ = DrawProperty<TaroHelper::Optional<ArkUI_FontWeight>>();
        old_paint_style_->fontStyle_ = DrawProperty<TaroHelper::Optional<ArkUI_FontStyle>>();
        old_paint_style_->fontFamily_ = DrawProperty<TaroHelper::Optional<std::string>>();
        old_paint_style_->lineHeight_ = DrawProperty<TaroHelper::Optional<Dimension>>();
        old_paint_style_->letterSpacing_ = DrawProperty<TaroHelper::Optional<float>>();
        old_paint_style_->wordBreak_ = DrawProperty<TaroHelper::Optional<OH_Drawing_WordBreakType>>();
        old_paint_style_->textAlign_ = DrawProperty<TaroHelper::Optional<ArkUI_TextAlignment>>();
        old_paint_style_->webkitLineClamp_ = DrawProperty<TaroHelper::Optional<int32_t>>();
        old_paint_style_->whiteSpace_ = DrawProperty<TaroHelper::Optional<PropertyType::WhiteSpace>>();
        old_paint_style_->textDecorationLine_ = DrawProperty<TaroHelper::Optional<OH_Drawing_TextDecoration>>();
        old_paint_style_->textDecorationStyle_ = DrawProperty<TaroHelper::Optional<OH_Drawing_TextDecorationStyle>>();
        old_paint_style_->textOverflow_ = DrawProperty<TaroHelper::Optional<ArkUI_TextOverflow>>();
        old_paint_style_->boxShadow_ = DrawProperty<TaroHelper::Optional<TaroCSSOM::TaroStylesheet::BoxShadowItem>>();
    }

} // namespace TaroDOM
} // namespace TaroRuntime
