//
// Created on 2024/6/9.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "./ets.h"

#include <arkui/native_node_napi.h>

#include "arkjs/ArkJS.h"
#include "arkjs/Scope.h"
#include "helper/TaroTimer.h"
#include "runtime/NapiSetter.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroEtsNode::TaroEtsNode(TaroElementRef element)
        : TaroRenderNode(element) {};

    TaroEtsNode::~TaroEtsNode() {
        Dispose();
    };

    void TaroEtsNode::Build() {
        TIMER_FUNCTION();
        NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
        ArkJS arkJs(NativeNodeApi::env);
        NativeNodeApi *nativeNodeApi = NativeNodeApi::getInstance();
        std::shared_ptr<TaroDOM::TaroElement> sharedElement = element_ref_.lock();
        const napi_value &builderCallback = nativeNodeApi->GetBuilderCallback(sharedElement->context_->page_path_);

        napi_value argv[1] = {sharedElement->GetNodeValue()};
        napi_value componentContent = nullptr;
        ArkUI_NodeHandle nodeHandle = nullptr;
        napi_call_function(NativeNodeApi::env, NativeNodeApi::global, builderCallback, 1, argv, &componentContent);
        OH_ArkUI_GetNodeHandleFromNapiValue(NativeNodeApi::env, componentContent, &nodeHandle);
        TaroClassLifeStatistic::markNew("NodeHandle");
        SetArkUINodeHandle(nodeHandle);
        component_content_ref_ = arkJs.createReference(componentContent);
    };

    void TaroEtsNode::Update() {
        TIMER_FUNCTION();
        NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
        ArkJS arkJs(NativeNodeApi::env);

        napi_value componentContent = arkJs.getReferenceValue(component_content_ref_);
        NapiGetter contextGetter(componentContent);
        napi_valuetype builder_node_type = contextGetter.GetProperty("builderNode_").GetType();
        if (builder_node_type != napi_undefined) {
            // Note: 调用 ets 组件更新
            auto updateFunc = contextGetter.GetProperty("update");
            if (updateFunc.GetType() == napi_function) {
                const napi_value &builderUpdate = updateFunc.GetNapiValue();
                napi_value updateParam;
                napi_create_object(NativeNodeApi::env, &updateParam);
                std::shared_ptr<TaroDOM::TaroElement> sharedElement = element_ref_.lock();
                NapiSetter::SetProperty(updateParam, "node", sharedElement->GetNodeValue());
                napi_value updateArgv[1] = {updateParam};
                napi_value result;
                napi_call_function(NativeNodeApi::env, componentContent, builderUpdate, 1, updateArgv, &result);
            }
        }
    };

    void TaroEtsNode::Dispose() {
        if (component_content_ref_ == nullptr) {
            TARO_LOG_ERROR("TaroEtsNode", "Failed to dispose.");
            return;
        }

        NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
        ArkJS arkJs(NativeNodeApi::env);

        napi_value componentContent = arkJs.getReferenceValue(component_content_ref_);
        NapiGetter contextGetter(componentContent);
        napi_valuetype builder_node_type = contextGetter.GetProperty("builderNode_").GetType();
        if (builder_node_type != napi_undefined) {
            auto disposeFunc = contextGetter.GetProperty("dispose");
            if (disposeFunc.GetType() == napi_function) {
                const napi_value &builderDispose = disposeFunc.GetNapiValue();
                napi_value result;
                napi_call_function(NativeNodeApi::env, componentContent, builderDispose, 0, {}, &result);
            }
        }

        arkJs.deleteReference(component_content_ref_);
    };
} // namespace TaroDOM
} // namespace TaroRuntime
