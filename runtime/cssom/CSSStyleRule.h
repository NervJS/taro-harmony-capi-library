/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_CSSSTYLERULE_H
#define TARO_CAPI_HARMONY_DEMO_CSSSTYLERULE_H

#include <list>
#include <string>

#include "runtime/cssom/CSSRule.h"
#include "runtime/cssom/CSSStyleDeclaration.h"
#include "runtime/cssom/RuleFeature.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroElement;
}
namespace TaroCSSOM {
    class CSSRule;
    // 选择器结构
    struct Combinator {
        std::string className;
        MatchElement matchElement;
    };

    struct WeakPtrCompare {
        bool operator()(const std::weak_ptr<TaroDOM::TaroElement> &lhs,
                        const std::weak_ptr<TaroDOM::TaroElement> &rhs) const {
            auto sp_lhs = lhs.lock();
            auto sp_rhs = rhs.lock();
            if (!sp_lhs || !sp_rhs) {
                return !sp_lhs && sp_rhs; // 如果 lhs 为空而 rhs 不为空，返回 true（lhs < rhs）
            }
            return sp_lhs < sp_rhs; // 否则，按指向对象的地址排序
        }
    };

    class CSSStyleRule : public CSSRule {
        public:
        std::list<Combinator> combinator_;                 // 选择器链表
        std::unique_ptr<CSSStyleDeclaration> declaration_; // 样式申明
        bool has_env = false; // 是否有环境变量

        CSSStyleRule(std::list<Combinator> combinator, std::unique_ptr<CSSStyleDeclaration> declaration);

        virtual ~CSSStyleRule();

        void init();

        // 规则应用到样式上
        const PseudoType &getPseudo();
        void setPseudo(const PseudoType &);
        const std::string &getPseudoValue();
        void setPseudoValue(const std::string &);

        private:
        PseudoType pseudo_ = PseudoType::None;
        std::string pseudo_val_;
    };

    using CSSStyleRuleRef = std::shared_ptr<CSSStyleRule>;
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_CSSSTYLERULE_H
