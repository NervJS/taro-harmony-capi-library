/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "animation_config.h"

namespace TaroRuntime {
namespace TaroAnimate {
    TaroAnimationFillMode TaroAnimationConfig::parseFillMode(
        const std::string &fill_mode) {
        static std::unordered_map<std::string, TaroAnimationFillMode> map_fill_modes =
            {{"none", TaroAnimationFillMode::None},
             {"forwards", TaroAnimationFillMode::Forwards},
             {"backwards", TaroAnimationFillMode::Backwards},
             {"both", TaroAnimationFillMode::Both}};
        const auto iter = map_fill_modes.find(fill_mode);
        if (iter != map_fill_modes.end()) {
            return iter->second;
        }
        return TaroAnimationFillMode::None;
    }
    TaroAnimationDirection TaroAnimationConfig::parseDirection(
        const std::string &direction) {
        static std::unordered_map<std::string, TaroAnimationDirection>
            map_directions = {
                {"normal", TaroAnimationDirection::Normal},
                {"reverse", TaroAnimationDirection::Reverse},
                {"alternate", TaroAnimationDirection::Alternate},
                {"alternate-reverse", TaroAnimationDirection::Alternate_Reverse}};
        const auto iter = map_directions.find(direction);
        if (iter != map_directions.end()) {
            return iter->second;
        }
        return TaroAnimationDirection::Normal;
    }
} // namespace TaroAnimate
} // namespace TaroRuntime
