//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_WATER_FLOW_H
#define HARMONY_LIBRARY_WATER_FLOW_H

#include "runtime/dom/element/element.h"
#include "scroller_container.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TaroWaterFlowAttributes : public CommonAttributes {
        // waterflow attributes
        TaroHelper::Optional<int32_t> upperThresholdCount;
        TaroHelper::Optional<int32_t> lowerThresholdCount;
        TaroHelper::Optional<int32_t> cacheCount;
    };

    class TaroWaterFlow : public TaroScrollerContainer {
        public:
        TaroWaterFlow(napi_value node);
        ~TaroWaterFlow();

        void Build() override;
        void SetAttributesToRenderNode() override;
        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        void onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node = true) override;
        void onAppendChild(std::shared_ptr<TaroNode> child) override;
        void onReplaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) override;
        void onInsertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) override;
        bool bindListenEvent(const std::string&) override;
        int32_t GetCacheCount() override ;

        private:
        void GetLowerThresholdCount(const napi_value& value);
        void GetUpperThresholdCount(const napi_value& value);
        void GetCacheCount(const napi_value& value);
        void SetCacheCount();
        void handleOnScrollIndex();
        void callJSFunc(std::string eventType);

        std::unique_ptr<TaroWaterFlowAttributes> attributes_;
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_WATER_FLOW_H
