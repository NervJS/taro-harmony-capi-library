#pragma once

#include "evaluator.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroEvaluatorColor : public TaroEvaluator {
        public:
        virtual double evaluate(const double &begin, const double &end,
                                float fraction) override;

        virtual AnimPropType_MultiValue evaluate(const AnimPropType_MultiValue &begin,
                                                 const AnimPropType_MultiValue &end,
                                                 float fraction) override;
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
