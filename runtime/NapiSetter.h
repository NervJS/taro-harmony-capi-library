/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once
#include <set>
#include <string>
#include <napi/native_api.h>

#include "NativeNodeApi.h"

namespace TaroRuntime {

class NapiSetter {
    public:
    static int SetProperty(napi_value& node, const std::string& name, int value);
    static int SetProperty(napi_value& node, const std::string& name, const std::string& value);
    static int SetProperty(napi_value& node, const std::string& name, const char* value);
    static int SetProperty(napi_value& node, const std::string& name, const double value);
    static int SetProperty(napi_value& node, const std::string& name, bool value);
    static int SetProperty(napi_value& node, const std::string& name, napi_value& value);
    static int SetProperty(napi_value& node, const std::string& name, const std::set<std::string>& value);
};
} // namespace TaroRuntime
