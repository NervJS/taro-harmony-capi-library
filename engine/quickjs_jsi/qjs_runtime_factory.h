//
// Created on 2024/6/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once

#include <jsi/jsi.h>
#include <memory.h>

#include "engine/react_common/message_queue_thread.h"
#include "qjs_runtime_config.h"

std::unique_ptr<facebook::jsi::Runtime> createQuickJSRuntime(std::unique_ptr<QuickJSRuntimeConfig> config,
                                                             std::shared_ptr<MessageQueueThread> jsQueue);

std::unique_ptr<facebook::jsi::Runtime> createSharedQuickJSRuntime(const facebook::jsi::Runtime *sharedRuntime,
                                                                   std::unique_ptr<QuickJSRuntimeConfig> config);
