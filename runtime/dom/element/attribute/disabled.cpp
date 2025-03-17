//
// Created by zhutianjian on 24-5-29.
//

#include "disabled.h"

#include "arkjs/ArkJS.h"
#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace Attribute {
        void Disabled::SetValueFromNapi(const napi_value value) {
            ArkJS arkJs(NativeNodeApi::env);
            if (value && arkJs.getType(value) == napi_boolean) {
                this->set(arkJs.getBoolean(value));
            } else {
                this->set(false);
            }
        }

        void Disabled::SetValueToNode(std::shared_ptr<TaroRenderNode> renderNode,
                                      const bool value) {
            if (renderNode != nullptr) {
                ArkUI_NodeHandle node = renderNode->GetArkUINodeHandle();
                if (node != nullptr) {
                    ArkUI_NumberValue arkUI_NumberValue[] = {};
                    ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
                    arkUI_NumberValue[0].i32 = !value;
                    TaroRuntime::NativeNodeApi::getInstance()->setAttribute(
                        renderNode->GetArkUINodeHandle(), NODE_ENABLED, &item);
                }
            }
        }
    } // namespace Attribute
} // namespace TaroDOM
} // namespace TaroRuntime
