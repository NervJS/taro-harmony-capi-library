/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./selectory_query.h"

#include <regex>
#include <stack>
#include <string>

#include "arkjs/ArkJS.h"
#include "folly/Exception.h"
#include "helper/TaroTimer.h"
#include "helper/string.h"
#include "runtime/NapiGetter.h"
#include "thread/NapiTaskRunner.h"

namespace TaroRuntime {
std::vector<std::pair<char, std::string>> SelectorQuery::tokenizeSelector(const std::string& selectors) {
    TIMER_US_FUNCTION();
    std::vector<std::pair<char, std::string>> result;
    std::string token;
    const char combinator_chars[] = {'>', '+', '~', ' '};
    const char escape = '\\';
    char connector = 0;

    for (size_t i = 0; i < selectors.length(); ++i) {
        char c = selectors[i];

        if (c == escape) {
            char next = 0;
            if (i + 1 < selectors.length()) {
                next = selectors[++i];
            }
            if (next != escape && std::find(std::begin(combinator_chars), std::end(combinator_chars), next) == std::end(combinator_chars)) {
                token += c;
            }
            token += next;
            continue;
        }

        if (std::find(std::begin(combinator_chars), std::end(combinator_chars), c) != std::end(combinator_chars)) {
            if (!token.empty()) {
                result.emplace_back(connector, token);
                token.clear();
            }
            connector = c;
            // 处理连续空格
            while (i + 1 < selectors.length() && selectors[i + 1] == ' ') {
                ++i;
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        result.emplace_back(connector, token);
    }

    return std::move(result);
}

std::vector<std::string> SelectorQuery::tokenizeDOMSelector(const std::string& selector) {
    TIMER_US_FUNCTION();
    std::vector<std::string> result;
    std::string token;
    const char combinator_chars[] = {'[', '#', '.', ']', '\s'};
    const char escape = '\\';

    for (size_t i = 0; i < selector.length(); ++i) {
        char c = selector[i];

        if (c == escape) {
            char next = 0;
            if (i + 1 < selector.length()) {
                next = selector[++i];
            }

            if (next != escape && std::find(std::begin(combinator_chars), std::end(combinator_chars), next) == std::end(combinator_chars)) {
                token += c;
            }
            token += next;
            continue;
        }

        if (c == '[' || c == '#' || c == '.') {
            if (!token.empty()) {
                result.emplace_back(TaroHelper::string::trim(token));
                token.clear();
            }
        }
        token += c;
    }

    if (!token.empty()) {
        result.emplace_back(TaroHelper::string::trim(token));
    }

    return std::move(result);
}

std::shared_ptr<TaroDOM::TaroElement>
SelectorQuery::getElementById(std::shared_ptr<TaroDOM::TaroElement>& element, const std::string& id) {
    TIMER_US_FUNCTION();
    std::stack<std::shared_ptr<TaroDOM::TaroElement>> stack;
    stack.push(element);

    while (!stack.empty()) {
        auto next_element = stack.top();
        stack.pop();

        if (next_element->id_ == id || std::to_string(next_element->nid_) == id) {
            return next_element;
        }

        auto child_nodes = processChildNodes(next_element);
        // 反向遍历
        for (auto it = child_nodes.rbegin(); it != child_nodes.rend(); ++it) {
            if (auto child_el = std::static_pointer_cast<TaroDOM::TaroElement>(*it)) {
                stack.push(child_el);
            }
        }
    }

    return nullptr;
}

std::forward_list<std::weak_ptr<TaroDOM::TaroNode>>
SelectorQuery::getElementsByClassName(std::shared_ptr<TaroDOM::TaroElement>& element, const std::string& class_name) {
    TIMER_US_FUNCTION();
    if (element == nullptr)
        return {};
    std::forward_list<std::weak_ptr<TaroDOM::TaroNode>> result;
    auto it = result.before_begin();

    auto class_list = TaroDOM::ClassList(class_name);
    std::stack<std::shared_ptr<TaroDOM::TaroElement>> stack;
    stack.push(element);

    while (!stack.empty()) {
        auto next_element = stack.top();
        stack.pop();

        if (next_element->class_list_.is_match(class_list)) {
            it = result.insert_after(it, next_element);
        }

        auto child_nodes = processChildNodes(next_element);
        // 反向遍历
        for (auto it = child_nodes.rbegin(); it != child_nodes.rend(); ++it) {
            if (auto child_el = std::static_pointer_cast<TaroDOM::TaroElement>(*it)) {
                stack.push(child_el);
            }
        }
    }

    return std::move(result);
}

std::forward_list<std::weak_ptr<TaroDOM::TaroNode>>
SelectorQuery::getElementsByTagName(std::shared_ptr<TaroDOM::TaroElement>& element, const std::string& tag_name) {
    TIMER_US_FUNCTION();
    if (element == nullptr)
        return {};
    std::forward_list<std::weak_ptr<TaroDOM::TaroNode>> result;
    auto it = result.before_begin();

    std::stack<std::shared_ptr<TaroDOM::TaroElement>> stack;
    stack.push(element);

    while (!stack.empty()) {
        auto next_element = stack.top();
        stack.pop();

        if (checkTaroNodeByTagName(next_element, tag_name)) {
            it = result.insert_after(it, next_element);
        }

        auto child_nodes = processChildNodes(next_element);
        // 反向遍历
        for (auto it = child_nodes.rbegin(); it != child_nodes.rend(); ++it) {
            if (auto child_el = std::static_pointer_cast<TaroDOM::TaroElement>(*it)) {
                stack.push(child_el);
            }
        }
    }

    return std::move(result);
}

std::forward_list<std::weak_ptr<TaroDOM::TaroNode>>
SelectorQuery::querySelector(std::shared_ptr<TaroDOM::TaroNode>& node, const std::string& selector,
                             bool is_select_all) {
    TIMER_US_FUNCTION();
    std::vector<std::string> selectors = TaroHelper::StringUtils::splitWithEscape(selector, ",");
    std::forward_list<std::weak_ptr<TaroDOM::TaroNode>> combined;
    auto it = combined.before_begin();

    for (const auto& sel : selectors) {
        auto tokens = tokenizeSelector(sel);
        auto result = findTaroNodeWithSelectors(node, tokens, is_select_all);
        if (!result.empty()) {
            if (is_select_all) {
                it = combined.insert_after(it, result.begin(), result.end());
            } else {
                combined.insert_after(it, result.front());
            }
        }
    }

    return std::move(combined);
}

std::forward_list<std::weak_ptr<TaroDOM::TaroNode>>
SelectorQuery::findTaroNodeWithSelectors(std::shared_ptr<TaroDOM::TaroNode>& node,
                                         std::vector<std::pair<char, std::string>>& selectors,
                                         bool is_select_all) {
    TIMER_US_FUNCTION();
    if (selectors.empty())
        return {};

    if (auto element = std::static_pointer_cast<TaroDOM::TaroElement>(node)) {
        auto result = findTaroNodeWithSelector(node, selectors[0].first, selectors[0].second, is_select_all);
        for (size_t i = 1; i < selectors.size(); ++i) {
            std::forward_list<std::weak_ptr<TaroDOM::TaroNode>> next_result;
            auto it = next_result.before_begin();

            for (const auto& weak_child : result) {
                if (auto child = weak_child.lock()) {
                    auto next_child_result = findTaroNodeWithSelector(child, selectors[i].first, selectors[i].second, is_select_all);
                    it = next_result.insert_after(it, next_child_result.begin(), next_child_result.end());
                }
            }
            result = next_result;
        }
        return std::move(result);
    }

    return {};
}

std::forward_list<std::weak_ptr<TaroDOM::TaroNode>>
SelectorQuery::findTaroNodeWithSelector(std::shared_ptr<TaroDOM::TaroNode>& node,
                                        char& combinator,
                                        std::string& selector,
                                        bool is_select_all) {
    TIMER_US_FUNCTION();
    std::vector<std::string> tokens = tokenizeDOMSelector(selector);
    if (auto element = std::static_pointer_cast<TaroDOM::TaroElement>(node)) {
        auto child_result = findTaroNodeWithSelectorTokens(element, combinator, tokens, is_select_all);
        if (!child_result.empty()) {
            return std::move(child_result);
        }
    }

    return {};
}

std::forward_list<std::weak_ptr<TaroDOM::TaroNode>>
SelectorQuery::findTaroNodeWithSelectorTokens(std::shared_ptr<TaroDOM::TaroElement>& element,
                                              char& combinator,
                                              std::vector<std::string>& tokens,
                                              bool is_select_all) {
    TIMER_US_FUNCTION();
    std::forward_list<std::weak_ptr<TaroDOM::TaroNode>> result;
    auto it = result.before_begin();

    std::stack<std::shared_ptr<TaroDOM::TaroElement>> stack;
    stack.push(element);

    while (!stack.empty()) {
        auto nextElement = stack.top();
        stack.pop();

        if (combinator == 0 || combinator == ' ') {
            auto el = checkTaroNodeWithSelectorTokens(nextElement, tokens);
            if (el.lock()) {
                it = result.insert_after(it, el);
                if (!is_select_all) {
                    return std::move(result);
                }
            }

            auto child_nodes = processChildNodes(nextElement);
            // 反向遍历
            for (auto it = child_nodes.rbegin(); it != child_nodes.rend(); ++it) {
                if (auto child_el = std::static_pointer_cast<TaroDOM::TaroElement>(*it)) {
                    stack.push(child_el);
                }
            }
            continue;
        }
        if (combinator == '>') {
            for (const auto& child : processChildNodes(nextElement)) {
                if (auto child_el = std::static_pointer_cast<TaroDOM::TaroElement>(child)) {
                    auto child_result = checkTaroNodeWithSelectorTokens(child_el, tokens);
                    if (child_result.lock()) {
                        it = result.insert_after(it, child);
                        if (!is_select_all) {
                            return std::move(result);
                        }
                    }
                }
            }
            continue;
        }
        if (combinator == '+') {
            auto next = nextElement->GetNextSibling();
            if (auto next_el = std::static_pointer_cast<TaroDOM::TaroElement>(next)) {
                auto child_result = checkTaroNodeWithSelectorTokens(next_el, tokens);
                if (child_result.lock()) {
                    it = result.insert_after(it, child_result);
                    return std::move(result);
                }
            }
            continue;
        }
        if (combinator == '~') {
            auto next = nextElement->GetNextSibling();
            while (next) {
                if (auto next_el = std::static_pointer_cast<TaroDOM::TaroElement>(next)) {
                    auto child_result = checkTaroNodeWithSelectorTokens(next_el, tokens);
                    if (child_result.lock()) {
                        it = result.insert_after(it, child_result);
                        if (!is_select_all) {
                            return std::move(result);
                        }
                    }
                    next = next->GetNextSibling();
                }
            }
            continue;
        }
    }

    return std::move(result);
}

std::weak_ptr<TaroDOM::TaroElement>
SelectorQuery::checkTaroNodeWithSelectorTokens(std::shared_ptr<TaroDOM::TaroElement>& element, std::vector<std::string>& tokens) {
    TIMER_US_FUNCTION();
    for (const auto& token : tokens) {
        if (token.length() < 1) {
            continue;
        }

        if (token.front() == '#') {
            std::string id = token.substr(1);
            if (element->id_ != id && std::to_string(element->nid_) != id) {
                return {};
            }
        } else if (token.front() == '.') {
            if (element->class_list_.contains(token.substr(1)) == 0) {
                return {};
            }
        } else if (std::isalpha(token.front())) {
            if (checkTaroNodeByTagName(element, token) == nullptr) {
                return {};
            }
        } else if (token.front() == '[') {
            if (checkTaroNodeByAttribute(element, token) == nullptr) {
                return {};
            }
        } else {
            return {};
        }
    }

    return element;
}

std::shared_ptr<TaroDOM::TaroElement>
SelectorQuery::checkTaroNodeByTagName(std::shared_ptr<TaroDOM::TaroElement>& element, const std::string& tag_name) {
    TIMER_US_FUNCTION();

    TAG_NAME tag = TaroDOM::TaroElement::GetTagName(StringUtils::toUpperCase(tag_name));
    if (element->tag_name_ == tag) {
        return element;
    }

    return nullptr;
}

std::shared_ptr<TaroDOM::TaroElement>
SelectorQuery::checkTaroNodeByAttribute(std::shared_ptr<TaroDOM::TaroElement>& element, const std::string& attribute) {
    TIMER_US_FUNCTION();
    std::string token = attribute.substr(1, attribute.size() - 2);
    int pos = token.find('=');
    std::string key = TaroHelper::string::trim(token.substr(0, pos)).data();
    std::string value = token.substr(pos + 1);
    bool is_string = false;
    if (value.front() == '\'' || value.front() == '"') {
        value = value.substr(1, value.size() - 2);
        is_string = true;
    }
    value = TaroHelper::string::trim(value).data();
    ArkJS arkJs(NativeNodeApi::env);
    if (auto attrValue = element->GetAttributeNodeValue(key)) {
        auto attrType = arkJs.getType(attrValue);
        if (attrType != napi_undefined) {
            // Note: 判断 key 匹配 和 value 匹配
            if (pos <= 0 || (attrType == napi_string && arkJs.getString(attrValue) == value)) {
                return element;
            }
            if (!is_string && attrType == napi_boolean && (arkJs.getBoolean(attrValue) ? "true" : "false") == value) {
                return element;
            }
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<TaroDOM::TaroNode>>
SelectorQuery::processChildNodes(std::shared_ptr<TaroDOM::TaroElement>& element) {
    TIMER_US_FUNCTION();
    // Note: 通常应该从新页面开始查询
    if (element->tag_name_ == TaroRuntime::TAG_NAME::APP || element->tag_name_ == TaroRuntime::TAG_NAME::ENTRY_ASYNC) {
        return std::move(std::vector<std::shared_ptr<TaroDOM::TaroNode>>(element->child_nodes_.rbegin(), element->child_nodes_.rend()));
    }
    return element->child_nodes_;
}
} // namespace TaroRuntime
