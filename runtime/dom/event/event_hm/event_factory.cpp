//
// Created on 2024/4/29.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "event_factory.h"

#include "helper/TaroLog.h"
#include "runtime/dom/event/event_hm/event_types/event_type.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        TaroEventFactory *TaroEventFactory::instance() {
            static TaroEventFactory instance;
            return &instance;
        }

        TaroEventFactory::TaroEventFactory() {
            parser_factory_.resize(TARO_EVENT_TYPE_MAX, nullptr);
        }

        std::shared_ptr<TaroEventBase>
        TaroEventFactory::createEventDataObj(int event_type, const std::string &js_event_type,
                                             ArkUI_NodeEvent *event) {
            if (event_type < 0 || event_type >= TARO_EVENT_TYPE_MAX || event_type >= parser_factory_.size() || parser_factory_[event_type] == nullptr) {
                return nullptr;
            }

            return std::shared_ptr<TaroEventBase>(
                parser_factory_[event_type]->event_create_fun_(js_event_type, event));
        }

        int
        TaroEventFactory::registerHandler(int event_type, ArkUI_NodeEventType hm_ev_type,
                                          const EventCreateFun &new_fun) {
            if (event_type < 0 || event_type >= TARO_EVENT_TYPE_MAX || event_type >= parser_factory_.size() ||
                parser_factory_[event_type] != nullptr) {
                TARO_LOG_ERROR("TaroEvent", "invalid event_type:%{public}d", event_type);
                return -1;
            }

            TaroEventHelper *helper = new TaroEventHelper;
            helper->event_type_ = event_type;
            helper->hm_event_type_ = hm_ev_type;
            helper->event_create_fun_ = new_fun;
            parser_factory_[event_type] = helper;
            // TARO_LOG_DEBUG("Event", "register event_type:%{public}s hm_ev_type:%{public}d", event_type.c_str(), hm_ev_type);
            return 0;
        }

        int TaroEventFactory::getHmEventType(int event_type, ArkUI_NodeEventType &hm_event_type) {
            if (event_type < 0 || event_type >= TARO_EVENT_TYPE_MAX || event_type >= parser_factory_.size() ||
                parser_factory_[event_type] == nullptr) {
                TARO_LOG_ERROR("TaroEvent", "invalid event_type:%{public}d", event_type);
                return -1;
            }

            hm_event_type = parser_factory_[event_type]->hm_event_type_;
            return 0;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
