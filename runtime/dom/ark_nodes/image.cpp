/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "image.h"

#include "runtime/NativeNodeApi.h"
#include "runtime/dom/element/element.h"
#include "runtime/dom/element/text.h"
#include "runtime/tmp_resource_manager.h"

namespace TaroRuntime {

namespace TaroDOM {

    std::map<std::string, ArkUI_ObjectFit> IMAGE_MODE_MAP = {
        {"aspectFill", ARKUI_OBJECT_FIT_COVER},
        {"aspectFit", ARKUI_OBJECT_FIT_CONTAIN},
        {"scaleToFill", ARKUI_OBJECT_FIT_FILL}};

    TaroImageNode::TaroImageNode(TaroElementRef element)
        : TaroRenderNode(element) {
    }

    TaroImageNode::~TaroImageNode() {}

    void TaroImageNode::BuildSpanImage() {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        // 若是 SpanImage，需要销毁前面创建的 image 组件实例
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_IMAGE_SPAN));
    }

    bool isSvgUrl(const std::string& url) {
        // 先找到问号的位置，去除参数部分
        size_t queryPos = url.find('?');
        std::string cleanUrl = (queryPos != std::string::npos) ? url.substr(0, queryPos) : url;

        // 查找最后一个点的位置
        size_t dotPos = cleanUrl.find_last_of('.');
        if (dotPos == std::string::npos)
            return false;

        // 获取并转换扩展名为小写
        std::string extension = cleanUrl.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        return extension == "svg";
    }

    void TaroImageNode::Build() {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        SetArkUINodeHandle(nativeNodeApi->createNode(ARKUI_NODE_IMAGE));

        TaroClassLifeStatistic::markNew("NodeHandle");
        setDraggable(false);

        ArkUI_NumberValue imageMode[1];
        ArkUI_AttributeItem objectFitItem = {imageMode, 1};
        imageMode[0].i32 = ARKUI_OBJECT_FIT_FILL;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_IMAGE_OBJECT_FIT, &objectFitItem);
    }

    void TaroImageNode::setImageSrc(std::string src, const double height, const double width, bool lazyLoad) {
        current_src_ = src;
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        if (src.size() && src.starts_with("/data")) {
            // 本地链接
            auto tmpResInst = TaroTmpResource::GetInstance();
            auto it = TaroTmpResource::GetInstance()->tmp_pixels_manager_.find(src.c_str());
            if (it != tmpResInst->tmp_pixels_manager_.end()) {
                auto drawableDescriptor = OH_ArkUI_DrawableDescriptor_CreateFromPixelMap(it->second);
                ArkUI_AttributeItem item = {.object = drawableDescriptor};
                nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_IMAGE_SRC, &item);
                return;
            }
        }
        ArkUI_AttributeItem item = {.string = src.c_str()};
        TARO_LOG_DEBUG("TaroImage", "Image 地址：%{public}s", src.c_str());
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_IMAGE_SRC, &item);
    }

    void TaroImageNode::setImageSrc(ArkUI_DrawableDescriptor* drawableDescriptor) {
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_AttributeItem item = {.object = drawableDescriptor};
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_IMAGE_SRC, &item);
    }

    void TaroImageNode::setDraggable(bool enabled) {
        ArkUI_NumberValue value[1] = {};
        ArkUI_AttributeItem item = {value, 1};
        value[0].i32 = enabled;
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_IMAGE_DRAGGABLE, &item);
    }

    void TaroImageNode::setImageFillColor(uint32_t color) {
        ArkUI_NumberValue value[1];
        ArkUI_AttributeItem item = {value, 1};
        value[0].u32 = color;
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_IMAGE_FILL_COLOR, &item);
    }

    void TaroImageNode::setAlt(const std::string& uri) {
        ArkUI_AttributeItem item = {.string = uri.c_str()};
        NativeNodeApi::getInstance()->setAttribute(GetArkUINodeHandle(), NODE_IMAGE_ALT, &item);
    }

    void TaroImageNode::setErrorHolder(const std::string& uri) {
        TaroImageNode::setAlt(uri);
    }

    void TaroImageNode::setMode(const std::string& mode) {
        mode_ = mode;
        ArkUI_ObjectFit arkUI_ObjectFit = ARKUI_OBJECT_FIT_COVER;
        auto it = IMAGE_MODE_MAP.find(mode);
        if (it != IMAGE_MODE_MAP.end()) {
            arkUI_ObjectFit = it->second;
        }
        NativeNodeApi* nativeNodeApi = NativeNodeApi::getInstance();
        ArkUI_NumberValue imageMode[1];
        ArkUI_AttributeItem objectFitItem = {imageMode, 1};
        imageMode[0].i32 = arkUI_ObjectFit;
        nativeNodeApi->setAttribute(GetArkUINodeHandle(), NODE_IMAGE_OBJECT_FIT, &objectFitItem);
    }

    void TaroImageNode::Layout() {
        TaroRenderNode::Layout();
        repairSizeIfNeed();
    }

    void TaroImageNode::SetStyle(StylesheetRef style_ref) {
        TaroRenderNode::SetStyle(style_ref);
        if (style_ref_->color.has_value() && isSvgUrl(current_src_)) {
            setImageFillColor(style_ref_->color.value());
        }
    }

    void TaroImageNode::repairSizeIfNeed() {
        // 如果没有 image 的宽高信息，不干活
        if (std::isnan(image_raw_width) || std::isnan(image_raw_height))
            return;
        if (layoutDiffer_.GetDisplayStyle() == YGDisplayNone)
            return;
        if (!HasLayoutFlag(LAYOUT_STATE_FLAG::IS_FIRST_LAYOUT_FINISH))
            return;

        float cw = layoutDiffer_.computed_style_.width;
        float ch = layoutDiffer_.computed_style_.height;
        // 如果缩放模式是宽度自适应，同时容器没有显示定义高度，则用容器的宽度和图片的宽高比算出高度

        if (mode_ == "widthFix" && cw != 0) {
            float finalHeight = cw / image_raw_width * image_raw_height;
            if (ch != finalHeight) {
                SetHeight(finalHeight);
            }
        }

        // 如果缩放模式是高度自适应，同时容器没有显示定义宽度，则用容器的高度和图片的宽高比算出宽度
        if (mode_ == "heightFix" && ch != 0) {
            float finalWidth = ch / image_raw_height * image_raw_width;
            if (cw != finalWidth) {
                SetWidth(finalWidth);
            }
        }

        // 如果尺寸是宽高拉满
        if (mode_ == "scaleToFill") {
            // Calc本身首次布局的时候，宽高就是0，所以不需要再次设置
            // 宽高有设置的以设置的为准，没设置的以图片宽高为准
            if (cw == 0) {
                if (style_ref_->width.has_value() && style_ref_->width.value().Unit() == DimensionUnit::CALC)
                    return;
                SetWidth(image_raw_width);
            }
            if (ch == 0) {
                if (style_ref_->height.has_value() && style_ref_->height.value().Unit() == DimensionUnit::CALC)
                    return;
                SetHeight(image_raw_height);
            }
        }
    }

} // namespace TaroDOM
} // namespace TaroRuntime
