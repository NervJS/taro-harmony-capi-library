/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "ArkJS.h"

#include <stdexcept>
#include <string>

#include "napi/native_api.h"
#include "runtime/render.h"

static void
maybeThrowFromStatus(napi_env env, napi_status status, const char* message) {
    if (status != napi_ok) {
        napi_extended_error_info const* error_info;
        napi_get_last_error_info(env, &error_info);
        std::string msg_str = message;
        std::string error_code_msg_str = ". Error code: ";
        std::string status_str = error_info->error_message;
        std::string full_msg = msg_str + error_code_msg_str + status_str;
        auto c_str = full_msg.c_str();
        // stops a code execution after throwing napi_error
        folly::throw_exception(std::runtime_error(c_str));
    }
}

void ArkJS::maybeRethrowAsCpp(napi_status status) {
    if (status == napi_ok) {
        return;
    }
    bool hasThrown;
    napi_is_exception_pending(m_env, &hasThrown);
    if (status != napi_pending_exception && !hasThrown) {
        return;
    }
    napi_value nError;
    napi_get_and_clear_last_exception(m_env, &nError);

    auto message = getObjectProperty(nError, "message");
    auto messageStr = getString(message);
    auto stack = getObjectProperty(nError, "stack");
    auto stackStr = getString(stack);
    auto err_msg = messageStr + "\n" + stackStr;
    // 尝试发现异常超过1024将不会被打印
    folly::throw_exception(std::runtime_error(err_msg.substr(0, 768)));
}

ArkJS::ArkJS(napi_env env) {
    m_env = env;
}

napi_env ArkJS::getEnv() {
    return m_env;
}

napi_value ArkJS::call(
    napi_value callback,
    std::vector<napi_value> args,
    napi_value thisObject) {
    return call(callback, args.data(), args.size(), thisObject);
}

napi_value ArkJS::call(
    napi_value callback,
    const napi_value* args,
    int argsCount,
    napi_value thisObject) {
    napi_value result;
    auto status =
        napi_call_function(m_env, thisObject, callback, argsCount, args, &result);
    this->maybeRethrowAsCpp(status);
    return result;
}

napi_value ArkJS::createBoolean(bool value) {
    napi_value result;
    napi_get_boolean(m_env, value, &result);
    return result;
}

napi_value ArkJS::createInt(int value) {
    napi_value result;
    napi_create_int32(m_env, static_cast<int32_t>(value), &result);
    return result;
}

napi_value ArkJS::createDouble(double value) {
    napi_value result;
    napi_create_double(m_env, value, &result);
    return result;
}

napi_value ArkJS::createString(std::string const& str) {
    napi_value result;
    auto status =
        napi_create_string_utf8(m_env, str.c_str(), str.length(), &result);
    this->maybeThrowFromStatus(status, "Failed to create string");
    return result;
}

napi_value ArkJS::createObject() {
    napi_value result;
    auto status = napi_create_object(m_env, &result);
    this->maybeThrowFromStatus(status, "Failed to create an object");
    return result;
}

napi_value ArkJS::getUndefined() {
    napi_value result;
    napi_get_undefined(m_env, &result);
    return result;
}

napi_value ArkJS::getNull() {
    napi_value result;
    napi_get_null(m_env, &result);
    return result;
}

TaroNapiObjectBuilder ArkJS::createObjectBuilder() {
    return TaroNapiObjectBuilder(m_env, *this);
}

std::vector<napi_value> ArkJS::createFromDynamics(
    std::vector<folly::dynamic> dynamics) {
    std::vector<napi_value> results(dynamics.size());
    for (size_t i = 0; i < dynamics.size(); ++i) {
        results[i] = this->createFromDynamic(dynamics[i]);
    }
    return results;
}

napi_value ArkJS::createFromDynamic(folly::dynamic dyn) {
    if (dyn.isBool()) {
        return this->createBoolean(dyn.asBool());
    } else if (dyn.isInt()) {
        return this->createDouble(dyn.asInt());
    } else if (dyn.isDouble()) {
        return this->createDouble(dyn.asDouble());
    } else if (dyn.isString()) {
        return this->createString(dyn.asString());
    } else if (dyn.isArray()) {
        std::vector<napi_value> n_values(dyn.size());
        for (size_t i = 0; i < dyn.size(); ++i) {
            n_values[i] = this->createFromDynamic(dyn[i]);
        }
        return this->createArray(n_values);
    } else if (dyn.isObject()) {
        auto objectBuilder = this->createObjectBuilder();
        for (const auto& pair : dyn.items()) {
            objectBuilder.addProperty(
                pair.first.asString().c_str(), this->createFromDynamic(pair.second));
        }
        return objectBuilder.build();
    } else {
        return this->getUndefined();
    }
}

napi_value ArkJS::createFromException(std::exception const& e) {
    folly::dynamic errData = folly::dynamic::object;
    errData["message"] = e.what();
    return this->createFromDynamic(errData);
}

napi_value ArkJS::getReferenceValue(napi_ref ref) {
    if (ref == nullptr) {
        return nullptr;
    }
    napi_value result;
    auto status = napi_get_reference_value(m_env, ref, &result);
    this->maybeThrowFromStatus(status, "Couldn't get a reference value");
    return result;
}

napi_ref ArkJS::createReference(napi_value value) {
    if (value == nullptr) {
        return nullptr;
    }
    napi_ref result;
    auto status = napi_create_reference(m_env, value, 1, &result);
    this->maybeThrowFromStatus(status, "Couldn't create a reference");
    return result;
}

void ArkJS::deleteReference(napi_ref reference) {
    if (reference == nullptr) {
        return;
    }
    auto runner = TaroRuntime::Render::GetInstance()->GetTaskRunner();
    runner->ensureRunOnMainTask([this, reference] {
        // 减少传入引用的引用计数并返回生成的引用计数
        uint32_t result = 0;
        napi_status status;
        status = napi_reference_unref(m_env, reference, &result);
        this->maybeThrowFromStatus(status, "Couldn't unref a reference");
        if (result == 0) {
            status = napi_delete_reference(m_env, reference);
            this->maybeThrowFromStatus(status, "Couldn't delete a reference");
        }
    });
}

std::function<napi_value(napi_env, std::vector<napi_value>)>*
createNapiCallback(
    std::function<void(std::vector<folly::dynamic>)>&& callback) {
    return new std::function(
        [callback = std::move(callback)](
            napi_env env,
            std::vector<napi_value> callbackNapiArgs) -> napi_value {
            ArkJS arkJs(env);
            callback(arkJs.getDynamics(callbackNapiArgs));
            return arkJs.getUndefined();
        });
}

napi_value singleUseCallback(napi_env env, napi_callback_info info) {
    void* data;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, &data);
    auto callback = static_cast<
        std::function<napi_value(napi_env, std::vector<napi_value>)>*>(data);
    ArkJS arkJs(env);
    (*callback)(env, arkJs.getCallbackArgs(info));
    delete callback;
    return arkJs.getUndefined();
}

/*
 * The callback will be deallocated after is called. It cannot be called more
 * than once. Creates memory leaks if the callback is not called. Consider
 * changing this implementation when adding napi finalizers is supported. .
 */
napi_value ArkJS::createSingleUseCallback(
    std::function<void(std::vector<folly::dynamic>)>&& callback) {
    return createFunction(
        "callback", singleUseCallback, createNapiCallback(std::move(callback)));
}

napi_value ArkJS::createFunction(
    std::string const& name,
    napi_callback callback,
    void* data) {
    napi_value result;
    auto status = napi_create_function(
        m_env, name.c_str(), name.length(), callback, data, &result);
    this->maybeThrowFromStatus(status, "Couldn't create a function");
    return result;
}

napi_value ArkJS::createArray() {
    napi_value result;
    auto status = napi_create_array(m_env, &result);
    this->maybeThrowFromStatus(status, "Failed to create an array");
    return result;
}

napi_value ArkJS::createArray(std::vector<napi_value> values) {
    napi_value result = createArray();
    for (size_t i = 0; i < values.size(); i++) {
        setArrayElement(result, i, values[i]);
    }
    return result;
}

std::vector<napi_value> ArkJS::getCallbackArgs(napi_callback_info info) {
    size_t argc;
    napi_get_cb_info(m_env, info, &argc, nullptr, nullptr, nullptr);
    return getCallbackArgs(info, argc);
}

std::vector<napi_value> ArkJS::getCallbackArgs(napi_callback_info info, size_t args_count) {
    size_t argc = args_count;
    std::vector<napi_value> args(args_count, nullptr);
    napi_status status = napi_get_cb_info(m_env, info, &argc, args.data(), nullptr, nullptr);
    if (status != napi_ok) {
        TARO_LOG_ERROR("ArkJS", "napi_get_cb_info failed, ret=%{public}d, argCnt=%{public}d", status, args_count);
    }
    return args;
}

TaroNapiObject ArkJS::getObject(napi_value object) {
    return TaroNapiObject(*this, object);
}

TaroNapiObject ArkJS::getObject(napi_ref objectRef) {
    return TaroNapiObject(*this, this->getReferenceValue(objectRef));
}

napi_value ArkJS::getObjectProperty(napi_value object, std::string const& key) {
    return getObjectProperty(object, this->createString(key));
}

napi_value ArkJS::getObjectProperty(napi_value object, napi_value key) {
    napi_value result;
    auto status = napi_get_property(m_env, object, key, &result);
    this->maybeThrowFromStatus(status, "Failed to retrieve property from object");
    return result;
}

int ArkJS::setObjectProperty(napi_value object, std::string const& key, napi_value value) {
    return setObjectProperty(object, this->createString(key), value);
}

int ArkJS::setObjectProperty(napi_value object, napi_value key, napi_value value) {
    auto status = napi_set_property(m_env, object, key, value);
    this->maybeThrowFromStatus(status, "Failed to set property on object");
    return status;
}

bool ArkJS::getBoolean(napi_value value) {
    bool result;
    auto status = napi_get_value_bool(m_env, value, &result);
    this->maybeThrowFromStatus(status, "Failed to retrieve boolean value");
    return result;
}

int ArkJS::getInteger(napi_value value) {
    int result;
    auto status = napi_get_value_int32(m_env, value, &result);
    this->maybeThrowFromStatus(status, "Failed to retrieve integer value");
    return result;
}

double ArkJS::getDouble(napi_value value) {
    double result;
    auto status = napi_get_value_double(m_env, value, &result);
    this->maybeThrowFromStatus(status, "Failed to retrieve double value");
    return result;
}

napi_value ArkJS::getArrayElement(napi_value array, uint32_t index) {
    napi_value result;
    auto status = napi_get_element(m_env, array, index, &result);
    this->maybeThrowFromStatus(status, "Failed to retrieve value at index");
    return result;
}

int ArkJS::setArrayElement(napi_value array, uint32_t index, napi_value value) {
    auto status = napi_set_element(m_env, array, index, value);
    this->maybeThrowFromStatus(status, "Failed to set value at index");
    return status;
}

uint32_t ArkJS::getArrayLength(napi_value array) {
    uint32_t length;
    auto status = napi_get_array_length(m_env, array, &length);
    this->maybeThrowFromStatus(status, "Failed to read array length");
    return length;
}

std::vector<uint8_t> ArkJS::getArrayBuffer(napi_value array) {
    void* data;
    size_t length;
    auto status = napi_get_arraybuffer_info(m_env, array, &data, &length);
    this->maybeThrowFromStatus(status, "Failed to read array buffer");
    return std::vector<uint8_t>(
        static_cast<uint8_t*>(data), static_cast<uint8_t*>(data) + length);
}

std::vector<std::pair<napi_value, napi_value>> ArkJS::getObjectProperties(
    napi_value object) {
    napi_value propertyNames;
    auto status = napi_get_property_names(m_env, object, &propertyNames);
    this->maybeThrowFromStatus(status, "Failed to retrieve property names");
    uint32_t length = this->getArrayLength(propertyNames);
    std::vector<std::pair<napi_value, napi_value>> result;
    for (uint32_t i = 0; i < length; i++) {
        napi_value propertyName = this->getArrayElement(propertyNames, i);
        napi_value propertyValue = this->getObjectProperty(object, propertyName);
        result.emplace_back(propertyName, propertyValue);
    }
    return result;
}

std::string ArkJS::getString(napi_value value) {
    if (value == nullptr) {
        return "";
    }

    size_t length;
    napi_status status;
    status = napi_get_value_string_utf8(m_env, value, nullptr, 0, &length);
    this->maybeThrowFromStatus(status, "Failed to get the length of the string");
    std::string buffer(length, '\0');
    status = napi_get_value_string_utf8(
        m_env, value, buffer.data(), length + 1, &length);
    this->maybeThrowFromStatus(status, "Failed to get the string data");
    return buffer;
}

void ArkJS::maybeThrowFromStatus(napi_status status, const char* message) {
    ::maybeThrowFromStatus(m_env, status, message);
}

napi_valuetype ArkJS::getType(napi_value value) {
    napi_valuetype result;
    auto status = napi_typeof(m_env, value, &result);
    this->maybeThrowFromStatus(status, "Failed to get value type");
    return result;
}

folly::dynamic ArkJS::getDynamic(napi_value value) {
    switch (this->getType(value)) {
        case napi_undefined:
            return folly::dynamic(nullptr);
        case napi_null:
            return folly::dynamic(nullptr);
        case napi_boolean:
            return folly::dynamic(this->getBoolean(value));
        case napi_number:
            return folly::dynamic(this->getDouble(value));
        case napi_string:
            return folly::dynamic(this->getString(value));
        case napi_object: {
            bool isArray;
            auto status = napi_is_array(m_env, value, &isArray);
            assert(status == napi_ok);
            if (isArray) {
                auto arrayLength = this->getArrayLength(value);
                folly::dynamic result = folly::dynamic::array();
                for (uint32_t i = 0; i < arrayLength; ++i) {
                    result.push_back(this->getDynamic(this->getArrayElement(value, i)));
                }
                return result;
            } else {
                folly::dynamic result = folly::dynamic::object();
                for (auto [key, val] : this->getObject(value).getKeyValuePairs()) {
                    result[this->getString(key)] = this->getDynamic(val);
                }
                return result;
            }
        }
        default:
            return folly::dynamic(nullptr);
    }
}

std::vector<folly::dynamic> ArkJS::getDynamics(std::vector<napi_value> values) {
    std::vector<folly::dynamic> dynamics;
    for (auto value : values) {
        dynamics.push_back(this->getDynamic(value));
    }
    return dynamics;
}

std::vector<napi_value> ArkJS::convertIntermediaryValuesToNapiValues(
    std::vector<IntermediaryArg> args) {
    std::vector<napi_value> napiArgs;
    for (auto arg : args) {
        napiArgs.push_back(convertIntermediaryValueToNapiValue(arg));
    }
    return napiArgs;
}

napi_value ArkJS::convertIntermediaryValueToNapiValue(IntermediaryArg arg) {
    try {
        return this->createFromDynamic(std::get<folly::dynamic>(arg));
    } catch (const std::bad_variant_access& e) {
    }
    try {
        return this->createSingleUseCallback(
            std::move(std::get<IntermediaryCallback>(arg)));
    } catch (const std::bad_variant_access& e) {
        return this->getUndefined();
    }
}

bool ArkJS::isEqual(napi_value a, napi_value b) {
    bool result;
    auto status = napi_strict_equals(m_env, a, b, &result);
    this->maybeThrowFromStatus(status, "Failed to compare values");
    return result;
}

TaroNapiObjectBuilder::TaroNapiObjectBuilder(napi_env env, ArkJS arkJs)
    : m_env(env),
      m_arkJs(arkJs) {
    napi_value obj;
    auto status = napi_create_object(env, &obj);
    maybeThrowFromStatus(env, status, "Failed to create an object");
    m_object = obj;
}

TaroNapiObjectBuilder::TaroNapiObjectBuilder(
    napi_env env,
    ArkJS arkJs,
    napi_value object)
    : m_env(env),
      m_arkJs(arkJs),
      m_object(object) {}

TaroNapiObjectBuilder& TaroNapiObjectBuilder::addProperty(
    const char* name,
    napi_value value) {
    m_properties.emplace_back(name, value);
    return *this;
}

TaroNapiObjectBuilder& TaroNapiObjectBuilder::addProperty(
    const char* name,
    bool value) {
    addProperty(name, m_arkJs.createBoolean(value));
    return *this;
}

TaroNapiObjectBuilder& TaroNapiObjectBuilder::addProperty(
    const char* name,
    int value) {
    auto napi_value = m_arkJs.createInt(value);
    addProperty(name, napi_value);
    return *this;
}

TaroNapiObjectBuilder& TaroNapiObjectBuilder::addProperty(
    const char* name,
    char const* value) {
    addProperty(name, m_arkJs.createString(value));
    return *this;
}

TaroNapiObjectBuilder& TaroNapiObjectBuilder::addProperty(
    const char* name,
    std::string value) {
    addProperty(name, m_arkJs.createString(value));
    return *this;
}

TaroNapiObjectBuilder& TaroNapiObjectBuilder::addProperty(
    const char* name,
    folly::dynamic value) {
    addProperty(name, m_arkJs.createFromDynamic(value));
    return *this;
}

napi_value TaroNapiObjectBuilder::build() {
    if (!m_properties.empty()) {
        std::vector<napi_property_descriptor> properties;

        for (auto const& [key, value] : m_properties) {
            properties.push_back(napi_property_descriptor{
                key.c_str(), // UTF-8 encoded property name
                nullptr,     // name string as napi_value

                nullptr, // method implementation
                nullptr, // getter
                nullptr, // setter
                value,   // property value as napi_value

                napi_default_jsproperty, // attributes
                nullptr                  // data
            });
        }
        auto status = napi_define_properties(
            m_env, m_object, properties.size(), properties.data());
        maybeThrowFromStatus(m_env, status, "Failed to create an object");
    }
    return m_object;
}

TaroNapiObject::TaroNapiObject(ArkJS arkJs, napi_value object)
    : m_arkJs(arkJs),
      m_object(object) {}

napi_value TaroNapiObject::getProperty(std::string const& key) {
    return m_arkJs.getObjectProperty(m_object, key);
}

napi_value TaroNapiObject::getProperty(napi_value key) {
    return m_arkJs.getObjectProperty(m_object, key);
}

std::vector<std::pair<napi_value, napi_value>>
TaroNapiObject::getKeyValuePairs() {
    return m_arkJs.getObjectProperties(m_object);
}

bool ArkJS::isPromise(napi_value value) {
    bool result;
    napi_is_promise(m_env, value, &result);
    return result;
}

TaroNapiObjectBuilder ArkJS::getObjectBuilder(napi_value object) {
    return TaroNapiObjectBuilder(m_env, *this, object);
}

Promise::Promise(napi_env env, napi_value value)
    : m_arkJs(ArkJS(env)),
      m_value(value) {}

Promise& Promise::then(
    std::function<void(std::vector<folly::dynamic>)>&& callback) {
    auto obj = m_arkJs.getObject(m_value);
    obj.call("then", {m_arkJs.createSingleUseCallback(std::move(callback))});
    return *this;
}

Promise& Promise::catch_(
    std::function<void(std::vector<folly::dynamic>)>&& callback) {
    auto obj = m_arkJs.getObject(m_value);
    obj.call("catch", {m_arkJs.createSingleUseCallback(std::move(callback))});
    return *this;
}
