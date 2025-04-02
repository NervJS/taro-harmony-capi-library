/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_TRANSFORM_ORIGIN_H
#define HARMONY_LIBRARY_TRANSFORM_ORIGIN_H

#include <arkui/native_type.h>
#include <napi/native_api.h>
#include <string_view>
#include "helper/Optional.h"
#include "helper/scalar.h"
#include "runtime/cssom/stylesheet/attribute_base.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

struct TransformOriginData {
    Dimension x;
    Dimension y;
    Dimension z;
    Dimension px;
    Dimension py;
    Dimension pz;
};
inline bool operator==(const TransformOriginData& lhs, const TransformOriginData& rhs) {
    return lhs.x == rhs.x &&
           lhs.y == rhs.y &&
           lhs.z == rhs.z &&
           lhs.px == rhs.px &&
           lhs.py == rhs.py &&
           lhs.pz == rhs.pz;
}

inline bool operator!=(const TransformOriginData& lhs, const TransformOriginData& rhs) {
    return !(lhs == rhs);
}

class TransformOrigin : public AttributeBase<TransformOriginData> {
    public:
    TransformOrigin() = default;

    void setValueFromStringView(std::string_view) override;
    void setValueFromNapi(const napi_value&) override;
    void assign(const AttributeBase<TransformOriginData>&) override;
    bool isEqual(const Optional<TransformOriginData>& other) const;
    bool operator!=(const TransformOriginData& other) {
        return !this->isEqual(other);
    }

    private:
    TransformOriginData data_;
};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // HARMONY_LIBRARY_TRANSFORM_ORIGIN_H
