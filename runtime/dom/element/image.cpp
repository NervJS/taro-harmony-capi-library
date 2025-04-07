/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "image.h"

#include <arkui/drawable_descriptor.h>
#include <arkui/native_node.h>

#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/stylesheet/utils.h"
#include "runtime/dom/ark_nodes/image.h"
#include "runtime/dom/ark_nodes/text.h"
#include "runtime/dom/element/text.h"
#include "runtime/dom/event/event_hm/event_types/event_areachange.h"
#include "runtime/dom/event/event_hm/event_types/event_image.h"
#include "runtime/render.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroImage::TaroImage(napi_value node)
        : TaroElement(node) {
        SetIsInline(true);
        attributes_ = std::make_unique<TaroImageAttributes>();
    }

    void TaroImage::GetNodeAttributes() {
        TaroElement::GetNodeAttributes();

        GetLazyLoadAttribute(GetAttributeNodeValue("lazyLoad"));
        GetSrcAttribute(GetAttributeNodeValue("src"));
        GetPlaceholderAttribute(GetAttributeNodeValue("placeHolder"));
        GetErrorHolderAttribute(GetAttributeNodeValue("errorHolder"));
        GetModeAttribute(GetAttributeNodeValue("mode"));
    }

    void TaroImage::GetLazyLoadAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<bool> lazyLoad = getter.BoolNull();
        if (lazyLoad.has_value()) {
            attributes_->lazyLoad.set(lazyLoad.value());
        }
    }

    void TaroImage::GetSrcAttribute(const napi_value& value) {
        NapiGetter getter(value);
        napi_valuetype type = getter.Type();

        if (type == napi_string) {
            TaroHelper::Optional<std::string> src = getter.String();
            if (src.has_value()) {
                // 记录原始图片链接
                if (attributes_->src.has_value()) {
                    if (auto src_string = std::get_if<std::string>(&attributes_->src.value())) {
                        old_src_ = *src_string;
                    }
                }
                attributes_->src.set(src.value());
            }

        } else if (type == napi_object) {
            ArkUI_DrawableDescriptor* descriptor;
            int32_t res = OH_ArkUI_GetDrawableDescriptorFromResourceNapiValue(NativeNodeApi::env, value, &descriptor);
            if (res == ARKUI_ERROR_CODE_NO_ERROR) {
                attributes_->src.set(descriptor);
            }
        }
    }

    void TaroImage::GetPlaceholderAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<std::string> placeholder = getter.String();
        if (placeholder.has_value()) {
            attributes_->placeholder.set(placeholder.value());
        }
    }

    void TaroImage::GetErrorHolderAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<std::string> errorHolder = getter.String();
        if (errorHolder.has_value()) {
            attributes_->errorHolder.set(errorHolder.value());
        }
    }

    void TaroImage::GetModeAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<std::string> mode = getter.String();
        if (mode.has_value()) {
            attributes_->mode.set(mode.value());
        }
    }

    void TaroImage::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        auto parentNode = GetParentNode();
        auto parentElement = std::dynamic_pointer_cast<TaroText>(parentNode);
        if (parentElement) {
            if (!parentNode->HasRenderNode())
                return;
            for (auto child : parentNode->child_nodes_) {
                auto item = std::dynamic_pointer_cast<TaroDOM::TaroElement>(child);
                if (!item->is_init_) {
                    return;
                }
            }
            auto renderNode = parentNode->GetHeadRenderNode();
            auto taroTextNode = std::dynamic_pointer_cast<TaroTextNode>(renderNode);
            if (taroTextNode && taroTextNode->GetIsNeedUpdate()) {
                return;
            }
            renderNode->SetStyle(std::dynamic_pointer_cast<TaroDOM::TaroElement>(parentNode)->style_);
            renderNode->SetContent();
            return;
        }
        if (!HasRenderNode())
            return;
        std::shared_ptr<TaroImageNode> render_image = std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode());

        SetLazyLoadAttribute();
        SetPlaceholderAttribute();
        SetErrorHolderAttribute();
        SetSrcAttribute();
        // SetModeAttribute(); 必须在 render_image->SetStyle(style_); 前面，因为这一步会根据mode的设置width或者height为100%，
        // 这个设置只是备选，如果class或者style有设置，应该以后者为准
        SetModeAttribute();

        render_image->SetStyle(style_);
        if (attributes_->mode.has_value()) {
            auto mode = attributes_->mode.value();
            if (mode == "heightFix" || mode == "widthFix") {
                if (mode == "heightFix") {
                    if (style_ && !style_->height.has_value()) {
                        render_image->SetHeight(Dimension{1, DimensionUnit::PERCENT});
                    }
                    if (style_ && style_->width.has_value()) {
                        render_image->SetWidth(Dimension{0});
                    }
                } else if (mode == "widthFix") {
                    if (style_ && !style_->width.has_value()) {
                        render_image->SetWidth(Dimension{1, DimensionUnit::PERCENT});
                    }
                    if (style_ && style_->height.has_value()) {
                        render_image->SetHeight(Dimension{0});
                    }
                }
                if (attributes_->src.has_value()) {
                    if (auto src_string = std::get_if<std::string>(&attributes_->src.value())) {
                        // 获取图片解码的宽高尺寸信息，有缓存的情况下防止二次布局
                        std::weak_ptr<TaroImageNode> weak_render_image = render_image;

                        if (src_string->find("gif") == std::string::npos) {
                            TaroHelper::loadImage({.url = *src_string}, [weak_render_image](const std::variant<TaroHelper::ResultImageInfo, TaroHelper::ErrorImageInfo> result) mutable {
                                auto res = std::get_if<TaroHelper::ResultImageInfo>(&result);
                                auto render_image = weak_render_image.lock();
                                if (res && render_image) {
                                    render_image->image_raw_width = res->width;
                                    render_image->image_raw_height = res->height;
                                    render_image->repairSizeIfNeed();
                                    render_image->setImageSrc(res->result_DrawableDescriptor->get());
                                    render_image->relatedImageDrawableDescriptors.push_back(res->result_DrawableDescriptor);
                                }
                            });
                        }
                    }
                }
            } else {
                // https://developers.weixin.qq.com/miniprogram/dev/component/image.html#Bug-Tip 这个tip的逻辑
                if (!style_->width.has_value()) {
                    render_image->SetWidth(Dimension{320, DimensionUnit::DESIGN_PX});
                }
                if (!style_->height.has_value()) {
                    render_image->SetHeight(Dimension{240, DimensionUnit::DESIGN_PX});
                }
            }
            if (mode == "heightFix") {
                if (!render_image->is_first_layout_finish_ && std::isfinite(render_image->image_raw_width) && style_->height.has_value()) {
                    auto height = style_->height.value().ParseToVp(render_image->GetDimensionContext());
                    if (height.has_value()) {
                        render_image->SetWidth(Dimension{render_image->image_raw_width * height.value() / render_image->image_raw_height});
                    }
                }
            } else if (mode == "widthFix") {
                if (!render_image->is_first_layout_finish_ && std::isfinite(render_image->image_raw_height) && style_->width.has_value()) {
                    auto width = style_->width.value().ParseToVp(render_image->GetDimensionContext());
                    if (width.has_value()) {
                        render_image->SetHeight(Dimension{render_image->image_raw_height * width.value() / render_image->image_raw_width});
                    }
                }
            }
        }
    }

    void TaroImage::SetLazyLoadAttribute() {
        bool lazyLoad = false;
        if (attributes_->lazyLoad.has_value()) {
            lazyLoad = attributes_->lazyLoad.value();
        }
    }

    void TaroImage::SetSrcAttribute() {
        std::shared_ptr<TaroImageNode> render_image = std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode());
        double width = 0.0;
        double height = 0.0;
        if (style_->width.has_value()) {
            auto widthValue = style_->width.value().ParseToVp(render_image->GetDimensionContext());
            if (widthValue.has_value()) {
                width = widthValue.value();
            }
        }
        if (style_->height.has_value()) {
            auto heightValue = style_->height.value().ParseToVp(render_image->GetDimensionContext());
            if (heightValue.has_value()) {
                height = heightValue.value();
            }
        }
        if (attributes_->src.has_value()) {
            auto s = attributes_->src.value();
            bool clearRawSize = true;
            if (auto srcVal = std::get_if<std::string>(&s)) {
                if (!old_src_.size() || *srcVal == old_src_) { // 首次图片设置 / 前后链接一致
                    clearRawSize = false;
                }
                render_image->setImageSrc(*srcVal, height, width, attributes_->lazyLoad.value_or(false));
            } else if (auto srcVal = std::get_if<ArkUI_DrawableDescriptor*>(&s)) {
                render_image->setImageSrc(*srcVal);
            }
            if (clearRawSize) {
                render_image->image_raw_width = NAN;
                render_image->image_raw_height = NAN;
            }
        }
    }

    void TaroImage::SetPlaceholderAttribute() {
        std::shared_ptr<TaroImageNode> render_image = std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode());
        if (attributes_->placeholder.has_value()) {
            render_image->setAlt(attributes_->placeholder.value());
        }
    }

    void TaroImage::SetErrorHolderAttribute() {
        std::shared_ptr<TaroImageNode> render_image = std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode());
        if (attributes_->errorHolder.has_value()) {
            render_image->setErrorHolder(attributes_->errorHolder.value());
        }
    }

    void TaroImage::SetModeAttribute() {
        std::shared_ptr<TaroImageNode> render_image = std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode());
        if (attributes_->mode.has_value()) {
            std::string s = attributes_->mode.value();
            render_image->setMode(s);
        }
    }

    void TaroImage::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);

        // 没有初始化没有 attribute
        if (!is_init_ || GetHeadRenderNode() == nullptr)
            return;

        switch (name) {
            case ATTRIBUTE_NAME::LAZY_LOAD:
                GetLazyLoadAttribute(value);
                if (is_init_) {
                    SetLazyLoadAttribute();
                }
                break;
            case ATTRIBUTE_NAME::SRC:
                GetSrcAttribute(value);
                if (is_init_) {
                    auto parentNode = GetParentNode();
                    auto parentElement = std::dynamic_pointer_cast<TaroText>(parentNode);
                    if (!parentElement) {
                        SetSrcAttribute();
                    } else {
                        auto renderNode = parentElement->GetHeadRenderNode();
                        if (!renderNode)
                            return;
                        auto textRenderNode = std::dynamic_pointer_cast<TaroTextNode>(renderNode);
                        if (!textRenderNode)
                            return;
                        if (attributes_->src.has_value()) {
                            textRenderNode->UpdateImage(nid_, attributes_->src.value());
                        } else {
                            textRenderNode->UpdateImage(nid_, "");
                        }
                        textRenderNode->SetLayoutDirty(true);
                        textRenderNode->SetDrawDirty(true);
                        textRenderNode->SetContent();
                    }
                }
                break;
            case ATTRIBUTE_NAME::PLACEHOLDER:
                GetPlaceholderAttribute(value);
                if (is_init_) {
                    SetPlaceholderAttribute();
                }
                break;
            case ATTRIBUTE_NAME::ERROR_HOLDER:
                GetErrorHolderAttribute(value);
                if (is_init_) {
                    SetErrorHolderAttribute();
                }
                break;
            case ATTRIBUTE_NAME::MODE:
                GetModeAttribute(value);
                if (is_init_) {
                    auto parentNode = GetParentNode();
                    auto parentElement = std::dynamic_pointer_cast<TaroText>(parentNode);
                    if (!parentElement) {
                        SetModeAttribute();
                    } else {
                        auto renderNode = parentElement->GetHeadRenderNode();
                        if (!renderNode)
                            return;
                        auto textRenderNode = std::dynamic_pointer_cast<TaroTextNode>(renderNode);
                        if (!textRenderNode)
                            return;
                        textRenderNode->SetLayoutDirty(true);
                        textRenderNode->SetDrawDirty(true);
                        textRenderNode->SetContent();
                    }
                }
                break;
            default:
                break;
        }
    }

    std::string GetImageSrc(napi_value value) {
        NapiGetter getter(value);
        if (getter.Type() != napi_object) {
            getter = getter.GetValue();
        } else {
            getter = getter.GetProperty("src");
        }
        return getter.StringOr("");
    }

    void TaroImage::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            std::shared_ptr<TaroNode> parentNode = GetParentNode();
            if (!parentNode || !dynamic_cast<TaroText*>(parentNode.get())) {
                auto render_image = std::make_shared<TaroImageNode>(element);
                render_image->Build();
                SetRenderNode(render_image);
                HandleOnImageComplete();
            }
        }
    }

    bool TaroImage::bindListenEvent(const std::string& event_name) {
        if (event_name == "click") {
            auto parentNode = GetParentNode();
            if (parentNode) {
                auto parentElement = std::dynamic_pointer_cast<TaroText>(parentNode);
                if (parentElement) {
                    auto renderNode = parentElement->GetHeadRenderNode();
                    if (renderNode) {
                        auto textRenderNode = std::dynamic_pointer_cast<TaroTextNode>(renderNode);
                        if (textRenderNode) {
                            auto arkNode = textRenderNode->GetImageArkNode(nid_);
                            if (arkNode) {
                                event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name, nullptr, arkNode);
                                return true;
                            }
                        }
                    }
                }
            }
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else if (event_name == "complete") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_COMPLETE_ON_IMAGE, event_name);
        } else if (event_name == "load") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_COMPLETE_ON_IMAGE, event_name);
        } else {
            return false;
        }
        return true;
    }

    void TaroImage::HandleAttributeChanged(TaroRuntime::ATTRIBUTE_NAME name, const std::string& preValue, const std::string& curValue) {
        switch (name) {
            case TaroRuntime::ATTRIBUTE_NAME::SRC:
                if (preValue != curValue) {
                    attributes_->src.set(curValue);
                    std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode())->setImageSrc(curValue.c_str());
                }
                break;
            case TaroRuntime::ATTRIBUTE_NAME::PLACEHOLDER:
                if (preValue != curValue) {
                    attributes_->placeholder.set(curValue);
                    std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode())->setAlt(curValue);
                }
                break;
            default:
                TaroElement::HandleAttributeChanged(name, preValue, curValue);
                break;
        }
    }

    void TaroImage::HandleOnImageComplete() {
        auto complete_fun = [this](std::shared_ptr<TaroEvent::TaroEventBase> event, napi_value&) -> int {
            TARO_LOG_DEBUG("Taro HandleOnImageComplete", "HandleOnImageComplete call");
            auto comp_event = std::static_pointer_cast<TaroEvent::TaroEventCompleteOnImage>(event);
            // 表示加载状态，0表示数据加载成功，1表示解码成功。
            int status = comp_event->loading_status_;
            // 图片的宽度
            float w = comp_event->width_;
            // 图片的高度
            float h = comp_event->height_;
            // 当前组件的宽度
            float cw = comp_event->component_width_;
            // 当前组件的高度
            float ch = comp_event->component_height_;

            if (auto render_image = std::static_pointer_cast<TaroImageNode>(this->render_node_)) {
                if (std::isfinite(render_image->image_raw_height) && std::isfinite(render_image->image_raw_width)) {
                    return 0;
                }
                render_image->image_raw_height = h;
                render_image->image_raw_width = w;
                render_image->repairSizeIfNeed();
            }

            return 0;
        };
        event_emitter_->registerEvent_NoCallBack(TaroEvent::TARO_EVENT_TYPE_COMPLETE_ON_IMAGE, "complete", complete_fun);
    }

    void TaroImage::Build(std::shared_ptr<TaroElement>& reuse_element) {
        std::shared_ptr<TaroImageNode> new_node = std::dynamic_pointer_cast<TaroImageNode>(GetHeadRenderNode());
        auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
        auto ark_handle = reuse_element->GetNodeHandle();
        if (new_node == nullptr && ark_handle == nullptr) {
            is_init_ = false;
            Build();
            return;
        }
        if (new_node == nullptr) {
            new_node = std::make_shared<TaroImageNode>(element);
            new_node->SetArkUINodeHandle(ark_handle);
            reuse_element->event_emitter_->clearNodeEvent(ark_handle);
            SetRenderNode(new_node);
            new_node->UpdateDifferOldStyleFromElement(reuse_element);
            reuse_element->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
            NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
            nativeNodeApi->resetAttribute(ark_handle, NODE_IMAGE_SRC);
            return;
        }
        if (ark_handle == nullptr) {
            auto parent = new_node->parent_ref_.lock();
            if (parent) {
                new_node->Build();
                parent->UpdateChild(new_node);
                updateListenEvent();
                new_node->ClearDifferOldStyleFromElement();
            }
            return;
        }
        new_node->SetArkUINodeHandle(ark_handle);
        new_node->UpdateDifferOldStyleFromElement(reuse_element);
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        nativeNodeApi->resetAttribute(ark_handle, NODE_IMAGE_SRC);
        reuse_element->event_emitter_->clearNodeEvent(ark_handle);
        updateListenEvent();
        reuse_element->GetHeadRenderNode()->SetArkUINodeHandle(nullptr);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
