/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_NATIVE_NODE_API_H
#define TARO_CAPI_HARMONY_DEMO_NATIVE_NODE_API_H

#include <functional>
#include <string>
#include <unordered_map>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>
#include <js_native_api.h>
#include <napi/native_api.h>

namespace TaroRuntime {
class NativeNodeApi {
    private:
    static NativeNodeApi* instance;

    ArkUI_NativeNodeAPI_1* nodeAPI;
    std::unordered_map<std::string, napi_ref> builderCallback_;
    bool has_destroy_ = false;

    public:
    static NativeNodeApi* getInstance();
    static napi_env env;
    static napi_value exports;
    static napi_value global;
    static napi_value js_capi_bridge;

    ArkUI_NodeHandle createNode(ArkUI_NodeType nodeType);
    void disposeNode(ArkUI_NodeHandle nodeHandle);

    static const napi_extended_error_info* getLastErrorInfo();
    void addChild(ArkUI_NodeHandle parent, ArkUI_NodeHandle child);
    void removeChild(ArkUI_NodeHandle parent, ArkUI_NodeHandle child);
    void replaceChild(ArkUI_NodeHandle parent, ArkUI_NodeHandle prev,
                      ArkUI_NodeHandle next);
    void insertChildBefore(ArkUI_NodeHandle parent, ArkUI_NodeHandle child,
                           ArkUI_NodeHandle before);
    void insertChildAfter(ArkUI_NodeHandle parent, ArkUI_NodeHandle child,
                          ArkUI_NodeHandle after);
    ArkUI_NodeHandle getChildAt(ArkUI_NodeHandle node, int32_t position);
    ArkUI_NodeHandle getParent(ArkUI_NodeHandle node);
    void insertChildAt(ArkUI_NodeHandle parent, ArkUI_NodeHandle child,
                       int index);
    int32_t registerNodeEvent(ArkUI_NodeHandle nodeHandle,
                              ArkUI_NodeEventType eventType, int eventIndex);
    int32_t registerNodeEvent(ArkUI_NodeHandle nodeHandle,
                              ArkUI_NodeEventType eventType, int eventIndex, void* args);
    void unRegisterNodeEvent(ArkUI_NodeHandle nodeHandle,
                             ArkUI_NodeEventType eventType);
    int32_t registerNodeCustomEvent(ArkUI_NodeHandle node, ArkUI_NodeCustomEventType eventType, int32_t targetId, void* userData);

    void registerNodeEventReceiver(
        void (*eventReceiver)(ArkUI_NodeEvent* event));

    int addNodeEventReceiver(ArkUI_NodeHandle node, void (*eventReceiver)(ArkUI_NodeEvent* event));

    int removeNodeEventReceiver(ArkUI_NodeHandle node, void (*eventReceiver)(ArkUI_NodeEvent* event));

    void unregisterNodeCustomEvent(ArkUI_NodeHandle node,
                                   ArkUI_NodeCustomEventType eventType);

    void registerNodeCustomEventReceiver(
        void (*eventReceiver)(ArkUI_NodeCustomEvent* event));
    int32_t addNodeCustomEventReceiver(
        ArkUI_NodeHandle node,
        void (*eventReceiver)(ArkUI_NodeCustomEvent* event));
    int32_t removeNodeCustomEventReceiver(
        ArkUI_NodeHandle node,
        void (*eventReceiver)(ArkUI_NodeCustomEvent* event));

    void markDirty(ArkUI_NodeHandle node, ArkUI_NodeDirtyFlag dirtyFlag);

    // 设置属性
    void setAttribute(ArkUI_NodeHandle nodeHandle,
                      ArkUI_NodeAttributeType nodeAttributeType,
                      ArkUI_AttributeItem* item);
    const ArkUI_AttributeItem* getAttribute(
        ArkUI_NodeHandle nodeHandle, ArkUI_NodeAttributeType nodeAttributeType);
    // 重制属性，相当于卸载属性的设置内容
    void resetAttribute(ArkUI_NodeHandle nodeHandle,
                        ArkUI_NodeAttributeType arkUI_NodeAttributeType);

    // Note: ETS 组件 Builder 回调函数
    void RegisterBuilderCallback(std::string router, napi_ref builderCallback, napi_env env);
    napi_value GetBuilderCallback(std::string router) const;

    uint32_t getTotalChildCount(ArkUI_NodeHandle&);

    void setMeasuredSize(ArkUI_NodeHandle nodeHandle, int32_t width,
                         int32_t height);
    void setLayoutPosition(ArkUI_NodeHandle nodeHandle, int32_t positionX,
                           int32_t positionY);
    ArkUI_IntOffset getLayoutPosition(ArkUI_NodeHandle nodeHandle);
    ArkUI_IntSize getMeasuredSize(ArkUI_NodeHandle);
    ArkUI_NodeHandle getFirstChild(ArkUI_NodeHandle);
    ArkUI_IntSize measureNode(ArkUI_NodeHandle&);
    int32_t measureNode(ArkUI_NodeHandle&, ArkUI_LayoutConstraint* Constraint);
    int32_t layoutNode(ArkUI_NodeHandle node, int32_t positionX,
                       int32_t positionY);
    napi_value ConvertToJSON(napi_value value);
    std::string ConvertToJSONString(napi_value value);
    void setDestroy(bool has_destroy);
    bool hasDestroy();
};

} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_NATIVE_NODE_API_H
