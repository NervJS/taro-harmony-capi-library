/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "runtime/dom/element/text.h"

#include <codecvt>
#include <cstdint>
#include <arkui/styled_string.h>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_text_typography.h>

#include "helper/ImageLoader.h"
#include "helper/string.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/TaroYogaApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/cssom/font/FontFamilyManager.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/harmony_style_setter/harmony_style_setter.h"
#include "runtime/dom/ark_nodes/scroll.h"
#include "runtime/dom/element/element.h"
#include "runtime/dom/element/image.h"
#include "runtime/dom/element/scroll_view.h"
#include "runtime/render.h"
#include "text.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroTextNode::TaroTextNode(TaroElementRef element)
        : TaroRenderNode(element) {
        SetIsInline(true);
        SetShouldPosition(false);
        textStyled_ = std::make_shared<TextStyled>();
    }

    TaroTextNode::~TaroTextNode() {
        if (m_InnerTextNode) {
            NativeNodeApi::getInstance()->resetAttribute(m_InnerTextNode, NODE_TEXT_CONTENT_WITH_STYLED_STRING);
        }
        RemoveArkChild();
        // textStyled_->Destroy();
        auto textStyled = textStyled_;
        auto runner = Render::GetInstance()->GetTaskRunner();
        runner->runTask(TaroThread::TaskThread::MAIN, [textStyled] {
            if (textStyled) {
                textStyled->Destroy();
            }
        });
    }

    YGSize TextMeasureFunc(YGNodeConstRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
        SystraceSection s("Taro Layout:: Text Measure");
        float resultWidth = 0;
        float resultHeight = 0;

        auto textNode = static_cast<TaroTextNode*>(TaroYogaApi::getInstance()->getContext(node));
        auto typography = textNode->textStyled_->GetTypography();
        if (!textNode->textNodeStyle_) {
            textNode->textNodeStyle_ = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
        }
        auto textStyle = textNode->textNodeStyle_;
        if (textNode->HasImages() && !textNode->HasImagesLoaded()) {
            return (YGSize){resultWidth, resultHeight};
        }
        auto dimensionContext = textNode->GetDimensionContext();
        auto viewport_width = dimensionContext->viewport_width_;
        // 用来限制换行的最大宽度
        auto constraintWidth = width == 0 || std::isnan(width) ? viewport_width : width;
        float maxWidth = vp2Px(constraintWidth);

        // 当文本设置为 nowrap 属性，且不需要展示省略号时，给予其足够大的宽度去容纳这个单行文本。
        bool isNoWrap = textStyle->whiteSpace.has_value() && textStyle->whiteSpace.value() == PropertyType::WhiteSpace::NoWrap;
        if (isNoWrap) {
            auto textNodeStyle = textNode->style_ref_;
            bool isHidden = textNodeStyle->overflow.has_value() && textNodeStyle->overflow.value() == PropertyType::Overflow::Hidden;
            bool isEllipsis = textNodeStyle->textOverflow.has_value() && textNodeStyle->textOverflow.value() == ArkUI_TextOverflow::ARKUI_TEXT_OVERFLOW_ELLIPSIS;
            if (!isHidden || !isEllipsis) {
                maxWidth = std::numeric_limits<float>::max();
            }
        }
        textNode->textStyled_->TextTypographyLayout(typography, maxWidth);
        auto calcWidth = OH_Drawing_TypographyGetLongestLine(typography);
        resultWidth = px2Vp(calcWidth);
        auto calcHeight = px2Vp(OH_Drawing_TypographyGetHeight(typography));
        // 如果排版发生下scroll下，无需约束
        if (auto parent = textNode->parent_ref_.lock()) {
            if (auto scroll = std::dynamic_pointer_cast<TaroScrollView>(parent->element_ref_.lock())) {
                if (!scroll->is_scroll_x) {
                    heightMode = YGMeasureModeUndefined;
                }
            } else if (auto parentElement = std::dynamic_pointer_cast<TaroElement>(parent->element_ref_.lock())) {
                if (parentElement->style_->overflow.value_or(PropertyType::Overflow::Visible) == PropertyType::Overflow::Scroll) {
                    heightMode = YGMeasureModeUndefined;
                }
            }
        }

        switch (heightMode) {
            case YGMeasureModeAtMost: {
                resultHeight = std::min((float)calcHeight, height);
                break;
            }
            case YGMeasureModeUndefined:
                resultHeight = calcHeight;
                break;
            case YGMeasureModeExactly:
                resultHeight = height;
                break;
        }
        if (textNode->element_ref_.lock()) {
            TARO_LOG_DEBUG("dirty Measure Info", "class: %{public}s, resuleWidth: %{public}f, resultHeight %{public}f, widthmode %{public}d, heightmode %{public}d width %{public}f, height %{public}f", textNode->element_ref_.lock()->class_list_.value().c_str(), resultWidth, resultHeight, widthMode, heightMode, width, height);
        }
        textNode->SetIsNeedUpdate(true);
        textNode->SetMeasuredTextWidth(resultWidth);
        textNode->SetMeasuredTextHeight(resultHeight);
        return (YGSize){resultWidth, resultHeight};
    }

    bool TaroTextNode::HasImages() {
        return m_ImageInfos.size() > 0;
    }

    bool TaroTextNode::HasImagesLoaded() {
        return m_ImagesLoaded;
    }

    void TaroTextNode::SetMeasuredTextWidth(const float w) {
        m_MeasuredTextWidth = w;
    }

    void TaroTextNode::SetMeasuredTextHeight(const float h) {
        m_MeasuredTextHeight = h;
    }

    void GetTextChildren(std::shared_ptr<TaroElement>& element, std::vector<std::shared_ptr<TaroNode>>& children) {
        auto childNodeSize = element->child_nodes_.size();
        auto eleCast = std::dynamic_pointer_cast<TaroDOM::TaroElement>(element);
        if (childNodeSize < 1) {
            if (eleCast && eleCast->node_type_ == NODE_TYPE::TEXT_NODE) {
                children = {eleCast};
            }
        } else {
            children = element->child_nodes_;
        }
    }

    void TaroTextNode::OnDisplayChange(const PropertyType::Display& val, const PropertyType::Display& oldVal) {
        // 文本节点的父节点从 display none 切到 block 时最好重新排版一下
        if (oldVal == PropertyType::Display::None && val != PropertyType::Display::None) {
            textStyled_->SetHashBeenLayout(false);
        }
    }

    void TaroTextNode::SetStyle(StylesheetRef style_ref) {
        TaroRenderNode::SetStyle(style_ref);
        textNodeStyle_ = std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>();
        SetIsNeedUpdate(CheckAndSetFontStyle(textNodeStyle_));
        if (m_isFirstRender || m_IsNeedUpdate) {
            SetDrawDirty(true);
        }
    }

    void TaroTextNode::SetIsNeedUpdate(const bool isNeedUpdate) {
        m_IsNeedUpdate = isNeedUpdate;
    }

    bool TaroTextNode::GetIsNeedUpdate() {
        return m_IsNeedUpdate;
    }

    void TaroTextNode::ProcessImageResults(std::vector<std::shared_ptr<ImageInfo>>& images, ProcessImagesCallback&& onAllImagesLoaded) {
        auto results = std::make_shared<std::vector<ImageCallbackInfo>>();
        auto loadCounter = std::make_shared<int>(0);
        results->reserve(images.size());
        for (const auto& image : images) {
            TaroHelper::loadImage({.url = image->src}, [image, results, loadCounter, imagesSize = images.size(), onAllImagesLoaded](const ImageCallbackInfo& result) mutable {
                results->push_back(result);
                ++(*loadCounter);
                if (*loadCounter == imagesSize) {
                    onAllImagesLoaded(results);
                }
            });
        }
    }

    void TaroTextNode::SetTextMeasureFunc() {
        auto yogaInstance = TaroYogaApi::getInstance();
        if (!yogaInstance->hasMeasureFunc(ygNodeRef)) {
            yogaInstance->setContext(ygNodeRef, static_cast<void*>(shared_from_this().get()));
            if (YGNodeGetChildCount(ygNodeRef) == 0) {
                yogaInstance->setMeasureFunc(ygNodeRef, TextMeasureFunc);
            }
        }
        if (is_layout_dirty_ && YGNodeGetChildCount(ygNodeRef) == 0 && yogaInstance->hasMeasureFunc(ygNodeRef)) {
            yogaInstance->markDirty(ygNodeRef);
        }
    }

    void TaroTextNode::SetContent() {
        if (!m_HasContent)
            return;
        if (!m_isFirstRender && !m_IsNeedUpdate)
            return;

        auto element = element_ref_.lock();
        if (element) {
            if (m_HasExactlyImage) {
                auto dimensionContext = GetDimensionContext();
                std::vector<std::shared_ptr<TaroNode>> vec;
                GetTextChildren(element, vec);
                textStyled_->InitStyledString(style_ref_, textNodeStyle_, dimensionContext);
                m_ImagesLoaded = true;
                for (int32_t i = 0; i < vec.size(); i++) {
                    auto item = std::dynamic_pointer_cast<TaroDOM::TaroElement>(vec[i]);
                    item->PreBuild();
                    if (item->isTextElement()) {
                        auto textContent = std::dynamic_pointer_cast<TaroText>(item)->GetTextContent();
                        textStyled_->SetSingleTextStyle(style_ref_, item->style_, textContent, dimensionContext);
                    } else if (item->tag_name_ == TAG_NAME::IMAGE) {
                        auto it = std::find_if(m_ImageInfos.begin(), m_ImageInfos.end(), [i](const std::shared_ptr<ImageInfo>& info) {
                            return info->index == i;
                        });
                        if (it != m_ImageInfos.end()) {
                            textStyled_->SetSingleImageStyle(item->style_, *it, dimensionContext);
                        }
                    }
                }
                textStyled_->InitTypography();
                SetTextMeasureFunc();
            } else {
                if (m_ImageInfos.size() == 0) {
                    auto dimensionContext = GetDimensionContext();
                    std::vector<std::shared_ptr<TaroNode>> vec;
                    GetTextChildren(element, vec);
                    textStyled_->InitStyledString(style_ref_, textNodeStyle_, dimensionContext);
                    for (int32_t i = 0; i < vec.size(); i++) {
                        auto item = std::dynamic_pointer_cast<TaroDOM::TaroElement>(vec[i]);
                        item->PreBuild();
                        if (item->isTextElement()) {
                            auto textContent = std::dynamic_pointer_cast<TaroText>(item)->GetTextContent();
                            textStyled_->SetSingleTextStyle(style_ref_, item->style_, textContent, dimensionContext);
                        }
                    }
                    textStyled_->InitTypography();
                    SetTextMeasureFunc();
                } else {
                    std::weak_ptr<TaroTextNode> weakSelf = std::dynamic_pointer_cast<TaroTextNode>(shared_from_this());
                    ProcessImageResults(m_ImageInfos, [weakSelf](const std::shared_ptr<std::vector<ImageCallbackInfo>>& results) {
                        auto this_ = weakSelf.lock();
                        if (this_) {
                            auto element = this_->element_ref_.lock();
                            if (!element)
                                return;
                            auto dimensionContext = this_->GetDimensionContext();
                            this_->m_ImagesLoaded = true;
                            std::vector<std::shared_ptr<TaroNode>> vec;
                            GetTextChildren(element, vec);
                            this_->textStyled_->InitStyledString(this_->style_ref_, this_->textNodeStyle_, dimensionContext);
                            for (const auto& result : *results) {
                                if (auto info = std::get_if<TaroHelper::ResultImageInfo>(&result)) {
                                    for (auto& imageInfo : this_->m_ImageInfos) {
                                        if (imageInfo->src == info->url) {
                                            imageInfo->oriWidth = info->width;
                                            imageInfo->oriHeight = info->height;
                                            break;
                                        }
                                    }
                                }
                            }
                            for (int32_t i = 0; i < vec.size(); i++) {
                                auto item = std::dynamic_pointer_cast<TaroDOM::TaroElement>(vec[i]);
                                item->PreBuild();
                                if (item->isTextElement()) {
                                    auto textContent = std::dynamic_pointer_cast<TaroText>(item)->GetTextContent();
                                    this_->textStyled_->SetSingleTextStyle(this_->style_ref_, item->style_, textContent, dimensionContext);
                                } else if (item->tag_name_ == TAG_NAME::IMAGE) {
                                    auto it = std::find_if(this_->m_ImageInfos.begin(), this_->m_ImageInfos.end(), [i](const std::shared_ptr<ImageInfo>& info) {
                                        return info->index == i;
                                    });
                                    if (it != this_->m_ImageInfos.end()) {
                                        this_->textStyled_->SetSingleImageStyle(item->style_, *it, dimensionContext);
                                    }
                                }
                            }
                            this_->textStyled_->InitTypography();
                            this_->SetIsNeedUpdate(true);
                            this_->SetLayoutDirty(true);
                            this_->SetDrawDirty(true);
                            this_->SetTextMeasureFunc();
                        }
                    });
                }
            }
        }
    }

    void TaroTextNode::Build() {
        auto element = element_ref_.lock();
        m_ImageInfos.clear();
        m_ImageNodes.clear();
        textElementInfos_.clear();
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        if (!GetArkUINodeHandle()) {
            SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_STACK));
        }
        if (element) {
            std::vector<std::shared_ptr<TaroNode>> vec;
            GetTextChildren(element, vec);
            // 设置单个区域的样式
            auto size = vec.size();
            if (size == 0) {
                m_HasContent = false;
                return;
            }
            bool hasTextNode = false;
            for (int32_t i = 0; i < size; i++) {
                auto item = std::dynamic_pointer_cast<TaroDOM::TaroElement>(vec[i]);
                item->PreBuild();
                if (item->isTextElement()) {
                    auto textContent = (std::dynamic_pointer_cast<TaroText>(item))->GetTextContent();
                    if (!textContent.empty()) {
                        hasTextNode = true;
                    }
                } else if (item->tag_name_ == TAG_NAME::IMAGE) {
                    auto image = nativeNodeApi->createNode(ARKUI_NODE_IMAGE);
                    m_ImageNodes.emplace_back(image);
                    auto imageInfo = std::make_shared<ImageInfo>();
                    imageInfo->width = -1;
                    imageInfo->height = -1;
                    imageInfo->nid = item->nid_;
                    auto castItem = std::dynamic_pointer_cast<TaroImage>(item);
                    if (castItem->attributes_->src.has_value()) {
                        if (auto src = std::get_if<std::string>(&castItem->attributes_->src.value())) {
                            imageInfo->src = *src;
                            if (imageInfo->src.starts_with("//")) {
                                imageInfo->src.insert(0, "https:");
                            }
                        } else {
                            imageInfo->src = "";
                        }
                    } else {
                        imageInfo->src = "";
                    }
                    if (castItem->attributes_->mode.has_value()) {
                        auto it = ImageModeMap.find(castItem->attributes_->mode.value());
                        if (it != ImageModeMap.end()) {
                            imageInfo->mode = it->second;
                        } else {
                            imageInfo->mode = ScaleToFill;
                        }

                    } else {
                        imageInfo->mode = ScaleToFill;
                    }
                    if (imageInfo->mode != ScaleToFill) {
                        m_HasExactlyImage = false;
                    }
                    imageInfo->index = i;
                    m_ImageInfos.emplace_back(imageInfo);
                }
            }
            if (!hasTextNode && m_ImageNodes.size() == 0) {
                m_HasContent = false;
                return;
            }
            if (hasTextNode) {
                m_InnerTextNode = nativeNodeApi->createNode(ARKUI_NODE_TEXT);
                nativeNodeApi->addChild(GetArkUINodeHandle(), m_InnerTextNode);
            }
            for (int32_t i = 0; i < m_ImageNodes.size(); i++) {
                m_ImagesLoaded = false;
                nativeNodeApi->addChild(GetArkUINodeHandle(), m_ImageNodes[i]);
            }
        }
    }

    void TaroTextNode::PaintImages() {
        if (textStyled_->GetTypography()) {
            NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
            OH_Drawing_TextBox* textBox = OH_Drawing_TypographyGetRectsForPlaceholders(textStyled_->GetTypography());
            size_t textBoxSize = OH_Drawing_GetSizeOfTextBox(textBox);
            for (int32_t i = 0; i < textBoxSize; i++) {
                float left = OH_Drawing_GetLeftFromTextBox(textBox, i);
                float top = OH_Drawing_GetTopFromTextBox(textBox, i);
                auto imageInfo = m_ImageInfos[i];
                float width = imageInfo->width;
                float height = imageInfo->height;
                auto node = m_ImageNodes[i];
                if (!node)
                    continue;
                ArkUI_NumberValue positionValue[] = {0.0, 0.0};
                ArkUI_AttributeItem positionItem = {positionValue, 2};
                positionValue[0].f32 = px2Vp(left) + imageInfo->offsetLeft;
                auto textHeight = YGNodeLayoutGetHeight(ygNodeRef);
                auto fontHeight = px2Vp(OH_Drawing_TypographyGetHeight(textStyled_->GetTypography()));
                auto offsetTop = (textHeight - fontHeight) / 2;
                positionValue[1].f32 = px2Vp(top) + offsetTop + imageInfo->offsetTop;

                ArkUI_NumberValue sizeValue[] = {};
                ArkUI_AttributeItem sizeItem = {sizeValue, 1};
                sizeValue[0].f32 = width;
                nativeNodeApi->setAttribute(node, NODE_WIDTH, &sizeItem);
                sizeValue[0].f32 = height;
                nativeNodeApi->setAttribute(node, NODE_HEIGHT, &sizeItem);
                nativeNodeApi->setAttribute(node, NODE_POSITION, &positionItem);

                auto src = imageInfo->src;
                if (!src.empty()) {
                    std::weak_ptr<TaroTextNode> weakSelf = std::dynamic_pointer_cast<TaroTextNode>(shared_from_this());
                    TaroHelper::loadImage({.url = src}, [src, weakSelf, i](const ImageCallbackInfo& result) mutable {
                        auto renderText = weakSelf.lock();
                        if (renderText) {
                            const auto& imageNodes = renderText->m_ImageNodes;
                            const auto& imageInfos = renderText->m_ImageInfos;
                            if (i < imageNodes.size() && i < imageInfos.size()) {
                                const auto& imageNode = imageNodes[i];
                                const auto& imageInfo = imageInfos[i];
                                if (imageInfo->src != src)
                                    return;
                                ArkUI_AttributeItem srcItem;
                                if (auto info = std::get_if<TaroHelper::ResultImageInfo>(&result)) {
                                    srcItem = {.object = info->result_DrawableDescriptor};
                                } else {
                                    srcItem = {.string = src.c_str()};
                                }
                                NativeNodeApi::getInstance()->setAttribute(imageNode, NODE_IMAGE_SRC, &srcItem);
                            }
                        }
                    });
                }
            }
            OH_Drawing_TypographyDestroyTextBox(textBox);
        }
    }

    float TaroTextNode::GetUseToLayoutWidth() {
        auto yogaInstance = TaroYogaApi::getInstance();
        auto calcWidth = yogaInstance->getCalcLayoutWidth(ygNodeRef);
        auto useToLayoutWidth = calcWidth;
        if (useToLayoutWidth <= 0) {
            useToLayoutWidth = yogaInstance->getParentNodeWidthNotZero(ygNodeRef);
            if (useToLayoutWidth <= 0) {
                auto dimensionContext = GetDimensionContext();
                useToLayoutWidth = dimensionContext->viewport_width_;
            }
        }
        return useToLayoutWidth;
    }

    void TaroTextNode::SetSize() {
        if (!textStyled_->GetHasBeenLayout()) {
            auto useToLayoutWidth = GetUseToLayoutWidth();
            textStyled_->TextTypographyLayout(textStyled_->GetTypography(), vp2Px(useToLayoutWidth));
            if (textStyled_->GetHasBeenLayout() && m_InnerTextNode) {
                auto textCalcWidth = OH_Drawing_TypographyGetLongestLine(textStyled_->GetTypography());
                textCalcWidth = px2Vp(textCalcWidth);
                auto textCalcHeight = px2Vp(OH_Drawing_TypographyGetHeight(textStyled_->GetTypography()));
                auto yogaInstance = TaroYogaApi::getInstance();
                auto calcHeight = yogaInstance->getCalcLayoutHeight(ygNodeRef);
                YGMeasureMode heightMode = TaroGetHeightMeasureMode(ygNodeRef);
                switch (heightMode) {
                    case YGMeasureModeAtMost: {
                        textCalcHeight = std::min((float)calcHeight, textCalcHeight);
                        break;
                    }
                    case YGMeasureModeExactly:
                        textCalcHeight = calcHeight;
                        break;
                    case YGMeasureModeUndefined:
                        if (calcHeight > 0) {
                            textCalcHeight = calcHeight;
                        }
                        break;
                }

                float containerWidth = layoutDiffer_.computed_style_.width - layoutDiffer_.computed_style_.paddingLeft - layoutDiffer_.computed_style_.paddingRight;
                bool isNoWrap = style_ref_->whiteSpace.has_value() && style_ref_->whiteSpace.value() == PropertyType::WhiteSpace::NoWrap;
                bool isHidden = style_ref_->overflow.has_value() && style_ref_->overflow.value() == PropertyType::Overflow::Hidden;
                bool isEllipsis = style_ref_->textOverflow.has_value() && style_ref_->textOverflow.value() == ArkUI_TextOverflow::ARKUI_TEXT_OVERFLOW_ELLIPSIS;
                if (textCalcWidth > containerWidth && isNoWrap && isHidden && isEllipsis) {
                    SetMeasuredTextWidth(containerWidth);
                } else {
                    SetMeasuredTextWidth(textCalcWidth);
                }
                SetMeasuredTextHeight(textCalcHeight);
                if (m_MeasuredTextWidth < 0) {
                }
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setWidth(m_InnerTextNode, static_cast<double>(m_MeasuredTextWidth));
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setHeight(m_InnerTextNode, static_cast<double>(m_MeasuredTextHeight));
            }
        } else {
            if (m_InnerTextNode) {
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setWidth(m_InnerTextNode, static_cast<double>(m_MeasuredTextWidth));
                TaroCSSOM::TaroStylesheet::HarmonyStyleSetter::setHeight(m_InnerTextNode, static_cast<double>(m_MeasuredTextHeight));
            }
        }
    }

    void TaroTextNode::Layout() {
        SystraceSection s("Taro Layout:: Text Layout");
        TaroRenderNode::Layout();
        if (!m_HasContent)
            return;
        if (HasImages() && !HasImagesLoaded())
            return;
        if (!GetArkUINodeHandle())
            return;
        SetSize();
        if (HasImages() && textStyled_->GetHasBeenLayout()) {
            PaintImages();
        }
    }

    void TaroTextNode::ProcessTextAlign() {
        // 模拟text-align
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        auto textAlign = ArkUI_TextAlignment::ARKUI_TEXT_ALIGNMENT_START;
        if (style_ref_) {
            textAlign = style_ref_->textAlign.value_or(ArkUI_TextAlignment::ARKUI_TEXT_ALIGNMENT_START);
        }
        ArkUI_NumberValue sizeValue[] = {};
        ArkUI_AttributeItem sizeItem = {sizeValue, 1};
        if (textAlign == ARKUI_TEXT_ALIGNMENT_CENTER) {
            sizeValue[0].i32 = ArkUI_Alignment::ARKUI_ALIGNMENT_CENTER;
        } else if (textAlign == ARKUI_TEXT_ALIGNMENT_END) {
            sizeValue[0].i32 = ArkUI_Alignment::ARKUI_ALIGNMENT_END;
        } else {
            sizeValue[0].i32 = ArkUI_Alignment::ARKUI_ALIGNMENT_START;
        }
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_ALIGNMENT, &sizeItem);
    }

    void TaroTextNode::Paint() {
        SystraceSection s("Taro Draw:: Text Draw");
        TaroRenderNode::Paint();
        if (!m_HasContent)
            return;
        if (!m_isFirstRender && !m_IsNeedUpdate)
            return;
        if (HasImages() && !HasImagesLoaded())
            return;
        SetSize();
        ProcessTextAlign();
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        if (m_InnerTextNode && textStyled_->GetStyledString()) {
            nativeNodeApi->resetAttribute(m_InnerTextNode, NODE_TEXT_CONTENT_WITH_STYLED_STRING);
            ArkUI_AttributeItem content = {.object = textStyled_->GetStyledString()};
            nativeNodeApi->setAttribute(m_InnerTextNode, NODE_TEXT_CONTENT_WITH_STYLED_STRING, &content);
        }
        textStyled_->DestroyStyle();
        m_isFirstRender = false;
        SetIsNeedUpdate(false);
        textStyled_->SetHashBeenLayout(false);
        // TODO 暂时使用完之后不直接清理旧的 Typography，等华为确认，目前在析构的时候会清理，不会造成内存泄漏
        //         auto runner = Render::GetInstance()->GetTaskRunner();
        //         std::weak_ptr<TaroTextNode> weakSelf = std::dynamic_pointer_cast<TaroTextNode>(shared_from_this());
        //         runner->runTask(TaroThread::TaskThread::MAIN, [weakSelf] {
        //             auto this_ = weakSelf.lock();
        //             if (this_) {
        //                 this_->DestroyOldTypography();
        //             }
        //         });
    }

    void TaroTextNode::RemoveArkChild() {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        auto arkNode = GetArkUINodeHandle();
        auto totalChildCount = nativeNodeApi->getTotalChildCount(arkNode);
        if (totalChildCount == 0)
            return;
        for (int32_t i = totalChildCount - 1; i >= 0; i--) {
            auto child = nativeNodeApi->getChildAt(arkNode, i);
            if (child) {
                nativeNodeApi->removeChild(arkNode, child);
                nativeNodeApi->disposeNode(child);
            }
        }
        m_ImageNodes.clear();
    }

    void TaroTextNode::Reset() {
        if (m_InnerTextNode) {
            NativeNodeApi::getInstance()->resetAttribute(m_InnerTextNode, NODE_TEXT_CONTENT_WITH_STYLED_STRING);
        }
        RemoveArkChild();
        textStyled_->DestroyStyle();
        m_HasContent = true;
        m_HasExactlyImage = true;
        m_InnerTextNode = nullptr;
        textStyled_->SetHashBeenLayout(false);
    }

    bool TaroTextNode::GetIsFirstRender() {
        return m_isFirstRender;
    }

    void TaroTextNode::UpdateImage(int32_t id, srcType src) {
        auto element = element_ref_.lock();
        if (element) {
            for (auto& image : m_ImageInfos) {
                if (image->nid == id) {
                    if (auto srcValue = std::get_if<std::string>(&src)) {
                        image->src = *srcValue;
                        if (image->src.starts_with("//")) {
                            image->src.insert(0, "https:");
                        }
                    }
                    SetIsNeedUpdate(true);
                    return;
                }
            }
        }
    }

    ArkUI_NodeHandle TaroTextNode::GetImageArkNode(int32_t id) {
        auto element = element_ref_.lock();
        if (element) {
            for (int i = 0; i < m_ImageInfos.size(); i++) {
                if (m_ImageInfos[i]->nid == id) {
                    return m_ImageNodes[i];
                }
            }
        }
        return nullptr;
    }

    ArkUI_NodeHandle TaroTextNode::GetTextArkNode() {
        return m_InnerTextNode;
    }

    std::vector<ArkUI_NodeHandle>& TaroTextNode::GetImageArkNodeList() {
        return m_ImageNodes;
    }

    void TaroTextNode::SetTextRenderNodeInner(const ArkUI_NodeHandle& arkHandle) {
        m_InnerTextNode = arkHandle;
        if (m_InnerTextNode) {
            NativeNodeApi::getInstance()->resetAttribute(m_InnerTextNode, NODE_TEXT_CONTENT_WITH_STYLED_STRING);
        }
    }

    void TaroTextNode::SetTextRenderNodeImageInfos(const std::vector<std::shared_ptr<ImageInfo>>& infos) {
        m_ImageInfos = infos;
    }

    void TaroTextNode::SetTextRenderNodeImageNodes(const std::vector<ArkUI_NodeHandle>& nodes) {
        m_ImageNodes = nodes;
    }

    void TaroTextNode::SetTextRenderNodeHasContent(const bool& hasContent) {
        m_HasContent = hasContent;
    }

    void TaroTextNode::SetTextRenderNodeHasExactlyImage(const bool& hasExactlyImage) {
        m_HasExactlyImage = hasExactlyImage;
    }

    std::shared_ptr<TextElementInfo> TaroTextNode::GetLastTextElementInfo(int32_t index) {
        if (index == 0)
            return std::make_shared<TextElementInfo>();
        auto last = index - 1;
        if (last <= textElementInfos_.size() - 1) {
            return textElementInfos_[last];
        }
        return std::make_shared<TextElementInfo>();
    }

    std::shared_ptr<TextElementInfo> TaroTextNode::GetTextElementInfoFromRange(int32_t index, int32_t nid, int32_t start, int32_t end, int32_t& line) {
        auto textElementInfo = std::make_shared<TextElementInfo>();
        if (textStyled_->GetTypography() == nullptr) {
            textElementInfo->nid = nid;
            return textElementInfo;
        }
        auto textBox = OH_Drawing_TypographyGetRectsForRange(textStyled_->GetTypography(), start, end, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_TIGHT);
        size_t textBoxSize = OH_Drawing_GetSizeOfTextBox(textBox);
        textElementInfo->nid = nid;
        auto last = GetLastTextElementInfo(index);
        auto lastLastLineTextBox = last->GetLastLineTextBox();
        auto startPoint = TextPoint{.x = lastLastLineTextBox.end.x, .y = lastLastLineTextBox.start.y};
        auto endPoint = TextPoint{.x = lastLastLineTextBox.end.x, .y = lastLastLineTextBox.start.y};
        textElementInfo->AddLineTextBox(startPoint, endPoint);
        if (textBoxSize > 0) {
            for (int32_t j = 0; j < textBoxSize; j++) {
                auto top = OH_Drawing_GetTopFromTextBox(textBox, j);
                auto bottom = OH_Drawing_GetBottomFromTextBox(textBox, j);
                auto left = OH_Drawing_GetLeftFromTextBox(textBox, j);
                auto right = OH_Drawing_GetRightFromTextBox(textBox, j);
                auto width = right - left;
                // 超过了一行，另起一行
                auto lineWidth = OH_Drawing_TypographyGetLineWidth(textStyled_->GetTypography(), line);
                auto& lastLineBox = textElementInfo->GetLastLineTextBox();
                if (GreatOrEqualCustomPrecision(width + lastLineBox.end.x, lineWidth, 0.01)) {
                    line++;
                    auto newStartPoint = TextPoint{.x = left, .y = top};
                    auto newEndPoint = TextPoint{.x = width, .y = bottom};
                    textElementInfo->AddLineTextBox(newStartPoint, newEndPoint);
                } else {
                    // 不足一行继续累积
                    lastLineBox.start.y = top;
                    lastLineBox.end.x += width;
                    lastLineBox.end.y = bottom;
                }
            }
        }
        OH_Drawing_TypographyDestroyTextBox(textBox);
        return textElementInfo;
    }

    void TaroTextNode::GetAllTextElementInfos() {
        if (textElementInfos_.size() > 0)
            return;
        auto element = element_ref_.lock();
        if (element) {
            auto childNodes = element->child_nodes_;
            int32_t rangeIndex = 0;
            int32_t line = 0;
            for (int32_t i = 0; i < childNodes.size(); i++) {
                auto item = std::dynamic_pointer_cast<TaroDOM::TaroElement>(childNodes[i]);
                if (item->tag_name_ == TAG_NAME::TEXT) {
                    auto textContent = (std::dynamic_pointer_cast<TaroText>(item))->GetTextContent();
                    if (textContent.empty()) {
                        auto info = std::make_shared<TextElementInfo>();
                        auto last = GetLastTextElementInfo(i);
                        info->nid = item->nid_;
                        info->lineBox = last->lineBox;
                        textElementInfos_.emplace_back(info);
                        continue;
                    }
                    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
                    std::u32string u32str = converter.from_bytes(textContent);
                    auto textSize = u32str.size();
                    auto textElementInfo = GetTextElementInfoFromRange(i, item->nid_, rangeIndex, rangeIndex + textSize, line);
                    rangeIndex += textSize;
                    textElementInfos_.emplace_back(textElementInfo);
                } else if (item->tag_name_ == TAG_NAME::IMAGE) {
                    auto textElementInfo = GetTextElementInfoFromRange(i, item->nid_, rangeIndex, rangeIndex + 1, line);
                    rangeIndex++;
                    textElementInfos_.emplace_back(textElementInfo);
                }
            }
        }
    }

    bool TaroTextNode::CheckIsPointInsideRectangle(float x, float y, float rectTopLeftX, float rectTopLeftY, float rectBottomRightX, float rectBottomRightY) {
        return x >= rectTopLeftX && x <= rectBottomRightX && y >= rectTopLeftY && y <= rectBottomRightY;
    }

    bool TaroTextNode::GetIfInTextNodeArea(int32_t nid, float x, float y) {
        GetAllTextElementInfos();
        for (const auto& info : textElementInfos_) {
            if (info->nid == nid) {
                for (const auto& lineBox : info->lineBox) {
                    if (CheckIsPointInsideRectangle(x, y, lineBox.start.x, lineBox.start.y, lineBox.end.x, lineBox.end.y)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
} // namespace TaroDOM
} // namespace TaroRuntime
