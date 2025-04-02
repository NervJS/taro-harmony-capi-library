/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TESTNDK_BASE_H
#define TESTNDK_BASE_H

#pragma once

#include <string>
#include <arkui/native_type.h>

#include "helper/Optional.h"
#include "runtime/NapiGetter.h"
#include "runtime/constant.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace Attribute {
        class BaseHandle {
            public:
            std::shared_ptr<TaroRenderNode> render_node_;

            protected:
            TaroNode* node_owner_;
            napi_value attrs_ = nullptr;

            napi_value GetListenersNodeValue();
        };

        template <typename T>
        class Base : public virtual BaseHandle, public TaroHelper::Optional<T> {
            public:
            Base() = default;

            virtual void SetValueFromNapi(const napi_value) = 0;

            void SetValueToNode(T value) {
                SetValueToNode(render_node_, value);
            };

            virtual void SetValueToNode(std::shared_ptr<TaroRenderNode> renderNode, T value) = 0;

            void ResetValueToNode(T value = DEFAULT_VALUE);

            virtual void ResetValueToNode(std::shared_ptr<TaroRenderNode> renderNode, T value = DEFAULT_VALUE) {
                SetValueToNode(renderNode, value);
            }

            static constexpr T DEFAULT_VALUE = T{};
            static constexpr const std::string_view ATTR_NAME = "";
        };
    } // namespace Attribute
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TESTNDK_BASE_H
