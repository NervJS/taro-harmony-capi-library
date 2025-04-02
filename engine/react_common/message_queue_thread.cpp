/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

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
