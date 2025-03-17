//
// Created on 2024/5/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "CSSRule.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    ClassName::ClassName(std::string str)
        : value(str) {}

    ClassName::~ClassName() {}

    std::unordered_map<std::string, std::shared_ptr<ClassName>> ClassNamePool::pool_;

    std::shared_ptr<ClassName> ClassNamePool::GetClassName(const std::string &str) {
        auto it = pool_.find(str);
        if (it != pool_.end()) {
            return it->second;
        } else {
            auto new_shared_str = std::make_shared<ClassName>(str);
            pool_[str] = new_shared_str;
            return new_shared_str;
        }
    }

    void ClassNamePool::SetClassName(const std::string &str) {
        auto it = pool_.find(str);
        if (it == pool_.end()) {
            auto new_shared_str = std::make_shared<ClassName>(str);
            pool_[str] = new_shared_str;
        }
    }
} // namespace TaroCSSOM
} // namespace TaroRuntime
