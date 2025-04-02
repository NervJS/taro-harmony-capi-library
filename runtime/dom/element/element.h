/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_ELEMENT_H
#define TARO_CAPI_HARMONY_DEMO_ELEMENT_H

#include <list>
#include <memory>
#include <string>

#include "helper/TaroLog.h"
#include "runtime/constant.h"
#include "runtime/cssom/CSSStyleRule.h"
#include "runtime/cssom/invalidations/StyleValidity.h"
#include "runtime/cssom/stylesheet/IAttribute.h"
#include "runtime/dom/animation/node_animations.h"
#include "runtime/dom/ark_nodes/arkui_node.h"
#include "runtime/dom/class_list.h"
#include "runtime/dom/element/attribute/attribute.h"
#include "runtime/dom/element/attribute/event.h"
#include "runtime/dom/event/visible_info.h"

namespace TaroRuntime {
// 提前申明，防止CSSStyleRule内部引入Element导致循环连接
namespace TaroCSSOM {
    class CSSStyleRule;
    class ElementStylesheets;
} // namespace TaroCSSOM
namespace TaroDOM {
    class TaroPageContainer;
    class FormGroupManager;
    class TaroContext {
        private:
        static std::unordered_map<std::string, std::shared_ptr<TaroContext>> context_mapping_; // 路由和路由对应的页面上下文的映射

        public:
        static std::shared_ptr<TaroContext> GetContext(std::shared_ptr<TaroPageContainer> pageElement);
        static std::shared_ptr<TaroContext> CloneContext(std::shared_ptr<TaroContext> originContext, std::shared_ptr<TaroElement> measureRoot);

        TaroContext(std::shared_ptr<TaroPageContainer> pageElement);
        ~TaroContext();

        // 页面唯一id：带时间戳
        std::string page_id_;
        // 页面路由，不带时间戳
        std::string page_path_;
        // 页面根节点
        std::weak_ptr<TaroPageContainer> page_element_;
        std::shared_ptr<FormGroupManager> group_manger_;
        // 测量跟根节点
        std::weak_ptr<TaroElement> measure_root_;
    };

    class TaroElement : public TaroAttribute,
                        public TaroElementEvent,
                        public TaroClassLife<TaroElement> {
        public:
        TaroElement(napi_value node);

        virtual ~TaroElement();

        // 页面信息上下文
        std::shared_ptr<TaroContext> context_;
        // 类名
        ClassList class_list_ = ClassList(class_name_);
        std::vector<TaroCSSOM::CSSStyleRuleRef> rule_list_;
        // 标签名
        TAG_NAME tag_name_;
        // 样式
        std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> style_;
        std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> before_pseudo_style_;
        std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> after_pseudo_style_;
        // 伪元素
        std::shared_ptr<TaroRenderNode> before_render_node_;
        std::shared_ptr<TaroRenderNode> after_render_node_;
        // 层叠样式表
        std::list<std::shared_ptr<TaroCSSOM::CSSStyleRule>> rules_;
        bool is_reused_ = false;

        virtual void GetNodeAttributes() override;
        virtual void SetAttributesToRenderNode();
        std::variant<bool, int, std::string, StylesheetRef, napi_value> GetAttribute(std::string name);
        virtual std::variant<bool, int, std::string, StylesheetRef, napi_value> GetAttribute(ATTRIBUTE_NAME name, napi_value result) override;
        virtual int32_t GetCacheCount() {
            return 0;
        }

        void SetHeadRenderNode(
            std::shared_ptr<TaroRenderNode> head_render_node) override;

        void SetFooterRenderNode(
            std::shared_ptr<TaroRenderNode> footer_render_node) override;

        static TAG_NAME GetTagName(napi_value node);
        static TAG_NAME GetTagName(std::string str);

        static PROPERTY_NAME GetPropertyName(napi_value node);

        virtual int GetFuncCode(napi_value node);
        virtual int GetFuncCode(std::string str);

        // 样式失效标脏
        void invalidateStyle(TaroCSSOM::Validity validty);
        void invalidateStyleInternal();
        void markAncestorsForInvalidatedStyle();

        void SetIsInline(bool isInline);
        bool GetIsInline();

        void BuildProcess();

        void MountPseudoRenderNode();
        void MountFixedRenderNode();

        bool HasStateFlag(STATE_FLAG flag) const {
            return state_flags_.test(static_cast<size_t>(flag));
        }
        void SetStateFlag(STATE_FLAG flag) {
            if (HasStateFlag(flag))
                return;
            TARO_LOG_DEBUG("CSSOM", "标脏： %{public}hhu  %{public}s", flag,
                           class_list_.value().c_str());
            state_flags_.set(static_cast<size_t>(flag));
        }
        void ClearStateFlag(STATE_FLAG flag) {
            state_flags_.reset(static_cast<size_t>(flag));
        }
        bool GetAndClearStateFlag(STATE_FLAG flag) {
            if (state_flags_.test(static_cast<size_t>(flag))) {
                ClearStateFlag(flag);
                return true;
            }
            return false;
        }

        const std::shared_ptr<TaroElement> getPageRoot();

        const std::shared_ptr<TaroNode> getParentNode(TAG_NAME tagName);

        // 是否应用文本样式
        const bool enableTextStyleAttribute() {
            return isTextElement(); // FIXME 包括 Input、Textarea 等表单组件
        }

        const bool isPureTextNode() {
            return node_type_ == NODE_TYPE::TEXT_NODE;
        }

        const bool isTextElement() {
            return isPureTextNode() || tag_name_ == TAG_NAME::TEXT || tag_name_ == TAG_NAME::SPAN;
        }

        const bool isFormElement() {
            return tag_name_ == TAG_NAME::BUTTON ||
                   tag_name_ == TAG_NAME::INPUT ||
                   tag_name_ == TAG_NAME::TEXTAREA ||
                   tag_name_ == TAG_NAME::PICKER ||
                   tag_name_ == TAG_NAME::LABEL ||
                   tag_name_ == TAG_NAME::RADIO_GROUP ||
                   tag_name_ == TAG_NAME::RADIO ||
                   tag_name_ == TAG_NAME::CHECKBOX_GROUP ||
                   tag_name_ == TAG_NAME::CHECKBOX;
        }

        std::shared_ptr<TaroCSSOM::TaroStylesheet::Stylesheet> GetTextNodeStyleFromElement(const std::shared_ptr<TaroElement>& element);

        virtual void PreBuild();
        virtual void Build() = 0;
        virtual void PostBuild();
        virtual void Build(std::shared_ptr<TaroElement>& reuse_element);
        // 判断reuse_element 是否可以被当前节点复用
        virtual bool Reusable(std::shared_ptr<TaroElement>& reuse_element);

        void MatchStylesheet();

        bool IsFixed();

        void updateListenEvent();
        virtual void bindVisibleEvent(CallbackInfo& info_);
        virtual void unbindVisibleEvent();
        virtual void handleVisibilityInfo();
        void registerVisibleEvent();
        void registerAreaChangeEvent();
        void registerVisibleAreaChangeEvent();
        void registerOnScrollVisibleChangeEvent();

        //  通知Fixed节点进行显隐更新
        virtual void NotifyFixedElementVisibility() {};
        // 检查祖先是否显隐
        bool CheckAncestorsVisibility();

        // 寻找离自己最近的块级父节点（用于文本节点）
        std::shared_ptr<TaroElement> findClosedBlockParent();
        CallbackInfo info_;

        void CheckMatchEmptySelector();
        bool GetMatchEmptySelector();
        void SetMatchEmptySelector(bool val);

        // 验证自己是否在滚动容器中已经上屏，如果不是滚动容器则默认返回 true
        // 后期 list -> listItemGroup -> listItem 时要考虑两层 adapter 嵌套的情况
        bool checkIsInScrollContainerIndex(bool useCacheCount = false);
        bool is_need_match_style = false;

        napi_value ExecuteFunc(napi_value name, napi_value params);

        virtual napi_value ExecuteFunc(std::shared_ptr<TaroRenderNode> renderNode, napi_value name, napi_value params);

        protected:
        // 行内元素
        bool is_inline_;
        void AppendOrInsertIntoParent();
        virtual void HandleAttributeChanged(
            ATTRIBUTE_NAME name, const std::string& preValue,
            const std::string& curValue) override;
        void createJSAnimation();
        // 创建上下文
        void createContext();
        // 递归创建上下文
        void createContextRecursively(std::shared_ptr<TaroContext> context);
        // 获取事件方法
        // void InitHarmonyEvents();

        private:
        bool has_empty_selector_ = false;
    };

    using TaroElementRef = std::shared_ptr<TaroElement>;
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_ELEMENT_H
