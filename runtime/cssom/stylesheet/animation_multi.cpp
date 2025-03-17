#include "animation_multi.h"

#include "helper/TaroLog.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/transform.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void AnimationMulti::setNameFromNapi(const napi_value& napi_val) {
    if (napi_val == nullptr) {
        return;
    }
    if (!has_value()) {
        set(AnimationMultiOption());
    }
    auto& options = value();
    NapiGetter::ForEachInArray(napi_val,
                               [&](const ::napi_value& elem, const uint32_t& _) {
                                   NapiGetter getter(elem);
                                   options.names_.emplace_back(getter.StringOr(""));
                               });
}

void AnimationMulti::setKeyFramesFromNapi(const napi_value& napi_val) {
    if (napi_val == nullptr) {
        return;
    }
    if (!has_value()) {
        set(AnimationMultiOption());
    }
    auto& options = value();
    NapiGetter::ForEachInArray(napi_val,
                               [&](const ::napi_value& elem, const uint32_t& _) {
                                   NapiGetter getter(elem);
                                   std::vector<KeyFrame> vec_keyframe;
                                   getter.ForEachInArray([&](const napi_value& napi_keyframe, const uint32_t&) {
                                       auto keyframe = parseKeyframe(napi_keyframe);
                                       if (keyframe.has_value()) {
                                           vec_keyframe.emplace_back(keyframe.value());
                                       }
                                   });
                                   options.keyframes_.emplace_back(std::move(vec_keyframe));
                               });
}

Optional<KeyFrame> AnimationMulti::parseKeyframe(const ::napi_value& napi_keyframe) {
    Optional<KeyFrame> keyframe;
    auto per_getter = NapiGetter::GetProperty(napi_keyframe, "percentage");
    auto event_getter = NapiGetter::GetProperty(napi_keyframe, "event");
    if (per_getter.Type() != napi_number || !per_getter.Double().has_value() || event_getter.Type() != napi_object) {
        TARO_LOG_ERROR("TaroAnimation", "Invalid keyframe type");
        return keyframe;
    }

    keyframe.set(KeyFrame());
    auto& keyframe_data = keyframe.value();
    keyframe_data.percent = per_getter.Double(0.0);
    event_getter.ForEachInArray([&](const napi_value& item, const uint32_t&) {
        NapiGetter item_getter(item);
        auto vec_value = item_getter.NapiGetter::Vector();
        if (!vec_value.has_value() || vec_value.value().size() < 2) {
            return;
        }
        auto key_getter = vec_value.value()[0];
        auto value_getter = vec_value.value()[1];
        if (key_getter.Type() == napi_number && value_getter.Type() != napi_undefined) {
            CSSProperty::Type type = static_cast<CSSProperty::Type>(key_getter.Int32Or(0));
            switch (type) {
                case CSSProperty::Type::Left:
                case CSSProperty::Type::Right:
                case CSSProperty::Type::Top:
                case CSSProperty::Type::Bottom:
                case CSSProperty::Type::Width:
                case CSSProperty::Type::MarginBottom:
                case CSSProperty::Type::MarginLeft:
                case CSSProperty::Type::MarginRight:
                case CSSProperty::Type::MarginTop:
                case CSSProperty::Type::PaddingBottom:
                case CSSProperty::Type::PaddingLeft:
                case CSSProperty::Type::PaddingRight:
                case CSSProperty::Type::PaddingTop:
                case CSSProperty::Type::BorderBottomWidth:
                case CSSProperty::Type::BorderRightWidth:
                case CSSProperty::Type::BorderLeftWidth:
                case CSSProperty::Type::BorderTopWidth: {
                    AttributeBase<Dimension> lengthAttr;
                    lengthAttr.setValueFromNapi(value_getter.GetNapiValue());
                    if (lengthAttr.has_value()) {
                        keyframe_data.params.emplace_back(type, lengthAttr.value());
                    }
                } break;
                case CSSProperty::Type::BorderTopLeftRadius:
                case CSSProperty::Type::BorderTopRightRadius:
                case CSSProperty::Type::BorderBottomLeftRadius:
                case CSSProperty::Type::BorderBottomRightRadius:
                case CSSProperty::Type::BackgroundPositionX:
                case CSSProperty::Type::BackgroundPositionY: {
                    AttributeBase<Dimension> tLenthAttr;
                    tLenthAttr.setValueFromNapi(value_getter.GetNapiValue());
                    if (tLenthAttr.has_value()) {
                        keyframe_data.params.emplace_back(type, tLenthAttr.value());
                    }
                } break;
                case CSSProperty::Type::Color:
                case CSSProperty::Type::BackgroundColor:
                case CSSProperty::Type::BorderTopColor:
                case CSSProperty::Type::BorderRightColor:
                case CSSProperty::Type::BorderBottomColor:
                case CSSProperty::Type::BorderLeftColor: {
                    AttributeBase<TColor> colorAttr;
                    colorAttr.setValueFromNapi(value_getter.GetNapiValue());
                    if (colorAttr.has_value()) {
                        keyframe_data.params.emplace_back(
                            type, static_cast<double>(colorAttr.value()));
                    }
                } break;

                case CSSProperty::Type::Opacity: {
                    AttributeBase<float> numAttr;
                    numAttr.setValueFromNapi(value_getter.GetNapiValue());
                    if (numAttr.has_value()) {
                        keyframe_data.params.emplace_back(
                            type, static_cast<double>(numAttr.value()));
                    }
                } break;

                case CSSProperty::Type::Transform: {
                    Transform transform;
                    transform.setValueFromNapi(value_getter.GetNapiValue());
                    if (transform.has_value()) {
                        keyframe_data.params.emplace_back(
                            type, std::make_shared<TransformParam>(transform.value()));
                    }
                } break;
                default:
                    break;
                    ;
            }
        }
    });

    return keyframe;
}

void AnimationMulti::setDurationFromNapi(const napi_value& napi_val) {
    if (napi_val == nullptr) {
        return;
    }
    if (!has_value()) {
        set(AnimationMultiOption());
    }
    auto& options = value();
    NapiGetter::ForEachInArray(napi_val,
                               [&](const ::napi_value& elem, const uint32_t& _) {
                                   NapiGetter getter(elem);
                                   options.durations_.emplace_back(getter.Int32Or(0));
                               });
}

void AnimationMulti::setTimingFunctionFromNapi(const napi_value& napi_val) {
    if (napi_val == nullptr) {
        return;
    }
    if (!has_value()) {
        set(AnimationMultiOption());
    }
    auto& options = value();
    NapiGetter::ForEachInArray(napi_val,
                               [&](const ::napi_value& elem, const uint32_t& _) {
                                   NapiGetter getter(elem);
                                   options.timingFunctions_.emplace_back(getter.StringOr("ease"));
                               });
}

void AnimationMulti::setFillModeFromNapi(const napi_value& napi_val) {
    if (napi_val == nullptr) {
        return;
    }
    if (!has_value()) {
        set(AnimationMultiOption());
    }
    auto& options = value();
    NapiGetter::ForEachInArray(napi_val,
                               [&](const ::napi_value& elem, const uint32_t& _) {
                                   NapiGetter getter(elem);
                                   options.fill_modes_.emplace_back(getter.StringOr("none"));
                               });
}

void AnimationMulti::setDelayFromNapi(const napi_value& napi_val) {
    if (napi_val == nullptr) {
        return;
    }
    if (!has_value()) {
        set(AnimationMultiOption());
    }
    auto& options = value();
    NapiGetter::ForEachInArray(napi_val,
                               [&](const ::napi_value& elem, const uint32_t& _) {
                                   NapiGetter getter(elem);
                                   options.delays_.emplace_back(getter.Int32Or(0));
                               });
}

void AnimationMulti::setIterationCountFromNapi(const napi_value& napi_val) {
    if (napi_val == nullptr) {
        return;
    }
    if (!has_value()) {
        set(AnimationMultiOption());
    }
    auto& options = value();
    NapiGetter::ForEachInArray(napi_val,
                               [&](const ::napi_value& elem, const uint32_t& _) {
                                   NapiGetter getter(elem);
                                   options.interationCounts_.emplace_back(getter.Int32Or(1));
                               });
}

Optional<AnimationOption> AnimationMulti::getAnimation(size_t index) const {
    Optional<AnimationOption> option;
    if (!has_value() || value().names_.size() <= index || value().durations_.size() <= index) {
        return option;
    }
    option.set(AnimationOption());
    auto& option_data = option.value();
    option_data.name = value().names_[index];
    option_data.duration = value().durations_[index];
    option_data.timingFunction = value().timingFunctions_.size() <= index ? "ease" : value().timingFunctions_[index];
    option_data.delay = value().delays_.size() <= index ? 0 : value().delays_[index];
    option_data.interationCount = value().interationCounts_.size() <= index ? 1 : value().interationCounts_[index];
    option_data.direction = value().directions_.size() <= index ? "normal" : value().directions_[index];
    option_data.fill_mode = value().fill_modes_.size() <= index ? "none" : value().fill_modes_[index];
    return option;
}

Optional<AnimationOption> AnimationMulti::getAnimation(const std::string& name) const {
    if (has_value()) {
        const auto& data = value();
        for (size_t idx = 0; idx < data.names_.size(); idx++) {
            if (name == data.names_[idx]) {
                return getAnimation(idx);
            }
        }
    }
    return Optional<AnimationOption>();
}

void AnimationMulti::assign(const AttributeBase<AnimationMultiOption>& item) {
    if (!item.has_value()) {
        return;
    } else if (!this->has_value() && item.has_value()) {
        this->set(item);
    } else {
        auto option = this->value();
        if (item.has_value()) {
            auto& itemVal = item.value();
            if (itemVal.delays_.size()) {
                option.delays_ = itemVal.delays_;
            }
            if (itemVal.directions_.size()) {
                option.directions_ = itemVal.directions_;
            }
            if (itemVal.durations_.size()) {
                option.durations_ = itemVal.durations_;
            }
            if (itemVal.fill_modes_.size()) {
                option.fill_modes_ = itemVal.fill_modes_;
            }
            if (itemVal.interationCounts_.size()) {
                option.interationCounts_ = itemVal.interationCounts_;
            }
            if (itemVal.keyframes_.size()) {
                option.keyframes_ = itemVal.keyframes_;
            }
            if (itemVal.names_.size()) {
                option.names_ = itemVal.names_;
            }
            if (itemVal.timingFunctions_.size()) {
                option.timingFunctions_ = itemVal.timingFunctions_;
            }
            this->set(option);
        }
    }
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet