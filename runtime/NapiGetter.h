/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TESTNDK_NAPIGETTER_H
#define TESTNDK_NAPIGETTER_H

#include <functional>
#include <string>
#include <vector>
#include <js_native_api.h>

#include "helper/Optional.h"

using namespace TaroHelper;

namespace TaroRuntime {
class NapiGetter {
    public:
    NapiGetter(napi_value value);
    virtual ~NapiGetter();

    NapiGetter GetValue();
    static NapiGetter GetValue(const napi_value &value);
    NapiGetter GetProperty(const char *propertyPath);
    static NapiGetter GetProperty(const napi_value &node,
                                  const char *propertyPath);
    /** @deprecated 可能导致性能降低 */
    static NapiGetter GetPropertyWithPath(const napi_value &node,
                                          const char *propertyPath);
    static std::vector<NapiGetter> GetAllPropertyNames(const napi_value &node);
    std::vector<NapiGetter> GetAllPropertyNames();

    static napi_valuetype GetValueTypeFromNode(napi_value node);
    static napi_value GetPropertyFromNode(const napi_value &node,
                                          const char *propertyName);
    static napi_value GetPropertyFromNode(const napi_value &node,
                                          const char *propertyName,
                                          napi_value &nodeValue);
    /** @deprecated 可能导致性能降低 */
    static napi_value GetPropertyFromNodeWithPath(const napi_value &node,
                                                  const char *propertyPath);
    static std::vector<napi_value> GetVectorFromNode(const napi_value &node);

    // 循环遍历数组
    static void ForEachInArray(
        const napi_value &node,
        std::function<void(const napi_value &, const uint32_t &)> callback);
    void ForEachInArray(
        std::function<void(const napi_value &, const uint32_t &)>);

    napi_valuetype GetType() const;
    void GetType(napi_valuetype &) const;
    napi_valuetype Type();
    TaroHelper::Optional<std::string> String();
    TaroHelper::Optional<std::string_view> StringView();
    std::string StringOr(std::string &default_value);
    std::string StringOr(const char *default_value);
    TaroHelper::Optional<int32_t> Int32();
    int32_t Int32Or(const int32_t &default_value);
    TaroHelper::Optional<uint32_t> UInt32();
    uint32_t UInt32(const uint32_t &);
    TaroHelper::Optional<double> Double();
    double DoubleOr(const double &default_value);
    double Double(const double &default_value);
    TaroHelper::Optional<bool> Bool();
    bool BoolOr(const bool &default_value);
    TaroHelper::Optional<bool> BoolNull();
    bool BoolNullOr(const bool &default_value);

    TaroHelper::Optional<std::vector<NapiGetter>> Vector();

    napi_value GetNapiValue() const;

    private:
    napi_value value_;
    napi_status status_;
};
} // namespace TaroRuntime

#endif // TESTNDK_NAPIGETTER_H
