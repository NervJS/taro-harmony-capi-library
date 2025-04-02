/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <sys/types.h>

#include "CSSMediaCondition.h"

namespace TaroRuntime {
namespace TaroCSSOM {

    class CSSMedia {
        public:
        int init(const napi_value& config);

        uint32_t id() const {
            return id_;
        }

        bool match() const {
            return match_;
        }

        void calc_match();

        private:
        // 媒体ID，与StyleSheet配合使用
        uint32_t id_ = 0;
        CSSMediaConditionPtr condition_ = nullptr;
        // 是否匹配成功
        bool match_ = false;
    };

    using CSSMediaPtr = std::shared_ptr<CSSMedia>;

    class CSSMedias {
        public:
        int init(const napi_value& config);

        const CSSMediaPtr getMedia(int32_t) const;

        // 获取match值
        bool match(int32_t) const;

        // 重新计算match值
        void calc_match();

        private:
        std::unordered_map<int32_t, CSSMediaPtr> medias_;
    };
    using CSSMediasPtr = std::shared_ptr<CSSMedias>;

} // namespace TaroCSSOM
} // namespace TaroRuntime
