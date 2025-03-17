#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <napi/native_api.h>
#include <uv.h>

#include "AbstractTaskRunner.h"
#include "DefaultExceptionHandler.h"
#include "exports/napi_bridge.h"

namespace TaroThread {

class NapiTaskRunner : public AbstractTaskRunner {
    public:
    NapiTaskRunner(
        napi_env env,
        ExceptionHandler exceptionHandler = defaultExceptionHandler);
    ~NapiTaskRunner() override;

    NapiTaskRunner(const NapiTaskRunner&) = delete;
    NapiTaskRunner& operator=(const NapiTaskRunner&) = delete;

    void runAsyncTask(Task&& task) override;
    void runSyncTask(Task&& task) override;

    bool isOnCurrentThread() const override;

    void setExceptionHandler(ExceptionHandler handler) override;

    private:
    napi_env env;
    uv_loop_t* getLoop() const;

    uv_async_t asyncHandle;
    std::mutex tasksMutex;
    std::queue<Task> tasksQueue;
    std::thread::id threadId;
    std::condition_variable cv;
    std::shared_ptr<std::atomic_bool> running =
        std::make_shared<std::atomic_bool>(true);
    ExceptionHandler exceptionHandler;
};

} // namespace TaroThread
