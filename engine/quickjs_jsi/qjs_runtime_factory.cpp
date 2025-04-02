/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#include "qjs_runtime_factory.h"

#include "qjs_runtime.h"

std::unique_ptr<facebook::jsi::Runtime> createQuickJSRuntime(std::unique_ptr<QuickJSRuntimeConfig> config,
                                                             std::shared_ptr<MessageQueueThread> jsQueue) {
    return std::make_unique<QuickJSRuntime>(std::move(config), jsQueue);
}

std::unique_ptr<facebook::jsi::Runtime> createSharedQuickJSRuntime(const facebook::jsi::Runtime* sharedRuntime,
                                                                   std::unique_ptr<QuickJSRuntimeConfig> config) {
    auto* sharedQuickJSRuntime = dynamic_cast<const QuickJSRuntime*>(sharedRuntime);
    return std::make_unique<QuickJSRuntime>(sharedQuickJSRuntime, std::move(config));
}
