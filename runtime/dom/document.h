//
// Created by zhutianjian on 24-6-9.
//

#ifndef TARO_HARMONY_DOCUMENT_H
#define TARO_HARMONY_DOCUMENT_H

#include <unordered_set>

#include "runtime/dom/element/element.h"
#include "runtime/dom/event_source.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroDocument : public TaroElement {
        public:
        static std::shared_ptr<TaroDocument> GetInstance() {
            static std::shared_ptr<TaroDocument> document = std::make_shared<TaroDocument>(nullptr);
            document->Build();
            return document;
        }

        TaroDocument(napi_value node);

        ~TaroDocument() = default;

        std::shared_ptr<TaroDOM::TaroElement> body_;
        std::shared_ptr<TaroDOM::TaroElement> app_;
        // React18 异步渲染入口
        std::shared_ptr<TaroDOM::TaroElement> entry_async_;

        const std::unordered_set<TaroRuntime::TAG_NAME> PLACEHOLDER_TAG_NAMES = {
            TAG_NAME::HTML,
            TAG_NAME::BODY,
            TAG_NAME::CONTAINER,
            TAG_NAME::APP,
            TAG_NAME::ENTRY_ASYNC,
        };

        bool isPlaceholderNode(std::shared_ptr<TaroElement> el) {
            if (el->node_type_ != NODE_TYPE::ELEMENT_NODE && el->node_type_ != NODE_TYPE::TEXT_NODE) {
                return true;
            }

            if (PLACEHOLDER_TAG_NAMES.find(el->tag_name_) != PLACEHOLDER_TAG_NAMES.end()) {
                return true;
            }
            return false;
        };

        std::shared_ptr<TaroPageContainer> findDOMPage(std::shared_ptr<TaroElement> el);

        void Build() override {};

        std::shared_ptr<TaroElement> CreateElement(napi_value &node);

        std::shared_ptr<TaroElement> GetElementById(int32_t id);

        void AttachTaroNode(std::shared_ptr<TaroElement> node);

        void DetachTaroNode(std::shared_ptr<TaroNode> node);

        void DetachTaroNode(int32_t id);

        void DetachAllTaroNode();

        private:
        TaroDOM::EventSource taro_nodes_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_DOCUMENT_H
