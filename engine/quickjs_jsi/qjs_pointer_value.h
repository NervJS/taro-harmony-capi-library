/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

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
