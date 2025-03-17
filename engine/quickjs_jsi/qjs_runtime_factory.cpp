//
// Created on 2024/6/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "qjs_runtime_factory.h"

#include "qjs_runtime.h"

std::unique_ptr<facebook::jsi::Runtime> createQuickJSRuntime(std::unique_ptr<QuickJSRuntimeConfig> config,
                                                             std::shared_ptr<MessageQueueThread> jsQueue) {
    return std::make_unique<QuickJSRuntime>(std::move(config), jsQueue);
}

std::unique_ptr<facebook::jsi::Runtime> createSharedQuickJSRuntime(const facebook::jsi::Runtime *sharedRuntime,
                                                                   std::unique_ptr<QuickJSRuntimeConfig> config) {
    auto *sharedQuickJSRuntime = dynamic_cast<const QuickJSRuntime *>(sharedRuntime);
    return std::make_unique<QuickJSRuntime>(sharedQuickJSRuntime, std::move(config));
}
