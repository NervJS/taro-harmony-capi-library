//
// Created on 2024/4/23.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_UTILS_H
#define TARO_CAPI_HARMONY_DEMO_UTILS_H

#include <string>
#include <arkui/native_type.h>

#include "css_property.h"
#include "helper/Optional.h"
#include "runtime/NapiGetter.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
class Stylesheet;
ArkUI_FlexDirection getFlexDirection(
    const std::shared_ptr<Stylesheet> &stylesheet);

TaroHelper::Optional<float> getFloat(TaroRuntime::NapiGetter &getter);
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_UTILS_H
