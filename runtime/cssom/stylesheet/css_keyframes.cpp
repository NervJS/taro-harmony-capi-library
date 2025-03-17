#include "css_keyframes.h"

#include "helper/TaroLog.h"
#include "helper/Time.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/transform.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
int CSSKeyframes::init(napi_value config, CSSMediasPtr medias) {
    version_ = TaroHelper::TaroTime::getCurrentMsTime();
    if (config == nullptr || NapiGetter::GetValueTypeFromNode(config) != napi_object) {
        return -1;
    }

    NapiGetter::ForEachInArray(config, [&](const napi_value& napi_keyframes, const uint32_t&) {
        auto media_getter = NapiGetter::GetProperty(napi_keyframes, "media");
        auto media_id = media_getter.Int32Or(0);
        if (medias != nullptr && !medias->match(media_id)) {
            TARO_LOG_DEBUG("CSSMedia", "media %{public}d", media_id);
            return;
        }

        auto keyframes = parseKeyframes(napi_keyframes);
        if (keyframes == nullptr || keyframes->name_.empty()) {
            return;
        }
        keyframes->media_id_ = media_id;
        // 重复的情况，加载使用最后一个media的数据
        auto iter = name_to_keyframes_.find(keyframes->name_);
        if (iter != name_to_keyframes_.end() && media_id<iter->second->media_id_> media_id) {
            return;
        }
        name_to_keyframes_[keyframes->name_] = keyframes;
    });
    return 0;
}

const KeyframesInfoPtr CSSKeyframes::getAnimKeyframes(const std::string& name) const {
    const auto iter = name_to_keyframes_.find(name);
    if (iter == name_to_keyframes_.end()) {
        return nullptr;
    }
    return iter->second;
}

KeyframesInfoPtr CSSKeyframes::parseKeyframes(const napi_value& napi_keyframes) {
    auto name_getter = NapiGetter::GetProperty(napi_keyframes, "name");
    if (name_getter.GetType() != napi_string || name_getter.StringOr("").empty()) {
        TARO_LOG_ERROR("TaroAnimation", "keyframes name is invalid");
        return nullptr;
    }

    auto keyframes = std::make_shared<KeyframesInfo>();
    keyframes->name_ = name_getter.StringOr("");

    auto kf_value_getter = NapiGetter::GetProperty(napi_keyframes, "keyframe");
    if (kf_value_getter.GetType() != napi_object) {
        TARO_LOG_ERROR("TaroAnimation", "keyframes:%{public}s is empty",
                       keyframes->name_.c_str());
        return nullptr;
    }

    kf_value_getter.ForEachInArray([this, &keyframes](const napi_value& napi_keyframe, const uint32_t&) {
        auto keyframe = parseKeyframe(napi_keyframe);
        if (keyframe.has_value()) {
            keyframes->keyframes_.emplace_back(keyframe.value());
        }
    });

    return keyframes;
}

Optional<KeyFrame> CSSKeyframes::parseKeyframe(const ::napi_value& napi_keyframe) {
    Optional<KeyFrame> keyframe;
    auto per_getter = NapiGetter::GetProperty(napi_keyframe, "percent");
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
                case CSSProperty::Type::Height:
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

uint64_t CSSKeyframes::getVersion() const {
    return version_;
}
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet