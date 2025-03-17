//
// Created on 2024/5/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once

#include <jsi/jsi.h>

#include "qjs_runtime.h"
#include "quickjs.h"

class HostObjectProxy {
    public:
    HostObjectProxy(
        QuickJSRuntime &runtime,
        JSContext *ctx,
        std::shared_ptr<facebook::jsi::HostObject> hostObject);

    void BindFinalizer();
    static void Finalizer(JSRuntime *rt, JSValue val);

    std::shared_ptr<facebook::jsi::HostObject> GetHostObject();

    public:
    JSValue weakHandle_;

    static JSClassID initHostObjectClass(JSRuntime *runtime);

    static JSClassID initArrayBufferClass(JSRuntime *runtime);

    static JSClassID initHostNativeStateObjectClass(JSRuntime *runtime);

    static JSValue Getter(JSContext *ctx, JSValueConst this_val, JSAtom atom, JSValueConst value);

    static int Setter(JSContext *ctx, JSValueConst this_val, JSAtom atom, JSValueConst value, JSValueConst receiver,
                      int flags);

    static int Enumerator(JSContext *ctx, JSPropertyEnum **ptab, uint32_t *plen, JSValueConst this_val);

    private:
    QuickJSRuntime &runtime_;
    JSContext *ctx_;
    std::shared_ptr<facebook::jsi::HostObject> hostObject_;
    friend void weakClassFinalizer(JSRuntime *rt, JSValue val);
};

class HostFunctionProxy {
    public:
    HostFunctionProxy(
        QuickJSRuntime &runtime,
        JSContext *ctx,
        facebook::jsi::HostFunctionType &&hostFunction);

    void BindFinalizer();
    static void Finalizer(JSRuntime *rt, JSValue val);

    facebook::jsi::HostFunctionType &GetHostFunction();

    public:
    JSValue weakHandle_;
    static JSClassID initHostFunctionClass(JSRuntime *runtime);
    static JSValue FunctionCallback(JSContext *ctx, JSValueConst func_obj, JSValueConst this_val, int argc, JSValueConst *argv, int flags);

    private:
    QuickJSRuntime &runtime_;
    JSContext *ctx_;
    facebook::jsi::HostFunctionType hostFunction_;
};
