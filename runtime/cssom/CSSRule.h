/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_CSSRULE_H
#define TARO_CAPI_HARMONY_DEMO_CSSRULE_H

#include <list>
#include <string>

namespace TaroRuntime {
namespace TaroCSSOM {
    class CSSStyleRule;

    // 基类
    // 继承Observer: 订阅ClassName的变化
    class CSSRule : public std::enable_shared_from_this<CSSRule> {
    };

    // ClassName：发布者
    class ClassName {
        public:
        std::string value;
        std::list<std::shared_ptr<CSSStyleRule>> rules_;

        ClassName(std::string str);

        virtual ~ClassName();
    };

    // ClassName 池
    class ClassNamePool {
        public:
        static std::shared_ptr<ClassName> GetClassName(const std::string& str);

        static void SetClassName(const std::string& str);

        static std::unordered_map<std::string, std::shared_ptr<ClassName>> pool_;
    };
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_CSSRULE_H
