/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <unordered_map>
#include <vector>
#include <js_native_api.h>

#include "runtime/NapiGetter.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    // featureID
    enum class MediaFeatureId {
        Invalid = 0,
        Width = 1,
        Height = 2,
        AspectRatio = 3,
        DisplayMode = 4,
        Orientation = 5,
        DeviceWidth = 6,
        DeviceHeight = 7,
        DeviceAspectRatio = 8,
        Resolution = 9,
    };

    // 表达式条件
    enum class MediaOpType {
        None = 0,
        NOT = 1,
        AND = 2,
        OR = 3,
        MAX = 4, // 仅做校验使用
    };
    // feature内比较类型
    enum class MediaCompType {
        Equal = 0,
        GreaterThan = 1,
        GreaterThanEqual = 2,
        LessThan = 3,
        LessThanEqual = 4,
    };

    class CSSMediaItem {
        public:
        virtual ~CSSMediaItem() = default;
        void setOperate(MediaOpType op) {
            op_ = op;
        }

        // 重新计算match值
        virtual bool calc_match() = 0;

        bool match() {
            return match_;
        }

        protected:
        MediaOpType op_ = MediaOpType::None;
        bool match_ = false;
    };

    using CSSMediaItemPtr = std::shared_ptr<CSSMediaItem>;
} // namespace TaroCSSOM
} // namespace TaroRuntime
