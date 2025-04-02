/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
