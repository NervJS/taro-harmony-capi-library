/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "NapiGetter.h"

#include "helper/StringUtils.h"
#include "helper/TaroLog.h"
#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {

NapiGetter::NapiGetter(napi_value value)
    : value_(value) {}

NapiGetter::~NapiGetter() {}

NapiGetter NapiGetter::GetValue() {
    return GetValue(value_);
}

NapiGetter NapiGetter::GetValue(const napi_value& value) {
    NapiGetter napi_getter(value);
    return napi_getter;
}

NapiGetter NapiGetter::GetProperty(const char* propertyName) {
    return GetProperty(value_, propertyName);
}

NapiGetter NapiGetter::GetProperty(const napi_value& node, const char* propertyName) {
    napi_value value = GetPropertyFromNode(node, propertyName);
    return NapiGetter::GetValue(value);
}

NapiGetter NapiGetter::GetPropertyWithPath(const napi_value& node,
                                           const char* propertyPath) {
    napi_value value = GetPropertyFromNodeWithPath(node, propertyPath);
    return NapiGetter::GetValue(value);
}

std::vector<NapiGetter> NapiGetter::GetAllPropertyNames() {
    return GetAllPropertyNames(value_);
};

std::vector<NapiGetter> NapiGetter::GetAllPropertyNames(const napi_value& node) {
    napi_value propertyNames = nullptr;
    std::vector<NapiGetter> nameVector;
    if (node == nullptr || GetValueTypeFromNode(node) != napi_object)
        return nameVector;

    napi_get_all_property_names(
        NativeNodeApi::env, node, napi_key_collection_mode::napi_key_own_only,
        napi_key_filter::napi_key_all_properties,
        napi_key_conversion::napi_key_keep_numbers, &propertyNames);
    if (propertyNames != nullptr) {
        uint32_t length;
        napi_get_array_length(NativeNodeApi::env, propertyNames, &length);
        napi_status status;
        for (uint32_t i = 0; i < length; i++) {
            napi_value nameKey;
            status = napi_get_element(NativeNodeApi::env, propertyNames, i, &nameKey);
            if (status != napi_ok) {
                continue;
            }
            NapiGetter napi_getter(nameKey);
            nameVector.push_back(napi_getter);
        }
    }
    return nameVector;
}

std::vector<napi_value> NapiGetter::GetVectorFromNode(const napi_value& node) {
    uint32_t length;
    napi_get_array_length(NativeNodeApi::env, node, &length);
    std::vector<napi_value> valueVector;
    valueVector.reserve(length);
    napi_status status;
    for (uint32_t i = 0; i < length; i++) {
        napi_value key;
        status = napi_get_element(NativeNodeApi::env, node, i, &key);
        if (status != napi_ok) {
            continue;
        }
        valueVector.push_back(key);
    }
    return valueVector;
}

napi_valuetype NapiGetter::GetValueTypeFromNode(napi_value node) {
    napi_valuetype napiType = napi_undefined;
    if (node != nullptr) {
        napi_typeof(TaroRuntime::NativeNodeApi::env, node, &napiType);
    }
    return napiType;
}

napi_value NapiGetter::GetPropertyFromNode(const napi_value& node, const char* propertyName) {
    napi_value value = nullptr;
    return GetPropertyFromNode(node, propertyName, value);
}

napi_value NapiGetter::GetPropertyFromNode(const napi_value& node, const char* propertyName, napi_value& nodeValue) {
    /* 更换为下面性能更优的方法
    napi_value nodeValue = nullptr;
    bool blsHasProperty;
    napi_has_named_property(NativeNodeApi::env, node, propertyName,
    &blsHasProperty); if (!blsHasProperty) { return nodeValue;
    }
    napi_get_named_property(NativeNodeApi::env, node, propertyName, &nodeValue);
    */
    if (node == nullptr) {
        return nullptr;
    }

    auto ret_status = napi_get_named_property(NativeNodeApi::env, node, propertyName, &nodeValue);
    if (ret_status != napi_ok) {
        return nullptr;
    }
    napi_valuetype result;
    napi_typeof(NativeNodeApi::env, nodeValue, &result);
    if (result == napi_undefined) {
        return nullptr;
    }
    return nodeValue;
}

napi_value NapiGetter::GetPropertyFromNodeWithPath(const napi_value& node,
                                                   const char* propertyPath) {
    napi_value nvCurrentNode = node;
    std::vector<std::string> vecProperties = StringUtils::split(propertyPath, ".");
    std::string strCurrentPath;
    for (const std::string& propertyName : vecProperties) {
        if (!strCurrentPath.empty()) {
            strCurrentPath += ".";
        }
        strCurrentPath += propertyName;
        bool blsHasProperty = false;
        napi_has_named_property(NativeNodeApi::env, nvCurrentNode,
                                propertyName.c_str(), &blsHasProperty);
        if (!blsHasProperty) {
            TARO_LOG_DEBUG("NapiGetter", "属性 '%{public}s' 找不到", strCurrentPath.c_str());
            return nullptr;
        }
        napi_value nvNextNode;
        napi_status nvStatus = napi_get_named_property(
            NativeNodeApi::env, nvCurrentNode, propertyName.c_str(), &nvNextNode);
        if (nvStatus != napi_ok) {
            TARO_LOG_DEBUG("NapiGetter", "属性 '%{public}s' 获取失败", strCurrentPath.c_str());
            return nullptr;
        }
        nvCurrentNode = nvNextNode;
    }
    return nvCurrentNode;
}

void NapiGetter::ForEachInArray(
    std::function<void(const napi_value&, const uint32_t&)> callback) {
    if (value_) {
        ForEachInArray(value_, callback);
    }
}

void NapiGetter::ForEachInArray(
    const napi_value& node,
    std::function<void(const napi_value&, const uint32_t&)> callback) {
    // 判断是否是 Array
    bool isArray = false;
    napi_status status =
        napi_is_array(TaroRuntime::NativeNodeApi::env, node, &isArray);
    if (isArray && status == napi_ok) {
        uint32_t length;
        napi_get_array_length(TaroRuntime::NativeNodeApi::env, node, &length);
        for (uint32_t i = 0; i < length; i++) {
            napi_value element;
            status =
                napi_get_element(TaroRuntime::NativeNodeApi::env, node, i, &element);
            callback(element, i);
        }
    }
}

napi_valuetype NapiGetter::GetType() const {
    napi_valuetype type;
    GetType(type);
    return type;
}

void NapiGetter::GetType(napi_valuetype& type) const {
    if (value_ != nullptr) {
        napi_typeof(TaroRuntime::NativeNodeApi::env, value_, &type);
    }
}

napi_valuetype NapiGetter::Type() {
    return GetValueTypeFromNode(value_);
}

Optional<bool> NapiGetter::Bool() {
    Optional<bool> optional;
    if (!value_) {
        return optional;
    }
    bool bool_value = false;
    status_ = napi_get_value_bool(NativeNodeApi::env, value_, &bool_value);
    if (status_ == napi_ok) {
        optional.set(bool_value);
    }
    return optional;
}

bool NapiGetter::BoolOr(const bool& default_value) {
    if (value_) {
        Optional<bool> optional = Bool();
        return optional.value_or(default_value);
    }
    return default_value;
}

Optional<bool> NapiGetter::BoolNull() {
    Optional<bool> optional;
    if (!value_) {
        return optional;
    }
    napi_valuetype type = Type();
    bool bool_value = false;
    if (type == napi_boolean) {
        status_ = napi_get_value_bool(NativeNodeApi::env, value_, &bool_value);
        if (status_ == napi_ok) {
            optional.set(bool_value);
            return optional;
        }
    } else if (type == napi_null) {
        optional.set(false);
        return optional;
    }
    return optional;
}

bool NapiGetter::BoolNullOr(const bool& default_value) {
    if (value_) {
        Optional<bool> optional = BoolNull();
        return optional.value_or(default_value);
    }
    return default_value;
}

Optional<std::string> NapiGetter::String() {
    Optional<std::string> optional;
    if (!value_) {
        return optional;
    }
    size_t strLength;
    status_ = napi_get_value_string_utf8(NativeNodeApi::env, value_, nullptr, 0,
                                         &strLength);
    if (status_ != napi_ok) {
        return optional;
    }
    std::string str_value(strLength, '\0');
    status_ = napi_get_value_string_utf8(
        NativeNodeApi::env, value_, &str_value[0], strLength + 1, &strLength);
    if (status_ == napi_ok) {
        optional.set(str_value);
    }
    return optional;
}

TaroHelper::Optional<std::string_view> NapiGetter::StringView() {
    Optional<std::string_view> optional;
    if (!value_) {
        return optional;
    }
    size_t strLength;
    status_ = napi_get_value_string_utf8(NativeNodeApi::env, value_, nullptr, 0,
                                         &strLength);
    if (status_ != napi_ok) {
        return optional;
    }
    std::shared_ptr<std::string> sp =
        std::make_shared<std::string>(strLength, '\0');
    status_ = napi_get_value_string_utf8(NativeNodeApi::env, value_, &(*sp)[0],
                                         strLength + 1, &strLength);
    if (status_ == napi_ok) {
        return std::string_view(*sp);
    }
}

std::string NapiGetter::StringOr(std::string& default_value) {
    if (value_) {
        Optional<std::string> optional = String();
        return optional.value_or(default_value);
    }
    return default_value;
}

std::string NapiGetter::StringOr(const char* default_value) {
    std::string string_default_value = default_value;
    if (value_) {
        Optional<std::string> optional = String();
        return optional.value_or(string_default_value);
    }
    return string_default_value;
}

Optional<int32_t> NapiGetter::Int32() {
    Optional<int32_t> optional;
    if (!value_) {
        return optional;
    }
    int32_t int32_value;
    status_ = napi_get_value_int32(NativeNodeApi::env, value_, &int32_value);
    if (status_ == napi_ok) {
        optional.set(int32_value);
    }
    return optional;
}

int32_t NapiGetter::Int32Or(const int32_t& default_value) {
    if (value_) {
        Optional<int32_t> optional = Int32();
        return optional.value_or(default_value);
    }
    return default_value;
}

Optional<uint32_t> NapiGetter::UInt32() {
    Optional<uint32_t> optional;
    if (!value_) {
        return optional;
    }
    uint32_t uint32_value;
    status_ = napi_get_value_uint32(NativeNodeApi::env, value_, &uint32_value);
    if (status_ == napi_ok) {
        optional.set(uint32_value);
    }
    return optional;
}

uint32_t NapiGetter::UInt32(const uint32_t& default_value) {
    if (value_) {
        Optional<uint32_t> optional = UInt32();
        return optional.value_or(default_value);
    }
    return default_value;
}

napi_value NapiGetter::GetNapiValue() const {
    return value_;
}

Optional<double> NapiGetter::Double() {
    Optional<double> optional;
    if (!value_) {
        return optional;
    }
    double double_value;
    status_ = napi_get_value_double(NativeNodeApi::env, value_, &double_value);
    if (status_ == napi_ok) {
        optional.set(double_value);
    }
    return optional;
}

double NapiGetter::Double(const double& default_value) {
    if (value_) {
        Optional<double> optional = Double();
        return optional.value_or(default_value);
    }
    return default_value;
}

double NapiGetter::DoubleOr(const double& default_value) {
    if (value_) {
        Optional<double> optional = Double();
        return optional.value_or(default_value);
    }
    return default_value;
}

Optional<std::vector<NapiGetter>> NapiGetter::Vector() {
    Optional<std::vector<NapiGetter>> optional;
    if (!value_) {
        return optional;
    }
    uint32_t length = 0;
    napi_get_array_length(TaroRuntime::NativeNodeApi::env, value_, &length);

    std::vector<NapiGetter> vector_value;
    vector_value.reserve(length);

    for (uint32_t i = 0; i < length; i++) {
        napi_value element;
        status_ =
            napi_get_element(TaroRuntime::NativeNodeApi::env, value_, i, &element);
        if (status_ == napi_ok) {
            NapiGetter getter(element);
            vector_value.push_back(getter);
        }
    }
    optional.set(vector_value);
    return optional;
};
} // namespace TaroRuntime
