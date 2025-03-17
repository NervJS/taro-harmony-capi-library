#include "event_adapter.h"

#include "runtime/NativeNodeApi.h"
#include "runtime/dom/event/event_hm/event_generator_hm.h"
#include "runtime/render.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
TaroEventAdapter* TaroEventAdapter::instance() {
    static auto s_instance = new TaroEventAdapter();
    return s_instance;
}

bool TaroEventAdapter::disposeArkNode(ArkUI_NodeHandle node_handle) {
    auto iter = ark_to_keepers_.find(node_handle);
    if (iter == ark_to_keepers_.end()) {
        return false;
    }

    auto& keeper = iter->second;
    std::unordered_set<ArkUI_NodeEventType> event_types;
    // 释放所有监听的事件
    for (const auto& node_events : iter->second.node_list_) {
        for (auto event_type : node_events.second) {
            if (event_types.count(event_type) > 0) {
                continue;
            }
            event_types.insert(event_type);
            NativeNodeApi::getInstance()->unRegisterNodeEvent(node_handle, event_type);
        }
    }
    ark_to_keepers_.erase(iter);
    return true;
}

int TaroEventAdapter::registerNodeEvent(int nid, ArkUI_NodeHandle node_handle,
                                        ArkUI_NodeEventType event_type, int target_id, void* args) {
    int ret = 0;
    if (ark_to_keepers_.count(node_handle) == 0) {
        NativeNodeApi::getInstance()->addNodeEventReceiver(node_handle, eventReceiver);
    }
    auto& keeper = ark_to_keepers_[node_handle];
    auto& event_types = keeper.node_list_[nid];
    // 已经存在，不注册
    if (event_types.count(event_type) > 0) {
        return 0;
    }

    // 未注册过
    if (keeper.node_list_.size() <= 1 || !hasRegister(keeper, event_type)) {
        ret = NativeNodeApi::getInstance()->registerNodeEvent(node_handle, event_type, nid, args);
    }
    event_types.insert(event_type);
    return ret;
}

void TaroEventAdapter::unRegisterNodeEvent(int nid, ArkUI_NodeHandle node_handle, ArkUI_NodeEventType event_type) {
    TARO_LOG_DEBUG("TaroEventAdapter", "unregister nid:%{public}d node_handle:%{public}p event_type:%{public}d",
                   nid, node_handle, event_type);
    auto iter_keeper = ark_to_keepers_.find(node_handle);
    if (iter_keeper == ark_to_keepers_.end()) {
        return;
    }
    auto iter_node = iter_keeper->second.node_list_.find(nid);
    if (iter_node == iter_keeper->second.node_list_.end()) {
        return;
    }
    auto remove_cnt = iter_node->second.erase(event_type);
    if (remove_cnt == 1) {
        // 没有节点监听了，删除
        if (!hasRegister(iter_keeper->second, event_type)) {
            NativeNodeApi::getInstance()->unRegisterNodeEvent(node_handle, event_type);
        }
    }

    // node下事件空了，删除node
    if (iter_node->second.empty()) {
        iter_keeper->second.node_list_.erase(iter_node);
    }
    // ArkUI_NodeHandle下的node为空了，删除ArkUI_NodeHandle
    if (iter_keeper->second.node_list_.empty()) {
        ark_to_keepers_.erase(iter_keeper);
    }
}

void TaroEventAdapter::clearNodeEvent(int nid, ArkUI_NodeHandle node_handle) {
    TARO_LOG_DEBUG("TaroEventAdapter", "clearNodeEvent nid:%{public}d node_handle:%{public}p",
                   nid, node_handle);
    auto iter_keeper = ark_to_keepers_.find(node_handle);
    if (iter_keeper == ark_to_keepers_.end()) {
        return;
    }

    auto iter_node = iter_keeper->second.node_list_.find(nid);
    if (iter_node == iter_keeper->second.node_list_.end()) {
        return;
    }

    auto event_set = iter_node->second;
    iter_keeper->second.node_list_.erase(iter_node);
    for (auto event_type : event_set) {
        // 没有节点监听了，删除
        if (!hasRegister(iter_keeper->second, event_type)) {
            NativeNodeApi::getInstance()->unRegisterNodeEvent(node_handle, event_type);
        }
    }

    // ArkUI_NodeHandle下的node为空了，删除ArkUI_NodeHandle
    if (iter_keeper->second.node_list_.empty()) {
        ark_to_keepers_.erase(iter_keeper);
    }
}

bool TaroEventAdapter::hasRegister(const TaroEventAdapter_Keeper& keeper, ArkUI_NodeEventType event_type) {
    for (const auto& node_event : keeper.node_list_) {
        if (node_event.second.count(event_type) > 0) {
            return true;
        }
    }
    return false;
}

void TaroEventAdapter::eventReceiver(ArkUI_NodeEvent* event) {
    TaroEventAdapter::instance()->dispatchEvent(event);
}

void TaroEventAdapter::dispatchEvent(ArkUI_NodeEvent* event) {
    auto node_handle = OH_ArkUI_NodeEvent_GetNodeHandle(event);
    auto event_type = OH_ArkUI_NodeEvent_GetEventType(event);
    auto iter_keeper = ark_to_keepers_.find(node_handle);
    if (iter_keeper == ark_to_keepers_.end()) {
        return;
    }

    auto tmp_keeper = iter_keeper->second;

    for (const auto& node_events : tmp_keeper.node_list_) {
        TARO_LOG_DEBUG("TaroEventAdapter", "dispatchEvent nid:%{public}d node_handle:%{public}p",
                       node_events.first, node_handle);
        // 未监听此事件
        if (node_events.second.count(event_type) == 0) {
            continue;
        }
        auto taro_node = TaroDocument::GetInstance()->GetElementById(node_events.first);
        if (taro_node == nullptr) {
            continue;
        }
        auto generator = dynamic_pointer_cast<EventGenerator_HM>(taro_node->getEventEmitter()->getGenerator(EventGeneratorType::Event));
        if (generator) {
            generator->dispatchEvent(event);
        }

        // 丑陋的延时释放，为了不在事件回调过程中发生可能的删除节点操作
//         if (taro_node.use_count() == 1) {
//             auto runner = Render::GetInstance()->GetTaskRunner();
//             runner->runTask(TaroThread::TaskThread::MAIN, [taro_node] {});
//         }
    }
}

} // namespace TaroRuntime::TaroDOM::TaroEvent
