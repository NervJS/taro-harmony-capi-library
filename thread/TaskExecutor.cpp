/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "TaskExecutor.h"

#include "folly/lang/Exception.h"
#include "NapiTaskRunner.h"
#include "ThreadTaskRunner.h"
#include "helper/TaroLog.h"
#include "qos/qos.h"

namespace TaroThread {

TaskExecutor::TaskExecutor(napi_env mainEnv, bool shouldEnableBackground) {
    auto mainTaskRunner = std::make_shared<NapiTaskRunner>(mainEnv);
    auto jsTaskRunner = std::make_shared<ThreadTaskRunner>("TARO_JS");
    auto backgroundExecutor = shouldEnableBackground ? std::make_shared<ThreadTaskRunner>("TARO_BACKGROUND") : nullptr;
    m_taskRunners = {mainTaskRunner, jsTaskRunner, backgroundExecutor};
    this->runTask(TaskThread::JS, [this]() {
        this->setTaskThreadPriority(QoS_Level::QOS_USER_INTERACTIVE);
    });
    if (shouldEnableBackground) {
        this->runTask(TaskThread::BACKGROUND, [this]() {
            this->setTaskThreadPriority(QoS_Level::QOS_USER_INTERACTIVE);
        });
    }
}

void TaskExecutor::setTaskThreadPriority(QoS_Level level) {
    int ret = OH_QoS_SetThreadQoS(level);
    std::array<char, 16> buffer = {0};
    pthread_getname_np(pthread_self(), buffer.data(), sizeof(buffer));

    TARO_LOG_INFO("TARO_THREAD", "TaskExecutor::setTaskThreadPriority %{public}s%{public}s", buffer.data(), (ret == 0 ? " SUCCESSFUL" : " FAILED"));
}

void TaskExecutor::runTask(TaskThread thread, Task&& task) {
    m_taskRunners[thread]->runAsyncTask(std::move(task));
}

void TaskExecutor::runSyncTask(TaskThread thread, Task&& task) {
    auto waitsOnThread = m_waitsOnThread[thread];
    if (waitsOnThread.has_value() && isOnTaskThread(waitsOnThread.value())) {
        throw std::runtime_error("Deadlock detected");
    }
    auto currentThread = getCurrentTaskThread();
    if (currentThread.has_value()) {
        m_waitsOnThread[currentThread.value()] = thread;
    }
    std::exception_ptr thrownError;
    m_taskRunners[thread]->runSyncTask([task = std::move(task), &thrownError]() {
        try {
            task();
        } catch (const std::exception& e) {
            thrownError = std::current_exception();
        }
    });
    if (thrownError) {
        std::rethrow_exception(thrownError);
    }
    if (currentThread.has_value()) {
        m_waitsOnThread[currentThread.value()] = std::nullopt;
    }
}

void TaskExecutor::ensureRunOnMainTask(Task&& task, bool isSync) {
    if (isOnTaskThread(TaskThread::MAIN)) {
        task();
        return;
    }
#if IS_DEBUG
    folly::throw_exception(std::runtime_error("TaskExecutor: operation must be called on main thread"));
#else
    TARO_LOG_ERROR("TaskExecutor", "operation must be called on main thread");
#endif
    if (isSync) {
        runSyncTask(TaskThread::MAIN, std::move(task));
    } else {
        runTask(TaskThread::MAIN, std::move(task));
    }
}

bool TaskExecutor::isOnTaskThread(TaskThread thread) const {
    auto runner = m_taskRunners[thread];
    return runner && runner->isOnCurrentThread();
}

std::optional<TaskThread> TaskExecutor::getCurrentTaskThread() const {
    if (isOnTaskThread(TaskThread::MAIN)) {
        return TaskThread::MAIN;
    } else if (isOnTaskThread(TaskThread::JS)) {
        return TaskThread::JS;
    } else if (isOnTaskThread(TaskThread::BACKGROUND)) {
        return TaskThread::BACKGROUND;
    } else {
        return std::nullopt;
    }
}

void TaskExecutor::setExceptionHandler(ExceptionHandler handler) {
    for (auto& taskRunner : m_taskRunners) {
        if (taskRunner) {
            taskRunner->setExceptionHandler(handler);
        }
    }
}

} // namespace TaroThread
