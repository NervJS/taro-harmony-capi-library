/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <string>
#include <vector>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "./attribute_base.h"
#include "./css_property.h"
#include "animation.h"
#include "helper/Optional.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {

        struct TransitionParam {
            CSSProperty::Type prop_type_ = CSSProperty::Type::Invalid;
            KeyframeValue prop_value_;
            uint32_t duration_ = 0;
            uint32_t delay = 0;
            std::string timing_function_;
        };
        struct TransitionOption {
            std::vector<int32_t> properties_;
            std::vector<uint32_t> durations_;
            std::vector<std::string> timing_functions_;
            std::vector<uint32_t> delays_;

            friend bool operator==(const TransitionOption& lhs, const TransitionOption& rhs) {
                return (lhs.properties_ == rhs.properties_ &&
                        lhs.durations_ == rhs.durations_ &&
                        lhs.timing_functions_ == rhs.timing_functions_ &&
                        lhs.delays_ == rhs.delays_);
            }

            friend bool operator!=(const TransitionOption& lhs, const TransitionOption& rhs) {
                return !(lhs == rhs);
            }
        };
        class Transition : public AttributeBase<TransitionOption> {
            public:
            Transition() = default;
            void setValueFromNapi(const napi_value& value) override {};
            void setValueFromStringView(std::string_view value) override {};
            void assign(const AttributeBase<TransitionOption>& item) override;
            std::shared_ptr<TransitionParam> getTransitionParam(CSSProperty::Type prop_type, const Stylesheet& sheet);
            void setPropertyFromNapi(const napi_value& napi_value);
            void setDurationFromNapi(const napi_value& napi_value);
            void setTimingFunctionFromNapi(const napi_value& napi_value);
            void setDelayFromNapi(const napi_value& napi_value);
            bool isActivePropType(CSSProperty::Type prop_type);

            private:
            bool checkPropType(CSSProperty::Type prop_type, int trans_property);
            bool getStyleValue(CSSProperty::Type prop_type, const Stylesheet& sheet, KeyframeValue& value);

            private:
            // 集合子属性对应的集合属性
            static std::unordered_map<CSSProperty::Type, CSSProperty::Type> map_multi_property_;
        };
    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime
