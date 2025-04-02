/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_LABEL_ELEMENT_H
#define HARMONY_LIBRARY_LABEL_ELEMENT_H
#include <arkui/native_node.h>

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroLabel : public TaroElement {
        public:
        TaroLabel(napi_value node);
        ~TaroLabel() = default;
        void SetAttributesToRenderNode() override;
        void Build() override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        void LinkElement(std::shared_ptr<TaroElement> element);
        std::shared_ptr<TaroNode> GetElementById(const std::string& id);
        std::shared_ptr<TaroNode> GetFirstNode();
        void ProcessForNode(napi_value value);
        void RegisterEventCallback(TAG_NAME nodeName, std::function<void(std::shared_ptr<void>)> callback);
        void HandleEvents();

        private:
        bool hasFor = false;
        std::function<void(std::shared_ptr<void>)> callback_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_LABEL_H
