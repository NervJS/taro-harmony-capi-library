//
// Created by zhutianjian on 24-8-19.
//

#pragma once

#include <vector>
#include <arkui/native_node_napi.h>
#include <napi/native_api.h>
#include <node_api.h>

namespace TaroHarmonyLibrary {
class Document {
    public:
    static napi_value BuildTaroNode(napi_env env, napi_callback_info info);
    static napi_value CreateTaroNode(napi_env env, napi_callback_info info);
    static napi_value AppendTaroNode(napi_env env, napi_callback_info info);
    static napi_value RemoveTaroNode(napi_env env, napi_callback_info info);
    static napi_value ReplaceTaroNode(napi_env env, napi_callback_info info);
    static napi_value InsertBeforeTaroNode(napi_env env, napi_callback_info info);

    static napi_value SetTaroNodeAttribute(napi_env env, napi_callback_info info);
    static napi_value RemoveTaroNodeAttribute(napi_env env, napi_callback_info info);
    static napi_value GetTaroNodeAttribute(napi_env env, napi_callback_info info);
    static napi_value HasTaroNodeAttribute(napi_env env, napi_callback_info info);
    static napi_value HasTaroNodeAttributes(napi_env env, napi_callback_info info);

    static napi_value GetTaroNodeById(napi_env env, napi_callback_info info);
    static napi_value GetTaroNodesByTagName(napi_env env, napi_callback_info info);
    static napi_value GetTaroNodesByClassName(napi_env env, napi_callback_info info);
    static napi_value QuerySelectDOM(napi_env env, napi_callback_info info);
    static napi_value QuerySelectDOMSync(napi_env env, napi_callback_info info);

    static napi_value GetTaroNodeProperty(napi_env env, napi_callback_info info);
    static napi_value GetTaroNodeChildAt(napi_env env, napi_callback_info info);

    static const std::vector<napi_property_descriptor> desc;
};
} // namespace TaroHarmonyLibrary
