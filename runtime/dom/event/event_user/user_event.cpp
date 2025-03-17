#include "user_event.h"

#include "runtime/NapiSetter.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
UserEventBase::UserEventBase() {
    gen_type_ = EventGeneratorType::UserDefine;
}

UserEventBase::UserEventBase(const std::string& js_event_type) {
    gen_type_ = EventGeneratorType::UserDefine;
    js_event_type_ = js_event_type;
}

napi_value& UserEventBase::detail() {
    if (js_detail_ == nullptr) {
        napi_create_object(NativeNodeApi::env, &js_detail_);
    }
    return js_detail_;
}

int UserEventBase::serializeFun() {
    napi_value event_obj = GetNapiValue();
    if (event_obj) {
        NapiSetter::SetProperty(event_obj, "detail", js_detail_);
        return 0;
    }
    return -1;
}
} // namespace TaroRuntime::TaroDOM::TaroEvent
