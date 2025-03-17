//
// Created by zhutianjian on 24-9-20.
//

#include "framework_react.h"

#include "arkjs/ArkJS.h"
#include "helper/StringUtils.h"
#include "helper/TaroAllocationMetrics.h"
#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "helper/api_cost_statistic.h"
#include "runtime/render.h"

namespace TaroHarmonyLibrary {
const std::vector<napi_property_descriptor> FrameworkReact::desc = {
    {"updatePropsByPayload", nullptr, FrameworkReact::UpdatePropsByPayload, nullptr, nullptr, nullptr, napi_default, nullptr},
};

napi_value FrameworkReact::UpdatePropsByPayload(napi_env env, napi_callback_info info) {
    API_FUNC_COST();
    TIMER_US_FUNCTION();
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 3);

    TaroRuntime::Render* render = TaroRuntime::Render::GetInstance();
    int32_t nid = TaroRuntime::TaroDOM::TaroNode::GetNodeId(args[0]);
    auto node = render->document_->GetElementById(nid);
    if (node == nullptr) {
        TARO_LOG_ERROR("FrameworkReact", "nid[%{public}d] is not found", nid);
        return nullptr;
    }

    auto oddProps = arkJs.getObject(args[1]);
    napi_value updatePayload = args[2];
    for (size_t i = 0; i < arkJs.getArrayLength(updatePayload); i += 2) {
        std::string name = arkJs.getString(arkJs.getArrayElement(updatePayload, i));
        napi_value value = arkJs.getArrayElement(updatePayload, i + 1);
        napi_value oldValue = oddProps.getProperty(name);
        napi_valuetype valueType = value ? arkJs.getType(value) : napi_undefined;

        if (name == "key" || name == "children" || name == "ref") {
            // skip
            continue;
        }
        if (name == "className") {
            name = "class";
        }
        if (name.starts_with("on")) {
            bool isCapture = name.ends_with("Capture");
            std::string eventName = StringUtils::toLowerCase(name).substr(2);
            if (isCapture) {
                eventName = eventName.substr(0, eventName.size() - 7);
            }

            if (valueType == napi_function) {
                napi_valuetype oddValueType = oldValue ? arkJs.getType(oldValue) : napi_undefined;
                if (oddValueType == napi_function) {
                    node->updateNodeEventListener(eventName, oldValue, value); // isCapture sideEffect=false
                } else {
                    node->addNodeEventListener(eventName, value); // isCapture
                }
            } else {
                node->removeNodeEventListener(eventName, oldValue);
            }
        } else if (name == "dangerouslySetInnerHTML") {
            // const newHtml = (value as DangerouslySetInnerHTML)?.__html ?? ''
            // const oldHtml = (oldValue as DangerouslySetInnerHTML)?.__html ?? ''
            // if (newHtml || oldHtml) {
            //   if (oldHtml !== newHtml) {
            //     dom.innerHTML = newHtml
            //   }
            // }
        } else if (valueType != napi_function) {
            if (valueType == napi_undefined || valueType == napi_null) {
                node->RemoveAttribute(name);
            } else {
                node->SetAttribute(name, value);
            }
        }
    }

    TARO_PRINT_MEMORY();
    return nullptr;
}
} // namespace TaroHarmonyLibrary
