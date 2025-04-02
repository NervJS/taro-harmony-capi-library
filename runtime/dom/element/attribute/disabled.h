/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TESTNDK_DISABLED_H
#define TESTNDK_DISABLED_H

#include <string>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace Attribute {
        class Disabled : public Base<bool> {
            public:
            void SetValueFromNapi(const napi_value) override;

            using Base::SetValueToNode;
            void SetValueToNode(std::shared_ptr<TaroRenderNode> renderNode, const bool value) override;

            static constexpr const bool DEFAULT_VALUE = false;
            static constexpr const std::string_view ATTR_NAME = "disabled";
        };
    } // namespace Attribute
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TESTNDK_DISABLED_H
