/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_CSSSTYLESHEET_H
#define TARO_CAPI_HARMONY_DEMO_CSSSTYLESHEET_H

#include <vector>

#include "./CSSStyleRule.h"
#include "./stylesheet/IAttribute.h"
#include "media/CSSMedias.h"
#include "runtime/cssom/dimension/context.h"
#include "runtime/cssom/stylesheet/css_keyframes.h"
#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroCSSOM {

    struct ElementStylesheets {
        std::shared_ptr<TaroStylesheet::Stylesheet> stylesheet;
        std::shared_ptr<TaroStylesheet::Stylesheet> before_stylesheet;
        std::shared_ptr<TaroStylesheet::Stylesheet> after_stylesheet;
    };

    // 样式规则的结构体
    using TRules = std::unordered_map<std::string, std::vector<std::shared_ptr<CSSStyleRule>>>;
    class CSSStyleSheet {
        private:
        std::unordered_map<std::string, TRules> rulesMapping_; // 路由和路由对应的页面样式规则的映射
        std::unordered_map<std::string, DimensionContextRef> current_page_dimension_; // 路由和对应的 dimension 上下文映射
        std::unordered_map<std::string, TaroStylesheet::CSSKeyframesPtr> keyframes_;
    
        public:
        static CSSStyleSheet *GetInstance() {
            static CSSStyleSheet *css_style_sheet = new CSSStyleSheet();
            return css_style_sheet;
        }

        CSSStyleSheet();

        virtual ~CSSStyleSheet();

        void InitConfig(const napi_value &config);
        void Init(const napi_value &router, const napi_value &styleRules, const napi_value &config);
        DimensionContextRef GetCurrentPageDimension(const std::string& page_path);
        void UpdateCurrentPageDimension(const std::string& page_path);

        static void setStylesheet(
            std::shared_ptr<TaroStylesheet::Stylesheet> &style_ptr,
            napi_value hmStyle);

        static void setAttribute(
            napi_value hmStyle, char *name,
            std::function<void(napi_valuetype, napi_value)> callback);

        // 合并层叠样式
        ElementStylesheets CombineCascadeStyle(
            const std::shared_ptr<TaroDOM::TaroElement> &element,
            std::vector<CSSStyleRuleRef> &);

        // 嵌套样式匹配
        std::vector<CSSStyleRuleRef> MatchNestingSelector(
            const std::shared_ptr<TaroDOM::TaroElement> &element,
            const TaroDOM::ClassList &classList,
            const std::string &router);

        // 根据classNames获取匹配到的最终styles
        ElementStylesheets getStylesheetByClassNames(
            const std::shared_ptr<TaroDOM::TaroElement> &element,
            const TaroDOM::ClassList &classList,
            const std::string &router);

        // 拼接页面路由，在路由后面拼接视窗宽度，来识别不同视窗下的样式
        std::string getStylePagePath(std::string page_path);

        // 根据animation name获取keyframes信息
        const TaroStylesheet::KeyframesInfoPtr getAnimKeyframes(const std::string &name, const std::string &router) const;

        static TaroStylesheet::CSSKeyframesPtr getAnimKeyframesPtr(const std::string &router);
    };

    // 匹配解析nth-child的值
    static std::array<int, 2> parseNthChild(const std::string &str) {
        std::array<int, 2> result = {0, 0}; // 初始化结果数组为 [0, 0]

        if (str == "odd") {
            result[0] = 2;
            result[1] = 1;
        } else if (str == "even") {
            result[0] = 2;
        } else {
            size_t operatorPos = str.find_first_of("+-");
            if (operatorPos != std::string::npos) {
                // 处理 xn+y 或 xn-y 的情况
                std::string xStr = str.substr(0, operatorPos);
                std::string yStr = str.substr(operatorPos);
                if (!xStr.empty() && xStr.back() == 'n') {
                    xStr.pop_back();
                    result[0] = xStr.empty() ? 1 : std::stoi(xStr);
                }
                if (!yStr.empty()) {
                    result[1] = std::stoi(yStr);
                }
            } else {
                // 处理 xn 或 y 的情况
                if (!str.empty() && str.back() == 'n') {
                    std::string xStr = str.substr(0, str.length() - 1);
                    result[0] = xStr.empty() ? 1 : std::stoi(xStr);
                } else {
                    try {
                        result[1] = std::stoi(str);
                    } catch (const std::invalid_argument &) {
                        // 处理非法的 y 值
                        result[1] = 0;
                    }
                }
            }
        }
        return result;
    }

    using StylesheetRef = std::shared_ptr<TaroStylesheet::Stylesheet>;
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_CSSSTYLESHEET_H
