#include "ArkTSBridge.h"

#include <boost/exception/diagnostic_information.hpp>
#include <glog/logging.h>

namespace Taro {

std::shared_ptr<ArkTSBridge> ArkTSBridge::instance = nullptr;

ArkTSBridge::ArkTSBridge(napi_env env, napi_ref napiBridgeRef)
    : m_arkJs(ArkJS(env)), m_arkTSBridgeRef(napiBridgeRef) {
    LOG(INFO) << "ArkTSBridge::ArkTSBridge";
}

void ArkTSBridge::initializeInstance(
    napi_env env,
    napi_ref arkTSBridgeHandler) {
    instance =
        std::shared_ptr<ArkTSBridge>(new ArkTSBridge(env, arkTSBridgeHandler));
}

ArkTSBridge::Shared ArkTSBridge::getInstance() {
    return instance;
}

ArkTSBridge::~ArkTSBridge() {
    m_arkJs.deleteReference(m_arkTSBridgeRef);
}

void ArkTSBridge::handleError(std::exception_ptr ex) {
    try {
        LOG(ERROR) << boost::diagnostic_information(ex);
        std::rethrow_exception(ex);
    } catch (const std::exception& e) {
        m_arkJs.getObject(m_arkTSBridgeRef)
            .call("handleError", {m_arkJs.createFromException(e)});
    }
}

auto ArkTSBridge::getDisplayMetrics() -> DisplayMetrics {
    auto napiBridgeObject = m_arkJs.getReferenceValue(m_arkTSBridgeRef);
    auto methodImpl =
        m_arkJs.getObjectProperty(napiBridgeObject, "getDisplayMetrics");
    auto napiResult = m_arkJs.call<0>(methodImpl, {});
    return DisplayMetrics::fromNapiValue(m_arkJs.getEnv(), napiResult);
}

auto PhysicalPixels::fromNapiValue(napi_env env, napi_value value)
    -> PhysicalPixels {
    ArkJS arkJs(env);
    return {
        static_cast<float>(
            arkJs.getDouble(arkJs.getObjectProperty(value, "width"))),
        static_cast<float>(
            arkJs.getDouble(arkJs.getObjectProperty(value, "height"))),
        static_cast<float>(
            arkJs.getDouble(arkJs.getObjectProperty(value, "scale"))),
        static_cast<float>(
            arkJs.getDouble(arkJs.getObjectProperty(value, "fontScale"))),
        static_cast<float>(
            arkJs.getDouble(arkJs.getObjectProperty(value, "densityDpi"))),
    };
}

auto DisplayMetrics::fromNapiValue(napi_env env, napi_value value)
    -> DisplayMetrics {
    ArkJS arkJs(env);
    return {
        PhysicalPixels::fromNapiValue(
            env, arkJs.getObjectProperty(value, "windowPhysicalPixels")),
        PhysicalPixels::fromNapiValue(
            env, arkJs.getObjectProperty(value, "screenPhysicalPixels")),
    };
}

} // namespace Taro
