/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#define SET_DRAW_STYLE_WITH_IDX(PROPERTY, EDGE)                   \
    {                                                             \
        paintDiffer_.paint_style_->PROPERTY.value[EDGE].set(val);                            \
        SetDrawDirty(true);                                       \
    }

#define RESET_DRAW_STYLE_WITH_IDX(PROPERTY, EDGE)                 \
    {                                                             \
        if (paintDiffer_.paint_style_->PROPERTY.value[EDGE].has_value()) {                   \
            paintDiffer_.paint_style_->PROPERTY.value[EDGE].reset();                         \
            SetDrawDirty(true);                                   \
        }                                                         \
    }

#define GET_DRAW_STYLE_WITH_IDX(PROPERTY, EDGE)                   \
    {                                                             \
        return paintDiffer_.paint_style_->PROPERTY.value[EDGE];                              \
    }

#define GENERATE_DRAW_STYLE_WITH_IDX_FUNCTIONS(NAME, TYPE, PROPERTY) \
    void BaseRenderNode::Set##NAME(const TYPE &val, const TaroEdge &edge) { \
        SET_DRAW_STYLE_WITH_IDX(PROPERTY, to_underlying(edge));   \
    }                                                             \
    void BaseRenderNode::Reset##NAME(const TaroEdge &edge) {      \
        RESET_DRAW_STYLE_WITH_IDX(PROPERTY, to_underlying(edge)); \
    }                                                             \
    const Optional<TYPE> &BaseRenderNode::Get##NAME(const TaroEdge &edge) const { \
        GET_DRAW_STYLE_WITH_IDX(PROPERTY, to_underlying(edge));   \
    }

#define SET_DRAW_STYLE(PROPERTY)                                  \
    {                                                             \
        paintDiffer_.paint_style_->PROPERTY.value.set(val);                                  \
        SetDrawDirty(true);                                       \
    }

#define RESET_DRAW_STYLE(PROPERTY)                                \
    {                                                             \
        if (paintDiffer_.paint_style_->PROPERTY.value.has_value()) {                         \
            paintDiffer_.paint_style_->PROPERTY.value.reset();                               \
            SetDrawDirty(true);                                   \
        }                                                         \
    }

#define GET_DRAW_STYLE(PROPERTY)                                  \
    {                                                             \
        return paintDiffer_.paint_style_->PROPERTY.value;                                    \
    }

#define SET_DRAW_LAYOUT_STYLE(PROPERTY)                           \
    {                                                             \
        paintDiffer_.paint_style_->PROPERTY.value.set(val);                                  \
        SetDrawDirty(true);                                       \
        SetLayoutDirty(true);                                     \
    }

#define RESET_DRAW_LAYOUT_STYLE(PROPERTY)                        \
    {                                                            \
        if (paintDiffer_.paint_style_->PROPERTY.value.has_value()) {                        \
            paintDiffer_.paint_style_->PROPERTY.value.reset();                              \
            SetDrawDirty(true);                                  \
            SetLayoutDirty(true);                                \
        }                                                        \
    }

#define GENERATE_DRAW_STYLE_FUNCTIONS(NAME, TYPE, PROPERTY)       \
    void BaseRenderNode::Set##NAME(const TYPE &val) {             \
        SET_DRAW_STYLE(PROPERTY);                                 \
    }                                                             \
    void BaseRenderNode::Reset##NAME() {                          \
        RESET_DRAW_STYLE(PROPERTY);                               \
    }                                                             \
    const Optional<TYPE> &BaseRenderNode::Get##NAME() const {     \
        GET_DRAW_STYLE(PROPERTY);                                 \
    }


#define GENERATE_DRAW_LAYOUT_STYLE_FUNCTIONS(NAME, TYPE, PROPERTY)       \
    void BaseRenderNode::Set##NAME(const TYPE &val) {             \
        SET_DRAW_LAYOUT_STYLE(PROPERTY);                                 \
    }                                                             \
    void BaseRenderNode::Reset##NAME() {                          \
        RESET_DRAW_LAYOUT_STYLE(PROPERTY);                               \
    }                                                             \
    const Optional<TYPE> &BaseRenderNode::Get##NAME() const {     \
        GET_DRAW_STYLE(PROPERTY);                                 \
    }

#define CHECK_AND_SET_FONT_STYLE(CSS_PROPERTY, DRAW_PROPERTY, DEFAULT_VALUE)                           \
    {                                                                                                  \
        if (IS_PAINT_DIFFER_PROPERTY_CHANGED(DRAW_PROPERTY)) {                                         \
            if (!IS_PAINT_DIFFER_PROPERTY_REMOVED(DRAW_PROPERTY)) {                                    \
                if ((paintDiffer_.paint_style_->DRAW_PROPERTY).value.has_value()) {                     \
                    CSS_PROPERTY.set((paintDiffer_.paint_style_->DRAW_PROPERTY).value.value());         \
                }                                                                                      \
            } else {                                                                                   \
                if ((DEFAULT_VALUE).has_value()) {                                                     \
                    CSS_PROPERTY.set((DEFAULT_VALUE).value());                                         \
                }                                                                                      \
            }                                                                                          \
            isChanged = true;                                                                          \
        } else if ((paintDiffer_.paint_style_->DRAW_PROPERTY).value.has_value()) {                      \
            CSS_PROPERTY.set((paintDiffer_.paint_style_->DRAW_PROPERTY).value.value());                 \
        }                                                                                              \
    }

#define IS_PROPERTY_WITH_IDX_CHANGED(PROPERTY) \
    (paint_style_.PROPERTY.force_update || \
    (old_paint_style_.PROPERTY.value[0].has_value() != paint_style_.PROPERTY.value[0].has_value()) || \
    (old_paint_style_.PROPERTY.value[1].has_value() != paint_style_.PROPERTY.value[1].has_value()) || \
    (old_paint_style_.PROPERTY.value[2].has_value() != paint_style_.PROPERTY.value[2].has_value()) || \
    (old_paint_style_.PROPERTY.value[3].has_value() != paint_style_.PROPERTY.value[3].has_value()) || \
    (old_paint_style_.PROPERTY.value[0].has_value() && paint_style_.PROPERTY.value[0].has_value() && \
     old_paint_style_.PROPERTY.value[0].value() != paint_style_.PROPERTY.value[0].value()) || \
    (old_paint_style_.PROPERTY.value[1].has_value() && paint_style_.PROPERTY.value[1].has_value() && \
     old_paint_style_.PROPERTY.value[1].value() != paint_style_.PROPERTY.value[1].value()) || \
    (old_paint_style_.PROPERTY.value[2].has_value() && paint_style_.PROPERTY.value[2].has_value() && \
     old_paint_style_.PROPERTY.value[2].value() != paint_style_.PROPERTY.value[2].value()) || \
    (old_paint_style_.PROPERTY.value[3].has_value() && paint_style_.PROPERTY.value[3].has_value() && \
     old_paint_style_.PROPERTY.value[3].value() != paint_style_.PROPERTY.value[3].value()))

#define IS_PROPERTY_WITH_IDX_REMOVED(PROPERTY) \
    ((old_paint_style_.PROPERTY.value[0].has_value() || \
      old_paint_style_.PROPERTY.value[1].has_value() || \
      old_paint_style_.PROPERTY.value[2].has_value() || \
      old_paint_style_.PROPERTY.value[3].has_value()) && \
     !paint_style_.PROPERTY.value[0].has_value() && \
     !paint_style_.PROPERTY.value[1].has_value() && \
     !paint_style_.PROPERTY.value[2].has_value() && \
     !paint_style_.PROPERTY.value[3].has_value())

#define IS_PROPERTY_CHANGED(PROPERTY) \
    (paint_style_.PROPERTY.force_update || \
    (old_paint_style_.PROPERTY.value.has_value() != paint_style_.PROPERTY.value.has_value()) || \
    (old_paint_style_.PROPERTY.value.has_value() && paint_style_.PROPERTY.value.has_value() && \
     old_paint_style_.PROPERTY.value.value() != paint_style_.PROPERTY.value.value()))

#define IS_PROPERTY_REMOVED(PROPERTY) \
    (old_paint_style_.PROPERTY.value.has_value() && !paint_style_.PROPERTY.value.has_value())

#define IS_PAINT_DIFFER_PROPERTY_CHANGED(PROPERTY) \
    (paintDiffer_.paint_style_->PROPERTY.force_update || \
    (paintDiffer_.old_paint_style_->PROPERTY.value.has_value() != paintDiffer_.paint_style_->PROPERTY.value.has_value()) || \
    (paintDiffer_.old_paint_style_->PROPERTY.value.has_value() && paintDiffer_.paint_style_->PROPERTY.value.has_value() && \
     paintDiffer_.old_paint_style_->PROPERTY.value.value() != paintDiffer_.paint_style_->PROPERTY.value.value()))

#define IS_PAINT_DIFFER_PROPERTY_REMOVED(PROPERTY) \
    (paintDiffer_.old_paint_style_->PROPERTY.value.has_value() && !paintDiffer_.paint_style_->PROPERTY.value.has_value())
