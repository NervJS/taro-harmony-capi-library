//
// Created on 2024/6/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once
#include <memory>
#include <optional>
#include <string>
#include <glog/logging.h>
#include <jsi/JSIDynamic.h>
#include <jsi/jsi.h>

#include "engine/react_common/module_registry.h"

/**
 * Holds and creates JS representations of the modules in ModuleRegistry
 */
class JSINativeModules {
    public:
    explicit JSINativeModules(std::shared_ptr<ModuleRegistry> moduleRegistry);
    facebook::jsi::Value getModule(facebook::jsi::Runtime &rt, const facebook::jsi::PropNameID &name);
    void reset();

    private:
    std::optional<facebook::jsi::Function> m_genNativeModuleJS;
    std::shared_ptr<ModuleRegistry> m_moduleRegistry;
    std::unordered_map<std::string, facebook::jsi::Object> m_objects;

    std::optional<facebook::jsi::Object> createModule(
        facebook::jsi::Runtime &rt,
        const std::string &name);
};
