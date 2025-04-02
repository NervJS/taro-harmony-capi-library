/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#include "jsi_executor.h"

#include <sstream>
#include <stdexcept>
#include <folly/Conv.h>
#include <folly/json.h>
#include <glog/logging.h>
#include <jsi/JSIDynamic.h>
#include <jsi/instrumentation.h>

#include "engine/react_common/error_utils.h"
#include "engine/react_common/js_big_string.h"
#include "engine/react_common/module_registry.h"
#include "engine/react_common/systrace_section.h"

class JSIExecutor::NativeModuleProxy : public facebook::jsi::HostObject {
    public:
    NativeModuleProxy(std::shared_ptr<JSINativeModules> nativeModules)
        : weakNativeModules_(nativeModules) {}

    facebook::jsi::Value get(facebook::jsi::Runtime &rt, const facebook::jsi::PropNameID &name) override {
        if (name.utf8(rt) == "name") {
            return facebook::jsi::String::createFromAscii(rt, "NativeModules");
        }

        auto nativeModules = weakNativeModules_.lock();
        if (!nativeModules) {
            return nullptr;
        }

        return nativeModules->getModule(rt, name);
    }

    void set(facebook::jsi::Runtime &, const facebook::jsi::PropNameID &, const facebook::jsi::Value &) override {
        throw std::runtime_error(
            "Unable to put on NativeModules: Operation unsupported");
    }

    private:
    std::weak_ptr<JSINativeModules> weakNativeModules_;
};

namespace {

// basename_r isn't in all iOS SDKs, so use this simple version instead.
std::string simpleBasename(const std::string &path) {
    size_t pos = path.rfind("/");
    return (pos != std::string::npos) ? path.substr(pos) : path;
}

} // namespace

JSIExecutor::JSIExecutor(
    std::shared_ptr<facebook::jsi::Runtime> runtime,
    std::shared_ptr<ExecutorDelegate> delegate,
    const JSIScopedTimeoutInvoker &scopedTimeoutInvoker,
    RuntimeInstaller runtimeInstaller)
    : runtime_(runtime),
      delegate_(delegate),
      nativeModules_(std::make_shared<JSINativeModules>(
          delegate ? delegate->getModuleRegistry() : nullptr)),
      moduleRegistry_(delegate ? delegate->getModuleRegistry() : nullptr),
      scopedTimeoutInvoker_(scopedTimeoutInvoker),
      runtimeInstaller_(runtimeInstaller) {
    runtime_->global().setProperty(
        *runtime, "__jsiExecutorDescription", runtime->description());
}

void JSIExecutor::initializeRuntime() {
    SystraceSection s("JSIExecutor::initializeRuntime");
    runtime_->global().setProperty(
        *runtime_,
        "nativeModuleProxy",
        facebook::jsi::Object::createFromHostObject(
            *runtime_, std::make_shared<NativeModuleProxy>(nativeModules_)));

    runtime_->global().setProperty(
        *runtime_,
        "nativeFlushQueueImmediate",
        facebook::jsi::Function::createFromHostFunction(
            *runtime_,
            facebook::jsi::PropNameID::forAscii(*runtime_, "nativeFlushQueueImmediate"),
            1,
            [this](
                facebook::jsi::Runtime &,
                const facebook::jsi::Value &,
                const facebook::jsi::Value *args,
                size_t count) {
                if (count != 1) {
                    throw std::invalid_argument(
                        "nativeFlushQueueImmediate arg count must be 1");
                }
                callNativeModules(args[0], false);
                return facebook::jsi::Value::undefined();
            }));

    runtime_->global().setProperty(
        *runtime_,
        "nativeCallSyncHook",
        facebook::jsi::Function::createFromHostFunction(
            *runtime_,
            facebook::jsi::PropNameID::forAscii(*runtime_, "nativeCallSyncHook"),
            1,
            [this](
                facebook::jsi::Runtime &,
                const facebook::jsi::Value &,
                const facebook::jsi::Value *args,
                size_t count) { return nativeCallSyncHook(args, count); }));

    runtime_->global().setProperty(
        *runtime_,
        "globalEvalWithSourceUrl",
        facebook::jsi::Function::createFromHostFunction(
            *runtime_,
            facebook::jsi::PropNameID::forAscii(*runtime_, "globalEvalWithSourceUrl"),
            1,
            [this](
                facebook::jsi::Runtime &,
                const facebook::jsi::Value &,
                const facebook::jsi::Value *args,
                size_t count) { return globalEvalWithSourceUrl(args, count); }));

    if (runtimeInstaller_) {
        runtimeInstaller_(*runtime_);
    }
}

void JSIExecutor::loadBundle(
    std::unique_ptr<const JSBigString> script,
    std::string sourceURL) {
    SystraceSection s("JSIExecutor::loadBundle");

    std::string scriptName = simpleBasename(sourceURL);

    runtime_->evaluateJavaScript(
        std::make_unique<BigStringBuffer>(std::move(script)), sourceURL);
    flush();
}

// void JSIExecutor::setBundleRegistry(std::unique_ptr<RAMBundleRegistry> r) {
// }

void JSIExecutor::registerBundle(
    uint32_t bundleId,
    const std::string &bundlePath) {
    const auto tag = folly::to<std::string>(bundleId);

    auto script = JSBigFileString::fromPath(bundlePath);
    if (script->size() == 0) {
        throw std::invalid_argument("Empty bundle registered with ID " + tag + " from " + bundlePath);
    }
    runtime_->evaluateJavaScript(std::make_unique<BigStringBuffer>(std::move(script)),
                                 JSExecutor::getSyntheticBundlePath(bundleId, bundlePath));
}

// Looping on \c drainMicrotasks until it completes or hits the retries bound.
static void performMicrotaskCheckpoint(facebook::jsi::Runtime &runtime) {
    uint8_t retries = 0;
    // A heuristic number to guard infinite or absurd numbers of retries.
    const static unsigned int kRetriesBound = 255;

    while (retries < kRetriesBound) {
        try {
            // The default behavior of \c drainMicrotasks is unbounded execution.
            // We may want to make it bounded in the future.
            if (runtime.drainMicrotasks()) {
                break;
            }
        } catch (facebook::jsi::JSError &error) {
            handleJSError(runtime, error, true);
        }
        retries++;
    }

    if (retries == kRetriesBound) {
        throw std::runtime_error("Hits microtasks retries bound.");
    }
}

void JSIExecutor::callFunction(
    const std::string &moduleId,
    const std::string &methodId,
    const folly::dynamic &arguments) {
    SystraceSection s(
        "JSIExecutor::callFunction", "moduleId", moduleId, "methodId", methodId);
    if (!callFunctionReturnFlushedQueue_) {
        bindBridge();
    }

    // Construct the error message producer in case this times out.
    // This is executed on a background thread, so it must capture its parameters
    // by value.
    auto errorProducer = [=] {
        std::stringstream ss;
        ss << "moduleID: " << moduleId << " methodID: " << methodId;
        return ss.str();
    };

    facebook::jsi::Value ret = facebook::jsi::Value::undefined();
    try {
        scopedTimeoutInvoker_(
            [&] {
                ret = callFunctionReturnFlushedQueue_->call(
                    *runtime_,
                    moduleId,
                    methodId,
                    valueFromDynamic(*runtime_, arguments));
            },
            std::move(errorProducer));
    } catch (...) {
        std::throw_with_nested(
            std::runtime_error("Error calling " + moduleId + "." + methodId));
    }

    performMicrotaskCheckpoint(*runtime_);

    callNativeModules(ret, true);
}

void JSIExecutor::invokeCallback(
    const double callbackId,
    const folly::dynamic &arguments) {
    SystraceSection s("JSIExecutor::invokeCallback", "callbackId", callbackId);
    if (!invokeCallbackAndReturnFlushedQueue_) {
        bindBridge();
    }
    facebook::jsi::Value ret;
    try {
        ret = invokeCallbackAndReturnFlushedQueue_->call(
            *runtime_, callbackId, valueFromDynamic(*runtime_, arguments));
    } catch (...) {
        std::throw_with_nested(std::runtime_error(
            folly::to<std::string>("Error invoking callback ", callbackId)));
    }

    performMicrotaskCheckpoint(*runtime_);

    callNativeModules(ret, true);
}

void JSIExecutor::setGlobalVariable(
    std::string propName,
    std::unique_ptr<const JSBigString> jsonValue) {
    SystraceSection s("JSIExecutor::setGlobalVariable", "propName", propName);
    runtime_->global().setProperty(
        *runtime_,
        propName.c_str(),
        facebook::jsi::Value::createFromJsonUtf8(
            *runtime_,
            reinterpret_cast<const uint8_t *>(jsonValue->c_str()),
            jsonValue->size()));
}

std::string JSIExecutor::getDescription() {
    return "JSI (" + runtime_->description() + ")";
}

void *JSIExecutor::getJavaScriptContext() {
    return runtime_.get();
}

bool JSIExecutor::isInspectable() {
    return runtime_->isInspectable();
}

void JSIExecutor::handleMemoryPressure(int pressureLevel) {
    // The level is an enum value passed by the Android OS to an onTrimMemory
    // event callback. Defined in ComponentCallbacks2.
    enum AndroidMemoryPressure {
        TRIM_MEMORY_BACKGROUND = 40,
        TRIM_MEMORY_COMPLETE = 80,
        TRIM_MEMORY_MODERATE = 60,
        TRIM_MEMORY_RUNNING_CRITICAL = 15,
        TRIM_MEMORY_RUNNING_LOW = 10,
        TRIM_MEMORY_RUNNING_MODERATE = 5,
        TRIM_MEMORY_UI_HIDDEN = 20,
    };
    const char *levelName;
    switch (pressureLevel) {
        case TRIM_MEMORY_BACKGROUND:
            levelName = "TRIM_MEMORY_BACKGROUND";
            break;
        case TRIM_MEMORY_COMPLETE:
            levelName = "TRIM_MEMORY_COMPLETE";
            break;
        case TRIM_MEMORY_MODERATE:
            levelName = "TRIM_MEMORY_MODERATE";
            break;
        case TRIM_MEMORY_RUNNING_CRITICAL:
            levelName = "TRIM_MEMORY_RUNNING_CRITICAL";
            break;
        case TRIM_MEMORY_RUNNING_LOW:
            levelName = "TRIM_MEMORY_RUNNING_LOW";
            break;
        case TRIM_MEMORY_RUNNING_MODERATE:
            levelName = "TRIM_MEMORY_RUNNING_MODERATE";
            break;
        case TRIM_MEMORY_UI_HIDDEN:
            levelName = "TRIM_MEMORY_UI_HIDDEN";
            break;
        default:
            levelName = "UNKNOWN";
            break;
    }

    switch (pressureLevel) {
        case TRIM_MEMORY_RUNNING_LOW:
        case TRIM_MEMORY_RUNNING_MODERATE:
        case TRIM_MEMORY_UI_HIDDEN:
            // For non-severe memory trims, do nothing.
            LOG(INFO) << "Memory warning (pressure level: " << levelName
                      << ") received by JS VM, ignoring because it's non-severe";
            break;
        case TRIM_MEMORY_BACKGROUND:
        case TRIM_MEMORY_COMPLETE:
        case TRIM_MEMORY_MODERATE:
        case TRIM_MEMORY_RUNNING_CRITICAL:
            // For now, pressureLevel is unused by collectGarbage.
            // This may change in the future if the JS GC has different styles of
            // collections.
            LOG(INFO) << "Memory warning (pressure level: " << levelName
                      << ") received by JS VM, running a GC";
            runtime_->instrumentation().collectGarbage(levelName);
            break;
        default:
            // Use the raw number instead of the name here since the name is
            // meaningless.
            LOG(WARNING) << "Memory warning (pressure level: " << pressureLevel
                         << ") received by JS VM, unrecognized pressure level";
            break;
    }
}

void JSIExecutor::bindBridge() {
    std::call_once(bindFlag_, [this] {
        SystraceSection s("JSIExecutor::bindBridge (once)");
        facebook::jsi::Value batchedBridgeValue =
            runtime_->global().getProperty(*runtime_, "__fbBatchedBridge");
        if (batchedBridgeValue.isUndefined() || !batchedBridgeValue.isObject()) {
            throw facebook::jsi::JSINativeException(
                "Could not get BatchedBridge, make sure your bundle is packaged correctly");
        }

        facebook::jsi::Object batchedBridge = batchedBridgeValue.asObject(*runtime_);
        callFunctionReturnFlushedQueue_ = batchedBridge.getPropertyAsFunction(
            *runtime_, "callFunctionReturnFlushedQueue");
        invokeCallbackAndReturnFlushedQueue_ = batchedBridge.getPropertyAsFunction(
            *runtime_, "invokeCallbackAndReturnFlushedQueue");
        flushedQueue_ =
            batchedBridge.getPropertyAsFunction(*runtime_, "flushedQueue");
    });
}

void JSIExecutor::callNativeModules(const facebook::jsi::Value &queue, bool isEndOfBatch) {
    SystraceSection s("JSIExecutor::callNativeModules");
    // If this fails, you need to pass a fully functional delegate with a
    // module registry to the factory/ctor.
    CHECK(delegate_) << "Attempting to use native modules without a delegate";
#if 0 // maybe useful for debugging
  std::string json = runtime_->global().getPropertyAsObject(*runtime_, "JSON")
    .getPropertyAsFunction(*runtime_, "stringify").call(*runtime_, queue)
    .getString(*runtime_).utf8(*runtime_);
#endif

    delegate_->callNativeModules(
        *this, dynamicFromValue(*runtime_, queue), isEndOfBatch);
}

void JSIExecutor::flush() {
    SystraceSection s("JSIExecutor::flush");
    if (flushedQueue_) {
        facebook::jsi::Value ret = flushedQueue_->call(*runtime_);
        performMicrotaskCheckpoint(*runtime_);
        callNativeModules(ret, true);
        return;
    }

    // When a native module is called from JS, BatchedBridge.enqueueNativeCall()
    // is invoked.  For that to work, require('BatchedBridge') has to be called,
    // and when that happens, __fbBatchedBridge is set as a side effect.
    facebook::jsi::Value batchedBridge =
        runtime_->global().getProperty(*runtime_, "__fbBatchedBridge");
    // So here, if __fbBatchedBridge doesn't exist, then we know no native calls
    // have happened, and we were able to determine this without forcing
    // BatchedBridge to be loaded as a side effect.
    if (!batchedBridge.isUndefined()) {
        // If calls were made, we bind to the JS bridge methods, and use them to
        // get the pending queue of native calls.
        bindBridge();
        facebook::jsi::Value ret = flushedQueue_->call(*runtime_);
        performMicrotaskCheckpoint(*runtime_);
        callNativeModules(ret, true);
    } else if (delegate_) {
        // If we have a delegate, we need to call it; we pass a null list to
        // callNativeModules, since we know there are no native calls, without
        // calling into JS again.  If no calls were made and there's no delegate,
        // nothing happens, which is correct.
        callNativeModules(nullptr, true);
    }
}

facebook::jsi::Value JSIExecutor::nativeCallSyncHook(const facebook::jsi::Value *args, size_t count) {
    if (count != 3) {
        throw std::invalid_argument("nativeCallSyncHook arg count must be 3");
    }

    if (!args[2].isObject() || !args[2].asObject(*runtime_).isArray(*runtime_)) {
        throw std::invalid_argument(
            folly::to<std::string>("method parameters should be array"));
    }

    unsigned int moduleId = static_cast<unsigned int>(args[0].getNumber());
    unsigned int methodId = static_cast<unsigned int>(args[1].getNumber());
    std::string moduleName;
    std::string methodName;

    if (moduleRegistry_) {
        moduleName = moduleRegistry_->getModuleName(moduleId);
        methodName = moduleRegistry_->getModuleSyncMethodName(moduleId, methodId);
    }

    MethodCallResult result = delegate_->callSerializableNativeHook(
        *this, moduleId, methodId, dynamicFromValue(*runtime_, args[2]));

    /**
     * Note:
     * In RCTNativeModule, std::nullopt is returned from
     * callSerializableNativeHook when executing a NativeModule method fails.
     * Therefore, it's safe to not terminate the syncMethodCall when std::nullopt
     * is returned.
     *
     * TODO: In JavaNativeModule, std::nullopt is returned when the synchronous
     * NativeModule method has the void return type. Change this to return
     * folly::dynamic(nullptr) instead, so that std::nullopt is reserved for
     * exceptional scenarios.
     *
     * TODO: Investigate CxxModule infra to see if std::nullopt is used for
     * returns in exceptional scenarios.
     **/

    if (!result.has_value()) {
        return facebook::jsi::Value::undefined();
    }

    facebook::jsi::Value returnValue = valueFromDynamic(*runtime_, result.value());
    return returnValue;
}

facebook::jsi::Value JSIExecutor::globalEvalWithSourceUrl(const facebook::jsi::Value *args, size_t count) {
    if (count != 1 && count != 2) {
        throw std::invalid_argument(
            "globalEvalWithSourceUrl arg count must be 1 or 2");
    }

    auto code = args[0].asString(*runtime_).utf8(*runtime_);
    std::string url;
    if (count > 1 && args[1].isString()) {
        url = args[1].asString(*runtime_).utf8(*runtime_);
    }

    return runtime_->evaluateJavaScript(
        std::make_unique<facebook::jsi::StringBuffer>(std::move(code)), url);
}

void bindNativeLogger(facebook::jsi::Runtime &runtime, Logger logger) {
    runtime.global().setProperty(
        runtime,
        "nativeLoggingHook",
        facebook::jsi::Function::createFromHostFunction(
            runtime,
            facebook::jsi::PropNameID::forAscii(runtime, "nativeLoggingHook"),
            2,
            [logger = std::move(logger)](
                facebook::jsi::Runtime &runtime,
                const facebook::jsi::Value &,
                const facebook::jsi::Value *args,
                size_t count) {
                if (count != 2) {
                    throw std::invalid_argument(
                        "nativeLoggingHook takes 2 arguments");
                }
                logger(
                    args[0].asString(runtime).utf8(runtime),
                    folly::to<unsigned int>(args[1].asNumber()));
                return facebook::jsi::Value::undefined();
            }));
}

void bindNativePerformanceNow(facebook::jsi::Runtime &runtime) {
    runtime.global().setProperty(
        runtime,
        "nativePerformanceNow",
        facebook::jsi::Function::createFromHostFunction(
            runtime,
            facebook::jsi::PropNameID::forAscii(runtime, "nativePerformanceNow"),
            0,
            [](facebook::jsi::Runtime &runtime,
               const facebook::jsi::Value &,
               const facebook::jsi::Value *args,
               size_t count) { return facebook::jsi::Value(JSExecutor::performanceNow()); }));
}
