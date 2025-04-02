/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "runtime/dom/animation/animator/evaluator_color.h"

namespace TaroRuntime {
namespace TaroAnimate {
    enum class TaroEvaluatorType { Base = 0,
                                   Color = 1 };
    class TaroEvaluators final {
        public:
        TaroEvaluators();
        ~TaroEvaluators();
        static const std::shared_ptr<TaroEvaluator> getEvaluator(
            TaroEvaluatorType type);

        private:
        std::shared_ptr<TaroEvaluator> base_ = nullptr;
        std::shared_ptr<TaroEvaluatorColor> color_ = nullptr;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
