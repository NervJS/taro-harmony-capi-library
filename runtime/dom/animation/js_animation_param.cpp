/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "js_animation_param.h"

#include <js_native_api_types.h>

#include "helper/TaroLog.h"
#include "runtime/cssom/stylesheet/attribute_base.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/params/transform_param/transform_param.h"
#include "runtime/cssom/stylesheet/transform.h"
#include "runtime/cssom/stylesheet/types/TColor.h"

namespace TaroRuntime {
namespace TaroAnimate {
    int TaroJsStepOption::setFromNode(const napi_value& napi_val) {
        auto napi_duration = NapiGetter::GetPropertyFromNode(napi_val, "duration");
        if (napi_duration != nullptr) {
            NapiGetter duration_getter(napi_duration);
            duration_ = duration_getter.UInt32(0);
        }

        auto napi_delay = NapiGetter::GetPropertyFromNode(napi_val, "delay");
        if (napi_delay != nullptr) {
            NapiGetter delay_getter(napi_delay);
            delay_ = delay_getter.UInt32(0);
        }

        auto napi_timing = NapiGetter::GetPropertyFromNode(napi_val, "timingFunction");
        if (napi_timing != nullptr) {
            NapiGetter timing_getter(napi_timing);
            timingFunction_ = timing_getter.StringOr("ease");
        }

        auto napi_origin = NapiGetter::GetPropertyFromNode(napi_val, "transformOrigin");
        if (napi_origin != nullptr) {
            NapiGetter origin_getter(napi_origin);
            transformOrigin_ = origin_getter.StringOr("");
        }

        auto napi_rule = NapiGetter::GetPropertyFromNode(napi_val, "rule");
        if (napi_rule != nullptr) {
            auto rule_names_getter = NapiGetter::GetAllPropertyNames(napi_rule);
            for (auto& rule_name_getter : rule_names_getter) {
                std::string rule_name = rule_name_getter.StringOr("");
                if (rule_name.empty()) {
                    continue;
                }
                auto rule_value_getter = NapiGetter::GetProperty(napi_rule, rule_name.c_str());
                parseKeyframe(rule_name, rule_value_getter);
            }
        }
        return 0;
    }

    void TaroJsStepOption::parseKeyframe(const std::string& name, NapiGetter& napi_getter) {
        static std::unordered_map<std::string, CSSProperty::Type> s_map_length = {
            {"width", CSSProperty::Type::Width}, {"height", CSSProperty::Type::Height}, {"left", CSSProperty::Type::Left}, {"right", CSSProperty::Type::Right}, {"top", CSSProperty::Type::Top}, {"bottom", CSSProperty::Type::Bottom}};
        auto iter_length = s_map_length.find(name);
        if (iter_length != s_map_length.end()) {
            TaroCSSOM::TaroStylesheet::AttributeBase<Dimension> lengthAttr;
            lengthAttr.setValueFromNapi(napi_getter.GetNapiValue());
            if (lengthAttr.has_value()) {
                keyframes.emplace_back(iter_length->second, lengthAttr.value());
            }
            return;
        }

        if (name == "backgroundColor") {
            TaroCSSOM::TaroStylesheet::AttributeBase<TaroCSSOM::TaroStylesheet::TColor> colorAttr;
            colorAttr.setValueFromNapi(napi_getter.GetNapiValue());
            if (colorAttr.has_value()) {
                keyframes.emplace_back(CSSProperty::Type::BackgroundColor, static_cast<double>(colorAttr.value()));
            }
            return;
        }

        if (name == "opacity") {
            TaroCSSOM::TaroStylesheet::AttributeBase<float> numAttr;
            numAttr.setValueFromNapi(napi_getter.GetNapiValue());
            if (numAttr.has_value()) {
                keyframes.emplace_back(CSSProperty::Type::Opacity, static_cast<double>(numAttr.value()));
            }
            return;
        }

        if (name == "transform") {
            std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformParam> transform = std::make_shared<TaroCSSOM::TaroStylesheet::TransformParam>();

            napi_value node = napi_getter.GetNapiValue();
        
            auto item_names_getter = NapiGetter::GetAllPropertyNames(node);
            for (auto& item_name_getter : item_names_getter) {
                std::string item_name = item_name_getter.StringOr("");
                if (item_name.empty()) {
                    continue;
                }
                std::shared_ptr<TaroCSSOM::TaroStylesheet::TransformItemBase> item;
                auto napi_item_val = NapiGetter::GetPropertyFromNode(node, item_name.c_str());
                if (napi_item_val == nullptr) {
                    continue;
                }
      
                if (item_name == "Matrix") {
                    item = TaroCSSOM::TaroStylesheet::Transform::parseTransformItem(
                        TaroCSSOM::TaroStylesheet::ETransformType::MATRIX, node);
                } else if (item_name == "Rotate") {
                    item = TaroCSSOM::TaroStylesheet::Transform::parseTransformItem(
                        TaroCSSOM::TaroStylesheet::ETransformType::ROTATE, napi_item_val);
                } else if (item_name == "Scale") {
                    item = TaroCSSOM::TaroStylesheet::Transform::parseTransformItem(
                        TaroCSSOM::TaroStylesheet::ETransformType::SCALE, napi_item_val);
                } else if (item_name == "Skew") {
                    item = TaroCSSOM::TaroStylesheet::Transform::parseTransformItem(
                        TaroCSSOM::TaroStylesheet::ETransformType::SKEW, napi_item_val);
                } else if (item_name == "Translate") {
                    item = TaroCSSOM::TaroStylesheet::Transform::parseTransformItem(
                        TaroCSSOM::TaroStylesheet::ETransformType::TRANSLATE, napi_item_val);
                }
                if (item != nullptr) {
                    transform->data.emplace_back(item);
                }
            }
            if (!transform->data.empty()) {
                keyframes.emplace_back(CSSProperty::Type::Transform, transform);
            } 
        }
    }

    int TaroJsAnimationOption::setFromNode(const napi_value& napi_val) {
        auto steps = NapiGetter::GetPropertyFromNode(napi_val, "actions");
        if (steps == nullptr) {
            return -1;
        }

        bool parse_ok = true;
        NapiGetter::ForEachInArray(steps, [&](const ::napi_value& n_val, const uint32_t& index) {
            TaroJsStepOption step_option;
            int ret = step_option.setFromNode(n_val);
            if (ret != 0) {
                parse_ok = false;
                TARO_LOG_ERROR("TaroAnimation", "setFromNode failed, ret = %{public}d", ret);
                return;
            }
            steps_.emplace_back(std::move(step_option));
        });
        if (parse_ok && !steps_.empty()) {
            return 0;
        }
        return -2;
    }
} // namespace TaroAnimate
} // namespace TaroRuntime
