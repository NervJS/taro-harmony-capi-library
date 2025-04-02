/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "NapiTaskRunner.h"

#include <atomic>
#include <napi/native_api.h>
#include <uv.h>

#include "arkjs/Scope.h"
#include "helper/TaroLog.h"

namespace TaroThread {

NapiTaskRunner::NapiTaskRunner(napi_env env, ExceptionHandler exceptionHandler)
    : env(env),
      exceptionHandler(std::move(exceptionHandler)) {
    // NOTE: let's hope the JS runtime doesn't move between system threads...
    threadId = std::this_thread::get_id();
    auto loop = getLoop();
    asyncHandle.data = static_cast<void*>(this);
    // 初始化异步句柄 asyncHandle，在调用 uv_async_send 后，主线程会在事件循环合适的时机里调用 uv_async_init 中传入的回调函数
    uv_async_init(loop, &asyncHandle, [](auto handle) {
        auto runner = static_cast<NapiTaskRunner*>(handle->data);

        // https://nodejs.org/api/n-api.html#napi_handle_scope
        // "For any invocations of code outside the execution of a native method
        // (...) the module is required to create a scope before invoking any
        // functions that can result in the creation of JavaScript values"
        NapiHandleScopeWrapper wrapper(runner->env);

        std::queue<Task> tasksQueue;
        {
            std::unique_lock<std::mutex> lock(runner->tasksMutex);
            std::swap(tasksQueue, runner->tasksQueue);
        }
        while (!tasksQueue.empty()) {
            auto task = std::move(tasksQueue.front());
            tasksQueue.pop();
            try {
                task();
            } catch (std::exception const& e) {
                runner->exceptionHandler(std::current_exception());
            }
        }
    });
}

NapiTaskRunner::~NapiTaskRunner() {
    running->store(false);
    cv.notify_all();
    uv_close(reinterpret_cast<uv_handle_t*>(&asyncHandle), nullptr);
}

void NapiTaskRunner::runAsyncTask(Task&& task) {
    std::unique_lock<std::mutex> lock(tasksMutex);
    tasksQueue.push(task);
    uv_async_send(&asyncHandle);
}

void NapiTaskRunner::runSyncTask(Task&& task) {
    if (isOnCurrentThread()) {
        task();
        return;
    }
    std::unique_lock<std::mutex> lock(tasksMutex);
    std::atomic_bool done{false};
    tasksQueue.push([this, &done, task = std::move(task)]() {
        task();
        done = true;
        cv.notify_all();
    });
    uv_async_send(&asyncHandle);
    cv.wait(lock, [running = this->running, &done] {
        return !(running->load()) || done.load();
    });
}

bool NapiTaskRunner::isOnCurrentThread() const {
    return threadId == std::this_thread::get_id();
}

void NapiTaskRunner::setExceptionHandler(ExceptionHandler handler) {
    exceptionHandler = std::move(handler);
}

uv_loop_t* NapiTaskRunner::getLoop() const {
    uv_loop_t* loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    return loop;
}

} // namespace TaroThread
