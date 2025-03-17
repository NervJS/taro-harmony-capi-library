//
// Created on 2024/5/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "napi_bridge.h"

#include <unordered_map>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>

#include "arkjs/ArkJS.h"
#include "arkjs/ArkTSBridge.h"
#include "helper/Debug.h"
#include "helper/TaroLog.h"
#include "helper/api_cost_statistic.h"
#include "helper/life_statistic.h"
#include "taro/instance.h"
#include "thread/NapiTaskRunner.h"
#include "thread/TaskExecutor.h"
#include "thread/ThreadTaskRunner.h"

namespace TaroHarmonyLibrary {
const std::vector<napi_property_descriptor> NapiBridge::desc = {
    {"createTaroInstance", nullptr, NapiBridge::createTaroInstance, nullptr, nullptr, nullptr, napi_default, nullptr},
    // {"createQJSEngineInstance", nullptr, NapiBridge::createQJSEngineInstance, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"destroyTaroInstance", nullptr, NapiBridge::destroyTaroInstance, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"getNextTaroInstanceId", nullptr, NapiBridge::getNextTaroInstanceId, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"loadScript", nullptr, NapiBridge::loadScript, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"initializeArkTSBridge", nullptr, NapiBridge::initializeArkTSBridge, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"onInit", nullptr, NapiBridge::onInit, nullptr, nullptr, nullptr, napi_default, nullptr},
    {"startEngineTest", nullptr, NapiBridge::startEngineTest, nullptr, nullptr, nullptr, napi_default, nullptr},
};

std::mutex taroInstanceByIdMutex;
std::unordered_map<size_t, std::shared_ptr<TaroInstance>> taroInstanceById;
static auto cleanupRunner = std::make_unique<TaroThread::ThreadTaskRunner>("TARO_CLEANUP");

napi_value NapiBridge::onInit(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);
    auto shouldClearTaroInstances = arkJs.getBoolean(args[0]);
    if (shouldClearTaroInstances) {
        /**
         * 清理由于应用没有正常退出导致的 QJSEngine 没有被清理的问题
         */
        cleanupRunner->runAsyncTask([] {
            decltype(taroInstanceById) instances;
            {
                std::lock_guard<std::mutex> lock(taroInstanceByIdMutex);
                std::swap(taroInstanceById, instances);
            }
            instances.clear();
        });
    }
    auto isDebugModeEnabled = false;
#ifdef IS_DEBUG
    isDebugModeEnabled = true;
#endif
    return arkJs.createObjectBuilder().addProperty("isDebugModeEnabled", isDebugModeEnabled).build();
}

napi_value NapiBridge::initializeArkTSBridge(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 1);
    auto bridgeHandlerRef = arkJs.createReference(args[0]);
    Taro::ArkTSBridge::initializeInstance(env, bridgeHandlerRef);
    // #ifdef C_API_ARCH
    //     ArkUINodeRegistry::initialize(ArkTSBridge::getInstance());
    // #endif
    return arkJs.getUndefined();
}

napi_value NapiBridge::loadScript(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    try {
        auto args = arkJs.getCallbackArgs(info, 4);
        size_t instanceId = arkJs.getDouble(args[0]);
        auto lock = std::lock_guard<std::mutex>(taroInstanceByIdMutex);
        auto it = taroInstanceById.find(instanceId);
        if (it == taroInstanceById.end()) {
            return arkJs.getUndefined();
        }
        auto &taroInstance = it->second;
        auto onFinishRef = arkJs.createReference(args[3]);

        taroInstance->loadScript(
            arkJs.getArrayBuffer(args[1]),
            arkJs.getString(args[2]),
            [taskExecutor = taroInstance->getTaskExecutor(), env, onFinishRef](
                const std::string &errorMsg) {
                taskExecutor->runTask(
                    TaroThread::TaskThread::MAIN, [env, onFinishRef, errorMsg]() {
                        ArkJS arkJs(env);
                        auto listener = arkJs.getReferenceValue(onFinishRef);
                        arkJs.call<1>(listener, {arkJs.createString(errorMsg)});
                        arkJs.deleteReference(onFinishRef);
                    });
            });
    } catch (...) {
        Taro::ArkTSBridge::getInstance()->handleError(std::current_exception());
    }

    return arkJs.getUndefined();
}

napi_value NapiBridge::getNextTaroInstanceId(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    try {
        static std::atomic_size_t nextId = 0;
        auto id = nextId++;
        return ArkJS(env).createInt(id);
    } catch (...) {
        Taro::ArkTSBridge::getInstance()->handleError(std::current_exception());
        return ArkJS(env).createInt(-1);
    }
}

napi_value NapiBridge::createTaroInstance(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    try {
        auto args = arkJs.getCallbackArgs(info, 10);
        size_t instanceId = arkJs.getDouble(args[0]);
        auto shouldEnableBackgroundExecutor = arkJs.getBoolean(args[1]);

        auto taroInstance = std::make_shared<TaroInstance>(instanceId, env, shouldEnableBackgroundExecutor);
        auto lock = std::lock_guard<std::mutex>(taroInstanceByIdMutex);
        if (taroInstanceById.find(instanceId) != taroInstanceById.end()) {
            TARO_LOG_ERROR("#Taro_ARK", "TaroInstance with the following id %{public}d has been already created", instanceId);
        }
        auto [it, _inserted] = taroInstanceById.emplace(instanceId, std::move(taroInstance));
        it->second->start();
    } catch (...) {
        Taro::ArkTSBridge::getInstance()->handleError(std::current_exception());
    }
    return arkJs.getUndefined();
}

napi_value NapiBridge::destroyTaroInstance(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    try {
        auto args = arkJs.getCallbackArgs(info, 1);
        size_t taroInstanceId = arkJs.getDouble(args[0]);
        cleanupRunner->runAsyncTask([taroInstanceId] {
            std::shared_ptr<TaroInstance> instance;
            {
                auto lock = std::lock_guard<std::mutex>(taroInstanceByIdMutex);
                if (auto it = taroInstanceById.find(taroInstanceId);
                    it != taroInstanceById.end()) {
                    std::swap(it->second, instance);
                    taroInstanceById.erase(taroInstanceId);
                }
            }
        });
    } catch (...) {
        Taro::ArkTSBridge::getInstance()->handleError(std::current_exception());
    }
    return arkJs.getUndefined();
}

class CustomEventListener : public ::testing::EmptyTestEventListener {
    public:
    virtual void OnTestEnd(const ::testing::TestInfo &test_info) override {
        if (test_info.result()->Failed()) {
            TARO_LOG_ERROR("TARO_TEST", "Test failed: %{public}s.%{public}s", test_info.test_case_name(),
                           test_info.name());
        } else {
            TARO_LOG_INFO("TARO_TEST", "Test passed: %{public}s.%{public}s", test_info.test_case_name(),
                          test_info.name());
        }
    }
};

napi_value NapiBridge::startEngineTest(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    ArkJS arkJs(env);
    try {
#if IS_TEST_OPEN == 1
        ::testing::InitGoogleTest();
        ::testing::TestEventListeners &listeners = ::testing::UnitTest::GetInstance()->listeners();
        CustomEventListener *listener = new CustomEventListener();
        listeners.Append(listener);
        RUN_ALL_TESTS();
        delete listener;
#endif
    } catch (...) {
        Taro::ArkTSBridge::getInstance()->handleError(std::current_exception());
    }
    return arkJs.getUndefined();
}
} // namespace TaroHarmonyLibrary
