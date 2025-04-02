/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#pragma once

#include <string>
#include <jsi/jsi.h>
#include <quickjs.h>

#include "qjs_runtime.h"

class QuickJSRuntime;

class QJSJSIValueConverter {
    private:
    QJSJSIValueConverter() = delete;
    ~QJSJSIValueConverter() = delete;
    QJSJSIValueConverter(QJSJSIValueConverter &&) = delete;

    public:
    static facebook::jsi::Value ToJSIValue(
        QuickJSRuntime &runtime,
        JSValueConst value);

    static JSValue ToQJSValue(
        const QuickJSRuntime &runtime,
        const facebook::jsi::Value &value);

    static JSValue ToQuickJSString(
        const QuickJSRuntime &runtime,
        const facebook::jsi::String &string);

    static JSValue ToQuickJSString(
        const QuickJSRuntime &runtime,
        const facebook::jsi::PropNameID &propName);

    static JSValue ToQuickJSString(
        const QuickJSRuntime &runtime,
        const std::shared_ptr<const facebook::jsi::Buffer> &buffer);

    static JSValue ToQuickJSSymbol(
        const QuickJSRuntime &runtime,
        const facebook::jsi::Symbol &symbol);

    static JSValue ToQuickJSObject(
        const QuickJSRuntime &runtime,
        const facebook::jsi::Object &object);

    static JSValue ToQuickJSArray(
        const QuickJSRuntime &runtime,
        const facebook::jsi::Array &array);

    static JSValue ToQuickJSFunction(
        const QuickJSRuntime &runtime,
        const facebook::jsi::Function &function);

    static facebook::jsi::PropNameID ToJSIPropNameID(
        const QuickJSRuntime &runtime,
        JSAtom property);

    static std::string ToSTLString(
        QuickJSRuntime &runtime,
        JSValueConst string);
};
