/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#include "qjs_runtime.h"

#include <ostream>
#include <sstream>
#include <jsi/jsilib.h>

#include "jsi_qjs_value_converter.h"
#include "qjs_host_proxy.h"
#include "qjs_pointer_value.h"
#include "quickjs.h"

QuickJSRuntime::QuickJSRuntime(std::unique_ptr<QuickJSRuntimeConfig> config, std::shared_ptr<MessageQueueThread> jsQueue)
    : config_(std::move(config)), jsQueue_(jsQueue) {
    runtime_ = JS_NewRuntime();
    context_ = JS_NewContext(runtime_);
    weakClassId_ = HostObjectProxy::initHostObjectClass(runtime_);
    weakFunctionId_ = HostFunctionProxy::initHostFunctionClass(runtime_);
    arrayBufferClassId_ = HostObjectProxy::initArrayBufferClass(runtime_);
    nativeStateClassId_ = HostObjectProxy::initHostNativeStateObjectClass(runtime_);
}

QuickJSRuntime::QuickJSRuntime(const QuickJSRuntime *quickJSRuntime, std::unique_ptr<QuickJSRuntimeConfig> config)
    : config_(std::move(config)) {
    isSharedRuntime_ = true;
    runtime_ = quickJSRuntime->runtime_;
    jsQueue_ = quickJSRuntime->jsQueue_;
    context_ = JS_NewContext(runtime_);

    JSValue globalObject = JS_GetGlobalObject(context_);
    JSValue parentGlobalObject = JS_GetGlobalObject(quickJSRuntime->context_);

    bool inheritProtoResult = JS_SetPrototype(context_, globalObject, parentGlobalObject);
    if (!inheritProtoResult) {
        throw facebook::jsi::JSError(*this, "Unable to inherit prototype from parent shared runtime.");
    }

    JS_FreeValue(context_, globalObject);
    JS_FreeValue(context_, parentGlobalObject);
}

QuickJSRuntime::~QuickJSRuntime() {
    JS_FreeContext(context_);
    JS_FreeRuntime(runtime_);
}

// TODO: 在 V8 的实现中，添加了对 script 的缓存，当前版本暂时不考虑，后续可以考虑添加优化
facebook::jsi::Value QuickJSRuntime::ExecuteScript(const JSValue &script, const std::string &sourceURL) {
    if (!JS_IsString(script)) {
        throw facebook::jsi::JSError(*this, "Script must be a string");
    }

    const char *scriptStr = JS_ToCString(context_, script);
    if (!scriptStr) {
        throw facebook::jsi::JSError(*this, "Failed to convert script to C string");
    }

    JSValue result = JS_Eval(context_, scriptStr, strlen(scriptStr), sourceURL.c_str(), JS_EVAL_TYPE_GLOBAL);
    JS_FreeCString(context_, scriptStr);

    if (JS_IsException(result)) {
        JSValue exception = JS_GetException(context_);
        ReportException(context_, exception);
        JS_FreeValue(context_, exception);
        return {};
    }

    return QJSJSIValueConverter::ToJSIValue(*this, result);
}

void QuickJSRuntime::ReportException(JSContext *ctx, JSValue exception) const {
    std::ostringstream ss;

    JSValue exceptionStr = JS_ToString(ctx, exception);
    std::string exceptionMessage = QJSJSIValueConverter::ToSTLString(const_cast<QuickJSRuntime &>(*this), exceptionStr);
    JS_FreeValue(ctx, exceptionStr);

    JSValue stackStr = JS_GetPropertyStr(ctx, exception, "stack");
    if (!JS_IsUndefined(stackStr)) {
        std::string stackTrace = QJSJSIValueConverter::ToSTLString(const_cast<QuickJSRuntime &>(*this), stackStr);
        ss << stackTrace << std::endl;
    }
    JS_FreeValue(ctx, stackStr);

    // Get the line number and source line if available
    JSValue lineNum = JS_GetPropertyStr(ctx, exception, "lineNumber");
    if (JS_IsNumber(lineNum)) {
        int line = JS_VALUE_GET_INT(lineNum);
        ss << ":" << line << ": " << exceptionMessage << std::endl;

        JSValue sourceLine = JS_GetPropertyStr(ctx, exception, "sourceLine");
        if (JS_IsString(sourceLine)) {
            std::string sourceLineStr = QJSJSIValueConverter::ToSTLString(const_cast<QuickJSRuntime &>(*this), sourceLine);
            ss << sourceLineStr << std::endl;

            JSValue columnNum = JS_GetPropertyStr(ctx, exception, "columnNumber");
            if (JS_IsNumber(columnNum)) {
                int column = JS_VALUE_GET_INT(columnNum);
                for (int i = 0; i < column; i++) {
                    ss << " ";
                }
                ss << "^" << std::endl;
            }
            JS_FreeValue(ctx, columnNum);
        }
        JS_FreeValue(ctx, sourceLine);
    }
    JS_FreeValue(ctx, lineNum);

    throw facebook::jsi::JSError(const_cast<QuickJSRuntime &>(*this), ss.str());
}

//
// jsi::Runtime implementations
//
facebook::jsi::Value QuickJSRuntime::evaluateJavaScript(const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
                                                        const std::string &sourceURL) {
    JSValue string = QJSJSIValueConverter::ToQuickJSString(*this, buffer);
    if (!JS_IsException(string)) {
        return ExecuteScript(string, sourceURL);
    }
    return {};
}

std::shared_ptr<const facebook::jsi::PreparedJavaScript>
QuickJSRuntime::prepareJavaScript(const std::shared_ptr<const facebook::jsi::Buffer> &buffer, std::string sourceURL) {
    return std::make_shared<facebook::jsi::SourceJavaScriptPreparation>(buffer, std::move(sourceURL));
}

facebook::jsi::Value QuickJSRuntime::evaluatePreparedJavaScript(const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &js) {
    assert(dynamic_cast<const facebook::jsi::SourceJavaScriptPreparation *>(js.get()) &&
           "preparedJavaScript must be a SourceJavaScriptPreparation");
    auto sourceJs = std::static_pointer_cast<const facebook::jsi::SourceJavaScriptPreparation>(js);
    return evaluateJavaScript(sourceJs, sourceJs->sourceURL());
}

bool QuickJSRuntime::drainMicrotasks(int maxMicrotasksHint) {
    JSContext *ctx = context_;

    while (JS_IsJobPending(runtime_)) {
        int err = JS_ExecutePendingJob(runtime_, &ctx);
        if (err < 0) {
            JSValue exception = JS_GetException(ctx);
            JS_FreeValue(ctx, exception);
            return false;
        }
    }

    return true;
}

facebook::jsi::Object QuickJSRuntime::global() {
    return make<facebook::jsi::Object>(new QuickJSPointerValue(context_, JS_GetGlobalObject(context_)));
}

std::string QuickJSRuntime::description() {
    std::ostringstream ss;
    ss << "<QuickJSRuntime@" << this << ">";
    return ss.str();
}

bool QuickJSRuntime::isInspectable() {
    return false;
}

facebook::jsi::Runtime::PointerValue *QuickJSRuntime::cloneSymbol(const facebook::jsi::Runtime::PointerValue *pv) {
    if (!pv) {
        return nullptr;
    }

    const QuickJSPointerValue *qjsPointerValue = static_cast<const QuickJSPointerValue *>(pv);
    JSValue originalSymbol = qjsPointerValue->Get();
    assert(JS_IsSymbol(originalSymbol));

    JSValue clonedSymbol = JS_DupValue(context_, originalSymbol);
    return new QuickJSPointerValue(context_, clonedSymbol);
}

facebook::jsi::Runtime::PointerValue *QuickJSRuntime::cloneBigInt(const facebook::jsi::Runtime::PointerValue *pv) {
    if (!pv) {
        return nullptr;
    }

    const QuickJSPointerValue *qjsPointerValue = static_cast<const QuickJSPointerValue *>(pv);
    JSValue originalBigInt = qjsPointerValue->Get();
    assert(JS_IsBigInt(context_, originalBigInt));

    JSValue clonedBigInt = JS_DupValue(context_, originalBigInt);
    return new QuickJSPointerValue(context_, clonedBigInt);
}

facebook::jsi::Runtime::PointerValue *QuickJSRuntime::cloneString(const facebook::jsi::Runtime::PointerValue *pv) {
    if (!pv) {
        return nullptr;
    }

    const QuickJSPointerValue *qjsPointerValue = static_cast<const QuickJSPointerValue *>(pv);
    JSValue originalString = qjsPointerValue->Get();
    assert(JS_IsString(originalString));

    JSValue clonedString = JS_DupValue(context_, originalString);
    return new QuickJSPointerValue(context_, clonedString);
}

facebook::jsi::Runtime::PointerValue *QuickJSRuntime::cloneObject(const facebook::jsi::Runtime::PointerValue *pv) {
    if (!pv) {
        return nullptr;
    }

    const QuickJSPointerValue *qjsPointerValue = static_cast<const QuickJSPointerValue *>(pv);
    JSValue originalObject = qjsPointerValue->Get();
    assert(JS_IsObject(originalObject));

    JSValue clonedObject = JS_DupValue(context_, originalObject);
    return new QuickJSPointerValue(context_, clonedObject);
}

facebook::jsi::Runtime::PointerValue *QuickJSRuntime::clonePropNameID(const facebook::jsi::Runtime::PointerValue *pv) {
    return cloneString(pv);
}

facebook::jsi::PropNameID QuickJSRuntime::createPropNameIDFromAscii(const char *str, size_t length) {
    QuickJSPointerValue *value = QuickJSPointerValue::createFromOneByte(context_, str, length);
    if (!value) {
        throw facebook::jsi::JSError(*this, "QuickJSPointerValue::createFromString() - string creation failed.");
    }

    return make<facebook::jsi::PropNameID>(value);
}

facebook::jsi::PropNameID QuickJSRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) {
    QuickJSPointerValue *value = QuickJSPointerValue::createFromUtf8(context_, utf8, length);
    if (!value) {
        throw facebook::jsi::JSError(*this,
                                     "QuickJSPointerValue::createPropNameIDFromUtf8() - string creation failed.");
    }

    return make<facebook::jsi::PropNameID>(value);
}

facebook::jsi::PropNameID QuickJSRuntime::createPropNameIDFromString(const facebook::jsi::String &str) {
    std::string utf8Value = utf8(str);

    return createPropNameIDFromUtf8(reinterpret_cast<const uint8_t *>(utf8Value.c_str()), utf8Value.length());
}

facebook::jsi::PropNameID QuickJSRuntime::createPropNameIDFromSymbol(const facebook::jsi::Symbol &sym) {
    const QuickJSPointerValue *quickPointerValue = static_cast<const QuickJSPointerValue *>(getPointerValue(sym));
    assert(JS_IsSymbol(quickPointerValue->Get()));
    return make<facebook::jsi::PropNameID>(const_cast<PointerValue *>(getPointerValue(sym)));
}

std::string QuickJSRuntime::utf8(const facebook::jsi::PropNameID &sym) {
    const QuickJSPointerValue *quickPointerValue = static_cast<const QuickJSPointerValue *>(getPointerValue(sym));
    JSValue value = quickPointerValue->Get();
    return QJSJSIValueConverter::ToSTLString(*this, value);
}

bool QuickJSRuntime::compare(const facebook::jsi::PropNameID &a, const facebook::jsi::PropNameID &b) {
    const QuickJSPointerValue *quickJSPointerValueA = static_cast<const QuickJSPointerValue *>(getPointerValue(a));
    const QuickJSPointerValue *quickJSPointerValueB = static_cast<const QuickJSPointerValue *>(getPointerValue(b));

    if (!quickJSPointerValueA || !quickJSPointerValueB) {
        throw std::invalid_argument("Invalid PropNameID");
    }

    JSValue jsStrA = quickJSPointerValueA->Get();
    JSValue jsStrB = quickJSPointerValueB->Get();

    bool result = JS_StrictEq(context_, jsStrA, jsStrB);

    JS_FreeValue(context_, jsStrA);
    JS_FreeValue(context_, jsStrB);

    return result;
}

std::string QuickJSRuntime::symbolToString(const facebook::jsi::Symbol &symbol) {
    return facebook::jsi::Value(*this, symbol).toString(*this).utf8(*this);
}

facebook::jsi::BigInt QuickJSRuntime::createBigIntFromInt64(int64_t value) {
    JSValue qjsBigInt = JS_NewBigInt64(context_, value);
    return make<facebook::jsi::BigInt>(new QuickJSPointerValue(context_, qjsBigInt));
}

facebook::jsi::BigInt QuickJSRuntime::createBigIntFromUint64(uint64_t value) {
    JSValue qjsUnsignedBigInt = JS_NewBigUint64(context_, value);
    return make<facebook::jsi::BigInt>(new QuickJSPointerValue(context_, qjsUnsignedBigInt));
}

bool QuickJSRuntime::bigintIsInt64(const facebook::jsi::BigInt &value) {
    const QuickJSPointerValue *qjsPointerValue = static_cast<const QuickJSPointerValue *>(getPointerValue(value));
    JSValue jsValue = qjsPointerValue->Get();

    assert(JS_IsBigInt(context_, jsValue));

    int64_t int64Value;
    int ret = JS_ToBigInt64(context_, &int64Value, jsValue);
    JS_FreeValue(context_, jsValue);

    return ret == 0;
}

bool QuickJSRuntime::bigintIsUint64(const facebook::jsi::BigInt &value) {
    const QuickJSPointerValue *qjsPointerValue = static_cast<const QuickJSPointerValue *>(getPointerValue(value));
    JSValue jsValue = qjsPointerValue->Get();

    assert(JS_IsBigInt(context_, jsValue));

    int64_t int64Value;
    int ret = JS_ToBigInt64(context_, &int64Value, jsValue);
    JS_FreeValue(context_, jsValue);

    if (ret != 0) {
        return false;
    }

    return int64Value >= 0 && static_cast<uint64_t>(int64Value) <= UINT64_MAX;
}

uint64_t QuickJSRuntime::truncate(const facebook::jsi::BigInt &value) {
    const QuickJSPointerValue *qjsPointerValue = static_cast<const QuickJSPointerValue *>(getPointerValue(value));
    JSValue jsValue = qjsPointerValue->Get();

    assert(JS_IsBigInt(context_, jsValue));

    int64_t int64Value = 0;
    JS_ToBigInt64(context_, &int64Value, jsValue);
    JS_FreeValue(context_, jsValue);

    return static_cast<uint64_t>(int64Value);
}

facebook::jsi::String QuickJSRuntime::bigintToString(const facebook::jsi::BigInt &value, int radix) {
    const QuickJSPointerValue *qjsPointerValue = static_cast<const QuickJSPointerValue *>(getPointerValue(value));
    JSValue jsValue = qjsPointerValue->Get();

    assert(JS_IsBigInt(context_, jsValue));

    JSValue globalObj = JS_GetGlobalObject(context_);
    JSValue bigIntObj = JS_GetPropertyStr(context_, globalObj, "BigInt");
    JSValue bigIntProto = JS_GetPropertyStr(context_, bigIntObj, "prototype");
    JSValue toStringFunc = JS_GetPropertyStr(context_, bigIntProto, "toString");
    JSValue jsRadix = JS_NewInt32(context_, radix);
    JSValue jsResult = JS_Call(context_, toStringFunc, jsValue, 1, &jsRadix);

    JS_FreeValue(context_, jsRadix);
    JS_FreeValue(context_, toStringFunc);
    JS_FreeValue(context_, bigIntProto);
    JS_FreeValue(context_, bigIntObj);
    JS_FreeValue(context_, globalObj);

    assert(JS_IsString(jsResult));

    return make<facebook::jsi::String>(new QuickJSPointerValue(context_, jsResult));
}

facebook::jsi::String QuickJSRuntime::createStringFromAscii(const char *str, size_t length) {
    QuickJSPointerValue *value = QuickJSPointerValue::createFromOneByte(context_, str, length);
    if (!value) {
        throw facebook::jsi::JSError(*this, "createFromOneByte() - string creation failed.");
    }
    return make<facebook::jsi::String>(value);
}

facebook::jsi::String QuickJSRuntime::createStringFromUtf8(const uint8_t *str, size_t length) {
    QuickJSPointerValue *value = QuickJSPointerValue::createFromUtf8(context_, str, length);
    if (!value) {
        throw facebook::jsi::JSError(*this, "createFromOneByte() - string creation failed.");
    }
    return make<facebook::jsi::String>(value);
}

std::string QuickJSRuntime::utf8(const facebook::jsi::String &str) {
    const QuickJSPointerValue *quickPointerValue = static_cast<const QuickJSPointerValue *>(getPointerValue(str));
    JSValue value = quickPointerValue->Get();
    assert(JS_IsString(value));
    return QJSJSIValueConverter::ToSTLString(*this, value);
}

facebook::jsi::Object QuickJSRuntime::createObject() {
    JSValue object = JS_NewObject(context_);
    return make<facebook::jsi::Object>(new QuickJSPointerValue(context_, object));
}

facebook::jsi::Object QuickJSRuntime::createObject(std::shared_ptr<facebook::jsi::HostObject> hostObject) {
    HostObjectProxy *hostObjectProxy = new HostObjectProxy(*this, context_, hostObject);
    hostObjectProxy->BindFinalizer();

    return make<facebook::jsi::Object>(new QuickJSPointerValue(context_, hostObjectProxy->weakHandle_));
}

std::shared_ptr<facebook::jsi::HostObject> QuickJSRuntime::getHostObject(const facebook::jsi::Object &object) {
    assert(isHostObject(object));

    JSValue quickObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    HostObjectProxy *hostObjectProxy = static_cast<HostObjectProxy *>(JS_GetOpaque(quickObject, weakClassId_));

    assert(hostObjectProxy);
    return hostObjectProxy->GetHostObject();
}

facebook::jsi::HostFunctionType &QuickJSRuntime::getHostFunction(const facebook::jsi::Function &function) {
    assert(isHostFunction(function));

    const QuickJSPointerValue *quickJSPointerValue = static_cast<const QuickJSPointerValue *>(getPointerValue(function));

    assert(JS_IsFunction(context_, quickJSPointerValue->Get()));

    JSValue quickJSFunction = QJSJSIValueConverter::ToQuickJSFunction(*this, function);
    HostFunctionProxy *hostFunctionProxy = static_cast<HostFunctionProxy *>(JS_GetOpaque(quickJSFunction, weakFunctionId_));

    assert(hostFunctionProxy);
    return hostFunctionProxy->GetHostFunction();
}

bool QuickJSRuntime::hasNativeState(const facebook::jsi::Object &object) {
    JSValue quickJSObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);

    return JS_GetOpaque(quickJSObject, nativeStateClassId_) != NULL;
}

std::shared_ptr<facebook::jsi::NativeState> QuickJSRuntime::getNativeState(const facebook::jsi::Object &object) {
    if (isHostObject(object)) {
        throw facebook::jsi::JSINativeException("native state unsupported on HostObject");
    }
    assert(hasNativeState(object));

    JSValue quickJSObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    auto *nativeStatePtr = reinterpret_cast<std::shared_ptr<facebook::jsi::NativeState> *>(JS_GetOpaque(quickJSObject, nativeStateClassId_));
    return std::shared_ptr(*nativeStatePtr);
}

void QuickJSRuntime::setNativeState(const facebook::jsi::Object &object, std::shared_ptr<facebook::jsi::NativeState> state) {
    if (isHostObject(object)) {
        throw facebook::jsi::JSINativeException("native state unsupported on HostObject");
    }

    JSContext *ctx = context_;
    JSValue jsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    JSValue newObject = JS_NewObjectProtoClass(ctx, jsObject, nativeStateClassId_);
    if (JS_IsException(newObject)) {
        throw facebook::jsi::JSError(*this, "Unable to create new Object for setNativeState");
    }

    auto *nativeStatePtr = new std::shared_ptr<facebook::jsi::NativeState>(std::move(state));
    JS_SetOpaque(newObject, reinterpret_cast<void *>(nativeStatePtr));

    JSValue assignFunc = JS_GetPropertyStr(ctx, JS_GetGlobalObject(ctx), "Object.assign");
    if (JS_IsFunction(ctx, assignFunc)) {
        JSValue args[2] = {newObject, jsObject};
        JSValue result = JS_Call(ctx, assignFunc, JS_UNDEFINED, 2, args);
        JS_FreeValue(ctx, result);
    }
    JS_FreeValue(ctx, assignFunc);

    // 使用 newObject 替换原 object 在 jsi 中的指针值
    QuickJSPointerValue *quickJSPointerValue = static_cast<QuickJSPointerValue *>(const_cast<Runtime::PointerValue *>(getPointerValue(object)));
    quickJSPointerValue->Reset(context_, newObject);
}

facebook::jsi::Value QuickJSRuntime::getProperty(const facebook::jsi::Object &object, const facebook::jsi::PropNameID &name) {
    JSContext *ctx = context_;

    JSValue jsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    JSValue propName = QJSJSIValueConverter::ToQuickJSString(*this, name);
    JSAtom atom = JS_ValueToAtom(context_, propName);
    JSValue jsValue = JS_GetProperty(ctx, jsObject, atom);
    JS_FreeAtom(ctx, atom);

    if (JS_IsException(jsValue)) {
        JSValue exception = JS_GetException(ctx);
        ReportException(ctx, exception);
        JS_FreeValue(ctx, exception);
        JS_FreeValue(ctx, jsValue);
        return facebook::jsi::Value::undefined();
    }

    facebook::jsi::Value value = QJSJSIValueConverter::ToJSIValue(*this, jsValue);
    JS_FreeValue(ctx, jsValue);

    return value;
}

facebook::jsi::Value QuickJSRuntime::getProperty(const facebook::jsi::Object &object, const facebook::jsi::String &name) {
    JSContext *ctx = context_;

    JSValue jsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    JSValue propName = QJSJSIValueConverter::ToQuickJSString(*this, name);
    JSAtom atom = JS_ValueToAtom(context_, propName);
    JSValue jsValue = JS_GetProperty(ctx, jsObject, atom);
    JS_FreeAtom(ctx, atom);

    if (JS_IsException(jsValue)) {
        JSValue exception = JS_GetException(ctx);
        ReportException(ctx, exception);
        JS_FreeValue(ctx, exception);
        JS_FreeValue(ctx, jsValue);
        return facebook::jsi::Value::undefined();
    }

    facebook::jsi::Value value = QJSJSIValueConverter::ToJSIValue(*this, jsValue);
    JS_FreeValue(ctx, jsValue);

    return value;
}

bool QuickJSRuntime::hasProperty(const facebook::jsi::Object &object, const facebook::jsi::PropNameID &name) {
    JSContext *ctx = context_;

    JSValue jsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    JSValue propName = QJSJSIValueConverter::ToQuickJSString(*this, name);
    JSAtom atom = JS_ValueToAtom(context_, propName);

    int result = JS_HasProperty(ctx, jsObject, atom);
    JS_FreeAtom(ctx, atom);

    if (result < 0) {
        JSValue exception = JS_GetException(ctx);
        ReportException(ctx, exception);
        JS_FreeValue(ctx, exception);
        return false;
    }

    return result != 0;
}

bool QuickJSRuntime::hasProperty(const facebook::jsi::Object &object, const facebook::jsi::String &name) {
    JSContext *ctx = context_;

    JSValue jsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    JSValue propName = QJSJSIValueConverter::ToQuickJSString(*this, name);
    JSAtom atom = JS_ValueToAtom(context_, propName);

    int result = JS_HasProperty(ctx, jsObject, atom);
    JS_FreeAtom(ctx, atom);

    if (result < 0) {
        JSValue exception = JS_GetException(ctx);
        ReportException(ctx, exception);
        JS_FreeValue(ctx, exception);
        return false;
    }

    return result != 0;
}

void QuickJSRuntime::setPropertyValue(
    const facebook::jsi::Object &object,
    const facebook::jsi::PropNameID &name, const facebook::jsi::Value &value) {
    JSValue jsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    JSValue propName = QJSJSIValueConverter::ToQuickJSString(*this, name);
    JSValue jsValue = QJSJSIValueConverter::ToQJSValue(*this, value);
    JSAtom prop = JS_ValueToAtom(context_, propName);

    if (JS_SetProperty(context_, jsObject, prop, jsValue) < 0) {
        throw facebook::jsi::JSError(*this, "QuickJSRuntime::setPropertyValue failed.");
    }
}

void QuickJSRuntime::setPropertyValue(
    const facebook::jsi::Object &object,
    const facebook::jsi::String &name, const facebook::jsi::Value &value) {
    JSValue jsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    JSValue propName = QJSJSIValueConverter::ToQuickJSString(*this, name);
    JSValue jsValue = QJSJSIValueConverter::ToQJSValue(*this, value);
    JSAtom prop = JS_ValueToAtom(context_, propName);

    if (JS_SetProperty(context_, jsObject, prop, jsValue) < 0) {
        throw facebook::jsi::JSError(*this, "QuickJSRuntime::setPropertyValue failed.");
    }
}

bool QuickJSRuntime::isArray(const facebook::jsi::Object &object) const {
    JSValue qjsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    bool result = JS_IsArray(context_, qjsObject);
    JS_FreeValue(context_, qjsObject); // 确保释放 JSValue
    return result;
}

bool QuickJSRuntime::isArrayBuffer(const facebook::jsi::Object &object) const {
    JSValue qjsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);

    // QuickJS 并没有直接的 JS_IsArrayBuffer API，但是我们可以通过判断对象的类名是否为 "ArrayBuffer" 来实现
    if (JS_IsObject(qjsObject)) {
        JSValue global = JS_GetGlobalObject(context_);
        JSValue arrayBufferConstructor = JS_GetPropertyStr(context_, global, "ArrayBuffer");
        JSValue proto = JS_GetPropertyStr(context_, arrayBufferConstructor, "prototype");

        JSValue isArrayBuffer = JS_GetPropertyStr(context_, proto, "isPrototypeOf");
        JSValue result = JS_Call(context_, isArrayBuffer, proto, 1, &qjsObject);

        bool isArrayBufferBool = JS_ToBool(context_, result);

        JS_FreeValue(context_, global);
        JS_FreeValue(context_, arrayBufferConstructor);
        JS_FreeValue(context_, proto);
        JS_FreeValue(context_, isArrayBuffer);
        JS_FreeValue(context_, result);

        return isArrayBufferBool;
    }

    return false;
}

bool QuickJSRuntime::isFunction(const facebook::jsi::Object &object) const {
    JSValue qjsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);
    bool result = JS_IsFunction(context_, qjsObject);
    JS_FreeValue(context_, qjsObject); // 确保释放 JSValue
    return result;
}

bool QuickJSRuntime::isHostObject(const facebook::jsi::Object &object) const {
    JSValue qjsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);

    return JS_GetOpaque(qjsObject, weakClassId_) != NULL;
}

bool QuickJSRuntime::isHostFunction(const facebook::jsi::Function &function) const {
    JSValue qjsFunction = QJSJSIValueConverter::ToQuickJSFunction(*this, function);

    return JS_GetOpaque(qjsFunction, weakFunctionId_) != NULL;
}

facebook::jsi::Array QuickJSRuntime::getPropertyNames(const facebook::jsi::Object &object) {
    JSContext *ctx = context_;

    JSPropertyEnum *props;
    uint32_t prop_count;
    JSValue qjsObject = QJSJSIValueConverter::ToQuickJSObject(*this, object);

    if (JS_GetOwnPropertyNames(ctx, &props, &prop_count, qjsObject, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) < 0) {
        JSValue exception = JS_GetException(ctx);
        ReportException(ctx, exception);
        JS_FreeValue(ctx, exception);
        throw facebook::jsi::JSError(*this, "Failed to get property names");
    }
    // 创建一个数组来存储属性名
    JSValue result = JS_NewArray(ctx);

    // 遍历属性名
    for (uint32_t i = 0; i < prop_count; i++) {
        // 使用 JS_AtomToString 获取属性名的字符串表示
        JSValue prop_name = JS_AtomToString(ctx, props[i].atom);
        // 将属性名添加到结果数组中
        JS_SetPropertyUint32(ctx, result, i, prop_name);
        // 释放 atom
        JS_FreeAtom(ctx, props[i].atom);
    }

    js_free(ctx, props);

    return make<facebook::jsi::Object>(new QuickJSPointerValue(context_, result)).getArray(*this);
}

facebook::jsi::WeakObject QuickJSRuntime::createWeakObject(const facebook::jsi::Object &weakObject) {
    const QuickJSPointerValue *qjsPointerValue = static_cast<const QuickJSPointerValue *>(getPointerValue(weakObject));
    assert(JS_IsObject(qjsPointerValue->Get()));

    int currentWeakId = nextWeakId_;
    nextWeakId_++;
    std::string weakIdStr = "__weakObject" + std::to_string(currentWeakId);
    JSAtom atom = JS_NewAtom(context_, weakIdStr.c_str());
    JS_DefinePropertyValue(context_, JS_GetGlobalObject(context_), atom, qjsPointerValue->Get(), JS_PROP_CONFIGURABLE);
    JSValue weakRef = JS_AtomToValue(context_, atom);

    JS_FreeAtom(context_, atom);

    return make<facebook::jsi::WeakObject>(new QuickJSPointerValue(context_, std::move(weakRef)));
}

facebook::jsi::Value QuickJSRuntime::lockWeakObject(const facebook::jsi::WeakObject &weakObject) {
    const QuickJSPointerValue *qjsPointerValue = static_cast<const QuickJSPointerValue *>(getPointerValue(weakObject));
    assert(JS_IsObject(qjsPointerValue->Get()));

    return QJSJSIValueConverter::ToJSIValue(*this, qjsPointerValue->Get());
}

facebook::jsi::Array QuickJSRuntime::createArray(size_t length) {
    JSValue array = JS_NewArray(context_);
    JS_SetPropertyUint32(context_, array, 0, JS_NewUint32(context_, static_cast<uint32_t>(length)));
    return make<facebook::jsi::Object>(new QuickJSPointerValue(context_, array)).getArray(*this);
}

facebook::jsi::ArrayBuffer QuickJSRuntime::createArrayBuffer(std::shared_ptr<facebook::jsi::MutableBuffer> buffer) {
    JSValue arrayBuffer = JS_NewArrayBuffer(context_, buffer->data(), buffer->size(), nullptr, nullptr, false);
    if (JS_IsException(arrayBuffer)) {
        JSValue exception = JS_GetException(context_);
        ReportException(context_, exception);
        JS_FreeValue(context_, exception);
        throw facebook::jsi::JSError(*this, "Failed to create ArrayBuffer");
    }

    auto *mutableBuffer = new std::shared_ptr<facebook::jsi::MutableBuffer>(std::move(buffer));

    JS_SetOpaque(arrayBuffer, mutableBuffer);
    JS_SetClassProto(context_, arrayBufferClassId_, arrayBuffer);

    return make<facebook::jsi::Object>(new QuickJSPointerValue(context_, arrayBuffer)).getArrayBuffer(*this);
}

size_t QuickJSRuntime::size(const facebook::jsi::Array &array) {
    JSValue qjsArray = QJSJSIValueConverter::ToQuickJSArray(*this, array);
    assert(JS_IsArray(context_, qjsArray));
    JSValue length_val = JS_GetPropertyStr(context_, qjsArray, "length");
    int32_t length;

    if (JS_ToInt32(context_, &length, length_val) < 0) {
        JS_FreeValue(context_, length_val);
        return -1;
    }

    JS_FreeValue(context_, length_val);

    return length;
}

size_t QuickJSRuntime::size(const facebook::jsi::ArrayBuffer &arrayBuffer) {
    JSValue qjsObject = QJSJSIValueConverter::ToQuickJSObject(*this, arrayBuffer);
    // TODO: 不确定这里将 arrayBuffer 判断为 object 是否准确
    assert(JS_IsObject(qjsObject));

    size_t byteLength;
    JS_GetArrayBuffer(context_, &byteLength, qjsObject);

    return byteLength;
}

uint8_t *QuickJSRuntime::data(const facebook::jsi::ArrayBuffer &arrayBuffer) {
    JSValue qjsObject = QJSJSIValueConverter::ToQuickJSObject(*this, arrayBuffer);

    // TODO: 不确定这里将 arrayBuffer 判断为 object 是否准确
    assert(JS_IsObject(qjsObject));

    size_t byteLength;
    return JS_GetArrayBuffer(context_, &byteLength, qjsObject);
}

facebook::jsi::Value QuickJSRuntime::getValueAtIndex(const facebook::jsi::Array &array, size_t i) {
    JSValue jsArray = QJSJSIValueConverter::ToQuickJSArray(*this, array);
    assert(JS_IsArray(context_, jsArray));

    JSValue result = JS_GetPropertyUint32(context_, jsArray, static_cast<uint32_t>(i));
    if (JS_IsException(result)) {
        JSValue exception = JS_GetException(context_);
        ReportException(context_, exception);
        JS_FreeValue(context_, exception);
        throw facebook::jsi::JSError(*this, "QuickJSRuntime::getValueAtIndex failed.");
    }

    return QJSJSIValueConverter::ToJSIValue(*this, result);
}

void QuickJSRuntime::setValueAtIndexImpl(
    const facebook::jsi::Array &array,
    size_t i, const facebook::jsi::Value &value) {
    JSValue jsArray = QJSJSIValueConverter::ToQuickJSArray(*this, array);
    assert(JS_IsArray(context_, jsArray));

    JSValue jsValue = QJSJSIValueConverter::ToQJSValue(*this, value);
    if (JS_SetPropertyUint32(context_, jsArray, static_cast<uint32_t>(i), jsValue) < 0) {
        JSValue exception = JS_GetException(context_);
        ReportException(context_, exception);
        JS_FreeValue(context_, exception);
        throw facebook::jsi::JSError(*this, "QuickJSRuntime::setValueAtIndexImpl failed.");
    }
}

// TODO: 此方法不确定是否正确，需要补充测试用例验证
facebook::jsi::Function QuickJSRuntime::createFunctionFromHostFunction(const facebook::jsi::PropNameID &name, unsigned int paramCount,
                                                                       facebook::jsi::HostFunctionType func) {
    HostFunctionProxy *hostFunctionProxy = new HostFunctionProxy(*this, context_, std::move(func));
    hostFunctionProxy->BindFinalizer();
    JSValue jsFunction = hostFunctionProxy->weakHandle_;

    JS_DefinePropertyValueStr(context_, jsFunction, "name", QJSJSIValueConverter::ToQuickJSString(*this, name), JS_PROP_CONFIGURABLE);

    return make<facebook::jsi::Object>(new QuickJSPointerValue(context_, jsFunction)).getFunction(*this);
    ;
}

facebook::jsi::Value QuickJSRuntime::call(const facebook::jsi::Function &function, const facebook::jsi::Value &jsThis, const facebook::jsi::Value *args,
                                          size_t count) {
    JSValue qjsFunction = QJSJSIValueConverter::ToQuickJSFunction(*this, function);
    JSValue qjsReceiver;

    if (jsThis.isUndefined()) {
        qjsReceiver = JS_GetGlobalObject(context_);
    } else {
        qjsReceiver = QJSJSIValueConverter::ToQJSValue(*this, jsThis);
    }
    std::vector<JSValue> qjsArgs;
    for (size_t i = 0; i < count; ++i) {
        qjsArgs.push_back(QJSJSIValueConverter::ToQJSValue(*this, args[i]));
    }

    JSValue result = JS_Call(context_, qjsFunction, qjsReceiver, static_cast<int>(count), qjsArgs.data());

    JS_FreeValue(context_, qjsReceiver);
    for (size_t i = 0; i < count; ++i) {
        JS_FreeValue(context_, qjsArgs[i]);
    }

    if (JS_IsException(result)) {
        JSValue exception = JS_GetException(context_);
        ReportException(context_, exception);
        JS_FreeValue(context_, exception);
    }

    facebook::jsi::Value jsiResult = QJSJSIValueConverter::ToJSIValue(*this, result);

    JS_FreeValue(context_, result);

    return jsiResult;
}

facebook::jsi::Value QuickJSRuntime::callAsConstructor(const facebook::jsi::Function &function,
                                                       const facebook::jsi::Value *args, size_t count) {
    JSValue qjsFunction = QJSJSIValueConverter::ToQuickJSFunction(*this, function);

    std::vector<JSValue> qjsArgs;
    for (size_t i = 0; i < count; ++i) {
        qjsArgs.push_back(QJSJSIValueConverter::ToQJSValue(*this, args[i]));
    }

    JSValue result = JS_CallConstructor(context_, qjsFunction, static_cast<int>(count), qjsArgs.data());

    for (size_t i = 0; i < count; ++i) {
        JS_FreeValue(context_, qjsArgs[i]);
    }

    if (JS_IsException(result)) {
        JSValue exception = JS_GetException(context_);
        ReportException(context_, exception);
        JS_FreeValue(context_, exception);
    }

    facebook::jsi::Value jsiResult = QJSJSIValueConverter::ToJSIValue(*this, result);

    JS_FreeValue(context_, result);

    return jsiResult;
}

bool QuickJSRuntime::strictEquals(const facebook::jsi::Symbol &a, const facebook::jsi::Symbol &b) const {
    JSValue qjsSymbolA = QJSJSIValueConverter::ToQuickJSSymbol(*this, a);
    JSValue qjsSymbolB = QJSJSIValueConverter::ToQuickJSSymbol(*this, b);

    bool result = JS_StrictEq(context_, qjsSymbolA, qjsSymbolB);

    JS_FreeValue(context_, qjsSymbolA);
    JS_FreeValue(context_, qjsSymbolB);

    return result;
}

bool QuickJSRuntime::strictEquals(const facebook::jsi::BigInt &a, const facebook::jsi::BigInt &b) const {
    JSValue qjsBigIntA = (static_cast<const QuickJSPointerValue *>(getPointerValue(a))->Get());
    JSValue qjsBigIntB = (static_cast<const QuickJSPointerValue *>(getPointerValue(b))->Get());

    bool result = JS_StrictEq(context_, qjsBigIntA, qjsBigIntB);

    JS_FreeValue(context_, qjsBigIntA);
    JS_FreeValue(context_, qjsBigIntB);

    return result;
}

bool QuickJSRuntime::strictEquals(const facebook::jsi::String &a, const facebook::jsi::String &b) const {
    JSValue qjsStringA = QJSJSIValueConverter::ToQuickJSString(*this, a);
    JSValue qjsStringB = QJSJSIValueConverter::ToQuickJSString(*this, b);

    bool result = JS_StrictEq(context_, qjsStringA, qjsStringB);

    JS_FreeValue(context_, qjsStringA);
    JS_FreeValue(context_, qjsStringB);

    return result;
}

bool QuickJSRuntime::strictEquals(const facebook::jsi::Object &a, const facebook::jsi::Object &b) const {
    JSValue qjsObjectA = QJSJSIValueConverter::ToQuickJSObject(*this, a);
    JSValue qjsObjectB = QJSJSIValueConverter::ToQuickJSObject(*this, b);

    bool result = JS_StrictEq(context_, qjsObjectA, qjsObjectB);

    JS_FreeValue(context_, qjsObjectA);
    JS_FreeValue(context_, qjsObjectB);

    return result;
}

bool QuickJSRuntime::instanceOf(const facebook::jsi::Object &o, const facebook::jsi::Function &f) {
    JSValue qjsObject = QJSJSIValueConverter::ToQuickJSObject(*this, o);
    JSValue qjsFunction = QJSJSIValueConverter::ToQuickJSFunction(*this, f);

    int result = JS_IsInstanceOf(context_, qjsObject, qjsFunction);

    JS_FreeValue(context_, qjsObject);
    JS_FreeValue(context_, qjsFunction);

    if (result < 0) {
        JSValue exception = JS_GetException(context_);
        JS_FreeValue(context_, exception);
        return false;
    }

    return result > 0;
}
