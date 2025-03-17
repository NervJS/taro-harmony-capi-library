//
// Created on 2024/6/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "thread/TaskExecutor.h"

class MessageQueueThreadInterface {
    public:
    virtual ~MessageQueueThreadInterface() {}
    virtual void runOnQueue(std::function<void()>&&) = 0;
    // runOnQueueSync and quitSynchronous are dangerous.  They should only be
    // used for initialization and cleanup.
    virtual void runOnQueueSync(std::function<void()>&&) = 0;
    // Once quitSynchronous() returns, no further work should run on the queue.
    virtual void quitSynchronous() = 0;
};

class MessageQueueThread : public MessageQueueThreadInterface {
    public:
    MessageQueueThread(std::shared_ptr<TaroThread::TaskExecutor> const& taskExecutor);
    virtual ~MessageQueueThread();

    void runOnQueue(std::function<void()>&& func) override;

    void runOnQueueSync(std::function<void()>&& func) override;

    void quitSynchronous() override;

    private:
    std::shared_ptr<TaroThread::TaskExecutor> taskExecutor;
};
