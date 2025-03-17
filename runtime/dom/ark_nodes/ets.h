//
// Created on 2024/6/9.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once

#include "arkui_node.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroEtsNode : public TaroRenderNode {
        public:
        TaroEtsNode(const TaroElementRef element);

        ~TaroEtsNode();

        void Build() override;

        void Update();

        void Dispose();

        void AppendChild(const std::shared_ptr<TaroRenderNode> &child) override {};
        void RemoveChild(const std::shared_ptr<TaroRenderNode> &child) override {};
        void ReplaceChild(const std::shared_ptr<TaroRenderNode> &old_child, const std::shared_ptr<TaroRenderNode> &new_child) override {};
        void InsertChildAt(const std::shared_ptr<TaroRenderNode> &child, uint8_t index) override {};
        void InsertChildBefore(const std::shared_ptr<TaroRenderNode> &child, const std::shared_ptr<TaroRenderNode> &sibling) override {};
        void InsertChildAfter(const std::shared_ptr<TaroRenderNode> &child, const std::shared_ptr<TaroRenderNode> &sibling) override {};

        private:
        napi_ref component_content_ref_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
