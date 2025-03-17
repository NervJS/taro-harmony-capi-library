//
// Created on 2024/5/28.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "jsi_qjs_value_converter.h"

#include <jsi/jsi.h>

#include "qjs_pointer_value.h"

// 将 QuickJS 的 JSValue 转换为 JSI 的 Value
facebook::jsi::Value QJSJSIValueConverter::ToJSIValue(QuickJSRuntime &runtime, JSValueConst value) {
    JSContext *ctx = runtime.context_;

    if (JS_IsUndefined(value)) {
        return facebook::jsi::Value::undefined();
    } else if (JS_IsNull(value)) {
        return facebook::jsi::Value::null();
    } else if (JS_IsBool(value)) {
        return facebook::jsi::Value(JS_ToBool(ctx, value));
    } else if (JS_IsNumber(value)) {
        double number;
        if (JS_ToFloat64(ctx, &number, value) < 0) {
            throw std::runtime_error("Failed to convert JSValue to float64");
        }
        return facebook::jsi::Value(number);
    } else if (JS_IsString(value)) {
        return QuickJSRuntime::make<facebook::jsi::String>(new QuickJSPointerValue(ctx, value));
    } else if (JS_IsSymbol(value)) {
        return QuickJSRuntime::make<facebook::jsi::Symbol>(new QuickJSPointerValue(ctx, value));
    } else if (JS_IsObject(value)) {
        return QuickJSRuntime::make<facebook::jsi::Object>(new QuickJSPointerValue(ctx, value));
    }

    return facebook::jsi::Value::undefined();
}

JSValue QJSJSIValueConverter::ToQuickJSString(const QuickJSRuntime &runtime, const facebook::jsi::String &string) {
    const QuickJSPointerValue *qjsPointerValue =
        static_cast<const QuickJSPointerValue *>(runtime.getPointerValue(string));
    JSValue value = qjsPointerValue->Get();
    assert(JS_IsString(value));
    return JS_DupValue(runtime.context_, value);
}

JSValue QJSJSIValueConverter::ToQuickJSString(const QuickJSRuntime &runtime, const facebook::jsi::PropNameID &propName) {
    const QuickJSPointerValue *qjsPointerValue =
        static_cast<const QuickJSPointerValue *>(runtime.getPointerValue(propName));
    JSValue value = qjsPointerValue->Get();
    assert(JS_IsString(value));
    return JS_DupValue(runtime.context_, value);
}

JSValue QJSJSIValueConverter::ToQuickJSString(const QuickJSRuntime &runtime, const std::shared_ptr<const facebook::jsi::Buffer> &buffer) {
    // 使用 QuickJS 创建一个字符串值
    JSValue value = JS_NewStringLen(runtime.context_, reinterpret_cast<const char *>(buffer->data()), buffer->size());
    // 返回创建的字符串值
    return value;
}

JSValue QJSJSIValueConverter::ToQuickJSSymbol(const QuickJSRuntime &runtime, const facebook::jsi::Symbol &symbol) {
    const QuickJSPointerValue *qjsPointerValue =
        static_cast<const QuickJSPointerValue *>(runtime.getPointerValue(symbol));
    JSValue value = qjsPointerValue->Get();
    assert(JS_IsSymbol(value));
    return JS_DupValue(runtime.context_, value);
}

JSValue QJSJSIValueConverter::ToQuickJSObject(const QuickJSRuntime &runtime, const facebook::jsi::Object &object) {
    const QuickJSPointerValue *qjsPointerValue =
        static_cast<const QuickJSPointerValue *>(runtime.getPointerValue(object));
    JSValue value = qjsPointerValue->Get();
    assert(JS_IsObject(value));
    return JS_DupValue(runtime.context_, value);
}

JSValue QJSJSIValueConverter::ToQuickJSArray(const QuickJSRuntime &runtime, const facebook::jsi::Array &array) {
    const QuickJSPointerValue *qjsPointerValue =
        static_cast<const QuickJSPointerValue *>(runtime.getPointerValue(array));
    JSValue value = qjsPointerValue->Get();
    assert(JS_IsArray(runtime.context_, value));
    return JS_DupValue(runtime.context_, value);
}

JSValue QJSJSIValueConverter::ToQuickJSFunction(const QuickJSRuntime &runtime,
                                                const facebook::jsi::Function &function) {
    const QuickJSPointerValue *qjsPointerValue =
        static_cast<const QuickJSPointerValue *>(runtime.getPointerValue(function));
    JSValue value = qjsPointerValue->Get();
    assert(JS_IsFunction(runtime.context_, value));
    return JS_DupValue(runtime.context_, value);
}

// 将 JSI 的 Value 转换为 QuickJS 的 JSValue
JSValue QJSJSIValueConverter::ToQJSValue(const QuickJSRuntime &runtime, const facebook::jsi::Value &value) {
    if (value.isUndefined()) {
        return JS_UNDEFINED;
    } else if (value.isNull()) {
        return JS_NULL;
    } else if (value.isBool()) {
        return JS_NewBool(runtime.context_, value.getBool());
    } else if (value.isNumber()) {
        return JS_NewFloat64(runtime.context_, value.getNumber());
    } else if (value.isString()) {
        return ToQuickJSString(runtime, value.getString(const_cast<QuickJSRuntime &>(runtime)));
    } else if (value.isObject()) {
        return ToQuickJSObject(runtime, value.getObject(const_cast<QuickJSRuntime &>(runtime)));
    } else {
        // What are you?
        std::abort();
    }
}

facebook::jsi::PropNameID QJSJSIValueConverter::ToJSIPropNameID(const QuickJSRuntime &runtime, JSAtom property) {
    JSValue symbol = JS_AtomToValue(runtime.context_, property);
    return runtime.make<facebook::jsi::PropNameID>(new QuickJSPointerValue(runtime.context_, symbol));
}

std::string QJSJSIValueConverter::ToSTLString(QuickJSRuntime &runtime, JSValueConst string) {
    assert(JS_IsString(string));
    const char *str = JS_ToCString(runtime.context_, string);
    if (str) {
        std::string result(str);
        JS_FreeCString(runtime.context_, str);
        return result;
    }
    return {};
}
