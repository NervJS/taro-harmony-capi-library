/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#include "js_executor.h"

std::string JSExecutor::getSyntheticBundlePath(uint32_t bundleId, const std::string& bundlePath) {
    return folly::to<std::string>("seg-", bundleId, ".js");
}

double JSExecutor::performanceNow() {
    auto time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(time.time_since_epoch()).count();

    constexpr double NANOSECONDS_IN_MILLISECOND = 1000000.0;
    return duration / NANOSECONDS_IN_MILLISECOND;
}
