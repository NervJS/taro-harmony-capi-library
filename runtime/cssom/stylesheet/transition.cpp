/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "transition.h"

#include <utility>
#include <js_native_api_types.h>

#include "helper/TaroLog.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/stylesheet/IAttribute.h"
#include "runtime/cssom/stylesheet/css_property.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {

        std::unordered_map<CSSProperty::Type, CSSProperty::Type> Transition::map_multi_property_ =
            {
                {CSSProperty::Type::Left, CSSProperty::Type::Position},
                {CSSProperty::Type::Right, CSSProperty::Type::Position},
                {CSSProperty::Type::Top, CSSProperty::Type::Position},
                {CSSProperty::Type::Bottom, CSSProperty::Type::Position},
                {CSSProperty::Type::BorderLeftWidth, CSSProperty::Type::BorderWidth},
                {CSSProperty::Type::BorderRightWidth, CSSProperty::Type::BorderWidth},
                {CSSProperty::Type::BorderTopWidth, CSSProperty::Type::BorderWidth},
                {CSSProperty::Type::BorderBottomWidth, CSSProperty::Type::BorderWidth},
                {CSSProperty::Type::BorderLeftColor, CSSProperty::Type::BorderColor},
                {CSSProperty::Type::BorderRightColor, CSSProperty::Type::BorderColor},
                {CSSProperty::Type::BorderTopColor, CSSProperty::Type::BorderColor},
                {CSSProperty::Type::BorderBottomColor, CSSProperty::Type::BorderColor},
                {CSSProperty::Type::MarginLeft, CSSProperty::Type::Margin},
                {CSSProperty::Type::MarginRight, CSSProperty::Type::Margin},
                {CSSProperty::Type::MarginTop, CSSProperty::Type::Margin},
                {CSSProperty::Type::MarginBottom, CSSProperty::Type::Margin},
                {CSSProperty::Type::PaddingLeft, CSSProperty::Type::Padding},
                {CSSProperty::Type::PaddingRight, CSSProperty::Type::Padding},
                {CSSProperty::Type::PaddingTop, CSSProperty::Type::Padding},
                {CSSProperty::Type::PaddingBottom, CSSProperty::Type::Padding},
                {CSSProperty::Type::BorderTopLeftRadius, CSSProperty::Type::BorderRadius},
                {CSSProperty::Type::BorderTopRightRadius, CSSProperty::Type::BorderRadius},
                {CSSProperty::Type::BorderBottomLeftRadius, CSSProperty::Type::BorderRadius},
                {CSSProperty::Type::BorderBottomRightRadius, CSSProperty::Type::BorderRadius},
        };

        void Transition::assign(const AttributeBase<TransitionOption>& item) {
            if (!item.has_value()) return;
            if (!this->has_value()) {
                this->set(item);
            } else {
                if (!item.value().properties_.empty()) {
                    this->value().properties_ = item.value().properties_;
                }
                if (!item.value().durations_.empty()) {
                    this->value().durations_ = item.value().durations_;
                }
                if (!item.value().delays_.empty()) {
                    this->value().delays_ = item.value().delays_;
                }
                if (!item.value().timing_functions_.empty()) {
                    this->value().timing_functions_ = item.value().timing_functions_;
                }
            }
        }

        void Transition::setPropertyFromNapi(const napi_value& napi_value) {
            if (napi_value == nullptr) {
                return;
            }
            if (!has_value()) {
                set(TransitionOption());
            }

            auto& options = value();
            NapiGetter::ForEachInArray(napi_value,
                                       [&](const ::napi_value& elem,
                                           const uint32_t& _) {
                                           NapiGetter getter(elem);
                                           options.properties_.emplace_back(getter.Int32Or(0));
                                       });
        }

        void Transition::setDurationFromNapi(const napi_value& napi_value) {
            if (napi_value == nullptr) {
                return;
            }
            if (!has_value()) {
                set(TransitionOption());
            }

            auto& options = value();
            NapiGetter::ForEachInArray(napi_value,
                                       [&](const ::napi_value& elem,
                                           const uint32_t& _) {
                                           NapiGetter getter(elem);
                                           options.durations_.emplace_back(getter.Int32Or(0));
                                       });
        }
        void Transition::setTimingFunctionFromNapi(const napi_value& napi_value) {
            if (napi_value == nullptr) {
                return;
            }
            if (!has_value()) {
                set(TransitionOption());
            }

            auto& options = value();
            NapiGetter::ForEachInArray(napi_value,
                                       [&](const ::napi_value& elem,
                                           const uint32_t& _) {
                                           NapiGetter getter(elem);
                                           options.timing_functions_.emplace_back(getter.StringOr("ease"));
                                       });
        }
        void Transition::setDelayFromNapi(const napi_value& napi_value) {
            if (napi_value == nullptr) {
                return;
            }
            if (!has_value()) {
                set(TransitionOption());
            }

            auto& options = value();
            NapiGetter::ForEachInArray(napi_value,
                                       [&](const ::napi_value& elem,
                                           const uint32_t& _) {
                                           NapiGetter getter(elem);
                                           options.delays_.emplace_back(getter.Int32Or(0));
                                       });
        }

        std::shared_ptr<TransitionParam> Transition::getTransitionParam(
            CSSProperty::Type prop_type, const Stylesheet& sheet) {
            if (!has_value()) {
                return nullptr;
            }
            auto& option = value();
            int size = option.properties_.size();
            if (option.properties_.size() > option.durations_.size()) {
                size = option.durations_.size();
            }
            for (int index = size - 1; index >= 0; --index) {
                int check = checkPropType(prop_type, option.properties_[index]);
                if (check < 0) { // 终止
                    return nullptr;
                } else if (check == 0) { // check下一个属性
                    continue;
                }
                // duration为0，不进行动画
                if (option.durations_[index] <= 0) {
                    return nullptr;
                }
                auto param = std::make_shared<TransitionParam>();
                bool suc = getStyleValue(prop_type, sheet, param->prop_value_);
                if (!suc) {
                    TARO_LOG_ERROR("TaroTransition", "getStyleValue failed, prop_type:%{public}d",
                                   prop_type);
                    return nullptr;
                }
                param->prop_type_ = prop_type;
                param->duration_ = option.durations_[index];
                param->delay = option.delays_.size() > index ? option.delays_[index] : 0;
                param->timing_function_ = option.timing_functions_.size() > index ? option.timing_functions_[index] : "ease";
                return param;
            }
            return nullptr;
        }

        bool Transition::isActivePropType(CSSProperty::Type prop_type) {
            if (!has_value()) {
                return false;
            }

            auto& style_props = value().properties_;
            for (const auto& style_prop : style_props) {
                if (checkPropType(prop_type, style_prop)) {
                    return true;
                }
            }
            return false;
        }

        bool Transition::checkPropType(CSSProperty::Type prop_type, int trans_property) {
            // -1 所有属性
            if (trans_property < 0) {
                return true;
            }

            CSSProperty::Type trans_type = static_cast<CSSProperty::Type>(trans_property);
            // 属性匹配，可创建动画
            if (prop_type == trans_type) {
                return true;
            }

            const auto prop_type_iter = map_multi_property_.find(prop_type);
            // 属性不匹配，但是属于子属性
            // 如prop_type:border_left_width trans_property:border_width
            if (prop_type_iter != map_multi_property_.end() && prop_type_iter->second == trans_type) {
                return true;
            }

            // 不匹配属性
            return false;
        }

        bool Transition::getStyleValue(CSSProperty::Type prop_type, const Stylesheet& sheet, KeyframeValue& value) {
            switch (prop_type) {
                case CSSProperty::Type::Height: {
                    if (!sheet.height.has_value()) {
                        return false;
                    }
                    value = sheet.height.value();
                } break;
                case CSSProperty::Type::Width: {
                    if (!sheet.width.has_value()) {
                        return false;
                    }
                    value = sheet.width.value();
                } break;
                case CSSProperty::Type::MarginBottom: {
                    value = sheet.marginBottom.value_or(0);
                } break;
                case CSSProperty::Type::MarginLeft: {
                    value = sheet.marginLeft.value_or(0);
                } break;
                case CSSProperty::Type::MarginRight: {
                    value = sheet.marginRight.value_or(0);
                } break;
                case CSSProperty::Type::MarginTop: {
                    value = sheet.marginTop.value_or(0);
                } break;
                case CSSProperty::Type::PaddingBottom: {
                    value = sheet.paddingBottom.value_or(0);
                } break;
                case CSSProperty::Type::PaddingLeft: {
                    value = sheet.paddingLeft.value_or(0);
                } break;
                case CSSProperty::Type::PaddingRight: {
                    value = sheet.paddingRight.value_or(0);
                } break;
                case CSSProperty::Type::PaddingTop: {
                    value = sheet.paddingTop.value_or(0);
                } break;
                case CSSProperty::Type::BorderBottomWidth: {
                    value = sheet.borderBottomWidth.value_or(0);
                } break;
                case CSSProperty::Type::BorderRightWidth: {
                    value = sheet.borderRightWidth.value_or(0);
                } break;
                case CSSProperty::Type::BorderLeftWidth: {
                    value = sheet.borderLeftWidth.value_or(0);
                } break;
                case CSSProperty::Type::BorderTopWidth: {
                    value = sheet.borderTopWidth.value_or(0);
                } break;
                case CSSProperty::Type::Color: {
                    value = (static_cast<double>(sheet.color.value_or(0x00000000)));
                } break;
                case CSSProperty::Type::BackgroundColor: {
                    value = (static_cast<double>(sheet.backgroundColor.value_or(0x00000000)));
                } break;
                case CSSProperty::Type::BorderTopColor: {
                    value = (static_cast<double>(sheet.borderTopColor.value_or(0x00000000)));
                } break;
                case CSSProperty::Type::BorderRightColor: {
                    value = (static_cast<double>(sheet.borderRightColor.value_or(0x00000000)));
                } break;
                case CSSProperty::Type::BorderBottomColor: {
                    value = (static_cast<double>(sheet.borderBottomColor.value_or(0x00000000)));
                } break;
                case CSSProperty::Type::BorderLeftColor: {
                    value = (static_cast<double>(sheet.borderLeftColor.value_or(0x00000000)));
                } break;
                case CSSProperty::Type::Transform: {
                    if (!sheet.transform.has_value()) {
                        value = TaroCSSOM::TaroStylesheet::TransformParam::staticTransformSystemValue();
                    } else {
                        value = std::make_shared<TransformParam>(sheet.transform.value());
                    }
                } break;
                case CSSProperty::Type::Opacity: {
                    value = static_cast<double>(sheet.opacity.value_or(1));
                } break;
                default:
                    return false;
            }
            return true;
        }

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime
