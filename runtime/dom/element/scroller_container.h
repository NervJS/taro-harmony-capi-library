/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_CPP_SCROLLER_CONTAINER_H
#define TARO_HARMONY_CPP_SCROLLER_CONTAINER_H

#include <memory>
#include <set>

#include "runtime/dirty_vsync_task.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {

    struct ScrollToParams {
        double xOffset;
        double yOffset;
        u_int32_t duration = 300;
    };

    struct TaroScrollerContainerAttributes : public CommonAttributes {
        // scrollerContainer attributes
        TaroHelper::Optional<bool> scrollX;
        TaroHelper::Optional<bool> scrollY;
        TaroHelper::Optional<double> scrollTop;
        TaroHelper::Optional<double> scrollLeft;
        TaroHelper::Optional<bool> showScrollbar;
        TaroHelper::Optional<bool> scrollWithAnimation;
        TaroHelper::Optional<int> scrollAnimationDuration;
    };

    enum TARO_SCROLL_PARENT_TYPE {
        TARO_ELEMENT_SCROLL,
        TARO_ELEMENT_LIST,
        TARO_ELEMENT_MAX
    };

    struct OffsetResult {
        double xOffset;
        double yOffset;
    };

    class TaroScrollerContainer : public TaroElement {
        public:
        TaroScrollerContainer(napi_value node);
        virtual ~TaroScrollerContainer() = default;

        std::unique_ptr<TaroScrollerContainerAttributes> attributes_;

        bool is_scroll_x;
        bool is_scroll_y;
        float scrollTop_ = 0.0f;
        float scrollLeft_ = 0.0f;
        uint32_t start_index = 0;
        uint32_t end_index = 0;
        bool is_visible_node_listener_registered = false;
        std::unordered_set<std::weak_ptr<TaroElement>, WeakPtrHash, WeakPtrEqual> on_visible_node;
        OffsetResult GetCurrentOffset();
        void ScrollTo(ScrollToParams params);

        virtual void Build() override;
        virtual void SetAttributesToRenderNode() override;
        virtual void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        virtual bool bindListenEvent(const std::string&) override;
        virtual void handleVisibleNode();

        protected:
        virtual bool IsScrollX();
        virtual void handleOnScroll();
        virtual void GetScrollXAttribute(const napi_value& value);
        virtual void GetScrollYAttribute(const napi_value& value);
        virtual void GetScrollTopAttribute(const napi_value& value);
        virtual void GetScrollLeftAttribute(const napi_value& value);
        virtual void GetScrollWithAnimation(const napi_value& value);
        virtual void GetShowScrollBarAttribute(const napi_value& value);
        virtual void GetScrollAnimationDuration(const napi_value& value);

        virtual void SetScrollXAttribute();
        virtual void SetScrollYAttribute();
        virtual void SetScrollDirection();
        virtual void SetScrollTopAttribute();
        virtual void SetScrollLeftAttribute();
        virtual void SetScrollWithAnimation();
        virtual void SetShowScrollBarAttribute();
        virtual void SetScrollAnimationDuration();

        virtual void findParentType();
        TARO_SCROLL_PARENT_TYPE parentType_ = TARO_ELEMENT_MAX;

        protected:
        PropertyType::Overflow overflowMode_ = PropertyType::Overflow::Visible;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // TARO_HARMONY_CPP_SCROLLER_CONTAINER_H
