/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_TEXT_H
#define TARO_HARMONY_TEXT_H

#include <cstdint>

#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/dom/ark_nodes/arkui_node.h"
#include "runtime/dom/ark_nodes/text_styled.h"
#include "runtime/dom/element/image.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TextPoint {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct TextLineBox {
        TextPoint start{};
        TextPoint end{};
    };

    struct TextElementInfo {
        int32_t nid = 0;
        std::vector<TextLineBox> lineBox;

        TextLineBox& GetLastLineTextBox() {
            if (lineBox.size() == 0) {
                lineBox.emplace_back(TextLineBox{});
            }
            return lineBox[lineBox.size() - 1];
        }

        void AddLineTextBox(TextPoint& start, TextPoint& end) {
            lineBox.emplace_back(TextLineBox{.start = start, .end = end});
        }
    };

    class TaroTextNode : public TaroRenderNode {
        using ImageCallbackInfo = std::variant<TaroHelper::ResultImageInfo, TaroHelper::ErrorImageInfo>;
        using ProcessImagesCallback = std::function<void(const std::shared_ptr<std::vector<ImageCallbackInfo>> &, std::weak_ptr<BaseRenderNode>)>;
        using srcType = std::variant<std::string, ArkUI_DrawableDescriptor*>;

        public:
        std::shared_ptr<TextStyled> textStyled_ = nullptr;
        std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> textNodeStyle_;
        std::vector<std::shared_ptr<TextElementInfo>> textElementInfos_;
        TaroTextNode(const TaroElementRef element);

        ~TaroTextNode();

        void SetStyle(StylesheetRef style_ref) override;

        void SetContent() override;

        void Build() override;

        void Layout() override;
        void Paint() override;

        void Reset();

        void SetIsNeedUpdate(const bool isNeedUpdate);
        bool GetIsNeedUpdate();
        bool GetIsFirstRender();

        bool HasImages();
        bool HasImagesLoaded();
        void UpdateImage(int32_t id, srcType src);
        ArkUI_NodeHandle GetImageArkNode(int32_t id);
        ArkUI_NodeHandle GetTextArkNode();
        std::vector<ArkUI_NodeHandle>& GetImageArkNodeList();
        bool GetIfInTextNodeArea(int32_t nid, float x, float y);
        std::shared_ptr<TextElementInfo> GetLastTextElementInfo(int32_t);
        std::shared_ptr<TextElementInfo> GetTextElementInfoFromRange(int32_t index, int32_t nid, int32_t start, int32_t end, int32_t& line);
        void GetAllTextElementInfos();
        bool CheckIsPointInsideRectangle(float x, float y, float rectTopLeftX, float rectTopLeftY, float rectBottomRightX, float rectBottomRightY);

        void SetMeasuredTextWidth(const float);
        void SetMeasuredTextHeight(const float);
        void AppendChild(const std::shared_ptr<TaroRenderNode>& child) override {};
        void RemoveChild(const std::shared_ptr<TaroRenderNode>& child) override {};
        void ReplaceChild(const std::shared_ptr<TaroRenderNode>& old_child, const std::shared_ptr<TaroRenderNode>& new_child) override {};
        void InsertChildAt(const std::shared_ptr<TaroRenderNode>& child, uint8_t index) override {};
        void InsertChildBefore(const std::shared_ptr<TaroRenderNode>& child, const std::shared_ptr<TaroRenderNode>& sibling) override {};
        void InsertChildAfter(const std::shared_ptr<TaroRenderNode>& child, const std::shared_ptr<TaroRenderNode>& sibling) override {};
        void OnDisplayChange(const PropertyType::Display& val, const PropertyType::Display& oldVal) override;

        void SetTextRenderNodeInner(const ArkUI_NodeHandle& arkHandle);
        void SetTextRenderNodeImageInfos(const std::vector<std::shared_ptr<ImageInfo>>& infos);
        void SetTextRenderNodeImageNodes(const std::vector<ArkUI_NodeHandle>& nodes);
        void SetTextRenderNodeHasContent(const bool& hasContent);
        void SetTextRenderNodeHasExactlyImage(const bool& hasExactlyImage);

        private:
        ArkUI_NodeHandle m_InnerTextNode = nullptr;
        std::vector<std::shared_ptr<ImageInfo>> m_ImageInfos;
        std::vector<ArkUI_NodeHandle> m_ImageNodes;
        bool m_HasContent = true;
        bool m_IsNeedUpdate = false;
        bool m_isFirstRender = true;
        bool m_ImagesLoaded = false;
        float m_MeasuredTextWidth = 0;
        float m_MeasuredTextHeight = 0;
        bool m_HasExactlyImage = true;
        void ProcessImageResults(std::vector<std::shared_ptr<ImageInfo>>& images, std::weak_ptr<BaseRenderNode> textNode, ProcessImagesCallback&& onAllImagesLoaded);
        void SetTextMeasureFunc();
        void PaintImages();
        void SetSize();
        void RemoveArkChild();
        void ProcessTextAlign();
        float GetUseToLayoutWidth();
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_TEXT_H
