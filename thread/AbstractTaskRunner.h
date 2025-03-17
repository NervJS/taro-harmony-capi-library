#pragma once

#include <exception>
#include <functional>

class AbstractTaskRunner {
    public:
    using Task = std::function<void()>;
    using ExceptionHandler = std::function<void(std::exception_ptr const)>;

    virtual void runAsyncTask(Task&& task) = 0;
    virtual void runSyncTask(Task&& task) = 0;

    virtual bool isOnCurrentThread() const = 0;

    virtual void setExceptionHandler(ExceptionHandler handler) = 0;

    virtual ~AbstractTaskRunner() = default;
};
