/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <functional>
#include <list>
#include <thread>
#include <glog/logging.h>

#include "engine/jsi_executor/jsi_executor.h"
#include "engine/react_common/native_to_js_bridge.h"
#include "exports/napi_bridge.h"
#include "folly/dynamic.h"
#include "native_logger.h"
#include "quickjs.h"
#include "thread/TaskExecutor.h"

struct InstanceCallback {
    virtual ~InstanceCallback() {}
    virtual void onBatchComplete() {}
    virtual void incrementPendingJSCalls() {}
    virtual void decrementPendingJSCalls() {}
};

class TaroInstance : std::enable_shared_from_this<TaroInstance> {
    public:
    TaroInstance(int id, napi_env env, bool shouldEnableBackgroundExecutor)
        : m_id(id), taskExecutor(std::make_shared<TaroThread::TaskExecutor>(env, shouldEnableBackgroundExecutor)) {};

    ~TaroInstance() {};

    std::shared_ptr<TaroThread::TaskExecutor> taskExecutor;

    void start();
    void initialize();
    void initializeBridge(
        std::unique_ptr<InstanceCallback> callback,
        std::shared_ptr<JSExecutorFactory> jsef,
        std::shared_ptr<MessageQueueThread> jsQueue,
        std::shared_ptr<ModuleRegistry> moduleRegistry);

    std::shared_ptr<TaroThread::TaskExecutor> getTaskExecutor();

    void loadScript(
        std::vector<uint8_t> &&bundle,
        std::string const sourceURL,
        std::function<void(const std::string)> &&onFinish);

    void loadScriptFromString(std::unique_ptr<const JSBigString> string, std::string sourceURL, bool loadSynchronously);

    protected:
    std::shared_ptr<MessageQueueThread> m_jsQueue;

    private:
    int m_id;
    std::shared_ptr<InstanceCallback> callback_;
    std::shared_ptr<NativeToJsBridge> nativeToJsBridge_;
    std::shared_ptr<ModuleRegistry> moduleRegistry_;
    void callNativeModules(folly::dynamic &&calls, bool isEndOfBatch);
    void loadBundle(std::unique_ptr<const JSBigString> startupScript, std::string startupScriptSourceURL);
    void loadBundleSync(std::unique_ptr<const JSBigString> startupScript, std::string startupScriptSourceURL);
    std::mutex m_syncMutex;
    std::condition_variable m_syncCV;
    bool m_syncReady = false;

    class JSCallInvoker : public CallInvoker {
        private:
        std::weak_ptr<NativeToJsBridge> m_nativeToJsBridge;
        std::mutex m_mutex;
        bool m_shouldBuffer = true;
        std::list<std::function<void()>> m_workBuffer;

        void scheduleAsync(std::function<void()> &&work);

        public:
        void setNativeToJsBridgeAndFlushCalls(
            std::weak_ptr<NativeToJsBridge> nativeToJsBridge);
        void invokeAsync(std::function<void()> &&work) override;
        void invokeSync(std::function<void()> &&work) override;
    };
    std::shared_ptr<JSCallInvoker> jsCallInvoker_ = std::make_shared<JSCallInvoker>();
};
