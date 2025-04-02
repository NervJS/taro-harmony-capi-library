/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_FLEX_BASIS_H
#define TARO_CAPI_HARMONY_DEMO_FLEX_BASIS_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "attribute_base.h"
#include "helper/Optional.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {
        // NODE_FLEX_BASIS
        class FlexBasis : public AttributeBase<Dimension> {
            public:
        };
    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_FLEX_BASIS_H
