/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
    static SwitchManager* instance;

    SwitchManager();

    public:
    SwitchManager(const SwitchManager&) = delete;
    SwitchManager& operator=(const SwitchManager&) = delete;

    static SwitchManager* getInstance();

    void setSwitch(Switches switchId, bool value);

    bool getSwitch(Switches switchId) const;

    void toggleSwitch(Switches switchId);

    // 打印所有开关的状态（用于调试）
    void printAllSwitches() const;
};
} // namespace TaroHelper

#endif // TARO_CAPI_HARMONY_DEMO_SWITCHMANAGER_H
