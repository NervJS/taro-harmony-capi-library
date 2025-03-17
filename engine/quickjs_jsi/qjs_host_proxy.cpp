//
// Created on 2024/5/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "qjs_host_proxy.h"

#include <cstddef>

#include "cutils.h"
#include "jsi_qjs_value_converter.h"

HostObjectProxy::HostObjectProxy(
    QuickJSRuntime &runtime,
    JSContext *ctx,
    std::shared_ptr<facebook::jsi::HostObject> hostObject)
    : runtime_(runtime), ctx_(ctx), hostObject_(hostObject) {}

static JSClassID weakRefId_;
static JSClassID weakClassId_;
static JSClassID arrayBufferClassId_;
static JSClassID nativeStateClassId_;

void weakClassFinalizer(JSRuntime *rt, JSValue val) {
    HostObjectProxy *hostObjectProxy = static_cast<HostObjectProxy *>(JS_GetOpaque(val, weakClassId_));
    if (hostObjectProxy->hostObject_.use_count() == 1) {
        hostObjectProxy->hostObject_.reset();
    }
    JS_FreeValueRT(rt, hostObjectProxy->weakHandle_);
    delete hostObjectProxy;
}

void nativeStateFinalizer(JSRuntime *rt, JSValue val) {
    delete reinterpret_cast<std::shared_ptr<facebook::jsi::NativeState> *>(JS_GetOpaque(val, nativeStateClassId_));
}

void arrayBufferFinalizer(JSRuntime *rt, JSValue val) {
    delete static_cast<std::shared_ptr<facebook::jsi::MutableBuffer> *>(JS_GetOpaque(val, arrayBufferClassId_));
}

static JSClassExoticMethods exoticMethods = {
    .get_own_property = nullptr,
    .get_own_property_names = HostObjectProxy::Enumerator,
    .delete_property = nullptr,
    .define_own_property = nullptr,
    .has_property = nullptr,
    .get_property = HostObjectProxy::Getter,
    .set_property = HostObjectProxy::Setter,
};

static JSClassDef weakClass_ = {
    .class_name = "weakClass",
    .finalizer = weakClassFinalizer,
    .exotic = &exoticMethods,
};

static JSClassDef arrayBufferClass_ = {
    .class_name = "arrayBufferClass",
    .finalizer = arrayBufferFinalizer};

static JSClassDef nativeStateClass_ = {
    .class_name = "nativeStateClass",
    .finalizer = nativeStateFinalizer};

JSClassID HostObjectProxy::initHostObjectClass(JSRuntime *runtime) {
    weakClassId_ = JS_NewClassID(&weakClassId_);
    JS_NewClass(runtime, weakClassId_, &weakClass_);

    return weakClassId_;
}

JSClassID HostObjectProxy::initArrayBufferClass(JSRuntime *runtime) {
    arrayBufferClassId_ = JS_NewClassID(&arrayBufferClassId_);
    JS_NewClass(runtime, arrayBufferClassId_, &arrayBufferClass_);

    return arrayBufferClassId_;
}

JSClassID HostObjectProxy::initHostNativeStateObjectClass(JSRuntime *runtime) {
    nativeStateClassId_ = JS_NewClassID(&nativeStateClassId_);
    JS_NewClass(runtime, nativeStateClassId_, &nativeStateClass_);

    return nativeStateClassId_;
}

void HostObjectProxy::BindFinalizer() {
    JSValue object = JS_NewObjectClass(ctx_, weakClassId_);
    if (!JS_IsObject(object))
        return;
    JS_SetOpaque(object, this);
    weakHandle_ = object;
}

std::shared_ptr<facebook::jsi::HostObject> HostObjectProxy::GetHostObject() {
    return hostObject_;
}

JSValue HostObjectProxy::Getter(JSContext *ctx, JSValueConst this_val, JSAtom atom, JSValueConst value) {
    HostObjectProxy *hostObjectProxy = static_cast<HostObjectProxy *>(JS_GetOpaque(this_val, weakClassId_));
    assert(hostObjectProxy);

    auto &runtime = hostObjectProxy->runtime_;
    facebook::jsi::PropNameID sym = QJSJSIValueConverter::ToJSIPropNameID(runtime, atom);
    facebook::jsi::Value ret;
    try {
        ret = hostObjectProxy->hostObject_->get(runtime, sym);
    } catch (const facebook::jsi::JSError &error) {
        JS_Throw(ctx, QJSJSIValueConverter::ToQJSValue(runtime, error.value()));
        return JS_EXCEPTION;
    } catch (const std::exception &ex) {
        auto excValue =
            runtime.global()
                .getPropertyAsFunction(runtime, "Error")
                .call(
                    runtime,
                    std::string("Exception in HostObject::get(property:") +
                        JS_AtomToCString(ctx, atom) +
                        std::string("): ") + ex.what());
        JS_Throw(ctx, QJSJSIValueConverter::ToQJSValue(runtime, excValue));
        return JS_EXCEPTION;
    } catch (...) {
        auto excValue =
            runtime.global()
                .getPropertyAsFunction(runtime, "Error")
                .call(
                    runtime,
                    std::string("Exception in HostObject::get(property:") +
                        JS_AtomToCString(ctx, atom) +
                        std::string("): <unknown>"));
        JS_Throw(ctx, QJSJSIValueConverter::ToQJSValue(runtime, excValue));
        return JS_EXCEPTION;
    }
    return QJSJSIValueConverter::ToQJSValue(runtime, ret);
}

int HostObjectProxy::Setter(
    JSContext *ctx,
    JSValueConst this_val,
    JSAtom atom,
    JSValueConst value,
    JSValueConst receiver,
    int flags) {
    HostObjectProxy *hostObjectProxy = static_cast<HostObjectProxy *>(JS_GetOpaque(this_val, weakClassId_));

    assert(hostObjectProxy);
    auto &runtime = hostObjectProxy->runtime_;
    facebook::jsi::PropNameID sym = QJSJSIValueConverter::ToJSIPropNameID(runtime, atom);
    try {
        hostObjectProxy->hostObject_->set(runtime, sym, QJSJSIValueConverter::ToJSIValue(runtime, value));
    } catch (const facebook::jsi::JSError &error) {
        JS_Throw(ctx, QJSJSIValueConverter::ToQJSValue(runtime, error.value()));
        return -1;
    } catch (const std::exception &ex) {
        auto excValue =
            runtime.global()
                .getPropertyAsFunction(runtime, "Error")
                .call(
                    runtime,
                    std::string("Exception in HostObject::set(property:") +
                        JS_AtomToCString(ctx, atom) +
                        std::string("): ") + ex.what());
        JS_Throw(ctx, QJSJSIValueConverter::ToQJSValue(runtime, excValue));
        return -1;
    } catch (...) {
        auto excValue =
            runtime.global()
                .getPropertyAsFunction(runtime, "Error")
                .call(
                    runtime,
                    std::string("Exception in HostObject::set(property:") +
                        JS_AtomToCString(ctx, atom) +
                        std::string("): <unknown>"));
        JS_Throw(ctx, QJSJSIValueConverter::ToQJSValue(runtime, excValue));
        return -1;
    }
    return 0;
}

int HostObjectProxy::Enumerator(JSContext *ctx, JSPropertyEnum **ptab, uint32_t *plen, JSValueConst this_val) {
    HostObjectProxy *hostObjectProxy = static_cast<HostObjectProxy *>(JS_GetOpaque(this_val, weakClassId_));

    auto &runtime = hostObjectProxy->runtime_;
    auto names = hostObjectProxy->hostObject_->getPropertyNames(runtime);
    *ptab = static_cast<JSPropertyEnum *>(js_malloc(ctx, names.size() * sizeof(JSPropertyEnum)));

    if (*ptab == nullptr) {
        return -1; // 内存分配失败,返回异常
    }

    for (uint32_t i = 0; i < names.size(); ++i) {
        JSValue name = QJSJSIValueConverter::ToQuickJSString(runtime, names[i]);
        (*ptab)[i].atom = JS_ValueToAtom(ctx, name);
        (*ptab)[i].is_enumerable = TRUE;
        JS_FreeValue(ctx, name);
    }

    // 设置属性数量
    *plen = names.size();

    return 0; // 成功返回
}

static JSClassID weakClassFunctionId_;
static JSClassDef weakFunctionClass_ = {
    .class_name = "weakFunctionClass",
    .finalizer = HostFunctionProxy::Finalizer,
    .call = HostFunctionProxy::FunctionCallback,
};

HostFunctionProxy::HostFunctionProxy(
    QuickJSRuntime &runtime,
    JSContext *ctx,
    facebook::jsi::HostFunctionType &&hostFunction)
    : runtime_(runtime),
      ctx_(ctx),
      hostFunction_(std::move(hostFunction)) {}

JSClassID HostFunctionProxy::initHostFunctionClass(JSRuntime *runtime) {
    weakClassFunctionId_ = JS_NewClassID(&weakClassFunctionId_);
    JS_NewClass(runtime, weakClassFunctionId_, &weakFunctionClass_);

    return weakClassFunctionId_;
}

void HostFunctionProxy::BindFinalizer() {
    JSValue object = JS_NewObjectClass(ctx_, weakClassFunctionId_);
    if (!JS_IsObject(object))
        return;
    JS_SetOpaque(object, this);
    weakHandle_ = object;
}

facebook::jsi::HostFunctionType &HostFunctionProxy::GetHostFunction() {
    return hostFunction_;
}

void HostFunctionProxy::Finalizer(JSRuntime *rt, JSValue val) {
    HostFunctionProxy *hostFunctionProxy = static_cast<HostFunctionProxy *>(JS_GetOpaque(val, weakClassFunctionId_));
    JS_FreeValueRT(rt, hostFunctionProxy->weakHandle_);
    delete hostFunctionProxy;
}

JSValue HostFunctionProxy::FunctionCallback(JSContext *ctx, JSValueConst func_obj, JSValueConst this_val, int argc, JSValueConst *argv, int flags) {
    HostFunctionProxy *hostFunctionProxy = (HostFunctionProxy *)JS_GetOpaque(func_obj, weakClassFunctionId_);
    if (!hostFunctionProxy) {
        JS_ThrowTypeError(ctx, "Invalid HostFunctionProxy");
        return JS_EXCEPTION;
    }

    auto &runtime = hostFunctionProxy->runtime_;

    // 将QuickJS的参数转换为JSI的值
    const unsigned maxStackArgCount = 8;
    facebook::jsi::Value stackArgs[maxStackArgCount];
    std::unique_ptr<facebook::jsi::Value[]> heapArgs;
    facebook::jsi::Value *args;
    if (argc > maxStackArgCount) {
        heapArgs = std::make_unique<facebook::jsi::Value[]>(argc);
        for (int i = 0; i < argc; i++) {
            heapArgs[i] = QJSJSIValueConverter::ToJSIValue(runtime, argv[i]);
        }
        args = heapArgs.get();
    } else {
        for (int i = 0; i < argc; i++) {
            stackArgs[i] = QJSJSIValueConverter::ToJSIValue(runtime, argv[i]);
        }
        args = stackArgs;
    }

    // 调用HostFunction并处理异常
    JSValue result;
    facebook::jsi::Value thisVal(QJSJSIValueConverter::ToJSIValue(runtime, this_val));

    try {
        result = QJSJSIValueConverter::ToQJSValue(
            runtime,
            hostFunctionProxy->hostFunction_(runtime, thisVal, args, argc));
    } catch (const facebook::jsi::JSError &error) {
        JS_Throw(ctx, QJSJSIValueConverter::ToQJSValue(runtime, error.value()));
        return JS_EXCEPTION;
    } catch (const std::exception &ex) {
        std::string exceptionString("Exception in HostFunction: ");
        exceptionString += ex.what();
        JSValue errorConstructor = JS_GetPropertyStr(ctx, JS_GetGlobalObject(ctx), "Error");
        JSValue temp = JS_NewString(ctx, exceptionString.c_str());
        JSValue errorObj = JS_Call(ctx, errorConstructor, JS_UNDEFINED, 1, &temp);
        JS_FreeValue(ctx, errorConstructor);
        JS_Throw(ctx, errorObj);
        return JS_EXCEPTION;
    } catch (...) {
        std::string exceptionString("Exception in HostFunction: <unknown>");
        JSValue errorConstructor = JS_GetPropertyStr(ctx, JS_GetGlobalObject(ctx), "Error");
        JSValue temp = JS_NewString(ctx, exceptionString.c_str());
        JSValue errorObj = JS_Call(ctx, errorConstructor, JS_UNDEFINED, 1, &temp);
        JS_FreeValue(ctx, errorConstructor);
        JS_Throw(ctx, errorObj);
        return JS_EXCEPTION;
    }

    return result;
}
