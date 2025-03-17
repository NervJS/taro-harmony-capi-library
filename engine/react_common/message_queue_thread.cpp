//
// Created on 2024/6/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "message_queue_thread.h"

MessageQueueThread::MessageQueueThread(std::shared_ptr<TaroThread::TaskExecutor> const &taskExecutor)
    : taskExecutor(taskExecutor) {}

MessageQueueThread::~MessageQueueThread() {};

void MessageQueueThread::runOnQueue(std::function<void()> &&func) {
    taskExecutor->runTask(TaroThread::TaskThread::JS, std::move(func));
}

void MessageQueueThread::runOnQueueSync(std::function<void()> &&func) {
    taskExecutor->runSyncTask(TaroThread::TaskThread::JS, std::move(func));
}

void MessageQueueThread::quitSynchronous() {
    // TODO!
}
