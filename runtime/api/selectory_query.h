/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <forward_list>

#include "runtime/constant.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
class SelectorQuery {
    public:
    static SelectorQuery* GetInstance() {
        static SelectorQuery* selector_query_instance = new SelectorQuery();
        return selector_query_instance;
    }

    std::vector<std::pair<char, std::string>> tokenizeSelector(const std::string& selectors);
    std::vector<std::string> tokenizeDOMSelector(const std::string& selector);

    std::shared_ptr<TaroDOM::TaroElement> getElementById(std::shared_ptr<TaroDOM::TaroElement>& element, const std::string& id);
    std::forward_list<std::weak_ptr<TaroDOM::TaroNode>> getElementsByClassName(std::shared_ptr<TaroDOM::TaroElement>& element, const std::string& class_name);
    std::forward_list<std::weak_ptr<TaroDOM::TaroNode>> getElementsByTagName(std::shared_ptr<TaroDOM::TaroElement>& element, const std::string& tag_name);

    std::forward_list<std::weak_ptr<TaroDOM::TaroNode>>
    querySelector(std::shared_ptr<TaroDOM::TaroNode>& node, const std::string& selector, bool is_select_all);

    std::forward_list<std::weak_ptr<TaroDOM::TaroNode>>
    findTaroNodeWithSelectors(std::shared_ptr<TaroDOM::TaroNode>& node, std::vector<std::pair<char, std::string>>& selectors, bool is_select_all);

    std::forward_list<std::weak_ptr<TaroDOM::TaroNode>>
    findTaroNodeWithSelector(std::shared_ptr<TaroDOM::TaroNode>& node, char& combinator, std::string& selector, bool is_select_all);

    protected:
    std::forward_list<std::weak_ptr<TaroDOM::TaroNode>>
    findTaroNodeWithSelectorTokens(std::shared_ptr<TaroDOM::TaroElement>& element, char& combinator, std::vector<std::string>& tokens, bool is_select_all);

    std::weak_ptr<TaroDOM::TaroElement>
    checkTaroNodeWithSelectorTokens(std::shared_ptr<TaroDOM::TaroElement>& element, std::vector<std::string>& tokens);

    std::shared_ptr<TaroDOM::TaroElement>
    checkTaroNodeByTagName(std::shared_ptr<TaroDOM::TaroElement>& element, const std::string& tag_name);

    std::shared_ptr<TaroDOM::TaroElement>
    checkTaroNodeByAttribute(std::shared_ptr<TaroDOM::TaroElement>& element, const std::string& attribute);

    std::vector<std::shared_ptr<TaroDOM::TaroNode>>
    processChildNodes(std::shared_ptr<TaroDOM::TaroElement>& element);
};
} // namespace TaroRuntime
