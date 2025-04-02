/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "utils.h"

#include "IAttribute.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/cssom/dimension/context.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {
        ArkUI_FlexDirection getFlexDirection(
            const std::shared_ptr<Stylesheet>& stylesheet) {
            // 根据direction判断使用row/column
            TaroStylesheet::FlexDirection* flexDirection = &(stylesheet->flexDirection);
            TaroStylesheet::Display* display = &(stylesheet->display);

            // 判别布局方向，决定使用什么布局
            ArkUI_FlexDirection flex_direction;
            if (display->has_value() && display->value() == PropertyType::Display::Flex) {
                flex_direction = flexDirection->has_value() ? flexDirection->value()
                                                            : ARKUI_FLEX_DIRECTION_ROW;
            } else {
                flex_direction = ARKUI_FLEX_DIRECTION_COLUMN;
            }
            return flex_direction;
        }

        TaroHelper::Optional<float> getFloat(TaroRuntime::NapiGetter& getter) {
            TaroHelper::Optional<float> res;
            napi_valuetype type;
            getter.GetType(type);
            if (type == napi_number) {
                auto getterValue = getter.Double();
                if (getterValue.has_value()) {
                    res.set(static_cast<float>(getterValue.value()));
                }
            } else if (type == napi_string) {
                auto getterValue = getter.String();
                if (getterValue.has_value()) {
                    try {
                        res.set(std::stof(getterValue.value()));
                    } catch (...) {
                        TARO_LOG_DEBUG("getFloat", "getFloat failed value:%{public}s", getterValue.value().c_str());
                    }
                }
            }
            return res;
        }

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime
