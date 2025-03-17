//
// Created on 2024/5/28.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <jsi/jsi.h>

#include "qjs_runtime.h"
#include "quickjs.h"

class QuickJSPointerValue final : public QuickJSRuntime::PointerValue {
    public:
    QuickJSPointerValue(JSContext *ctx, const JSValueConst &value);
    QuickJSPointerValue(JSContext *ctx, JSValue &&value);

    ~QuickJSPointerValue();

    JSValue Get() const;
    void Reset(JSContext *ctx, JSValueConst value);

    static QuickJSPointerValue *createFromOneByte(JSContext *ctx, const char *str, size_t length);
    static QuickJSPointerValue *createFromUtf8(JSContext *ctx, const uint8_t *str, size_t length);

    void invalidate();

    private:
    JSContext *ctx_;
    JSValue value_;
};
