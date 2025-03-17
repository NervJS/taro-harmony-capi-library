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
