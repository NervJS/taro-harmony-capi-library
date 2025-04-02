/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#include "qjs_pointer_value.h"

QuickJSPointerValue::QuickJSPointerValue(JSContext *ctx, const JSValueConst &value)
    : ctx_(ctx), value_(JS_DupValue(ctx, value)) {}

QuickJSPointerValue::QuickJSPointerValue(JSContext *ctx, JSValue &&value)
    : ctx_(ctx), value_(value) {
    value = JS_UNDEFINED;
}

QuickJSPointerValue::~QuickJSPointerValue() {
    JS_FreeValue(ctx_, value_);
}

JSValue QuickJSPointerValue::Get() const {
    return value_;
}

void QuickJSPointerValue::Reset(JSContext *ctx, JSValueConst value) {
    JS_FreeValue(ctx_, value_);
    value_ = JS_DupValue(ctx, value);
}

QuickJSPointerValue *QuickJSPointerValue::createFromOneByte(JSContext *ctx, const char *str, size_t length) {
    JSValue jsString = JS_NewStringLen(ctx, str, length);
    if (JS_IsException(jsString)) {
        return nullptr;
    }
    return new QuickJSPointerValue(ctx, jsString);
}

QuickJSPointerValue *QuickJSPointerValue::createFromUtf8(JSContext *ctx, const uint8_t *str, size_t length) {
    JSValue jsString = JS_NewStringLen(ctx, reinterpret_cast<const char *>(str), length);
    if (JS_IsException(jsString)) {
        return nullptr;
    }
    return new QuickJSPointerValue(ctx, jsString);
}

void QuickJSPointerValue::invalidate() {
    JS_FreeValue(ctx_, value_);
    value_ = JS_UNDEFINED;
    delete this;
}
