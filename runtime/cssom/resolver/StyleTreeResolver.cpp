//
// Created on 2024/5/21.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "./StyleTreeResolver.h"

#include <stack>

#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/dom/node_visitor.h"
#include "runtime/render.h"

namespace TaroRuntime {
namespace TaroCSSOM {

    void StyleTreeResolver::resolver(std::shared_ptr<TaroDOM::TaroElement> elementRoot) {
        std::stack<std::shared_ptr<TaroDOM::TaroElement>> stack;

        stack.push(elementRoot);
        while (!stack.empty()) {
            auto parent = stack.top();
            stack.pop();
            auto childNodes = parent->child_nodes_;
            for (int i = childNodes.size() - 1; i >= 0; --i) {
                auto currentElement = std::dynamic_pointer_cast<TaroDOM::TaroElement>(childNodes[i]);
                if (currentElement) {
                    // 寻找到脏节点
                    if (currentElement->GetAndClearStateFlag(STATE_FLAG::IS_COMPUTED_STYLE_INVALID_FLAG)) {
                        currentElement->ClearStateFlag(STATE_FLAG::IS_SUBTREE_COMPUTED_STYLE_INVALID_FLAG);

                        TARO_LOG_DEBUG("StyleTreeResolver", "%{public}s", currentElement->class_name_.c_str());
                        if (currentElement->context_ == nullptr) {
                            TARO_LOG_ERROR("StyleTreeResolver", "context_ is null, %{public}s %{public}d %{public}d",
                                currentElement->class_name_.c_str(), currentElement->tag_name_, currentElement->nid_);
                            continue;
                        }

                        auto rules_path = TaroCSSOM::CSSStyleSheet::GetInstance()->getStylePagePath(currentElement->context_->page_path_);
                        // 重新匹配样式
                        auto new_class_style =
                            TaroCSSOM::CSSStyleSheet::GetInstance()
                                ->getStylesheetByClassNames(
                                    currentElement,
                                    currentElement->class_list_, rules_path);

                        if (currentElement->inline_style_ == nullptr) {
                            currentElement->style_ = new_class_style.stylesheet;
                        } else {
                            currentElement->style_ =
                                TaroCSSOM::TaroStylesheet::Stylesheet::assign(
                                    new_class_style.stylesheet,
                                    currentElement->inline_style_);
                        }
                        currentElement->before_pseudo_style_ = new_class_style.before_stylesheet;
                        currentElement->after_pseudo_style_ = new_class_style.after_stylesheet;

                        currentElement->BuildProcess();
                    }
                    // 沿着脏标记顺藤摸瓜，寻找脏子树
                    if (!currentElement->GetAndClearStateFlag(
                            STATE_FLAG::DESCENDANT_NEED_STYLE_RESOLUTION)) {
                        continue;
                    }
                    stack.push(currentElement);
                }
            }
        }
    }
} // namespace TaroCSSOM

} // namespace TaroRuntime
