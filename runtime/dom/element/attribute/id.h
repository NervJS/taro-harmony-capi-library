/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TESTNDK_ID_H
#define TESTNDK_ID_H

#include <string>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "./base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace Attribute {
        class Id : public Base<std::string> {
            public:
            void SetValueFromNapi(const napi_value) override;

            using Base::SetValueToNode;
            void SetValueToNode(std::shared_ptr<TaroRenderNode> renderNode, const std::string value) override;

            static constexpr const char* DEFAULT_VALUE = "";
            static constexpr const std::string_view ATTR_NAME = "id";
        };
    } // namespace Attribute
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TESTNDK_ID_H
