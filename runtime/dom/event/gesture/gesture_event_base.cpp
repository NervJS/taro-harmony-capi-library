#include "gesture_event_base.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
int GestureEventBase::serializeFun() {
    napi_value event_obj = GetNapiValue();
    if (event_obj == nullptr) {
        return -1;
    }
    js_event_obj_ = event_obj;
    serialize();
    return 0;
}

GestureOperator::~GestureOperator() {
    if (recognizer_) {
        recognizer_ = nullptr;
    }
}
} // namespace TaroRuntime::TaroDOM::TaroEvent
