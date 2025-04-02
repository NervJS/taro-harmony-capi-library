/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_ICON_H
#define TARO_CAPI_HARMONY_DEMO_ICON_H

#include <arkui/drawable_descriptor.h>

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {

    struct TaroIconAttributes : public CommonAttributes {
        // icon attributes
        std::string type;
        TaroHelper::Optional<Dimension> size;
        TaroHelper::Optional<uint32_t> color;
    };

    class TaroIcon : public TaroElement {
        public:
        TaroIcon(napi_value node);

        ~TaroIcon();

        void Build() override;
        void Build(std::shared_ptr<TaroElement> &reuse_element) override;

        std::unique_ptr<TaroIconAttributes> attributes_;

        void SetAttributesToRenderNode() override;
        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        bool bindListenEvent(const std::string& event_name) override;

        private:
        bool isValidIconType = false;
        std::string iconPath;

        // 从Napi获取值，设置到属性上
        void GetSizeAttribute(const napi_value& value);
        void GetTypeAttribute(const napi_value& value);
        void GetColorAttribute(const napi_value& value);
        // 将属性设置到RenderNode上
        void SetSizeAttribute();
        void SetTypeAttribute();
        void SetColorAttribute();
    };
} // namespace TaroDOM

} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_ICON_H
