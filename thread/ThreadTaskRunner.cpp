#include "ThreadTaskRunner.h"

#include <atomic>
#include <exception>

#include "helper/TaroLog.h"

namespace TaroThread {

ThreadTaskRunner::ThreadTaskRunner(
    std::string name,
    ExceptionHandler exceptionHandler)
    : name(name), exceptionHandler(std::move(exceptionHandler)) {
    thread = std::thread([this] { runLoop(); });
    // 获取 handle 用于设置线程名字

    auto handle = thread.native_handle();
    pthread_setname_np(handle, name.c_str());
}

ThreadTaskRunner::~ThreadTaskRunner() {
    TARO_LOG_INFO("TARO_THREAD", "Shutting down thread runner %{public}s", name.c_str());
    running = false;
    cv.notify_all();
    thread.join();
}

void ThreadTaskRunner::runAsyncTask(Task&& task) {
    {
        std::unique_lock<std::mutex> lock(mutex);
        asyncTaskQueue.emplace(std::move(task));
    }
    // NOTE: this should be fine:
    // the only threads waiting on the condition variable are the
    // runner thread and the thread that called runSyncTask,
    // and if some thread is waiting in runSyncTask,
    // the runner thread should be running and executing its task
    // 如果当前线程不是 JS 线程，则通知 JS 线程继续执行 runLoop 中的逻辑
    // 如果当前线程是 JS 线程，则直接塞入 asyncTaskQueue 中，等待下一次 runLoop 执行
    if (std::this_thread::get_id() != thread.get_id()) {
        cv.notify_one();
    }
}

void ThreadTaskRunner::runSyncTask(Task&& task) {
    // 如果调用线程就是当前的 JS 线程，则直接运行
    if (isOnCurrentThread()) {
        task();
        return;
    }
    // 否则，则塞入队列 syncTaskQueue 中，等待调度
    std::unique_lock<std::mutex> lock(mutex);
    std::atomic_bool done{false};
    syncTaskQueue.emplace([task = std::move(task), &done] {
        task();
        done = true;
    });
    cv.notify_all();
    // JS 线程调度完该任务后，会通过下面的条件变量释放调用线程，同步任务在 runLoop 中执行完会调用 notify_all 来释放下面的线程堵塞
    cv.wait(lock, [this, &done] { return !running.load() || done.load(); });
}

bool ThreadTaskRunner::isOnCurrentThread() const {
    return std::this_thread::get_id() == thread.get_id();
}

void ThreadTaskRunner::setExceptionHandler(ExceptionHandler handler) {
    exceptionHandler = std::move(handler);
}

void ThreadTaskRunner::runLoop() {
    while (running) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return hasPendingTasks() || !running; });
        if (!running) {
            return;
        }
        if (!syncTaskQueue.empty()) {
            auto task = std::move(syncTaskQueue.front());
            syncTaskQueue.pop();
            // 在 runLoop 方法中，锁的主要目的是保护任务队列的访问和修改。
            // 当任务从队列中取出后，已经不再需要访问共享数据，因此可以立即释放锁，让其他线程能够继续访问和修改队列。
            lock.unlock();
            try {
                task();
            } catch (std::exception const& e) {
                exceptionHandler(std::current_exception());
            }
            // notify the threads that called runSyncTask.
            // it's not enough to notify one thread,
            // because there could be multiple threads calling runSyncTask
            // at the same time
            cv.notify_all();
            continue;
        }
        if (!asyncTaskQueue.empty()) {
            auto task = std::move(asyncTaskQueue.front());
            asyncTaskQueue.pop();
            lock.unlock();
            try {
                task();
            } catch (std::exception const& e) {
                exceptionHandler(std::current_exception());
            }
        }
    }
}

} // namespace TaroThread
