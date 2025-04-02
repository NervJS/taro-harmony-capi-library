/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_helper.h"

#include "arkjs/ArkJS.h"
#include "helper/TaroLog.h"
#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        EventHelper* EventHelper::instance() {
            static auto s_handler = new EventHelper;
            return s_handler;
        }

        int EventHelper::init(napi_value n_val) {
            ArkJS ark_js(NativeNodeApi::env);
            napi_value n_create_event = nullptr;
            n_create_event = ark_js.getObjectProperty(n_val, "createEvent");
            ref_create_event_ = ark_js.createReference(n_create_event);
            TARO_LOG_DEBUG("TaroEvent", "ref_create_event_:%{public}p", ref_create_event_);

            napi_value n_create_taro_event = nullptr;
            n_create_taro_event = ark_js.getObjectProperty(n_val, "createTaroEvent");
            ref_create_taro_event_ = ark_js.createReference(n_create_taro_event);
            TARO_LOG_DEBUG("TaroEvent", "ref_create_taro_event_:%{public}p", ref_create_taro_event_);

            napi_value n_event_handle = nullptr;
            n_event_handle = ark_js.getObjectProperty(n_val, "eventHandler");
            ref_event_handler_ = ark_js.createReference(n_event_handle);
            TARO_LOG_DEBUG("TaroEvent", "ref_event_handler_:%{public}p", ref_event_handler_);

            return 0;
        }

        int EventHelper::createEventObj(const std::string& event_type, napi_value& obj_event) {
            ArkJS ark_js(NativeNodeApi::env);
            auto create_fun = ark_js.getReferenceValue(ref_create_event_);
            if (!create_fun) {
                TARO_LOG_ERROR("ToraEvent", "get create event fun failed");
                return -1;
            }
            napi_value event_type_name = ark_js.createString(event_type);
            obj_event = ark_js.call(create_fun, {event_type_name});
            return 0;
        }

        int EventHelper::createTaroEventObj(const std::string& event_type, napi_value node, napi_value& obj_event) {
            ArkJS ark_js(NativeNodeApi::env);
            auto create_fun = ark_js.getReferenceValue(ref_create_taro_event_);
            if (!create_fun) {
                TARO_LOG_ERROR("ToraEvent", "get create taro event fun failed");
                return -1;
            }
            napi_value event_type_name = ark_js.createString(event_type);
            obj_event = ark_js.call(create_fun, {event_type_name, nullptr, node});
            return 0;
        }

        int EventHelper::eventHandler(napi_value event, const std::string& event_type, napi_value node) {
            ArkJS ark_js(NativeNodeApi::env);
            auto handle_fun = ark_js.getReferenceValue(ref_event_handler_);
            if (!handle_fun) {
                TARO_LOG_ERROR("ToraEvent", "get event handle fun failed");
                return -1;
            }

            napi_value event_type_name = ark_js.createString(event_type);
            ark_js.call(handle_fun, {event, event_type_name, node});
            return 0;
        }
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
