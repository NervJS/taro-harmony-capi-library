/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TESTNDK_NODE_H
#define TESTNDK_NODE_H

#include <bitset>
#include <arkui/native_node.h>

#include "helper/TaroLog.h"
#include "runtime/constant.h"
#include "runtime/dom/ark_nodes/arkui_node.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroNode : public std::enable_shared_from_this<TaroNode> {
        public:
        TaroNode(napi_value &node);

        virtual ~TaroNode();

        napi_ref ref_;
        int32_t nid_ = 0;
        int8_t node_type_;
        std::vector<std::shared_ptr<TaroNode> > child_nodes_;

        void SetParentNode(std::shared_ptr<TaroNode> parent_node);

        std::shared_ptr<TaroNode> GetParentNode();

        void SetPreviousSibling(std::shared_ptr<TaroNode> previous_sibling);

        std::shared_ptr<TaroNode> GetPreviousSibling();

        void SetNextSibling(std::shared_ptr<TaroNode> next_sibling);

        std::shared_ptr<TaroNode> GetNextSibling();

        bool HasRenderNode();

        std::shared_ptr<TaroRenderNode> GetHeadRenderNode();

        std::shared_ptr<TaroRenderNode> GetFooterRenderNode();

        ArkUI_NodeHandle GetNodeHandle();

        ArkUI_NodeHandle GetInnerNodeHandle();

        int32_t GetNodeId();

        static int32_t GetNodeId(napi_value node);

        NODE_TYPE GetNodeType();

        static NODE_TYPE GetNodeType(napi_value node);

        std::string GetNodeName();

        napi_value GetNodeValue();

        //         TaroEvent::TaroEventEmitterPtr getEventEmitter();

        // void dispatchEvent(ArkUI_NodeEvent *event);

        //         int triggerEventOnce(napi_value &js_event_name, napi_ref &callback);

        void detachTaroNodeReference(std::shared_ptr<TaroNode> node);

        void removeChild(std::shared_ptr<TaroNode> child);

        void insertChildAt(std::shared_ptr<TaroNode> child, uint32_t index);

        void appendChild(std::shared_ptr<TaroNode> child);

        void insertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild);

        void replaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild);

        void onDeleteRenderNode(std::shared_ptr<TaroNode> child);

        virtual void onAppendChild(std::shared_ptr<TaroNode> child) {};

        virtual void onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node = true);

        virtual void onReplaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) {};

        virtual void onInsertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) {};

        // TaroNode& cloneNode(bool deep);
        // TaroNode& setAttribute(std::string name, std::string value);
        // TaroNode& getAttribute(std::string name);
        // TaroNode& contains(std::shared_ptr<TaroNode> node);
        // TaroNode& compareDocumentPosition(std::shared_ptr<TaroNode> node);

        void TraverseAncestor(std::function<bool(std::shared_ptr<TaroNode>)>);

        void ProcessRenderNodeFromTaroNode(std::shared_ptr<TaroNode> node, std::function<void(std::weak_ptr<TaroRenderNode>)> func);

        bool is_init_ = false;
        // 子节点 BuildProcess 会被中断的节点
        bool lazy_node = false;
        // 虚拟列表容器节点
        bool lazy_container = false;
        // 虚拟列表容器的子孙节点哦
        bool lazy_child_item = false;
        bool ignore_pre_build = false;
        bool ignore_build = false;
        bool ignore_update_event = false;
        bool ignore_post_build = false;
        // for lazy container items, currently we only support list_item, list_item_group and flow_item,
        // indicates whether an item has been properly attached to the node adapter.
        bool is_attached = false;
        // for lazy container items, currently we only support list_item, list_item_group and flow_item,
        // indicates whether an item contains reusable element.
        bool reusable_ = false;

        protected:
        // 节点状态标记集合
        std::bitset<8> state_flags_;

        void SetRenderNode(std::shared_ptr<TaroRenderNode> render_node);

        virtual void SetHeadRenderNode(std::shared_ptr<TaroRenderNode> head_render_node);

        virtual void SetFooterRenderNode(std::shared_ptr<TaroRenderNode> footer_render_node);

        private:
        std::weak_ptr<TaroNode> parent_node_;
        std::weak_ptr<TaroNode> previous_sibling_;
        std::weak_ptr<TaroNode> next_sibling_;
        // 渲染头节点
        std::shared_ptr<TaroRenderNode> head_render_node_ = nullptr;
        // 渲染尾节点
        std::shared_ptr<TaroRenderNode> footer_render_node_ = nullptr;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TESTNDK_NODE_H
