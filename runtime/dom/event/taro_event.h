#pragma once
#include <string>
#include <arkui/native_node.h>
#include <napi/native_api.h>

#include "event_generator.h"
#include "runtime/dom/ark_nodes/arkui_node.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
class TaroEvent : public std::enable_shared_from_this<TaroEvent> {
    public:
    TaroEvent();
    virtual ~TaroEvent();
    int recallJS();

    virtual int serializeFun() {
        return 0;
    }

    public:
    std::string js_event_type_;

    bool bubbles_ = true;

    bool cancelable_ = true;

    bool end_ = false;

    bool stop_ = false;

    TaroElementRef target_ = nullptr;

    TaroElementRef cur_target_ = nullptr;

    uint32_t timestamp_ = 0;

    void SetNapiValue(napi_value event_obj);
    napi_value GetNapiValue();

    EventGeneratorType gen_type_ = EventGeneratorType::UserDefine;

    int event_type = -1;

    ArkUI_NodeHandle node_handle_ = nullptr;

    public:
    void memToJs();
    void jsToMem();
    void jsToMem_Partition();
    void memToJs_Partition();

    private:
    napi_ref napi_ref_ = nullptr;
    napi_handle_scope scope_ = nullptr;
};

using TaroEventPtr = std::shared_ptr<TaroEvent>;
} // namespace TaroRuntime::TaroDOM::TaroEvent