/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace Attribute {
        napi_value BaseHandle::GetListenersNodeValue() {
            // FIXME 复用节点可能导致 listeners 失效，后续再考虑减少获取 listeners 频率问题
            napi_value listeners = nullptr;
            NapiGetter::GetPropertyFromNode(node_owner_->GetNodeValue(), "__listeners", listeners);
            return listeners;
        }

        template <typename T>
        void Base<T>::ResetValueToNode(T value) {
            ResetValueToNode(render_node_, value);
        }
    } // namespace Attribute
} // namespace TaroDOM
} // namespace TaroRuntime
