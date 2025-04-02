/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#include "jsi_native_modules.h"

#include <jsi/JSIDynamic.h>

JSINativeModules::JSINativeModules(std::shared_ptr<ModuleRegistry> moduleRegistry)
    : m_moduleRegistry(std::move(moduleRegistry)) {}

facebook::jsi::Value JSINativeModules::getModule(facebook::jsi::Runtime &rt, const facebook::jsi::PropNameID &name) {
    if (!m_moduleRegistry) {
        return nullptr;
    }

    std::string moduleName = name.utf8(rt);

    const auto it = m_objects.find(moduleName);
    if (it != m_objects.end()) {
        return facebook::jsi::Value(rt, it->second);
    }

    auto module = createModule(rt, moduleName);
    if (!module.has_value()) {
        return nullptr;
    }

    auto result = m_objects.emplace(std::move(moduleName), std::move(*module)).first;

    facebook::jsi::Value ret = facebook::jsi::Value(rt, result->second);
    return ret;
}

void JSINativeModules::reset() {
    m_genNativeModuleJS = std::nullopt;
    m_objects.clear();
}

std::optional<facebook::jsi::Object> JSINativeModules::createModule(facebook::jsi::Runtime &rt, const std::string &name) {
    if (!m_genNativeModuleJS) {
        m_genNativeModuleJS = rt.global().getPropertyAsFunction(rt, "__fbGenNativeModule");
    }

    auto result = m_moduleRegistry->getConfig(name);
    if (!result.has_value()) {
        return std::nullopt;
    }

    facebook::jsi::Value moduleInfo =
        m_genNativeModuleJS->call(rt, facebook::jsi::valueFromDynamic(rt, result->config), static_cast<double>(result->index));
    CHECK(!moduleInfo.isNull()) << "Module returned from genNativeModule is null";
    CHECK(moduleInfo.isObject()) << "Module returned from genNativeModule isn't an Object";

    std::optional<facebook::jsi::Object> module(moduleInfo.asObject(rt).getPropertyAsObject(rt, "module"));

    return module;
}
