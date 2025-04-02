/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once
#include <array>
#include <memory>
#include <optional>
#include <napi/native_api.h>
#include <uv.h>

#include "AbstractTaskRunner.h"
#include "exports/napi_bridge.h"
#include "qos/qos.h"

namespace TaroThread {

enum TaskThread {
    MAIN = 0,   // main thread running the eTS event loop
    JS,         // React Native's JS runtime thread
    BACKGROUND, // background tasks queue
};

class TaskExecutor {
    public:
    using Task = AbstractTaskRunner::Task;
    using ExceptionHandler = AbstractTaskRunner::ExceptionHandler;
    using Shared = std::shared_ptr<TaskExecutor>;
    using Weak = std::weak_ptr<TaskExecutor>;

    TaskExecutor(napi_env mainEnv, bool shouldEnableBackground = false);

    void runTask(TaskThread thread, Task&& task);
    void runSyncTask(TaskThread thread, Task&& task);
    void ensureRunOnMainTask(Task&& task, bool isSync = false);

    bool isOnTaskThread(TaskThread thread) const;

    std::optional<TaskThread> getCurrentTaskThread() const;

    void setExceptionHandler(ExceptionHandler handler);

    private:
    void setTaskThreadPriority(QoS_Level);
    std::array<std::shared_ptr<AbstractTaskRunner>, TaskThread::BACKGROUND + 1>
        m_taskRunners;
    std::array<std::optional<TaskThread>, TaskThread::BACKGROUND + 1>
        m_waitsOnThread;
};

} // namespace TaroThread
