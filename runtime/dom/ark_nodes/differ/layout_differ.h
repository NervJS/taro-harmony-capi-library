/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "yoga/YGConfig.h"

namespace TaroRuntime {
namespace TaroDOM {

    class BaseRenderNode;

    struct ComputedStyle {
        float height;
        float width;
        float paddingTop;
        float paddingRight;
        float paddingBottom;
        float paddingLeft;
        float borderTop;
        float borderRight;
        float borderBottom;
        float borderLeft;
        float marginTop;
        float marginRight;
        float marginBottom;
        float marginLeft;
        double top;
        double left;
        double right;
        double bottom;
        double globalX;
        double globalY;
        YGOverflow overflow;
        YGDisplay display;
    };

    class LayoutDiffer {
        public:
        LayoutDiffer(BaseRenderNode* render_node)
            : base_node_(render_node) {};

        ~LayoutDiffer() {};

        void SetComputedStyle(YGNodeRef yGNodeRef, bool update_old_style = true);

        float GetComputedStyle(const char* name) const;

        YGOverflow GetOverFlowStyle();

        YGDisplay GetDisplayStyle();

        static void DiffAndSetStyle(ComputedStyle computed_style, ComputedStyle old_computed_style, BaseRenderNode* base_render_node);

        ComputedStyle computed_style_;

        ComputedStyle old_computed_style_;

        private:
        BaseRenderNode* base_node_;
    };

} // namespace TaroDOM
} // namespace TaroRuntime
