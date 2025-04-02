/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_BACKGROUND_IMAGE_H
#define TARO_CAPI_HARMONY_DEMO_BACKGROUND_IMAGE_H

#include <string>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "./attribute_base.h"
#include "./css_property.h"
#include "./params/background_image_param/background_image_param.h"
#include "helper/Optional.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

class BackgroundImage : public AttributeBase<BackgroundImageItem> {
    public:
    BackgroundImage() = default;
    BackgroundImage(const napi_value &);
    void setValueFromNapi(const napi_value &) override;

    void setValueFromStringView(std::string_view value) override;

    private:
    BackgroundImageItem item_;
};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_BACKGROUND_IMAGE_H
