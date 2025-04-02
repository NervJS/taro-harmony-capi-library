/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "NativeNodeApi.h"

#include <iostream>
#include <arkui/native_interface.h>
#include <arkui/native_node.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>

#include "folly/lang/Exception.h"
#include "helper/StringUtils.h"
#include "helper/TaroLog.h"
#include "render.h"
#include "runtime/dom/event/event_hm/event_adapter.h"
#include "yoga/YGNode.h"

using namespace TaroHelper;

namespace TaroRuntime {
napi_env NativeNodeApi::env = nullptr;
napi_value NativeNodeApi::global = nullptr;
napi_value NativeNodeApi::js_capi_bridge = nullptr;

NativeNodeApi *NativeNodeApi::instance = nullptr;

NativeNodeApi *NativeNodeApi::getInstance() {
    if (instance == nullptr) {
        instance = new NativeNodeApi();
        OH_ArkUI_GetModuleInterface(ARKUI_NATIVE_NODE, ArkUI_NativeNodeAPI_1, instance->nodeAPI);
        // instance -> nodeAPI = reinterpret_cast<ArkUI_NativeNodeAPI_1
        // *>(OH_ArkUI_GetNativeAPI(ARKUI_NATIVE_NODE, 1));
    }

    return instance;
}

ArkUI_NodeHandle NativeNodeApi::createNode(ArkUI_NodeType nodeType) {
    TaroClassLifeStatistic::markNew("NodeHandle");
    auto it = nodeAPI->createNode(nodeType);
    if (it == nullptr) {
        TARO_LOG_ERROR("NativeNodeApi", "failed");
    }
    return it;
}

void NativeNodeApi::disposeNode(ArkUI_NodeHandle nodeHandle) {
    if (has_destroy_) {
        return;
    }
    if (!nodeHandle) {
        return;
    }
    auto runner = Render::GetInstance()->GetTaskRunner();
    runner->ensureRunOnMainTask([nodeHandle] {
        bool has_clear = TaroDOM::TaroEvent::TaroEventAdapter::instance()->disposeArkNode(nodeHandle);
        TaroClassLifeStatistic::markDelete("NodeHandle");
        instance->nodeAPI->disposeNode(nodeHandle);
        if (has_clear) {
#if IS_DEBUG
            folly::throw_exception(std::runtime_error("event disposeArkNode has node_handle"));
#else
            TARO_LOG_FATAL("TaroEvent", "event disposeArkNode has node_handle");
#endif
        }
    });
}

void NativeNodeApi::setMeasuredSize(ArkUI_NodeHandle nodeHandle, int32_t width, int32_t height) {
    if (has_destroy_) {
        return;
    }
    int32_t status = nodeAPI->setMeasuredSize(nodeHandle, width, height);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
}

void NativeNodeApi::setLayoutPosition(ArkUI_NodeHandle nodeHandle, int32_t positionX, int32_t positionY) {
    if (has_destroy_) {
        return;
    }
    int32_t status = nodeAPI->setLayoutPosition(nodeHandle, positionX, positionY);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
}

ArkUI_IntOffset NativeNodeApi::getLayoutPosition(ArkUI_NodeHandle nodeHandle) {
    return nodeAPI->getLayoutPosition(nodeHandle);
}

ArkUI_IntSize NativeNodeApi::getMeasuredSize(ArkUI_NodeHandle nodeHandle) {
    return nodeAPI->getMeasuredSize(nodeHandle);
}

ArkUI_NodeHandle NativeNodeApi::getFirstChild(ArkUI_NodeHandle nodeHandler) {
    auto it = nodeAPI->getFirstChild(nodeHandler);
    if (it == nullptr) {
        TARO_LOG_ERROR("NativeNodeApi", "failed");
    }
    return it;
}

ArkUI_NodeHandle NativeNodeApi::getChildAt(ArkUI_NodeHandle node, int32_t position) {
    auto it = nodeAPI->getChildAt(node, position);
    if (it == nullptr) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, position=%{public}d", position);
    }
    return it;
}

ArkUI_NodeHandle NativeNodeApi::getParent(ArkUI_NodeHandle node) {
    auto it = nodeAPI->getParent(node);
    if (it == nullptr) {
        TARO_LOG_ERROR("NativeNodeApi", "failed");
    }
    return it;
}

ArkUI_IntSize NativeNodeApi::measureNode(ArkUI_NodeHandle &nodeHandler) {
    if (has_destroy_) {
        return {0, 0};
    }
    ArkUI_LayoutConstraint *layoutConstraint = OH_ArkUI_LayoutConstraint_Create();
    if (nodeAPI->measureNode(nodeHandler, layoutConstraint) == 0) {
        return getMeasuredSize(nodeHandler);
    }
    // 出错了
    return {0, 0};
}

int32_t NativeNodeApi::measureNode(ArkUI_NodeHandle &nodeHandler, ArkUI_LayoutConstraint *constraint) {
    int32_t status = nodeAPI->measureNode(nodeHandler, constraint);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
    return status;
}

uint32_t NativeNodeApi::getTotalChildCount(ArkUI_NodeHandle &nodeHandler) {
    return nodeAPI->getTotalChildCount(nodeHandler);
}

const napi_extended_error_info *NativeNodeApi::getLastErrorInfo() {
    const napi_extended_error_info *errorInfo = {0};
    napi_get_last_error_info(NativeNodeApi::env, &errorInfo);

    return errorInfo;
}

void NativeNodeApi::addChild(ArkUI_NodeHandle parent, ArkUI_NodeHandle child) {
    if (has_destroy_) {
        return;
    }
    int32_t status = nodeAPI->addChild(parent, child);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
}

void NativeNodeApi::removeChild(ArkUI_NodeHandle parent, ArkUI_NodeHandle child) {
    if (has_destroy_) {
        return;
    }
    if (parent == nullptr || child == nullptr) {
        TARO_LOG_ERROR("NativeNodeApi", "parent is %{public}s null", parent ? "not" : "");
        return;
    }
    int32_t status = nodeAPI->removeChild(parent, child);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
}

void NativeNodeApi::replaceChild(ArkUI_NodeHandle parent, ArkUI_NodeHandle prev, ArkUI_NodeHandle next) {
    if (has_destroy_) {
        return;
    }
    uint32_t count = getTotalChildCount(parent);
    for (uint32_t i = 0; i < count; i++) {
        const auto current = getChildAt(parent, i);
        if (current == prev) {
            removeChild(parent, current);
            insertChildAt(parent, next, i);
            return;
        }
    }
}

void NativeNodeApi::insertChildBefore(ArkUI_NodeHandle parent, ArkUI_NodeHandle child, ArkUI_NodeHandle before) {
    if (has_destroy_) {
        return;
    }
    int32_t status = nodeAPI->insertChildBefore(parent, child, before);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
}

void NativeNodeApi::insertChildAfter(ArkUI_NodeHandle parent, ArkUI_NodeHandle child, ArkUI_NodeHandle after) {
    if (has_destroy_) {
        return;
    }
    int32_t status = nodeAPI->insertChildAfter(parent, child, after);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
}

void NativeNodeApi::insertChildAt(ArkUI_NodeHandle parent, ArkUI_NodeHandle child, int index) {
    if (has_destroy_) {
        return;
    }
    int32_t status = nodeAPI->insertChildAt(parent, child, index);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
}

int32_t
NativeNodeApi::registerNodeEvent(ArkUI_NodeHandle nodeHandle, ArkUI_NodeEventType eventType, int eventIndex) {
    if (has_destroy_) {
        return -1;
    }
    int32_t status = nodeAPI->registerNodeEvent(nodeHandle, eventType, eventIndex, nullptr);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
    return status;
}

int32_t
NativeNodeApi::registerNodeEvent(ArkUI_NodeHandle nodeHandle, ArkUI_NodeEventType eventType, int eventIndex, void *args) {
    if (has_destroy_) {
        return -1;
    }
    int32_t status = nodeAPI->registerNodeEvent(nodeHandle, eventType, eventIndex, args);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
    return status;
}

void NativeNodeApi::unRegisterNodeEvent(ArkUI_NodeHandle nodeHandle, ArkUI_NodeEventType eventType) {
    return nodeAPI->unregisterNodeEvent(nodeHandle, eventType);
}

int NativeNodeApi::addNodeEventReceiver(ArkUI_NodeHandle node, void (*eventReceiver)(ArkUI_NodeEvent *event)) {
    return nodeAPI->addNodeEventReceiver(node, eventReceiver);
}

int NativeNodeApi::removeNodeEventReceiver(ArkUI_NodeHandle node, void (*eventReceiver)(ArkUI_NodeEvent *event)) {
    int32_t status = nodeAPI->removeNodeEventReceiver(node, eventReceiver);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
    return status;
}

int32_t NativeNodeApi::registerNodeCustomEvent(ArkUI_NodeHandle node, ArkUI_NodeCustomEventType eventType, int32_t targetId, void *userData) {
    int32_t status = nodeAPI->registerNodeCustomEvent(node, eventType, targetId, userData);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
    return status;
}
/**
    * @brief Remeasure.
    *
    * When this type of flag is specified, re-layout is triggered by default.
    *   NODE_NEED_MEASURE = 1,

    Re-layout.
    * NODE_NEED_LAYOUT,
    Re-rendering.
    NODE_NEED_RENDER,
*/
void NativeNodeApi::markDirty(ArkUI_NodeHandle node, ArkUI_NodeDirtyFlag dirtyFlag) {
    if (has_destroy_) {
        return;
    }
    nodeAPI->markDirty(node, dirtyFlag);
}

void NativeNodeApi::unregisterNodeCustomEvent(ArkUI_NodeHandle node, ArkUI_NodeCustomEventType eventType) {
    return nodeAPI->unregisterNodeCustomEvent(node, eventType);
}

void NativeNodeApi::registerNodeEventReceiver(void (*eventReceiver)(ArkUI_NodeEvent *event)) {
    return nodeAPI->registerNodeEventReceiver(eventReceiver);
}

int32_t NativeNodeApi::removeNodeCustomEventReceiver(ArkUI_NodeHandle node,
                                                     void (*eventReceiver)(ArkUI_NodeCustomEvent *event)) {
    int32_t status = nodeAPI->removeNodeCustomEventReceiver(node, eventReceiver);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
    return status;
}

int32_t NativeNodeApi::addNodeCustomEventReceiver(ArkUI_NodeHandle node,
                                                  void (*eventReceiver)(ArkUI_NodeCustomEvent *event)) {
    int32_t status = nodeAPI->addNodeCustomEventReceiver(node, eventReceiver);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
    return status;
}

void NativeNodeApi::registerNodeCustomEventReceiver(void (*eventReceiver)(ArkUI_NodeCustomEvent *event)) {
    return nodeAPI->registerNodeCustomEventReceiver(eventReceiver);
}

int32_t NativeNodeApi::layoutNode(ArkUI_NodeHandle node, int32_t positionX, int32_t positionY) {
    int32_t status = nodeAPI->layoutNode(node, positionX, positionY);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
    return status;
}

void NativeNodeApi::setAttribute(ArkUI_NodeHandle nodeHandle, ArkUI_NodeAttributeType nodeAttributeType,
                                 ArkUI_AttributeItem *item) {
    if (has_destroy_) {
        return;
    }
    if (nodeHandle == nullptr) return;
    int32_t status = nodeAPI->setAttribute(nodeHandle, nodeAttributeType, item);
    if (status == ARKUI_ERROR_CODE_ARKTS_NODE_NOT_SUPPORTED || status == ARKUI_ERROR_CODE_ATTRIBUTE_OR_EVENT_NOT_SUPPORTED) {
        // FIXME 设置属性时应该要判断部分属性不允许设置，ETS、SPAN 组件需要在后续修复
        TARO_LOG_WARN("NativeNodeApi", "failed: arkTs node、attribute or event not supported, status=%{public}d type=%{public}d value=%{public}s",
                      status, nodeAttributeType, item->string);
    } else if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        std::string error = "unknown error";
        switch (status) {
            case ARKUI_ERROR_CODE_PARAM_INVALID:
                error = "arkTs node param invalid";
                break;
            // case ARKUI_ERROR_CODE_ATTRIBUTE_OR_EVENT_NOT_SUPPORTED:
            //   error = "arkTs node attribute or event not supported";
            //   break;
            case ARKUI_ERROR_CODE_ADAPTER_NOT_BOUND:
                error = "arkTs node adapter not bound";
                break;
            case ARKUI_ERROR_CODE_ADAPTER_EXIST:
                error = "arkTs node adapter exist";
                break;
            case ARKUI_ERROR_CODE_CHILD_NODE_EXIST:
                error = "arkTs node child node exist";
                break;
            default:
                break;
        }
#if IS_DEBUG
        folly::throw_exception(std::runtime_error(error));
#else
        TARO_LOG_FATAL("NativeNodeApi", "failed: %{public}s, status=%{public}d  type=%{public}d value=%{public}s", error.c_str(), status,
                       nodeAttributeType, item->string);
#endif
    }
}

const ArkUI_AttributeItem *
NativeNodeApi::getAttribute(ArkUI_NodeHandle nodeHandle, ArkUI_NodeAttributeType nodeAttributeType) {
    return nodeAPI->getAttribute(nodeHandle, nodeAttributeType);
}

void NativeNodeApi::resetAttribute(ArkUI_NodeHandle nodeHandle, ArkUI_NodeAttributeType nodeAttributeType) {
    if (has_destroy_) {
        return;
    }
    int32_t status = nodeAPI->resetAttribute(nodeHandle, nodeAttributeType);
    if (status != ARKUI_ERROR_CODE_NO_ERROR) {
        TARO_LOG_ERROR("NativeNodeApi", "failed, status=%{public}d", status);
    }
}

void NativeNodeApi::RegisterBuilderCallback(std::string router, napi_ref builderCallback, napi_env env_) {
    auto it = builderCallback_.find(router);
    if (it == builderCallback_.end()) {
        builderCallback_[router] = builderCallback;
        env = env_;
    }
}

napi_value NativeNodeApi::GetBuilderCallback(std::string router) const {
    auto it = builderCallback_.find(router);
    if (it == builderCallback_.end()) {
        it = builderCallback_.find("");
    }
    if (it == builderCallback_.end()) {
        folly::throw_exception(std::runtime_error("builderCallback is null"));
    }
    napi_value callback;
    napi_get_reference_value(env, it->second, &callback);
    return callback;
}

napi_value NativeNodeApi::ConvertToJSON(napi_value value) {
    napi_value JSON;
    napi_get_named_property(env, global, "JSON", &JSON);

    napi_value stringify;
    napi_get_named_property(env, JSON, "stringify", &stringify);

    napi_value argv[1] = {value};
    napi_value result;
    napi_call_function(env, JSON, stringify, 1, argv, &result);

    size_t str_length;
    napi_get_value_string_utf8(env, result, nullptr, 0, &str_length);
    std::string str(str_length + 1, '\0');
    napi_get_value_string_utf8(env, result, &str[0], str.length(), nullptr);

    return result;
}

std::string NativeNodeApi::ConvertToJSONString(napi_value value) {
    napi_value JSON;
    napi_get_named_property(env, global, "JSON", &JSON);

    napi_value stringify;
    napi_get_named_property(env, JSON, "stringify", &stringify);

    napi_value argv[1] = {value};
    napi_value result;
    napi_call_function(env, JSON, stringify, 1, argv, &result);

    size_t str_length;
    napi_get_value_string_utf8(env, result, nullptr, 0, &str_length);
    std::string str(str_length + 1, '\0');
    napi_get_value_string_utf8(env, result, &str[0], str.length(), nullptr);

    return str;
}

void NativeNodeApi::setDestroy(bool has_destroy) {
    has_destroy_ = has_destroy;
}

bool NativeNodeApi::hasDestroy() {
    return has_destroy_;
}
} // namespace TaroRuntime
