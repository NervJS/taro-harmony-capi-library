/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once
#include <unordered_map>
#include <stdint.h>

#include "helper/life_statistic.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroFrameTrigger : public std::enable_shared_from_this<TaroFrameTrigger>,
                             public TaroHelper::TaroClassLife<TaroFrameTrigger> {
        public:
        TaroFrameTrigger();
        virtual ~TaroFrameTrigger();
        virtual void tick(uint64_t current_time) = 0;
        virtual void pause() = 0;
        virtual void resume() = 0;
        void enable(uint32_t xid);
        void disable();
        // 禁用拷贝构造函数
        TaroFrameTrigger(const TaroFrameTrigger &) = delete;
        // 禁用赋值运算符
        TaroFrameTrigger &operator=(const TaroFrameTrigger &) = delete;

        private:
        uint32_t id_ = 0;
        uint32_t xid_ = 0; // 隶属于哪个页面
    };

} // namespace TaroAnimate
} // namespace TaroRuntime
