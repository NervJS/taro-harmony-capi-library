/*
 * Modify from https://github.com/facebook/react-native/
 * Apache-2.0 License https://github.com/facebook/react-native/blob/main/LICENSE
 */

#pragma once

#include <jsi/jsi.h>

#include "engine/react_common/message_queue_thread.h"
#include "jsi_qjs_value_converter.h"
#include "qjs_runtime_config.h"
#include "quickjs.h"

class QuickJSRuntime : public facebook::jsi::Runtime {
    private:
    int nextWeakId_;
    JSRuntime* runtime_;
    JSContext* context_;
    JSClassID weakClassId_;
    JSClassID weakFunctionId_;
    JSClassID arrayBufferClassId_;
    JSClassID nativeStateClassId_;

    facebook::jsi::Value ExecuteScript(const JSValue& script, const std::string& sourceURL);
    void ReportException(JSContext* ctx, JSValue exception) const;
    std::unique_ptr<QuickJSRuntimeConfig> config_;
    std::shared_ptr<MessageQueueThread> jsQueue_;
    bool isSharedRuntime_ = false;

    public:
    QuickJSRuntime(
        std::unique_ptr<QuickJSRuntimeConfig> config,
        std::shared_ptr<MessageQueueThread> jsQueue);
    QuickJSRuntime(
        const QuickJSRuntime* quickJSRuntime,
        std::unique_ptr<QuickJSRuntimeConfig> config);
    ~QuickJSRuntime();

    public:
    facebook::jsi::Value evaluateJavaScript(
        const std::shared_ptr<const facebook::jsi::Buffer>& buffer,
        const std::string& sourceURL) override;
    std::shared_ptr<const facebook::jsi::PreparedJavaScript> prepareJavaScript(
        const std::shared_ptr<const facebook::jsi::Buffer>& buffer,
        std::string sourceURL) override;
    facebook::jsi::Value evaluatePreparedJavaScript(
        const std::shared_ptr<const facebook::jsi::PreparedJavaScript>& js)
        override;

    bool drainMicrotasks(int maxMicrotasksHint = -1) override;
    facebook::jsi::Object global() override;
    std::string description() override;
    bool isInspectable() override;

    protected:
    PointerValue* cloneSymbol(const Runtime::PointerValue* pv) override;
    PointerValue* cloneBigInt(const Runtime::PointerValue* pv) override;
    PointerValue* cloneString(const Runtime::PointerValue* pv) override;
    PointerValue* cloneObject(const Runtime::PointerValue* pv) override;
    PointerValue* clonePropNameID(const Runtime::PointerValue* pv) override;

    facebook::jsi::PropNameID createPropNameIDFromAscii(
        const char* str,
        size_t length) override;
    facebook::jsi::PropNameID createPropNameIDFromUtf8(
        const uint8_t* utf8,
        size_t length) override;
    facebook::jsi::PropNameID createPropNameIDFromString(
        const facebook::jsi::String& str) override;
    facebook::jsi::PropNameID createPropNameIDFromSymbol(
        const facebook::jsi::Symbol& sym) override;
    std::string utf8(const facebook::jsi::PropNameID&) override;
    bool compare(
        const facebook::jsi::PropNameID&,
        const facebook::jsi::PropNameID&) override;

    std::string symbolToString(const facebook::jsi::Symbol&) override;

    facebook::jsi::BigInt createBigIntFromInt64(int64_t) override;
    facebook::jsi::BigInt createBigIntFromUint64(uint64_t) override;
    bool bigintIsInt64(const facebook::jsi::BigInt&) override;
    bool bigintIsUint64(const facebook::jsi::BigInt&) override;
    uint64_t truncate(const facebook::jsi::BigInt&) override;
    facebook::jsi::String bigintToString(const facebook::jsi::BigInt&, int)
        override;

    facebook::jsi::String createStringFromAscii(const char* str, size_t length)
        override;
    facebook::jsi::String createStringFromUtf8(const uint8_t* utf8, size_t length)
        override;
    std::string utf8(const facebook::jsi::String&) override;

    facebook::jsi::Object createObject() override;
    facebook::jsi::Object createObject(
        std::shared_ptr<facebook::jsi::HostObject> hostObject) override;
    std::shared_ptr<facebook::jsi::HostObject> getHostObject(
        const facebook::jsi::Object&) override;
    facebook::jsi::HostFunctionType& getHostFunction(
        const facebook::jsi::Function&) override;

    bool hasNativeState(const facebook::jsi::Object&) override;
    std::shared_ptr<facebook::jsi::NativeState> getNativeState(
        const facebook::jsi::Object&) override;
    void setNativeState(
        const facebook::jsi::Object&,
        std::shared_ptr<facebook::jsi::NativeState> state) override;

    facebook::jsi::Value getProperty(
        const facebook::jsi::Object&,
        const facebook::jsi::PropNameID& name) override;
    facebook::jsi::Value getProperty(
        const facebook::jsi::Object&,
        const facebook::jsi::String& name) override;
    bool hasProperty(
        const facebook::jsi::Object&,
        const facebook::jsi::PropNameID& name) override;
    bool hasProperty(
        const facebook::jsi::Object&,
        const facebook::jsi::String& name) override;
    void setPropertyValue(
        const facebook::jsi::Object&,
        const facebook::jsi::PropNameID& name,
        const facebook::jsi::Value& value) override;
    void setPropertyValue(
        const facebook::jsi::Object&,
        const facebook::jsi::String& name,
        const facebook::jsi::Value& value) override;

    bool isArray(const facebook::jsi::Object&) const override;
    bool isArrayBuffer(const facebook::jsi::Object&) const override;
    bool isFunction(const facebook::jsi::Object&) const override;
    bool isHostObject(const facebook::jsi::Object&) const override;
    bool isHostFunction(const facebook::jsi::Function&) const override;
    facebook::jsi::Array getPropertyNames(const facebook::jsi::Object&) override;

    facebook::jsi::WeakObject createWeakObject(
        const facebook::jsi::Object&) override;
    facebook::jsi::Value lockWeakObject(
        const facebook::jsi::WeakObject&) override;

    facebook::jsi::Array createArray(size_t length) override;
    facebook::jsi::ArrayBuffer createArrayBuffer(
        std::shared_ptr<facebook::jsi::MutableBuffer> buffer) override;
    size_t size(const facebook::jsi::Array&) override;
    size_t size(const facebook::jsi::ArrayBuffer&) override;
    uint8_t* data(const facebook::jsi::ArrayBuffer&) override;
    facebook::jsi::Value getValueAtIndex(const facebook::jsi::Array&, size_t i)
        override;
    void setValueAtIndexImpl(
        const facebook::jsi::Array&,
        size_t i,
        const facebook::jsi::Value& value) override;
    facebook::jsi::Function createFunctionFromHostFunction(
        const facebook::jsi::PropNameID& name,
        unsigned int paramCount,
        facebook::jsi::HostFunctionType func) override;
    facebook::jsi::Value call(
        const facebook::jsi::Function&,
        const facebook::jsi::Value& jsThis,
        const facebook::jsi::Value* args,
        size_t count) override;
    facebook::jsi::Value callAsConstructor(
        const facebook::jsi::Function&,
        const facebook::jsi::Value* args,
        size_t count) override;

    bool strictEquals(
        const facebook::jsi::Symbol& a,
        const facebook::jsi::Symbol& b) const override;
    bool strictEquals(
        const facebook::jsi::BigInt& a,
        const facebook::jsi::BigInt& b) const override;
    bool strictEquals(
        const facebook::jsi::String& a,
        const facebook::jsi::String& b) const override;
    bool strictEquals(
        const facebook::jsi::Object& a,
        const facebook::jsi::Object& b) const override;

    bool instanceOf(
        const facebook::jsi::Object& o,
        const facebook::jsi::Function& f) override;

    private:
    friend class QJSJSIValueConverter;
    friend class QuickJSPointerValue;
    friend class HostObjectProxy;
    friend class HostFunctionProxy;
};
