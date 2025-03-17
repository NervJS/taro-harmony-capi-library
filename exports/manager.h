/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef NATIVE_XCOMPONENT_PLUGIN_MANAGER_H
#define NATIVE_XCOMPONENT_PLUGIN_MANAGER_H

#include <string>
#include <unordered_map>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/drawable_descriptor.h>
#include <arkui/native_node.h>
#include <arkui/native_node_napi.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>
#include <node_api.h>

namespace TaroHarmonyLibrary {
class Manager {
    public:
    ~Manager();

    static const std::vector<napi_property_descriptor> desc;

    static napi_value InitJSCAPIBridge(napi_env env, napi_callback_info info);

    static napi_value OnEntryAbilityEvent(napi_env env, napi_callback_info info);
    static napi_value OnXComponentStateChange(napi_env env, napi_callback_info info);
    static napi_value GetComputedStyle(napi_env env, napi_callback_info info);

    static napi_value RegisterEtsBuilder(napi_env env, napi_callback_info info);
    static napi_value InitStylesheet(napi_env env, napi_callback_info info);
    static napi_value UpdateDimensionContext(napi_env env, napi_callback_info info);
    static napi_value UpdateStylesheet(napi_env env, napi_callback_info info);
    static napi_value UpdateEnvRule(napi_env env, napi_callback_info info);
    static napi_value CreateObserve(napi_env env, napi_callback_info info);
    static napi_value DisconnectObserve(napi_env env, napi_callback_info info);
    static napi_value ScrollTo(napi_env env, napi_callback_info info);
    static napi_value GetCurrentOffset(napi_env env, napi_callback_info info);
    static napi_value UpdatePageSync(napi_env env, napi_callback_info info);
    static napi_value NextTick(napi_env env, napi_callback_info info);
    static napi_value LoadImage(napi_env env, napi_callback_info info);
    
    static napi_value Unstable_unsafe_SetSwiperOverflow(napi_env env, napi_callback_info info);

    static napi_value RegisterResource(napi_env env, napi_callback_info info);
    static ArkUI_DrawableDescriptor *GetDrawableDescriptor(std::string &key);
    static napi_value RegisterFontFamily(napi_env env, napi_callback_info info);
    static napi_value InitFileManger(napi_env env, napi_callback_info info);

    static napi_value ExecuteNodeFunc(napi_env env, napi_callback_info info);
    
    static napi_value Unstable_SetPageIsTextNeedLayout(napi_env env, napi_callback_info info);

    private:
    static std::unordered_map<std::string, ArkUI_DrawableDescriptor *> resourceMap_;
};
} // namespace TaroHarmonyLibrary
#endif // NATIVE_XCOMPONENT_PLUGIN_MANAGER_H
