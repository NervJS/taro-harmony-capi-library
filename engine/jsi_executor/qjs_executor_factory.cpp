/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#include "qjs_executor_factory.h"

#include <thread>

#include "engine/quickjs_jsi/qjs_runtime_factory.h"
#include "engine/react_common/message_queue_thread.h"
#include "engine/react_common/systrace_section.h"

namespace {

std::unique_ptr<facebook::jsi::Runtime> makeQuickJSRuntimeSystraced(std::unique_ptr<QuickJSRuntimeConfig> config,
                                                                    std::shared_ptr<MessageQueueThread> jsQueue) {
    SystraceSection s("QuickJSExecutorFactory::makeQuickJSRuntimeSystraced");

    return createQuickJSRuntime(std::move(config), jsQueue);
}

} // namespace

std::unique_ptr<JSExecutor>
QuickJSExecutorFactory::createJSExecutor(std::shared_ptr<ExecutorDelegate> delegate,
                                         std::shared_ptr<MessageQueueThread> jsQueue) {
    std::unique_ptr<facebook::jsi::Runtime> quickJSRuntime = makeQuickJSRuntimeSystraced(std::move(config_), jsQueue);

    // Add js engine information to Error.prototype so in error reporting we
    // can send this information.
    auto errorPrototype =
        quickJSRuntime->global().getPropertyAsObject(*quickJSRuntime, "Error").getPropertyAsObject(*quickJSRuntime, "prototype");
    errorPrototype.setProperty(*quickJSRuntime, "jsEngine", "quickJS");

    return std::make_unique<QuickJSExecutor>(std::move(quickJSRuntime), delegate, jsQueue, timeoutInvoker_, runtimeInstaller_);
}

QuickJSExecutor::QuickJSExecutor(std::shared_ptr<facebook::jsi::Runtime> runtime, std::shared_ptr<ExecutorDelegate> delegate,
                                 std::shared_ptr<MessageQueueThread> jsQueue,
                                 const JSIScopedTimeoutInvoker &timeoutInvoker, RuntimeInstaller runtimeInstaller)
    : JSIExecutor(runtime, delegate, timeoutInvoker, runtimeInstaller) {}
