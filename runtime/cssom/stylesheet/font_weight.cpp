//
// Created on 2024/4/25.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "./font_weight.h"

#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {
        static const std::unordered_map<int, ArkUI_FontWeight> FONT_WEIGHT_STRING_MAP =
            {
                {100, ARKUI_FONT_WEIGHT_W100},
                {200, ARKUI_FONT_WEIGHT_W200},
                {300, ARKUI_FONT_WEIGHT_W300},
                {400, ARKUI_FONT_WEIGHT_W400},
                {500, ARKUI_FONT_WEIGHT_W500},
                {600, ARKUI_FONT_WEIGHT_W600},
                {700, ARKUI_FONT_WEIGHT_W700},
                {800, ARKUI_FONT_WEIGHT_W800},
                {900, ARKUI_FONT_WEIGHT_W900},
        };

        static const std::unordered_map<std::string, ArkUI_FontWeight>
            FONT_WEIGHT_NUMBER_MAP = {{"bold", ARKUI_FONT_WEIGHT_BOLD},
                                      {"normal", ARKUI_FONT_WEIGHT_NORMAL},
                                      {"bolder", ARKUI_FONT_WEIGHT_BOLDER},
                                      {"lighter", ARKUI_FONT_WEIGHT_LIGHTER},
                                      {"medium", ARKUI_FONT_WEIGHT_MEDIUM},
                                      {"regular", ARKUI_FONT_WEIGHT_REGULAR}};

        FontWeight::FontWeight(const napi_value &napiValue) {
            setValueFromNapi(napiValue);
        }

        void FontWeight::setValueFromNapi(const napi_value &napiValue) {
            NapiGetter getter(napiValue);
            napi_valuetype type;
            getter.GetType(type);
            if (type == napi_number) {
                auto getterValue = getter.Int32();
                if (getterValue.has_value()) {
                    if (getterValue.value() >= 100) {
                        auto it = FONT_WEIGHT_STRING_MAP.find(getterValue.value());
                        if (it != FONT_WEIGHT_STRING_MAP.end()) {
                            this->set(it->second);
                        } else {
                            this->set(ARKUI_FONT_WEIGHT_NORMAL); // Assuming 'normal' as default
                        }
                    } else {
                        this->set(static_cast<ArkUI_FontWeight>(getterValue.value()));
                    }
                }
            } else if (type == napi_string) {
                auto getterValue = getter.String();
                if (getterValue.has_value()) {
                    auto it = FONT_WEIGHT_NUMBER_MAP.find(getterValue.value());
                    if (it != FONT_WEIGHT_NUMBER_MAP.end()) {
                        this->set(it->second);
                    } else {
                        this->set(ARKUI_FONT_WEIGHT_NORMAL); // Assuming 'normal' as default
                    }
                }
            }
        }

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime
