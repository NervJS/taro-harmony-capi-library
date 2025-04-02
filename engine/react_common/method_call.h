/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

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

    MethodCall(int mod, int meth, folly::dynamic&& args, int cid)
        : moduleId(mod),
          methodId(meth),
          arguments(std::move(args)),
          callId(cid) {}
};

/// \throws std::invalid_argument
std::vector<MethodCall> parseMethodCalls(folly::dynamic&& calls);
