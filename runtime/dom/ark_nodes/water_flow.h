//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "arkui_node.h"
#include "arkui_node_adapter.h"
#include "runtime/dom/ark_nodes/scroll_container.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroWaterFlowNode : public TaroScrollContainerNode {
        public:
        TaroWaterFlowNode(const TaroElementRef element);
        ~TaroWaterFlowNode() override;

        void Build() override;

        void setTemplate(std::string temp);
        void setColumnGap(float gap);
        void setRowGap(float gap);

        // 设置瀑布流组件末尾的自定义显示组件
        void setFooter(const std::shared_ptr<TaroRenderNode> &footerNode);

        // 初始化waterflow section option
        void initSections(std::vector<std::shared_ptr<TaroNode>> &child_nodes_);
        void updateSectionCount(int32_t index, int32_t count);
        void updateSectionColumn(int32_t index, int32_t column);
        void updateRowGap(int32_t index, double gap);
        void updateColumnGap(int32_t index, double gap);
        void updateMargin(int32_t index, double marginTop, double marginRight, double marginBottom, double marginLeft);

        // 首次布局时会挂载 adapter
        void Layout() override;

        void setCacheCount(int32_t count);

        std::shared_ptr<TaroNodeAdapter> adapter_;

        private:
        bool init_adapter_;
        ArkUI_WaterFlowSectionOption *sections_;
        bool scrollX_;
        void LayoutSelf();
    };

} // namespace TaroDOM
} // namespace TaroRuntime