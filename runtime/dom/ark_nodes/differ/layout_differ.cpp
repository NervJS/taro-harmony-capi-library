//
// Created on 2024/9/14.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "./layout_differ.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"
#include "runtime/dom/ark_nodes/scroll_container.h"
#include "runtime/dom/ark_nodes/swiper.h"
#include "yoga/YGNodeLayout.h"
#include "yoga/YGNodeStyle.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {

void LayoutDiffer::SetComputedStyle(YGNodeRef yGNodeRef, bool update_old_style) {
    if (update_old_style) {
        old_computed_style_ = computed_style_;
    }
    
    computed_style_.width = YGNodeLayoutGetWidth(yGNodeRef);
    computed_style_.height = YGNodeLayoutGetHeight(yGNodeRef);
    computed_style_.top = YGNodeLayoutGetTop(yGNodeRef);
    computed_style_.left = YGNodeLayoutGetLeft(yGNodeRef);
    computed_style_.right = YGNodeLayoutGetRight(yGNodeRef);
    computed_style_.bottom = YGNodeLayoutGetBottom(yGNodeRef);
    computed_style_.paddingTop = YGNodeLayoutGetPadding(yGNodeRef, YGEdgeTop);
    computed_style_.paddingRight = YGNodeLayoutGetPadding(yGNodeRef, YGEdgeRight);
    computed_style_.paddingBottom = YGNodeLayoutGetPadding(yGNodeRef, YGEdgeBottom);
    computed_style_.paddingLeft = YGNodeLayoutGetPadding(yGNodeRef, YGEdgeLeft);
    computed_style_.borderTop = YGNodeLayoutGetBorder(yGNodeRef, YGEdgeTop);
    computed_style_.borderRight = YGNodeLayoutGetBorder(yGNodeRef, YGEdgeRight);
    computed_style_.borderBottom = YGNodeLayoutGetBorder(yGNodeRef, YGEdgeBottom);
    computed_style_.borderLeft = YGNodeLayoutGetBorder(yGNodeRef, YGEdgeLeft);
    computed_style_.marginTop = YGNodeLayoutGetMargin(yGNodeRef, YGEdgeTop);
    computed_style_.marginRight = YGNodeLayoutGetMargin(yGNodeRef, YGEdgeRight);
    computed_style_.marginBottom = YGNodeLayoutGetMargin(yGNodeRef, YGEdgeBottom);
    computed_style_.marginLeft = YGNodeLayoutGetMargin(yGNodeRef, YGEdgeLeft);
    computed_style_.overflow = YGNodeStyleGetOverflow(yGNodeRef);
    computed_style_.display = YGNodeStyleGetDisplay(yGNodeRef);
};

float LayoutDiffer::GetComputedStyle(const char *name) const {
    auto render_node = dynamic_cast<TaroRenderNode*>(base_node_);
    if (!render_node) 0.0f;
    if (strcmp(name, "height") == 0) {
        return computed_style_.height;
    } else if (strcmp(name, "width") == 0) {
        return computed_style_.width;
    } else if (strcmp(name, "paddingTop") == 0) {
        return computed_style_.paddingTop;
    } else if (strcmp(name, "paddingRight") == 0) {
        return computed_style_.paddingRight;
    } else if (strcmp(name, "paddingBottom") == 0) {
        return computed_style_.paddingBottom;
    } else if (strcmp(name, "paddingLeft") == 0) {
        return computed_style_.paddingLeft;
    } else if (strcmp(name, "borderTop") == 0) {
        return computed_style_.borderTop;
    } else if (strcmp(name, "borderRight") == 0) {
        return computed_style_.borderRight;
    } else if (strcmp(name, "borderBottom") == 0) {
        return computed_style_.borderBottom;
    } else if (strcmp(name, "borderLeft") == 0) {
        return computed_style_.borderLeft;
    } else if (strcmp(name, "marginTop") == 0) {
        return computed_style_.marginTop;
    } else if (strcmp(name, "marginRight") == 0) {
        return computed_style_.marginRight;
    } else if (strcmp(name, "marginBottom") == 0) {
        return computed_style_.marginBottom;
    } else if (strcmp(name, "marginLeft") == 0) {
        return computed_style_.marginLeft;
    } else if (strcmp(name, "top") == 0) {
        return computed_style_.top;
    } else if (strcmp(name, "left") == 0) {
        return computed_style_.left;
    } else if (strcmp(name, "right") == 0) {
        return computed_style_.right;
    } else if (strcmp(name, "bottom") == 0) {
        return computed_style_.bottom;
    } else if (strcmp(name, "globalX") == 0 || strcmp(name, "globalY") == 0) {
        GlobalPostion position = {0.0f, 0.0f};
        if (auto node = render_node->element_ref_.lock()) {
            auto render_node = node->GetHeadRenderNode();
            if (render_node) {
                position = render_node->calcGlobalPostion();
            }
        }

        if (strcmp(name, "globalX") == 0) {
            return position.global_x;
        } else {
            return position.global_y;
        }
    } else {
        // 如果属性名不匹配，返回默认值 0.0f
        TARO_LOG_ERROR("Render", "computed style[%{public}s] is not found", name);
        return 0.0f;
    }
};

YGOverflow LayoutDiffer::GetOverFlowStyle () {
    return computed_style_.overflow;
};

YGDisplay LayoutDiffer::GetDisplayStyle () {
    return computed_style_.display;
};


void LayoutDiffer::DiffAndSetStyle(ComputedStyle computed_style, ComputedStyle old_computed_style, BaseRenderNode* base_render_node) {
    auto render_node = dynamic_cast<TaroRenderNode*>(base_render_node);
    if (!render_node) return;
    const auto parent = render_node->parent_ref_.lock();
    bool is_parent_should_position = parent && parent->GetShouldPosition();
    bool is_first_layout = !render_node->is_first_layout_finish_;
    bool is_display_changed = old_computed_style.display != computed_style.display;
    bool is_width_changed = old_computed_style.width != computed_style.width;
    bool is_height_changed = old_computed_style.height != computed_style.height;
    bool is_position_changed = old_computed_style.left != computed_style.left || old_computed_style.top != computed_style.top;
    bool is_padding_changed = old_computed_style.paddingTop != computed_style.paddingTop ||
                              old_computed_style.paddingRight != computed_style.paddingRight ||
                              old_computed_style.paddingBottom != computed_style.paddingBottom ||
                              old_computed_style.paddingLeft != computed_style.paddingLeft;
    bool is_margin_changed = old_computed_style.marginTop != computed_style.marginTop ||
                             old_computed_style.marginRight != computed_style.marginRight ||
                             old_computed_style.marginBottom != computed_style.marginBottom ||
                             old_computed_style.marginLeft != computed_style.marginLeft;
    bool is_border_changed = old_computed_style.borderTop != computed_style.borderTop ||
                             old_computed_style.borderRight != computed_style.borderRight ||
                             old_computed_style.borderBottom != computed_style.borderBottom ||
                             old_computed_style.borderLeft != computed_style.borderLeft;
    bool is_overflow_changed = old_computed_style.overflow != computed_style.overflow;
    
    // Diff出新旧的计算属性，设置到ark_node上
    if (is_first_layout || is_display_changed) {
        auto visibility = computed_style.display == YGDisplayNone ? ARKUI_VISIBILITY_NONE : ARKUI_VISIBILITY_VISIBLE;

        TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setVisibility(render_node->ark_node_, visibility);
        // 广播通知Fixed需要重新识别其祖先节点是否已经显示/隐藏
        render_node->AddNotifyFixedRoot();
    }
    if (is_first_layout || is_width_changed) {
        TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setWidth(render_node->ark_node_, computed_style.width);
    }
    if (is_first_layout || is_height_changed) {
        TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setHeight(render_node->ark_node_, computed_style.height);
    }
    if (is_first_layout || is_border_changed) {
        TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setBorderWidth(
            render_node->ark_node_, computed_style.borderTop, computed_style.borderRight, computed_style.borderBottom, computed_style.borderLeft);
    }
    
    if (is_first_layout || is_overflow_changed) {
        bool is_overflow_hidden = computed_style.overflow == YGOverflowHidden || computed_style.overflow == YGOverflowScroll;
        
         if (dynamic_cast<TaroScrollContainerNode *>(render_node)) {
             is_overflow_hidden = true;
         } else if (dynamic_cast<TaroSwiperNode *>(render_node)) {
//              is_overflow_hidden = true;
         }
            
        TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setOverflow(render_node->ark_node_, is_overflow_hidden ? 1 : 0);
    }
    
    if (is_parent_should_position) {
        if (is_first_layout || is_position_changed) {
            ArkUI_NumberValue arkUI_NumberValue[] = {0.0, 0.0};
            ArkUI_AttributeItem item = {arkUI_NumberValue, 2};
            arkUI_NumberValue[0].f32 = computed_style.left;
            arkUI_NumberValue[1].f32 = computed_style.top;
            NativeNodeApi::getInstance()->setAttribute(render_node->ark_node_, NODE_POSITION, &item);
        }
        if (!render_node->GetShouldPosition() && (is_padding_changed || is_first_layout)) {
            TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setPadding(
            render_node->ark_node_, computed_style.paddingTop, computed_style.paddingRight, computed_style.paddingBottom, computed_style.paddingLeft);
        }
    } else {
        if (is_first_layout || is_margin_changed) {
            TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setMargin(
                render_node->ark_node_, computed_style.marginTop, computed_style.marginRight, computed_style.marginBottom, computed_style.marginLeft);
        }
    }
};

}
}