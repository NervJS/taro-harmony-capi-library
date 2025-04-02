/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <mutex>
#include <queue>

namespace TaroHelper {
template <typename T>
class SyncQueue {
    public:
    void enqueue(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(item);
        cond_var_.notify_one(); // 通知等待的线程
    }

    T dequeue() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [this] {
            return !queue_.empty();
        }); // 等待队列非空
        T item = queue_.front();
        queue_.pop();
        return item;
    }

    private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_var_;
};
} // namespace TaroHelper
