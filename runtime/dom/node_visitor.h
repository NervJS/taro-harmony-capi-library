/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_NODE_VISITOR_H
#define TARO_CAPI_HARMONY_DEMO_NODE_VISITOR_H

#include "runtime/dom/element/element.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    enum class VisitResult {
        Continue,
        Skip,
        Stop,
    };

    class TaroNodeVisitor {
        public:
        virtual ~TaroNodeVisitor() = default;

        virtual VisitResult visitNode(const std::shared_ptr<TaroNode>& node) {
            auto it = m_visitors.find(node->node_type_);
            if (it != m_visitors.end()) {
                auto visitor = it->second;
                (this->*visitor)(node);
            }
            return VisitResult::Continue;
        }

        template <typename T, typename U>
        void registerVisitor(int8_t nodeType, VisitResult (U::*visitor)(const std::shared_ptr<T>&)) {
            m_visitors[nodeType] = reinterpret_cast<VisitorFunction>(visitor);
        }

        void traverse(const std::shared_ptr<TaroNode>& node) {
            if (!node)
                return;
            traverseChildren(node);
        }

        private:
        void traverseChildren(const std::shared_ptr<TaroNode>& node) {
            for (const auto& child : node->child_nodes_) {
                auto result = visitNode(child);
                if (result == VisitResult::Stop)
                    return;
                if (result != VisitResult::Skip)
                    traverseChildren(child);
            }
        }

        using VisitorFunction = void (TaroNodeVisitor::*)(const std::shared_ptr<TaroNode>&);
        std::unordered_map<int8_t, VisitorFunction> m_visitors;
    };

    class ElementVisitor : public TaroNodeVisitor {
        public:
        using VisitElementCallback = std::function<VisitResult(const std::shared_ptr<TaroElement>&)>;

        ElementVisitor(VisitElementCallback callback)
            : m_callback(std::move(callback)) {
            registerVisitor<TaroElement, ElementVisitor>(1, &ElementVisitor::visitElement);
        }

        VisitResult visitElement(const std::shared_ptr<TaroElement>& node) {
            if (m_callback) {
                return m_callback(node);
            }
            return VisitResult::Stop;
        }

        private:
        VisitElementCallback m_callback;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_NODE_VISITOR_H
