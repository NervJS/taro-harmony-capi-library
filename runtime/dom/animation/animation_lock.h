/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <mutex>
namespace TaroRuntime {
namespace TaroAnimate {
class AnimationLock{
 public:
    static std::mutex& getAnimationLock();
};
} // namespace TaroAnimate
} // namespace TaroRuntime
