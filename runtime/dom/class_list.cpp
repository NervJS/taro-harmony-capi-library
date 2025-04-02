/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "class_list.h"

#include "helper/StringUtils.h"
#include "helper/TaroLog.h"
#include "runtime/cssom/invalidations/StyleInvalidator.h"

namespace TaroRuntime {
namespace TaroDOM {
    ClassList::ClassList(const std::string& class_name) {
        if (class_name.empty()) {
            return;
        }
        reset(class_name);
    }

    ClassList::~ClassList() {
    }

    void ClassList::bindElement(std::weak_ptr<TaroElement> el) {
        if (element_ref_.lock() == nullptr) {
            element_ref_ = el;
        }
    }

    bool ClassList::is_match(const ClassList& class_list) const {
        for (const auto& class_name : class_list) {
            if (contains(class_name) == 0) {
                return false;
            }
        }
        return true;
    }

    std::string ClassList::value() const {
        return to_string();
    }

    int ClassList::length() const {
        return size();
    }

    void ClassList::add(const std::vector<std::string>& args) {
        for (const auto& class_name : args) {
            add(class_name);
        }
    }

    int ClassList::add(const std::string& token) {
        if (checkTokenIsValid(token) != 0) {
            if (contains(token) == 0) {
                push_back(token);
                if (auto el = element_ref_.lock()) {
                    auto className = TaroCSSOM::ClassNamePool::GetClassName(token);
                    for (auto rule : className->rules_) {
                        for (auto& combinator : rule->combinator_) {
                            TaroCSSOM::Invalidator::markElementUpdateStyle(el, combinator.matchElement);
                        }
                    }
                }
                return 1;
            }
        } else {
            TARO_LOG_WARN("ClassList", "Add invalid classname: %{public}s", token.c_str());
        }
        return 0;
    }

    void ClassList::remove(const std::vector<std::string>& args) {
        for (const auto& class_name : args) {
            remove(class_name);
        }
    }

    int ClassList::remove(const std::string& token) {
        if (checkTokenIsValid(token) != 0) {
            auto it = std::find(begin(), end(), token);
            if (it != end()) {
                auto className = TaroCSSOM::ClassNamePool::GetClassName(it->c_str());
                erase(it);
                if (auto el = element_ref_.lock()) {
                    for (auto rule : className->rules_) {
                        for (auto& combinator : rule->combinator_) {
                            TaroCSSOM::Invalidator::markElementUpdateStyle(el, combinator.matchElement);
                        }
                    }
                }
                return 1;
            }
        } else {
            TARO_LOG_WARN("ClassList", "Remove invalid classname: %{public}s", token.c_str());
        }
        return 0;
    }

    int ClassList::contains(const std::string& token) const {
        if (checkTokenIsValid(token) != 0) {
            return std::find(begin(), end(), token) != end() ? 1 : 0;
        } else {
            TARO_LOG_WARN("ClassList", "Contains invalid classname: %{public}s", token.c_str());
        }
        return 0;
    }

    int ClassList::toggle(const std::string& token) {
        return toggle(token, contains(token) != 0);
    }

    int ClassList::toggle(const std::string& token, bool force) {
        if (checkTokenIsValid(token) != 0) {
            if (force) {
                add(token);
                return 1;
            } else {
                remove(token);
                return 0;
            }
        } else {
            TARO_LOG_WARN("ClassList", "Toggle invalid classname: %{public}s", token.c_str());
        }
        return 0;
    }

    int ClassList::replace(const std::string& token, const std::string& new_token) {
        if (checkTokenIsValid(token) != 0 && checkTokenIsValid(new_token) != 0) {
            auto it = std::find(begin(), end(), token);
            if (it != end()) {
                auto oddClass = TaroCSSOM::ClassNamePool::GetClassName(it->c_str());
                *it = new_token;
                auto newClass = TaroCSSOM::ClassNamePool::GetClassName(new_token);
                if (auto el = element_ref_.lock()) {
                    for (auto rule : oddClass->rules_) {
                        for (auto& combinator : rule->combinator_) {
                            TaroCSSOM::Invalidator::markElementUpdateStyle(el, combinator.matchElement);
                        }
                    }
                    for (auto rule : newClass->rules_) {
                        for (auto& combinator : rule->combinator_) {
                            TaroCSSOM::Invalidator::markElementUpdateStyle(el, combinator.matchElement);
                        }
                    }
                }
                return 1;
            }
        } else {
            TARO_LOG_WARN("ClassList", "Replace invalid classname: %{public}s, %{public}s", token.c_str(), new_token.c_str());
        }
        return 0;
    }

    void ClassList::reset(const std::string& class_name) {
        std::vector<std::string> list = TaroHelper::StringUtils::split(
            TaroHelper::string::trim(class_name).data(),
            delimiter);
        list.erase(std::remove_if(std::begin(list), std::end(list), [this](const std::string& str) {
                       return checkTokenIsValid(str) == 0;
                   }),
                   std::end(list));
        reset(list);
    }

    void ClassList::reset(const std::vector<std::string>& class_list) {
        std::vector<std::string> invalid_list;
        for (auto it = begin(); it != end();) {
            if (std::find(class_list.begin(), class_list.end(), *it) == class_list.end()) {
                invalid_list.push_back(*it);
            }
            ++it;
        }
        for (const auto& token : invalid_list) {
            remove(token);
        }
        for (const auto& token : class_list) {
            add(token);
        }
    }

    int ClassList::checkTokenIsValid(std::string token) const {
        if (token.empty() || token.find(delimiter) != std::string::npos) {
            return 0;
        }
        return 1;
    }

    std::string ClassList::to_string() const {
        return TaroHelper::StringUtils::join(*this, delimiter);
    }
} // namespace TaroDOM
} // namespace TaroRuntime
