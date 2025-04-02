/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "AbstractTaskRunner.h"
#include "DefaultExceptionHandler.h"

namespace TaroThread {

class ThreadTaskRunner : public AbstractTaskRunner {
    public:
    ThreadTaskRunner(
        std::string name,
        ExceptionHandler exceptionHandler = defaultExceptionHandler);
    ~ThreadTaskRunner() override;

    ThreadTaskRunner(const ThreadTaskRunner&) = delete;
    ThreadTaskRunner& operator=(const ThreadTaskRunner&) = delete;

    void runAsyncTask(Task&& task) override;
    void runSyncTask(Task&& task) override;

    bool isOnCurrentThread() const override;

    void setExceptionHandler(ExceptionHandler handler) override;

    private:
    void runLoop();

    bool hasPendingTasks() const {
        return !asyncTaskQueue.empty() || !syncTaskQueue.empty();
    }

    std::string name;
    std::atomic_bool running{true};
    std::thread thread;
    std::queue<std::function<void()>> asyncTaskQueue;
    std::queue<std::function<void()>> syncTaskQueue;
    std::mutex mutex;
    std::condition_variable cv;
    ExceptionHandler exceptionHandler;
};

} // namespace TaroThread
