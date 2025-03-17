#include "animation_lock.h"

namespace TaroRuntime {
namespace TaroAnimate {

std::mutex& AnimationLock::getAnimationLock() {
    static std::mutex lock;
    return lock;
}

} // namespace TaroAnimate
} // namespace TaroRuntime
