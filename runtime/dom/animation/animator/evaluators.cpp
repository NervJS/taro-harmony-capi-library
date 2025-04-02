/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "evaluators.h"

namespace TaroRuntime {
namespace TaroAnimate {

    TaroEvaluators::TaroEvaluators() {
        base_ = std::make_shared<TaroEvaluator>();
        color_ = std::make_shared<TaroEvaluatorColor>();
    }
    TaroEvaluators::~TaroEvaluators() {
        base_ = nullptr;
        color_ = nullptr;
    }

    const std::shared_ptr<TaroEvaluator> TaroEvaluators::getEvaluator(
        TaroEvaluatorType type) {
        static TaroEvaluators* instance = nullptr;
        if (instance == nullptr) {
            instance = new TaroEvaluators();
        }
        if (type == TaroEvaluatorType::Color) {
            return instance->color_;
        }
        return instance->base_;
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
