#pragma once

#include <list>
#include <string>
#include <unordered_map>

#include "arkjs/ArkJS.h"

namespace TaroRuntime::TaroDOM::TaroEvent {

class EventCenterData {
    public:
    EventCenterData() = default;
    virtual ~EventCenterData() = default;
    void setJsValue(napi_value n_val) {
        n_val_ = n_val;
        is_from_js_ = true;
    }
    napi_value memToJs() {
        if (!n_val_) {
            serialize();
        }
        return n_val_;
    };
    virtual void serialize() {}
    napi_value getNapiVal() {
        return n_val_;
    }

    protected:
    napi_value n_val_ = nullptr;
    bool is_from_js_ = false;
};

using EventCallBackFun = void (*)(std::shared_ptr<EventCenterData>, void*);
class EventCenter final {
    class CallBackInfo {
        public:
        virtual ~CallBackInfo() = default;
        bool is_js = false;
        bool is_once = false;
    };
    class CallBackInfo_CAPI : public CallBackInfo {
        public:
        EventCallBackFun call_fun_ = nullptr;
        void* context_ = nullptr;
    };
    class CallBackInfo_JS : public CallBackInfo {
        public:
        ~CallBackInfo_JS();
        napi_ref call_fun_ = nullptr;
        napi_ref context_ = nullptr;
    };
    using CallBackInfosPtr = std::shared_ptr<std::list<std::shared_ptr<CallBackInfo>>>;

    public:
    EventCenter() = default;
    ~EventCenter() = default;
    static EventCenter* instance();
    void on(const std::string& event_name, EventCallBackFun callback, void* context = nullptr);
    void on(const std::string& event_name, napi_value callback, napi_value context);
    void off(const std::string& event_name, EventCallBackFun callback, void* context = nullptr);
    void off(const std::string& event_name, napi_value callback, napi_value context);
    void once(const std::string& event_name, EventCallBackFun callback, void* context = nullptr);
    void once(const std::string& event_name, napi_value callback, napi_value context);
    void trigger(const std::string& event_name, napi_value n_val);
    void trigger(const std::string& event_name, std::shared_ptr<EventCenterData> data);

    private:
    void addListen(const std::string& event_name, EventCallBackFun callback, void* context, bool once);
    void addListen(const std::string& event_name, napi_value callback, napi_value context, bool once);
    void callbackJS(napi_ref call_fun, napi_ref context, std::vector<napi_value> data);

    private:
    std::unordered_map<std::string, CallBackInfosPtr> callbacks_;
};
} // namespace TaroRuntime::TaroDOM::TaroEvent
