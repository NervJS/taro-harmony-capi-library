/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_STYLEINVALIDATOR_H
#define TARO_CAPI_HARMONY_DEMO_STYLEINVALIDATOR_H

#include "runtime/cssom/CSSStyleRule.h"
#include "runtime/cssom/RuleFeature.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    class Invalidator {
        public:
        Invalidator() = default;

        // 标注节点样式更新
        static void markElementUpdateStyle(const std::shared_ptr<TaroDOM::TaroElement>&, MatchElement);

        static bool invalidateIfNeeded(const std::shared_ptr<TaroDOM::TaroElement>&);

        static void invalidateStyleForDescendants(const std::shared_ptr<TaroDOM::TaroElement>&);
    };

} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_STYLEINVALIDATOR_H
