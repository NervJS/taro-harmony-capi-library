/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
    const std::shared_ptr<Stylesheet>& stylesheet);

TaroHelper::Optional<float> getFloat(TaroRuntime::NapiGetter& getter);
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_UTILS_H
