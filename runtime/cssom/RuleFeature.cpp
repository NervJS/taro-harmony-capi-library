//
// Created on 2024/5/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "runtime/cssom/RuleFeature.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    RuleFeature::RuleFeature(MatchElement matchElement)
        : matchElement_(matchElement) {}
} // namespace TaroCSSOM
} // namespace TaroRuntime
