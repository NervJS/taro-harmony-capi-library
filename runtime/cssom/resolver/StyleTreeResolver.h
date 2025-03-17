//
// Created on 2024/5/21.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_STYLETREERESOLVER_H
#define TARO_CAPI_HARMONY_DEMO_STYLETREERESOLVER_H

#include "runtime/dom/ark_nodes/arkui_node.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    class StyleTreeResolver {
        public:
        void resolver(std::shared_ptr<TaroDOM::TaroElement> elementRoot);
    };
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_STYLETREERESOLVER_H
