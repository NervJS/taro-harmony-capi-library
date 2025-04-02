/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <js_native_api.h>
#include "folly/lang/Exception.h"

class NapiHandleScopeWrapper {
    public:
    NapiHandleScopeWrapper(napi_env env)
        : env(env) {
        if (napi_open_handle_scope(env, &handleScope) != napi_ok) {
#if IS_DEBUG
            folly::throw_exception(std::runtime_error("Failed to open handle scope"));
#endif
        }
    }

    ~NapiHandleScopeWrapper() {
        if (napi_close_handle_scope(env, handleScope) != napi_ok) {
#if IS_DEBUG
            folly::throw_exception(std::runtime_error("Failed to close handle scope"));
#endif
        }

    }

    NapiHandleScopeWrapper(const NapiHandleScopeWrapper&) = delete;
    NapiHandleScopeWrapper& operator=(const NapiHandleScopeWrapper&) = delete;
    NapiHandleScopeWrapper(NapiHandleScopeWrapper&&) = delete;
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;

    protected:
    napi_env env;
    napi_handle_scope handleScope;
};

class NapiEscapableScopeWrapper {
    public:
    NapiEscapableScopeWrapper(napi_env env)
        : env(env) {
        napi_open_escapable_handle_scope(env, &scope);
    }

    ~NapiEscapableScopeWrapper() {
        napi_close_escapable_handle_scope(env, scope);
    }

    napi_value Escape(napi_value value) {
        napi_value escapee = nullptr;
        napi_escape_handle(env, scope, value, &escapee);
        return escapee;
    }

    NapiEscapableScopeWrapper(const NapiEscapableScopeWrapper&) = delete;
    NapiEscapableScopeWrapper& operator=(const NapiEscapableScopeWrapper&) = delete;
    NapiEscapableScopeWrapper(NapiEscapableScopeWrapper&&) = delete;
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    napi_escapable_handle_scope scope;

    protected:
    napi_env env;
};
