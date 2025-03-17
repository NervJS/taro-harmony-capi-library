//
// Created on 2024/5/21.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_COMMON_H
#define TARO_CAPI_HARMONY_DEMO_COMMON_H

#include <string>
#include <variant>

#include "helper/Optional.h"
#include "runtime/cssom/dimension/dimension.h"

namespace TaroRuntime {

using LengthValue = std::variant<double, std::string>;

namespace TaroCSSOM {
    namespace TaroStylesheet {

        class BoundingBox {
            public:
            TaroHelper::Optional<Dimension> left;
            TaroHelper::Optional<Dimension> top;
            TaroHelper::Optional<Dimension> right;
            TaroHelper::Optional<Dimension> bottom;

            void traver(std::function<void(const std::string &, const Dimension &)>);

            bool operator==(const BoundingBox &other) const;
            bool operator!=(const BoundingBox &other) const;
            TaroHelper::Optional<Dimension> &operator[](const std::string &key);
        };

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_COMMON_H
