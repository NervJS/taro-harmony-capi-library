/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./CSSStyleRule.h"

#include "./CSSStyleSheet.h"

#define COMBINE_PROPERTY(property)                        \
    if (rule_stylesheet->property.has_value()) {          \
        stylesheet->property = rule_stylesheet->property; \
    }

namespace TaroRuntime {
namespace TaroCSSOM {
    CSSStyleRule::CSSStyleRule(std::list<Combinator> combinator,
                               std::unique_ptr<CSSStyleDeclaration> declaration)
        : combinator_(combinator), declaration_(std::move(declaration)) {}

    CSSStyleRule::~CSSStyleRule() {}

    void CSSStyleRule::init() {
        // 建立依赖关系
        for (auto combinator : combinator_) {
            // 获取classname所对应的指针，需要持有该指针
            std::shared_ptr<ClassName> class_name =
                ClassNamePool::GetClassName(combinator.className);
            class_name->rules_.push_back(
                std::static_pointer_cast<CSSStyleRule>(shared_from_this()));
        }
    }

    const PseudoType& CSSStyleRule::getPseudo() {
        return pseudo_;
    }

    void CSSStyleRule::setPseudo(const PseudoType& val) {
        pseudo_ = val;
    }

    const std::string& CSSStyleRule::getPseudoValue() {
        return pseudo_val_;
    }

    void CSSStyleRule::setPseudoValue(const std::string& val) {
        pseudo_val_ = val;
    }

} // namespace TaroCSSOM
} // namespace TaroRuntime
