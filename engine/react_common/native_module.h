/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#pragma once

#include <optional>
#include <string>
#include <vector>
#include <folly/dynamic.h>

struct MethodDescriptor {
    std::string name;
    // type is one of js MessageQueue.MethodTypes
    std::string type;

    MethodDescriptor(std::string n, std::string t)
        : name(std::move(n)),
          type(std::move(t)) {}
};

using MethodCallResult = std::optional<folly::dynamic>;

class NativeModule {
    public:
    virtual ~NativeModule() {}
    virtual std::string getName() = 0;
    virtual std::string getSyncMethodName(unsigned int methodId) = 0;
    virtual std::vector<MethodDescriptor> getMethods() = 0;
    virtual folly::dynamic getConstants() = 0;
    virtual void
    invoke(unsigned int reactMethodId, folly::dynamic&& params, int callId) = 0;
    virtual MethodCallResult callSerializableNativeHook(
        unsigned int reactMethodId,
        folly::dynamic&& args) = 0;
};
