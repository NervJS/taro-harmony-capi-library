//
// Created on 2024/6/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <jsi/jsi.h>

inline static void handleJSError(facebook::jsi::Runtime &runtime, const facebook::jsi::JSError &error, bool isFatal) {
    auto errorUtils = runtime.global().getProperty(runtime, "ErrorUtils");
    if (errorUtils.isUndefined() || !errorUtils.isObject() ||
        !errorUtils.getObject(runtime).hasProperty(runtime, "reportFatalError") ||
        !errorUtils.getObject(runtime).hasProperty(runtime, "reportError")) {
        // ErrorUtils was not set up. This probably means the bundle didn't
        // load properly.
        throw facebook::jsi::JSError(runtime,
                                     "ErrorUtils is not set up properly. Something probably went wrong trying to load the JS "
                                     "bundle. Trying to report error " +
                                         error.getMessage(),
                                     error.getStack());
    }

    // TODO(janzer): Rewrite this function to return the processed error
    // instead of just reporting it through the native module
    if (isFatal) {
        auto func = errorUtils.asObject(runtime).getPropertyAsFunction(runtime, "reportFatalError");

        func.call(runtime, error.value());
    } else {
        auto func = errorUtils.asObject(runtime).getPropertyAsFunction(runtime, "reportError");

        func.call(runtime, error.value());
    }
}
