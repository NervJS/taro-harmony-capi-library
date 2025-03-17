//
// Created on 2024/5/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_SWITCHMANAGER_H
#define TARO_CAPI_HARMONY_DEMO_SWITCHMANAGER_H
#include <iostream>
#include <string>
namespace TaroHelper {
enum class Switches {
    UseYoga = 0,
    NumSwitches
};
class SwitchManager {
    private:
    std::array<bool, static_cast<size_t>(Switches::NumSwitches)> switches;
    static SwitchManager *instance;

    SwitchManager();

    public:
    SwitchManager(const SwitchManager &) = delete;
    SwitchManager &operator=(const SwitchManager &) = delete;

    static SwitchManager *getInstance();

    void setSwitch(Switches switchId, bool value);

    bool getSwitch(Switches switchId) const;

    void toggleSwitch(Switches switchId);

    // 打印所有开关的状态（用于调试）
    void printAllSwitches() const;
};
} // namespace TaroHelper

#endif // TARO_CAPI_HARMONY_DEMO_SWITCHMANAGER_H
