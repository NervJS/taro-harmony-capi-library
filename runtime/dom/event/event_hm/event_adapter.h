/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <arkui/native_node.h>
#include <arkui/native_type.h>

namespace TaroRuntime::TaroDOM::TaroEvent {
struct TaroEventAdapter_Keeper {
    std::unordered_map<int32_t, std::unordered_set<ArkUI_NodeEventType>> node_list_;
};
class TaroEventAdapter final {
    enum class OperateType {
        registerNodeEvent = 0,
        unRegisterNodeEvent = 1,
        clearNodeEvent = 2,
    };
    struct OperateInfo {
        OperateType op_;
        int nid = 0;
        ArkUI_NodeHandle node_handle = nullptr;
        ArkUI_NodeEventType event_type;
    };

    public:
    static TaroEventAdapter* instance();

    // 节点释放时，需要清空所有的绑定内容，防止内存泄漏, true:发现了节点，false：没发现
    bool disposeArkNode(ArkUI_NodeHandle);

    // 某个节点注册事件
    int registerNodeEvent(int nid, ArkUI_NodeHandle node_handle, ArkUI_NodeEventType event_type, int target_id, void* args);

    // 某个节点取消事件
    void unRegisterNodeEvent(int nid, ArkUI_NodeHandle node_handle, ArkUI_NodeEventType event_type);

    // 清除节点上的事件
    void clearNodeEvent(int nid, ArkUI_NodeHandle node_handle);

    private:
    bool hasRegister(const TaroEventAdapter_Keeper& keeper, ArkUI_NodeEventType event_type);

    static void eventReceiver(ArkUI_NodeEvent* event);

    void dispatchEvent(ArkUI_NodeEvent* event);

    // ArkUI_NodeHandle->绑定的node节点
    std::unordered_map<ArkUI_NodeHandle, TaroEventAdapter_Keeper> ark_to_keepers_;
};
} // namespace TaroRuntime::TaroDOM::TaroEvent
