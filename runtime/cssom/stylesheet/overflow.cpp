/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./overflow.h"

#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {
        Overflow::Overflow(const napi_value& napiValue) {
            setValueFromNapi(napiValue);
        }

        void Overflow::setValueFromNapi(const napi_value& napiValue) {
            NapiGetter getter(napiValue);
            napi_valuetype type;
            getter.GetType(type);

            if (type == napi_string) {
                auto getterValue = getter.String();
                if (getterValue.has_value()) {
                    if (getterValue.value() == "hidden") {
                        this->set(PropertyType::Overflow::Hidden);
                    } else {
                        this->set(PropertyType::Overflow::Visible);
                    }
                }
            } else if (type == napi_number) {
                auto getterValue = getter.Int32();
                if (getterValue.has_value()) {
                    this->set(static_cast<PropertyType::Overflow>(getterValue.value()));
                }
            }
        }

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime
