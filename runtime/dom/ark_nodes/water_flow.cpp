//
// Created on 2024/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "water_flow.h"

#include <arkui/native_node.h>

#include "runtime/NativeNodeApi.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/dom/ark_nodes/flow_item.h"
#include "runtime/dom/element/flow_section.h"
#include "runtime/dom/element/water_flow.h"
#include "yoga/YGNodeStyle.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroWaterFlowNode::TaroWaterFlowNode(const TaroElementRef element)
        : TaroScrollContainerNode(element),
          adapter_(std::make_shared<TaroNodeAdapter>()),
          init_adapter_(false),
          sections_(nullptr),
          scrollX_(false) {
        TaroSetCanMeasureChild(ygNodeRef, false);
    }

    TaroWaterFlowNode::~TaroWaterFlowNode() {
        if (sections_) {
            OH_ArkUI_WaterFlowSectionOption_Dispose(sections_);
            sections_ = nullptr;
        }
        if (init_adapter_) {
            NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
            nativeNodeApi->resetAttribute(GetArkUINodeHandle(), NODE_WATER_FLOW_NODE_ADAPTER);
        }
    }

    void TaroWaterFlowNode::Build() {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_WATER_FLOW));
        TaroScrollContainerNode::setEdgeEffect(ARKUI_EDGE_EFFECT_NONE);
    }

    void TaroWaterFlowNode::setTemplate(std::string temp) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {nullptr, 0, temp.c_str(), nullptr};
        auto directionTemplate = scrollX_ ? NODE_WATER_FLOW_ROW_TEMPLATE : NODE_WATER_FLOW_COLUMN_TEMPLATE;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), directionTemplate, &item);
    }

    void TaroWaterFlowNode::setColumnGap(float gap) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item;
        ArkUI_NumberValue value[1] = {{.f32 = gap}};
        item = {value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_WATER_FLOW_COLUMN_GAP, &item);
    }

    void TaroWaterFlowNode::setRowGap(float gap) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item;
        ArkUI_NumberValue value[1] = {{.f32 = gap}};
        item = {value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_WATER_FLOW_ROW_GAP, &item);
    }

    void TaroWaterFlowNode::setCacheCount(int32_t count) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[1] = {{.i32 = count}};
        ArkUI_AttributeItem item{value, 1};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_WATER_FLOW_CACHED_COUNT, &item);
    }

    void TaroWaterFlowNode::Layout() {
        TaroRenderNode::Layout();
        if (!init_adapter_) {
            NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
            ArkUI_AttributeItem item{nullptr, 0, nullptr, adapter_->GetHandle()};
            nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_WATER_FLOW_NODE_ADAPTER, &item);
            init_adapter_ = true;
            auto renderNode = std::static_pointer_cast<TaroRenderNode>(shared_from_this());
            adapter_->setRenderNode(renderNode);
        }
        LayoutSelf();
    }

    void TaroWaterFlowNode::LayoutSelf() {
        for (auto child : children_refs_) {
            if (auto flowItem = std::dynamic_pointer_cast<TaroFlowItemNode>(child)) {
                flowItem->LayoutSelf();
            }
        }
    }

    void TaroWaterFlowNode::setFooter(const std::shared_ptr<TaroRenderNode> &footerNode) {
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item{nullptr, 0, nullptr, footerNode->GetArkUINodeHandle()};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_WATER_FLOW_FOOTER, &item);
    }

    void TaroWaterFlowNode::initSections(std::vector<std::shared_ptr<TaroNode>> &children) {
        int32_t size = children.size();
        if (sections_ == nullptr) {
            sections_ = OH_ArkUI_WaterFlowSectionOption_Create();
            if (sections_ == nullptr) {
                TARO_LOG_DEBUG("TaroWaterFlowNode", "create section option failed");
                return;
            }
        }

        OH_ArkUI_WaterFlowSectionOption_SetSize(sections_, size);
        for (int i = 0; i < size; i++) {
            auto child = std::dynamic_pointer_cast<TaroDOM::TaroFlowSection>(children[i]);
            if (child) {
                TARO_LOG_DEBUG("TaroWaterFlowNode", "initSections index:%{public}d,count:%{public}d,column:%{public}d",
                               i, child->child_nodes_.size(), child->column_);
                OH_ArkUI_WaterFlowSectionOption_SetItemCount(sections_, i, child->child_nodes_.size());
                OH_ArkUI_WaterFlowSectionOption_SetCrossCount(sections_, i, child->column_);
                if (child->rowGap_.has_value()) {
                    OH_ArkUI_WaterFlowSectionOption_SetRowGap(sections_, i, vp2Px(child->rowGap_.value()));
                }
                if (child->columnGap_.has_value()) {
                    OH_ArkUI_WaterFlowSectionOption_SetColumnGap(sections_, i, vp2Px(child->columnGap_.value()));
                }
            }
        }

        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[] = {0};
        value[0].i32 = 0;
        ArkUI_AttributeItem item{value, 1, nullptr, sections_};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_WATER_FLOW_SECTION_OPTION, &item);
    }

    void TaroWaterFlowNode::updateSectionCount(int32_t index, int32_t count) {
        int32_t curCount = OH_ArkUI_WaterFlowSectionOption_GetItemCount(sections_, index);
        curCount += count;
        OH_ArkUI_WaterFlowSectionOption_SetItemCount(sections_, index, curCount);

        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue value[] = {0};
        value[0].i32 = 0;
        ArkUI_AttributeItem item{value, 1, nullptr, sections_};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_WATER_FLOW_SECTION_OPTION, &item);
    }

    void TaroWaterFlowNode::updateSectionColumn(int32_t index, int32_t column) {
        auto weakSelf = std::weak_ptr<TaroWaterFlowNode>(std::static_pointer_cast<TaroWaterFlowNode>(shared_from_this()));
        DirtyTaskPipeline::GetInstance()->RegistryAdapterAttach([weakSelf, index, column]() {
            if (auto self_ = weakSelf.lock()) {
                self_->LayoutSelf();

                OH_ArkUI_WaterFlowSectionOption_SetCrossCount(self_->sections_, index, column);

                NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
                ArkUI_NumberValue value[] = {0};
                value[0].i32 = 0;
                ArkUI_AttributeItem item{value, 1, nullptr, self_->sections_};
                nativeNodeApi->setAttribute(self_->GetArkUINodeHandle(), NODE_WATER_FLOW_SECTION_OPTION, &item);
            }
        });
    }

    void TaroWaterFlowNode::updateRowGap(int32_t index, double gap) {
        auto weakSelf = std::weak_ptr<TaroWaterFlowNode>(std::static_pointer_cast<TaroWaterFlowNode>(shared_from_this()));
        DirtyTaskPipeline::GetInstance()->RegistryAdapterAttach([weakSelf, index, gap]() {
            if (auto self_ = weakSelf.lock()) {
                self_->LayoutSelf();

                OH_ArkUI_WaterFlowSectionOption_SetRowGap(self_->sections_, index, gap);
                NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
                ArkUI_NumberValue value[] = {0};
                value[0].i32 = 0;
                ArkUI_AttributeItem item{value, 1, nullptr, self_->sections_};
                nativeNodeApi->setAttribute(self_->GetArkUINodeHandle(), NODE_WATER_FLOW_SECTION_OPTION, &item);
            }
        });
    }

    void TaroWaterFlowNode::updateColumnGap(int32_t index, double gap) {
        auto weakSelf = std::weak_ptr<TaroWaterFlowNode>(std::static_pointer_cast<TaroWaterFlowNode>(shared_from_this()));
        DirtyTaskPipeline::GetInstance()->RegistryAdapterAttach([weakSelf, index, gap]() {
            if (auto self_ = weakSelf.lock()) {
                self_->LayoutSelf();

                OH_ArkUI_WaterFlowSectionOption_SetColumnGap(self_->sections_, index, gap);

                NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
                ArkUI_NumberValue value[] = {0};
                value[0].i32 = 0;
                ArkUI_AttributeItem item{value, 1, nullptr, self_->sections_};
                nativeNodeApi->setAttribute(self_->GetArkUINodeHandle(), NODE_WATER_FLOW_SECTION_OPTION, &item);
            }
        });
    }

    void TaroWaterFlowNode::updateMargin(int32_t index, double marginTop, double marginRight, double marginBottom, double marginLeft) {
        auto weakSelf = std::weak_ptr<TaroWaterFlowNode>(std::static_pointer_cast<TaroWaterFlowNode>(shared_from_this()));
        DirtyTaskPipeline::GetInstance()->RegistryAdapterAttach([weakSelf, index, marginTop, marginRight, marginBottom, marginLeft]() {
            if (auto self_ = weakSelf.lock()) {
                self_->LayoutSelf();

                OH_ArkUI_WaterFlowSectionOption_SetMargin(self_->sections_, index, marginTop, marginRight, marginBottom, marginLeft);

                NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
                ArkUI_NumberValue value[] = {0};
                value[0].i32 = 0;
                ArkUI_AttributeItem item{value, 1, nullptr, self_->sections_};
                nativeNodeApi->setAttribute(self_->GetArkUINodeHandle(), NODE_WATER_FLOW_SECTION_OPTION, &item);
            }
        });
    }

} // namespace TaroDOM
} // namespace TaroRuntime