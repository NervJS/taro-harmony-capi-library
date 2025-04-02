/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_LIST_H
#define HARMONY_LIBRARY_LIST_H

#include "runtime/dom/element/element.h"
#include "runtime/dom/element/scroller_container.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TaroListAttributes : public CommonAttributes {
        // List attributes
        TaroHelper::Optional<int32_t> upperThresholdCount;
        TaroHelper::Optional<int32_t> lowerThresholdCount;
        TaroHelper::Optional<int32_t> cacheCount;
        TaroHelper::Optional<bool> stickyHeader;
        TaroHelper::Optional<double> space;
    };

    class TaroList : public TaroScrollerContainer {
        public:
        TaroList(napi_value node);
        ~TaroList();

        void Build() override;
        void SetAttributesToRenderNode() override;
        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        void onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node = true) override;
        void onAppendChild(std::shared_ptr<TaroNode> child) override;
        void onReplaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) override;
        void onInsertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) override;
        bool bindListenEvent(const std::string&) override;
        int32_t GetCacheCount() override;

        private:
        void GetLowerThresholdCount(const napi_value& value);
        void GetUpperThresholdCount(const napi_value& value);
        void GetCacheCount(const napi_value& value);
        void GetStickyHeader(const napi_value& value);
        void GetSpace(const napi_value& value);
        void SetCacheCount();
        void SetStickyHeader();
        void SetSpace();
        void handleOnScrollIndex();

        std::unique_ptr<TaroListAttributes> attributes_;
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_LIST_H
