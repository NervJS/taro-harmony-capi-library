//
// Created on 2024/9/14.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
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
    LayoutDiffer(BaseRenderNode* render_node): base_node_(render_node) {};
    
    ~LayoutDiffer() {};
    
    void SetComputedStyle(YGNodeRef yGNodeRef, bool update_old_style = true);
    
    float GetComputedStyle(const char *name) const;

    YGOverflow GetOverFlowStyle ();
    
    YGDisplay GetDisplayStyle ();
    
    static void DiffAndSetStyle(ComputedStyle computed_style, ComputedStyle old_computed_style, BaseRenderNode* base_render_node);
    
    bool is_first_layout_finish_;
    
    ComputedStyle computed_style_;
    
    ComputedStyle old_computed_style_;
    
    private:
    BaseRenderNode* base_node_;
};

}
}