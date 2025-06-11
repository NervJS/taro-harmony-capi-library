/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <arkui/native_node.h>
#include <arkui/native_type.h>

#include "helper/Optional.h"
#include "helper/TaroLog.h"
#include "helper/life_statistic.h"
#include "runtime/cssom/stylesheet/common.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/params/transform_param/transform_param.h"
#include "runtime/cssom/stylesheet/transition.h"
#include "runtime/dom/animation/js_animation_param.h"
// #include "runtime/dom/animation/node_animations.h"
#include "runtime/dom/ark_nodes/arkui_base_node.h"
#include "runtime/dom/ark_nodes/differ/layout_differ.h"
#include "runtime/dom/ark_nodes/differ/paint_differ.h"
#include "runtime/keyframe_vsync.h"
#include "yoga/YGConfig.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroNodeAnimations;
}
// 前置申明
namespace TaroDOM {
    class TaroElement;
}

// 前置申明
namespace TaroCSSOM {
    namespace TaroStylesheet {
        class Stylesheet;
    }
} // namespace TaroCSSOM

namespace TaroDOM {
    using TaroElementRef = std::shared_ptr<TaroDOM::TaroElement>;
    using StylesheetRef = std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet>;

    struct CalcValue {
        TaroHelper::Optional<std::string> width;
        TaroHelper::Optional<std::string> height;
    };

    struct GlobalPostion {
        double global_x = 0.0f;
        double global_y = 0.0f;
        double offset_x = 0.0f;
        double offset_y = 0.0f;
        double scroll_x = 0.0f;
        double scroll_y = 0.0f;
        // 父亲及以上节点是否为 Visibility::NONE
        bool is_hidden = false;
        // 父亲及以上节点是懒加载容器的情况下，自身 item 是否在 index 范围内
        bool is_lazy_index_outside = false;
    };

    class TaroRenderNode : public BaseRenderNode, public TaroClassLife<TaroRenderNode> {
        public:
        TaroRenderNode();

        TaroRenderNode(const TaroElementRef element);

        virtual ~TaroRenderNode();

        int32_t uid_;
        std::weak_ptr<TaroDOM::TaroElement> element_ref_;
        // 合成样式
        StylesheetRef style_ref_;
        StylesheetRef old_style_ref_;
#if IS_DEBUG
        uint16_t element_nid_;
        std::string element_node_name_;
        std::string element_class_name_;
#endif

        std::weak_ptr<TaroRenderNode> parent_ref_;
        std::vector<std::shared_ptr<TaroRenderNode>> children_refs_;

        std::shared_ptr<TaroAnimate::TaroNodeAnimations> animation_ = nullptr;

        virtual void SetStyle(StylesheetRef style_ref);
        void MeasureLocal();
        void LayoutLocal();
        // 任何节点都能调用的 MeasureAndLayoutLocal 方法，会用父节点对其约束，然后重新测量和布局
        void MeasureAndLayoutLocal();
        // 而 Measure 虽然也能测量，但是当前节点的 position 只会测出 0,0，需要后续手动清除 yoga 节点的标脏
        virtual void Measure();

        void LayoutAll(bool layoutWithoutDiff = false);

        virtual void OnMeasure(ArkUI_NodeCustomEvent* event);
        virtual void OnLayout(ArkUI_NodeCustomEvent* event);

        // 布局
        virtual void Layout();
        // 绘制
        virtual void Paint();

        // 需要子类实现：构建节点->创建C-API节点
        virtual void Build() {};

        virtual float GetComputedStyle(const char* name) const;

        ArkUI_NodeHandle GetArkUINodeHandle();
        void SetArkUINodeHandle(ArkUI_NodeHandle handle);

        bool OnSetPropertyIntoNode(const CSSProperty::Type& property,
                                   const TaroChange& changeType,
                                   const std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet>& style) override;

        // 布局的影响会导致部分绘制样式（依赖布局尺寸）需要重新绘制
        void MakeDrawPropertyDirtyFromLayoutEffect();
        // 判断是否需要立即挂载
        bool IfImmediateAttach(const std::shared_ptr<TaroRenderNode>& child);

        virtual void AppendChild(const std::shared_ptr<TaroRenderNode>& child);
        virtual void RemoveChild(const std::shared_ptr<TaroRenderNode>& child);

        virtual void ReplaceChild(const std::shared_ptr<TaroRenderNode>& old_child, const std::shared_ptr<TaroRenderNode>& new_child);
        virtual void InsertChildAt(const std::shared_ptr<TaroRenderNode>& child, uint8_t index);
        virtual void InsertChildBefore(const std::shared_ptr<TaroRenderNode>& child, const std::shared_ptr<TaroRenderNode>& sibling);
        virtual void InsertChildAfter(const std::shared_ptr<TaroRenderNode>& child, const std::shared_ptr<TaroRenderNode>& sibling);

        void OnDisplayChange(const PropertyType::Display& val, const PropertyType::Display& oldVal) override;

        virtual void SetContent() {};
        void UpdateChild(const std::shared_ptr<TaroRenderNode>& child);

        void SetShouldPosition(bool flag);
        bool GetShouldPosition();
        void AddNotifyFixedRoot();

        std::shared_ptr<TaroRenderNode> GetRootRenderNode();

        // 绑定js动画
        int createJsAnimation(TaroAnimate::TaroJsAnimationOptionRef& option);

        // 判断是否脱离文档流
        bool IsDetachFromDocumentFlow();
        GlobalPostion calcGlobalPostion();
        // 用于节点复用
        void ForceUpdate();
        void ClearDifferOldStyleFromElement();
        void UpdateDifferOldStyleFromElement(std::weak_ptr<TaroDOM::TaroElement> element);

        bool is_apply_reused = false;
        friend class LayoutDiffer;
        friend class PaintDiffer;
        std::vector<std::shared_ptr<TaroHelper::ImagePixels>> relatedImageDrawableDescriptors;

        bool HasLayoutFlag(LAYOUT_STATE_FLAG flag) const {
            return state_flags_.test(static_cast<size_t>(flag));
        }
        void SetLayoutFlag(LAYOUT_STATE_FLAG flag) {
            if (HasLayoutFlag(flag))
                return;
            state_flags_.set(static_cast<size_t>(flag));
        }
        void ClearLayoutFlag(LAYOUT_STATE_FLAG flag) {
            state_flags_.reset(static_cast<size_t>(flag));
        }

        protected:
        // 自定义布局
        void SetCustomLayout();
        static void OnStaticCustomEvent(ArkUI_NodeCustomEvent* event);

        private:
        TaroCSSOM::TaroStylesheet::BackgroundSizeParam CalcBackgroundSize(Optional<TaroCSSOM::TaroStylesheet::BackgroundImageItem> backgroundImage);
        void setBackgroundImageAndPositionAndSize();
        ArkUI_NodeHandle ark_node_ = nullptr;
        bool should_position_ = true;

        // 节点状态标记集合
        std::bitset<8> state_flags_;

        void HandleBgImageLoad(const std::variant<TaroHelper::ResultImageInfo, TaroHelper::ErrorImageInfo>& result, const std::string url);

        static int32_t uid_flag_;
        static std::unordered_map<int32_t, std::weak_ptr<TaroRenderNode>> custom_layout_render_nodes_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
