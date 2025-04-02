/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#pragma once

#include <functional>
#include <memory>

#include "scheduler_priority.h"

using CallFunc = std::function<void()>;

/**
 * An interface for a generic native-to-JS call invoker. See BridgeJSCallInvoker
 * for an implementation.
 */
class CallInvoker {
    public:
    virtual void invokeAsync(CallFunc&& func) = 0;
    virtual void invokeAsync(SchedulerPriority /*priority*/, CallFunc&& func) {
        // When call with priority is not implemented, fall back to a regular async
        // execution
        invokeAsync(std::move(func));
    }
    virtual void invokeSync(CallFunc&& func) = 0;
    virtual ~CallInvoker() {}
};
