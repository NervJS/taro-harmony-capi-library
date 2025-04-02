/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#pragma once

#include <jsi/jsi.h>
#include <memory.h>

#include "engine/react_common/message_queue_thread.h"
#include "qjs_runtime_config.h"

std::unique_ptr<facebook::jsi::Runtime> createQuickJSRuntime(std::unique_ptr<QuickJSRuntimeConfig> config,
                                                             std::shared_ptr<MessageQueueThread> jsQueue);

std::unique_ptr<facebook::jsi::Runtime> createSharedQuickJSRuntime(const facebook::jsi::Runtime* sharedRuntime,
                                                                   std::unique_ptr<QuickJSRuntimeConfig> config);
