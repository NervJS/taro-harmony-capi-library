//
// Created on 2024/6/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <map>
#include <string>
#include <vector>
#include <folly/dynamic.h>

struct MethodCall {
    int moduleId;
    int methodId;
    folly::dynamic arguments;
    int callId;

    MethodCall(int mod, int meth, folly::dynamic &&args, int cid)
        : moduleId(mod),
          methodId(meth),
          arguments(std::move(args)),
          callId(cid) {}
};

/// \throws std::invalid_argument
std::vector<MethodCall> parseMethodCalls(folly::dynamic &&calls);
