/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <type_traits>

#include "../animation_prop_types.h"
#include "runtime/cssom/stylesheet/color.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/transform.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroEvaluator {
        public:
        virtual double evaluate(const double& begin, const double& end,
                                float fraction);

        virtual TaroCSSOM::TaroStylesheet::Color evaluate(
            const TaroCSSOM::TaroStylesheet::Color& begin,
            const TaroCSSOM::TaroStylesheet::Color& end, float fraction);

        virtual TaroCSSOM::TaroStylesheet::Transform evaluate(
            const TaroCSSOM::TaroStylesheet::Transform& begin,
            const TaroCSSOM::TaroStylesheet::Transform& end, float fraction);
        virtual std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam> evaluate(
            const std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>& begin,
            const std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam>& end,
            float fraction);

        virtual AnimPropType_MultiValue evaluate(const AnimPropType_MultiValue& begin,
                                                 const AnimPropType_MultiValue& end,
                                                 float fraction);

        virtual Dimension evaluate(
            const Dimension& begin,
            const Dimension& end,
            float fraction);

        protected:
        void convertRgbToUInt(double red, double green, double blue, double alpha,
                              uint32_t& argb);
        void convertUIntToRgb(uint32_t argb, double& red, double& green, double& blue,
                              double& alpha);
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
