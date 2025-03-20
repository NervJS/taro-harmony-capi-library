//
// Created on 2024/4/18.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_IMAGE_H
#define TARO_CAPI_HARMONY_DEMO_IMAGE_H

#include <variant>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>

#include "runtime/dom/element/element.h"
#include "helper/ImageLoader.h"

namespace TaroRuntime {
namespace TaroDOM {
    enum ImageMode {
        ScaleToFill = 0,
        AspectFit,
        AspectFill,
        WidthFix,
        HeightFix
    };
    const std::unordered_map<std::string, ImageMode> ImageModeMap = {
        {"", ScaleToFill},
        {"scaleToFill", ScaleToFill},
        {"aspectFit", AspectFit},
        {"aspectFill", AspectFill},
        {"widthFix", WidthFix},
        {"heightFix", HeightFix},
    };
    struct TaroImageAttributes : public CommonAttributes {
        using srcType = std::variant<std::string, ArkUI_DrawableDescriptor*>;
        // Image属性
        TaroHelper::Optional<srcType> src;
        TaroHelper::Optional<std::string> mode;
        TaroHelper::Optional<std::string> placeholder;
        TaroHelper::Optional<std::string> errorHolder;
        TaroHelper::Optional<bool> lazyLoad;
    };

    class TaroImage : public TaroElement {
        private:
        ArkUI_NodeHandle image_;

        public:
        TaroImage(napi_value node);

        ~TaroImage() = default;

        void Build() override;
        void Build(std::shared_ptr<TaroElement> &reuse_element) override;

        std::unique_ptr<TaroImageAttributes> attributes_;

        void GetNodeAttributes() override;
        void SetAttributesToRenderNode() override;
        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        void HandleAttributeChanged(TaroRuntime::ATTRIBUTE_NAME name, const std::string& preValue, const std::string& curValue) override;

        bool bindListenEvent(const std::string& event_name) override;

        private:
        void handleAreaChangeEvent();

        // 从Napi获取值，设置到属性上
        void GetLazyLoadAttribute(const napi_value& value);
        void GetSrcAttribute(const napi_value& value);
        void GetPlaceholderAttribute(const napi_value& value);
        void GetErrorHolderAttribute(const napi_value& value);
        void GetModeAttribute(const napi_value& value);
        // 将属性设置到RenderNode上
        void SetLazyLoadAttribute();
        void SetSrcAttribute();
        void SetPlaceholderAttribute();
        void SetErrorHolderAttribute();
        void SetModeAttribute();

        void HandleOnImageComplete();
    
        std::string old_src_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_IMAGE_H
