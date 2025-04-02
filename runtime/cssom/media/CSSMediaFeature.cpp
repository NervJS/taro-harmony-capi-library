/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "CSSMediaFeature.h"

#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/cssom/stylesheet/attribute_base.h"
#include "runtime/cssom/stylesheet/utils.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    int CSSMediaFeature::init(const napi_value& config) {
        NapiGetter getter(config);
        if (getter.GetType() != napi_object) {
            TARO_LOG_ERROR("CSSMedia", "parse feature failed");
            return -1;
        }
        auto arr_getter = getter.Vector();
        if (!arr_getter.has_value() || arr_getter.value().size() < 2 || arr_getter.value()[0].GetType() != napi_number || arr_getter.value()[1].GetType() != napi_number) {
            TARO_LOG_ERROR("CSSMedia", "parse feature arr failed");
            return -2;
        }

        feature_id_ = static_cast<MediaFeatureId>(arr_getter.value()[0].Int32Or(0));
        comp_type_ = static_cast<MediaCompType>(arr_getter.value()[1].Int32Or(0));

        if (arr_getter.value().size() > 2) {
            int ret = init_value(arr_getter.value()[2]);
            if (ret != 0) {
                TARO_LOG_ERROR("CSSMedia", "media(%{public}d) init value fail", feature_id_);
            }
            return ret;
        }
        return 0;
    }

    int CSSMediaFeature::init_value(NapiGetter& getter) {
        switch (feature_id_) {
            case MediaFeatureId::Height:
            case MediaFeatureId::Width:
            case MediaFeatureId::DeviceHeight:
            case MediaFeatureId::DeviceWidth: {
                TaroStylesheet::AttributeBase<Dimension> dim;
                // 默认是数字，但实际它是px，setValueFromNapi对于number会当成vp处理，所以下面需要 / 设计稿比例，换算回对应的px
                dim.setValueFromNapi(getter.GetNapiValue());
                if (!dim.has_value()) {
                    return -1;
                }
                if (auto val = dim.value().ParseToVp(); val.has_value()) {
                    value_ = val.value() / DimensionContext::GetCurrentContext()->design_ratio_;
                } else {
                    return -1;
                }

            } break;
            case MediaFeatureId::Resolution:
            case MediaFeatureId::AspectRatio:
            case MediaFeatureId::DeviceAspectRatio: {
                auto d_value = getter.Double();
                if (!d_value.has_value()) {
                    return -1;
                }
                value_ = d_value.value();
            } break;
            case MediaFeatureId::Orientation: {
                auto s_value = getter.String();
                if (!s_value.has_value()) {
                    return -1;
                }
                if (s_value.value() == "portrait") {
                    value_ = 0.0f;
                } else if (s_value.value() == "landscape") {
                    value_ = 1.0f;
                } else {
                    return -1;
                }
            } break;
            default:
                return -1;
        }
        return 0;
    }

    bool CSSMediaFeature::calc_match() {
        match_ = false;
        double cur_val = 0;
        auto dimensionContext = DimensionContext::GetCurrentContext();
        switch (feature_id_) {
            case MediaFeatureId::Height:
                cur_val = dimensionContext->viewport_height_ * dimensionContext->density_pixels_;
                break;
            case MediaFeatureId::Width:
                cur_val = dimensionContext->viewport_width_ * dimensionContext->density_pixels_;
                break;
            case MediaFeatureId::DeviceHeight:
                cur_val = dimensionContext->device_height_ * dimensionContext->density_pixels_;
                break;
            case MediaFeatureId::DeviceWidth:
                cur_val = dimensionContext->device_width_ * dimensionContext->density_pixels_;
                break;
            case MediaFeatureId::AspectRatio:
                cur_val = dimensionContext->viewport_width_ / dimensionContext->viewport_height_;
                break;
            case MediaFeatureId::DeviceAspectRatio: {
                cur_val = dimensionContext->device_width_ / dimensionContext->device_height_;
            } break;
            case MediaFeatureId::Resolution: {
                cur_val = dimensionContext->density_dpi_;
            } break;
            case MediaFeatureId::Orientation: {
                auto orientation = dimensionContext->orientation;
                cur_val = (orientation == Orientation::LANDSCAPE || orientation == Orientation::LANDSCAPE_INVERTED) ? 1.0f : 0.0f;
            } break;
            default:
                TARO_LOG_ERROR("CSSMedia", "feature %{public}d is not support", feature_id_);
                return false;
        }

        //
        match_ = compare_double(cur_val);
        return match_;
    }

    int CSSMediaFeature::compare_double(double cur_val) {
        switch (comp_type_) {
            case MediaCompType::Equal: {
                return NearEqual(cur_val, value_);
            } break;
            case MediaCompType::GreaterThan: {
                return GreatNotEqual(cur_val, value_);
            } break;
            case MediaCompType::GreaterThanEqual: {
                return GreatOrEqual(cur_val, value_);
            } break;
            case MediaCompType::LessThan: {
                return LessNotEqual(cur_val, value_);
            } break;
            case MediaCompType::LessThanEqual: {
                return LessOrEqual(cur_val, value_);
            } break;
        }
        return false;
    }

} // namespace TaroCSSOM
} // namespace TaroRuntime
