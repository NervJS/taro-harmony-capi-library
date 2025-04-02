/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
