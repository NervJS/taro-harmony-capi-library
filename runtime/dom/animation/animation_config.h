/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <string>

#include "runtime/dom/animation/animator/animation.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroAnimationConfig {
        public:
        static TaroAnimationFillMode parseFillMode(const std::string &fill_mode);
        static TaroAnimationDirection parseDirection(const std::string &direction);
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
