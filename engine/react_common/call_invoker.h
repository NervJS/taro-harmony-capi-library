//
// Created on 2024/6/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
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
    virtual void invokeAsync(CallFunc &&func) = 0;
    virtual void invokeAsync(SchedulerPriority /*priority*/, CallFunc &&func) {
        // When call with priority is not implemented, fall back to a regular async
        // execution
        invokeAsync(std::move(func));
    }
    virtual void invokeSync(CallFunc &&func) = 0;
    virtual ~CallInvoker() {}
};
