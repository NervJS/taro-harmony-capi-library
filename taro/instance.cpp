/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "instance.h"

#include "engine/jsi_executor/qjs_executor_factory.h"
#include "engine/quickjs_jsi/qjs_runtime.h"
#include "engine/quickjs_jsi/qjs_runtime_config.h"
#include "engine/react_common/systrace_section.h"

void TaroInstance::start() {
    this->initialize();
}

void TaroInstance::initialize() {
    std::vector<std::unique_ptr<NativeModule>> modules;
    auto instanceCallback = std::make_unique<InstanceCallback>();
    auto config = std::make_unique<QuickJSRuntimeConfig>();
    auto jsExecutorFactory = std::make_shared<QuickJSExecutorFactory>(
        // 执行 jsi runtime 相关初始化逻辑，在执行 js 代码前，比如添加 log 和 trace 等功能
        [](facebook::jsi::Runtime& rt) {
            bindNativeLogger(rt, nativeLogger);
        },
        JSIExecutor::defaultTimeoutInvoker,
        std::move(config));
    m_jsQueue = std::make_shared<MessageQueueThread>(this->taskExecutor);
    auto moduleRegistry = std::make_shared<ModuleRegistry>(std::move(modules));
    this->initializeBridge(
        std::move(instanceCallback),
        std::move(jsExecutorFactory),
        m_jsQueue,
        std::move(moduleRegistry));
};

void TaroInstance::initializeBridge(
    std::unique_ptr<InstanceCallback> callback,
    std::shared_ptr<JSExecutorFactory> jsef,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<ModuleRegistry> moduleRegistry) {
    callback_ = std::move(callback);
    moduleRegistry_ = std::move(moduleRegistry);
    jsQueue->runOnQueueSync([this, &jsef, jsQueue]() mutable {
        nativeToJsBridge_ = std::make_shared<NativeToJsBridge>(jsef.get(), moduleRegistry_, jsQueue, callback_);

        nativeToJsBridge_->initializeRuntime();
        /**
         * After NativeToJsBridge is created, the jsi::Runtime should exist.
         * Also, the JS message queue thread exists. So, it's safe to
         * schedule all queued up js Calls.
         */
        jsCallInvoker_->setNativeToJsBridgeAndFlushCalls(nativeToJsBridge_);

        std::lock_guard<std::mutex> lock(m_syncMutex);
        m_syncReady = true;
        m_syncCV.notify_all();
    });
    CHECK(nativeToJsBridge_);
};

std::shared_ptr<TaroThread::TaskExecutor> TaroInstance::getTaskExecutor() {
    return taskExecutor;
};

void TaroInstance::loadScript(
    std::vector<uint8_t>&& bundle,
    std::string const sourceURL,
    std::function<void(const std::string)>&& onFinish) {
    std::weak_ptr<TaroInstance> weakThis = shared_from_this();
    this->taskExecutor->runTask(TaroThread::TaskThread::JS,
                                [weakThis, bundle = std::move(bundle), sourceURL, onFinish = std::move(onFinish)]() mutable {
                                    if (auto self = weakThis.lock()) {
                                        std::unique_ptr<JSBigBufferString> jsBundle;
                                        jsBundle = std::make_unique<JSBigBufferString>(bundle.size());
                                        memcpy(jsBundle->data(), bundle.data(), bundle.size());
                                        try {
                                            self->loadScriptFromString(std::move(jsBundle), sourceURL, true);
                                            onFinish("");
                                        } catch (std::exception const& e) {
                                            try {
                                                std::rethrow_if_nested(e);
                                                onFinish(e.what());
                                            } catch (const std::exception& nested) {
                                                onFinish(e.what() + std::string("\n") + nested.what());
                                            }
                                        }
                                    }
                                });
}

void TaroInstance::loadScriptFromString(std::unique_ptr<const JSBigString> string, std::string sourceURL,
                                        bool loadSynchronously) {
    SystraceSection s("Instance::loadScriptFromString", "sourceURL", sourceURL);
    if (loadSynchronously) {
        loadBundleSync(std::move(string), std::move(sourceURL));
    } else {
        loadBundle(std::move(string), std::move(sourceURL));
    }
}

void TaroInstance::loadBundleSync(std::unique_ptr<const JSBigString> string, std::string sourceURL) {
    std::unique_lock<std::mutex> lock(m_syncMutex);
    m_syncCV.wait(lock, [this] {
        return m_syncReady;
    });

    SystraceSection s("Instance::loadBundleSync", "sourceURL", sourceURL);
    nativeToJsBridge_->loadBundleSync(std::move(string), std::move(sourceURL));
}

void TaroInstance::loadBundle(std::unique_ptr<const JSBigString> string, std::string sourceURL) {
    callback_->incrementPendingJSCalls();
    SystraceSection s("Instance::loadBundle", "sourceURL", sourceURL);
    nativeToJsBridge_->loadBundle(std::move(string), std::move(sourceURL));
}

void TaroInstance::JSCallInvoker::setNativeToJsBridgeAndFlushCalls(std::weak_ptr<NativeToJsBridge> nativeToJsBridge) {
    std::lock_guard<std::mutex> guard(m_mutex);

    m_shouldBuffer = false;
    m_nativeToJsBridge = nativeToJsBridge;
    while (m_workBuffer.size() > 0) {
        scheduleAsync(std::move(m_workBuffer.front()));
        m_workBuffer.pop_front();
    }
}

void TaroInstance::JSCallInvoker::invokeSync(std::function<void()>&& work) {
    // TODO: Replace JS Callinvoker with RuntimeExecutor.
    throw std::runtime_error("Synchronous native -> JS calls are currently not supported.");
}

void TaroInstance::JSCallInvoker::invokeAsync(std::function<void()>&& work) {
    std::lock_guard<std::mutex> guard(m_mutex);
    /**
     * Why is is necessary to queue up async work?
     *
     * 1. TurboModuleManager must be created synchronously after the Instance,
     *    before we load the source code. This is when the NativeModule system
     *    is initialized. RCTDevLoadingView shows bundle download progress.
     * 2. TurboModuleManager requires a JS CallInvoker.
     * 3. The JS CallInvoker requires the NativeToJsBridge, which is created on
     *    the JS thread in Instance::initializeBridge.
     *
     * Therefore, although we don't call invokeAsync before the JS bundle is
     * executed, this buffering is implemented anyways to ensure that work
     * isn't discarded.
     */
    if (m_shouldBuffer) {
        m_workBuffer.push_back(std::move(work));
        return;
    }

    scheduleAsync(std::move(work));
}

void TaroInstance::JSCallInvoker::scheduleAsync(std::function<void()>&& work) {
    if (auto strongNativeToJsBridge = m_nativeToJsBridge.lock()) {
        strongNativeToJsBridge->runOnExecutorQueue([work = std::move(work)](JSExecutor* executor) {
            work();
            executor->flush();
        });
    }
}
