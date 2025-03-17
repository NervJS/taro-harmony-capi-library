//
// Created on 2024/05/28.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_TRANSFORM_H
#define TARO_CAPI_HARMONY_DEMO_TRANSFORM_H

#include <string_view>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "./params/transform_param/transform_param.h"
#include "attribute_base.h"
#include "helper/Optional.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

class Transform : public AttributeBase<TransformParam> {
    public:
    Transform() = default;
    Transform(const napi_value &);
    void setValueFromStringView(std::string_view str) override;
    void setValueFromNapi(const napi_value &) override;
    void assign(const AttributeBase<TransformParam> &item) override;

    bool isEqual(const TaroHelper::Optional<TransformParam> &other) const;

    static std::shared_ptr<TransformItemBase> parseTransformItem(ETransformType type, const napi_value &elem);
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_TRANSFORM_H
