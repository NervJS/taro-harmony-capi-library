//
// Created on 2024/4/17.
//

#ifndef TARO_CAPI_HARMONY_TEXT_H
#define TARO_CAPI_HARMONY_TEXT_H

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>

#include "runtime/dom/ark_nodes/text.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TaroTextAttributes : public CommonAttributes {
        TaroHelper::Optional<std::string> textContent;
    };

    class TaroText : public TaroElement {
        public:
        TaroText(napi_value node);

        ~TaroText() = default;
        void Build() override;
    
        void Build(std::shared_ptr<TaroElement> &reuse_element) override;
    
        bool Reusable(std::shared_ptr<TaroElement> &reuse_element) override;

        std::unique_ptr<TaroTextAttributes> attributes_;

        void SetAttributesToRenderNode() override;

        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode,
                          ATTRIBUTE_NAME name, napi_value value) override;

        void RemoveAttribute(std::shared_ptr<TaroRenderNode> renderNode,
                             ATTRIBUTE_NAME name) override;

        void onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node = true) override;

        void onAppendChild(std::shared_ptr<TaroNode> child) override;

        void onReplaceChild(std::shared_ptr<TaroNode> newChild, std::shared_ptr<TaroNode> oldChild) override;

        void onInsertBefore(std::shared_ptr<TaroNode> child, std::shared_ptr<TaroNode> refChild) override;

        void UpdateContent(const bool isNeedReset = false);

        bool bindListenEvent(const std::string& event_name) override;

        std::string GetTextContent();
        std::string GetTextContent(napi_value value);
        
        void SetTextRenderNodeInfo(const std::shared_ptr<TaroTextNode>& renderNode);

        private:
        std::string text_context_;
        std::optional<std::string> m_text_content = std::nullopt;
        StylesheetRef m_old_style = nullptr;
        void handleAreaChangeEvent();
        void SetStyleContent(const std::shared_ptr<TaroRenderNode>&, const StylesheetRef&, const bool isForceUpdate = false);
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_TEXT_H
