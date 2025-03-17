//
// Created on 2024/6/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TESTNDK_STYLE_H
#define TESTNDK_STYLE_H

#include <string>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "./base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace Attribute {
        class Style : public Base<std::string> {
            public:
            void SetValueFromNapi(const napi_value) override;

            using Base::SetValueToNode;
            void SetValueToNode(std::shared_ptr<TaroRenderNode> renderNode, const std::string value) override;

            static constexpr const char *DEFAULT_VALUE = "";
            static constexpr const std::string_view ATTR_NAME = "style";
        };
    } // namespace Attribute
} // namespace TaroDOM
} // namespace TaroRuntime

#endif //  TESTNDK_STYLE_H
