//
// Created on 2024/5/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "helper/SwitchManager.h"
namespace TaroHelper {
SwitchManager *SwitchManager::instance = nullptr;

SwitchManager::SwitchManager() {
    switches.fill(false);
}

SwitchManager *SwitchManager::getInstance() {
    if (instance == nullptr) {
        instance = new SwitchManager();
    }
    return instance;
}

void SwitchManager::setSwitch(Switches switchId, bool value) {
    switches[static_cast<size_t>(switchId)] = value;
}

bool SwitchManager::getSwitch(Switches switchId) const {
    return switches[static_cast<size_t>(switchId)];
}

void SwitchManager::toggleSwitch(Switches switchId) {
    switches[static_cast<size_t>(switchId)] = !switches[static_cast<size_t>(switchId)];
}

void SwitchManager::printAllSwitches() const {
    for (size_t i = 0; i < switches.size(); ++i) {
        std::cout << "Switch" << i + 1 << " is " << (switches[i] ? "ON" : "OFF") << "\n";
    }
}
} // namespace TaroHelper