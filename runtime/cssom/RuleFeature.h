//
// Created on 2024/5/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_RULEFEATURE_H
#define TARO_CAPI_HARMONY_DEMO_RULEFEATURE_H

#include <cstdint>
namespace TaroRuntime {
namespace TaroCSSOM {
    enum class PseudoType {
        None = 0,
        Before = 1,
        After = 2,
        FirstChild = 3,
        LastChild = 4,
        NthChild = 5,
        Empty = 6,
    };

    enum class MatchElement : std::uint8_t {
        // 直接选择
        Subject = 0,
        // 直接后代选择器
        Parent = 1,
        // 后代选择器
        Ancestor = 2,
        // 多类选择器
        Multiple = 3,

        //  DirectSibling,
        //  IndirectSibling,
        //  AnySibling,
        //  ParentSibling,
        //  AncestorSibling,
        //  ParentAnySibling,
        //  AncestorAnySibling,
        //  HasChild,
        //  HasDescendant,
        //  HasSibling,
        //  HasSiblingDescendant,
        //  HasAnySibling,
        //  HasNonSubject,
        //  HasScopeBreaking,
        //  Host,
        //  HostChild
    };

    struct RuleFeature {
        RuleFeature(MatchElement matchElement);

        MatchElement matchElement_;
    };
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_RULEFEATURE_H
