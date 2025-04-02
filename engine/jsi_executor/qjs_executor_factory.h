/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#pragma once

#include "engine/quickjs_jsi/qjs_runtime_config.h"
#include "jsi_executor.h"

class QuickJSExecutorFactory : public JSExecutorFactory {
    public:
    explicit QuickJSExecutorFactory(
        JSIExecutor::RuntimeInstaller runtimeInstaller,
        const JSIScopedTimeoutInvoker &timeoutInvoker,
        std::unique_ptr<QuickJSRuntimeConfig> config)
        : runtimeInstaller_(runtimeInstaller),
          timeoutInvoker_(timeoutInvoker),
          config_(std::move(config)) {
        assert(timeoutInvoker_ && "Should not have empty timeoutInvoker");
    }

    std::unique_ptr<JSExecutor> createJSExecutor(
        std::shared_ptr<ExecutorDelegate> delegate,
        std::shared_ptr<MessageQueueThread> jsQueue) override;

    private:
    JSIExecutor::RuntimeInstaller runtimeInstaller_;
    JSIScopedTimeoutInvoker timeoutInvoker_;
    std::unique_ptr<QuickJSRuntimeConfig> config_;
};

class QuickJSExecutor : public JSIExecutor {
    public:
    QuickJSExecutor(
        std::shared_ptr<facebook::jsi::Runtime> runtime,
        std::shared_ptr<ExecutorDelegate> delegate,
        std::shared_ptr<MessageQueueThread> jsQueue,
        const JSIScopedTimeoutInvoker &timeoutInvoker,
        RuntimeInstaller runtimeInstaller);

    private:
    JSIScopedTimeoutInvoker timeoutInvoker_;
};
