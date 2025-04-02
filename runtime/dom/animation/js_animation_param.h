/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <string>

#include "runtime/cssom/stylesheet/animation.h"
#include "runtime/cssom/stylesheet/css_property.h"

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroJsStepOption {
        public:
        int32_t duration_ = 0;
        std::string timingFunction_ = "ease";
        uint32_t delay_ = 0;
        std::string transformOrigin_ = "50% 50% 0";
        std::vector<std::pair<CSSProperty::Type, TaroCSSOM::TaroStylesheet::KeyframeValue>> keyframes;
        int setFromNode(const napi_value& napi_val);

        private:
        void parseKeyframe(const std::string& name, NapiGetter& napi_getter);
        std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformItemBase> parseTransformItem(const napi_value& napi_val);
    };

    class TaroJsAnimationOption {
        public:
        std::vector<TaroJsStepOption> steps_;
        int setFromNode(const napi_value& napi_val);
    };

    using TaroJsAnimationOptionRef = std::shared_ptr<TaroJsAnimationOption>;
} // namespace TaroAnimate
} // namespace TaroRuntime
