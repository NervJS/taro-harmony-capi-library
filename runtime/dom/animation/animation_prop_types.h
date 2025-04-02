/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <optional>
#include <vector>
#include <stdint.h>

namespace TaroRuntime {
namespace TaroAnimate {
    class AnimPropType_Color {
        public:
        uint32_t value_ = 0;
    };

    class AnimPropType_MultiValue {
        public:
        std::vector<std::optional<double>> values_;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
