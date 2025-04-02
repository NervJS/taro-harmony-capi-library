/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TESTNDK_RENDER_H
#define TESTNDK_RENDER_H

#include <set>
#include <stack>
#include <thread>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>
#include <arkui/native_node_napi.h>
#include <native_vsync/native_vsync.h>
#include <node_api.h>

#include "runtime/dom/document.h"
#include "runtime/dom/element/element.h"
#include "runtime/dom/element/page_container.h"
#include "runtime/dom/node.h"
#include "thread/NapiTaskRunner.h"
#include "thread/TaskExecutor.h"

namespace TaroRuntime {
class Render {
    public:
    static Render* GetInstance() {
        static Render* render = new Render();
        render->GetTaskRunner();
        return render;
    }

    Render();

    ~Render();

    static std::shared_ptr<TaroDOM::TaroDocument> document_;

    static void AttachTaroPage(uint32_t nid,
                               std::shared_ptr<TaroDOM::TaroElement> parent);

    static void BuildElement(std::shared_ptr<TaroDOM::TaroElement> parent);

    static void SetTaroNodeAttribute(std::shared_ptr<TaroDOM::TaroElement> node,
                                     napi_value name, napi_value value);

    static void RemoveTaroNodeAttribute(
        std::shared_ptr<TaroDOM::TaroElement> node, napi_value name);

    static napi_value GetTaroNodeAttribute(std::shared_ptr<TaroDOM::TaroElement> node, napi_value name);

    static napi_value HasTaroNodeAttribute(std::shared_ptr<TaroDOM::TaroElement> node, napi_value name);

    static napi_value HasTaroNodeAttributes(std::shared_ptr<TaroDOM::TaroElement> node);

    std::shared_ptr<TaroDOM::TaroPageContainer> GetTaroPageById(uint32_t nid);

    void SetEtsXComponent(const uint32_t nid,
                          std::shared_ptr<ArkUI_NodeContentHandle> component);

    std::shared_ptr<TaroDOM::TaroPageContainer> FindPageById(const std::shared_ptr<TaroDOM::TaroNode>& node, uint32_t nid);

    std::shared_ptr<ArkUI_NodeContentHandle> GetEtsXComponent(const uint32_t nid);

    static void ExecuteNodeFunc(std::shared_ptr<TaroDOM::TaroElement> node, napi_value name, napi_value value);

    static std::queue<std::function<void(std::shared_ptr<TaroDOM::TaroElement>)>> renderFunctionQueen;
    std::shared_ptr<TaroThread::TaskExecutor> GetTaskRunner() {
        static std::shared_ptr<TaroThread::TaskExecutor> g_task_executor = nullptr;
        if (g_task_executor == nullptr) {
            g_task_executor = std::make_shared<TaroThread::TaskExecutor>(NativeNodeApi::env);
        }
        return g_task_executor;
    }

    OH_NativeVSync* event_vsync_;

    private:
    const std::string X_COMPONENT_ID_PREFIX = "taro_x_";
    std::unordered_map<uint32_t, std::shared_ptr<ArkUI_NodeContentHandle>>
        ets_x_components_;
};
} // namespace TaroRuntime

#endif // TESTNDK_RENDER_H
