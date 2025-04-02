/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "NapiSetter.h"

#include "helper/TaroLog.h"

namespace TaroRuntime {
int NapiSetter::SetProperty(napi_value &node, const std::string &name, int value) {
    napi_value n_value;
    napi_create_int32(NativeNodeApi::env, value, &n_value);
    napi_set_named_property(NativeNodeApi::env, node, name.c_str(), n_value);
    return 0;
}
int NapiSetter::SetProperty(napi_value &node, const std::string &name, const std::string &value) {
    napi_value n_value;
    napi_create_string_utf8(NativeNodeApi::env, value.c_str(), value.length(), &n_value);
    napi_set_named_property(NativeNodeApi::env, node, name.c_str(), n_value);
    return 0;
}

int NapiSetter::SetProperty(napi_value &node, const std::string &name, const char *value) {
    napi_value n_value;
    napi_create_string_utf8(NativeNodeApi::env, value, strlen(value), &n_value);
    napi_set_named_property(NativeNodeApi::env, node, name.c_str(), n_value);
    return 0;
}

int NapiSetter::SetProperty(napi_value &node, const std::string &name, const std::set<std::string> &value) {
    napi_value n_array;
    napi_create_array(NativeNodeApi::env, &n_array);
    int i = 0;
    for (auto iter = value.begin(); iter != value.end(); iter++) {
        napi_value n_value;
        napi_create_string_utf8(NativeNodeApi::env, iter->c_str(), iter->length(), &n_value);
        napi_set_element(NativeNodeApi::env, n_array, i, n_value);
        i++;
    }
    napi_set_named_property(NativeNodeApi::env, node, name.c_str(), n_array);
    return 0;
}

int NapiSetter::SetProperty(napi_value &node, const std::string &name, const double value) {
    napi_value n_value;
    napi_create_double(NativeNodeApi::env, value, &n_value);
    napi_set_named_property(NativeNodeApi::env, node, name.c_str(), n_value);
    return 0;
}
int NapiSetter::SetProperty(napi_value &node, const std::string &name, bool value) {
    napi_value n_value;
    napi_get_boolean(NativeNodeApi::env, value, &n_value);
    napi_set_named_property(NativeNodeApi::env, node, name.c_str(), n_value);
    return 0;
}

int NapiSetter::SetProperty(napi_value &node, const std::string &name, napi_value &value) {
    napi_set_named_property(NativeNodeApi::env, node, name.c_str(), value);
    return 0;
}
} // namespace TaroRuntime
