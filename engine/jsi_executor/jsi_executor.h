/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#pragma once

#include <functional>
#include <mutex>
#include <optional>
#include <jsi/jsi.h>

#include "engine/react_common/js_big_string.h"
#include "engine/react_common/js_executor.h"
#include "jsi_native_modules.h"

using JSIScopedTimeoutInvoker = std::function<void(
    const std::function<void()> &invokee,
    std::function<std::string()> errorMessageProducer)>;

class BigStringBuffer : public facebook::jsi::Buffer {
    public:
    BigStringBuffer(std::unique_ptr<const JSBigString> script)
        : script_(std::move(script)) {}

    size_t size() const override {
        return script_->size();
    }

    const uint8_t *data() const override {
        return reinterpret_cast<const uint8_t *>(script_->c_str());
    }

    private:
    std::unique_ptr<const JSBigString> script_;
};

class JSIExecutor : public JSExecutor {
    public:
    using RuntimeInstaller = std::function<void(facebook::jsi::Runtime &runtime)>;

    JSIExecutor(
        std::shared_ptr<facebook::jsi::Runtime> runtime,
        std::shared_ptr<ExecutorDelegate> delegate,
        const JSIScopedTimeoutInvoker &timeoutInvoker,
        RuntimeInstaller runtimeInstaller);
    void initializeRuntime() override;
    void loadBundle(
        std::unique_ptr<const JSBigString> script,
        std::string sourceURL) override;
    // void setBundleRegistry(std::unique_ptr<RAMBundleRegistry>) override;
    void registerBundle(uint32_t bundleId, const std::string &bundlePath)
        override;
    void callFunction(
        const std::string &moduleId,
        const std::string &methodId,
        const folly::dynamic &arguments) override;
    void invokeCallback(const double callbackId, const folly::dynamic &arguments)
        override;
    void setGlobalVariable(
        std::string propName,
        std::unique_ptr<const JSBigString> jsonValue) override;
    std::string getDescription() override;
    void *getJavaScriptContext() override;
    bool isInspectable() override;
    void handleMemoryPressure(int pressureLevel) override;

    // An implementation of JSIScopedTimeoutInvoker that simply runs the
    // invokee, with no timeout.
    static void defaultTimeoutInvoker(
        const std::function<void()> &invokee,
        std::function<std::string()> errorMessageProducer) {
        (void)errorMessageProducer;
        invokee();
    }

    void flush() override;

    private:
    class NativeModuleProxy;

    void bindBridge();
    void callNativeModules(const facebook::jsi::Value &queue, bool isEndOfBatch);
    facebook::jsi::Value nativeCallSyncHook(const facebook::jsi::Value *args, size_t count);
    //   TODO: 目测不需要此方法
    //   facebook::jsi::Value nativeRequire(const facebook::jsi::Value *args, size_t count);
    facebook::jsi::Value globalEvalWithSourceUrl(const facebook::jsi::Value *args, size_t count);

    std::shared_ptr<facebook::jsi::Runtime> runtime_;
    std::shared_ptr<ExecutorDelegate> delegate_;
    std::shared_ptr<JSINativeModules> nativeModules_;
    std::shared_ptr<ModuleRegistry> moduleRegistry_;
    std::once_flag bindFlag_;
    // std::unique_ptr<RAMBundleRegistry> bundleRegistry_;
    JSIScopedTimeoutInvoker scopedTimeoutInvoker_;
    RuntimeInstaller runtimeInstaller_;

    std::optional<facebook::jsi::Function> callFunctionReturnFlushedQueue_;
    std::optional<facebook::jsi::Function> invokeCallbackAndReturnFlushedQueue_;
    std::optional<facebook::jsi::Function> flushedQueue_;
};

using Logger = std::function<void(const std::string &message, unsigned int logLevel)>;
void bindNativeLogger(facebook::jsi::Runtime &runtime, Logger logger);

void bindNativePerformanceNow(facebook::jsi::Runtime &runtime);

double performanceNow();
