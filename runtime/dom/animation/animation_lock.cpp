/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "animation_lock.h"

namespace TaroRuntime {
namespace TaroAnimate {

std::mutex& AnimationLock::getAnimationLock() {
    static std::mutex lock;
    return lock;
}

} // namespace TaroAnimate
} // namespace TaroRuntime
