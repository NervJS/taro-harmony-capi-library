//
// Created on 2024/6/13.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "style.h"

#include "arkjs/ArkJS.h"
#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace Attribute {
        void Style::SetValueFromNapi(const napi_value value) {
            ArkJS arkJs(NativeNodeApi::env);
            if (value && arkJs.getType(value) == napi_string) {
                this->set(arkJs.getString(value));
            }
        }

        void Style::SetValueToNode(std::shared_ptr<TaroRenderNode> renderNode,
                                   const std::string value) {
            if (renderNode != nullptr) {
                ArkUI_NodeHandle node = renderNode->GetArkUINodeHandle();
                if (node != nullptr) {
                    ArkUI_AttributeItem item = {.string = value.c_str()};
                    TaroRuntime::NativeNodeApi::getInstance()->setAttribute(
                        renderNode->GetArkUINodeHandle(), NODE_ID, &item);
                }
            }
        }
    } // namespace Attribute
} // namespace TaroDOM
} // namespace TaroRuntime
