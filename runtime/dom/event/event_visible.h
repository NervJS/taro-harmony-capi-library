//
// Created on 2024/6/12.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#pragma once

#include <cstdint>
#include <unordered_map>
#include <napi/native_api.h>
#include "visible_info.h"
#include "runtime/dom/element/element.h"

class VisibleEventListener {
    public:
    static VisibleEventListener* GetInstance() {
        static VisibleEventListener* visible_event_listener =
            new VisibleEventListener();
        return visible_event_listener;
    }

    static bool compareDecimalPlaces(double num1, double num2,
                                     std::function<bool(int, int)> compareFunc) {
        int intPart1 = static_cast<int>(num1 * 10);
        int intPart2 = static_cast<int>(num2 * 10);
        return compareFunc(intPart1, intPart2);
    }

    static void callVisibilityFunc(const CallbackInfo& callback_info, const VisibilityInfo& info) {
        if (!callback_info.registerd) return;
        // 触发回调
        napi_value callback;
        napi_get_reference_value(TaroRuntime::NativeNodeApi::env,
                                    callback_info.callback_ref, &callback);

        // 创建参数对象
        napi_value args[1];
        napi_create_object(TaroRuntime::NativeNodeApi::env, &args[0]);

        // 设置 intersectionRatio 属性
        SetNamedProperty(TaroRuntime::NativeNodeApi::env, args[0],
                            "intersectionRatio", info.intersectionRatio);

        // 创建 intersectionRect 对象
        napi_value intersection_rect;
        napi_create_object(TaroRuntime::NativeNodeApi::env,
                            &intersection_rect);
        SetRectProperties(TaroRuntime::NativeNodeApi::env, intersection_rect,
                            "", info.rect.intersectionRect);
        SetRectPropertiesWithWidthHeight(TaroRuntime::NativeNodeApi::env,
                                            intersection_rect, "",
                                            info.rect.intersectionRect);

        // 创建 boundingClientRect 对象
        napi_value bounding_client_rect;
        napi_create_object(TaroRuntime::NativeNodeApi::env,
                            &bounding_client_rect);
        SetRectProperties(TaroRuntime::NativeNodeApi::env,
                            bounding_client_rect, "",
                            info.rect.boundingClientRect);
        SetRectPropertiesWithWidthHeight(TaroRuntime::NativeNodeApi::env,
                                            bounding_client_rect, "",
                                            info.rect.boundingClientRect);

        // 创建 relativeRect 对象
        napi_value relative_rect;
        napi_create_object(TaroRuntime::NativeNodeApi::env, &relative_rect);
        SetRectProperties(TaroRuntime::NativeNodeApi::env, relative_rect, "",
                            info.rect.relativeRect);

        // 设置 intersectionRect、boundingClientRect 和 relativeRect 属性
        napi_set_named_property(TaroRuntime::NativeNodeApi::env, args[0],
                                "intersectionRect", intersection_rect);
        napi_set_named_property(TaroRuntime::NativeNodeApi::env, args[0],
                                "boundingClientRect", bounding_client_rect);
        napi_set_named_property(TaroRuntime::NativeNodeApi::env, args[0],
                                "relativeRect", relative_rect);

        napi_value result;
        napi_call_function(TaroRuntime::NativeNodeApi::env, nullptr, callback,
                            1, args, &result);
    }

    static bool checkThresholds(std::unordered_map<float, bool>& thresholds,
                                float intersectionRatio) {
        // 检查 map 是否为空
        if (thresholds.empty()) {
            return false;
        }

        bool result = false;
        // 检查 intersectionRatio 是否与 thresholds
        // 中的某个值小数点后三位相等，并且对应的值是否为 false
        for (auto& pair : thresholds) {
            float threshold = pair.first;
            if (compareDecimalPlaces(
                    threshold, intersectionRatio,
                    [](int num1, int num2) { return num1 < num2; }) &&
                !pair.second) {
                // 将值设为 true
                pair.second = true;
                result = true;
            } else if (compareDecimalPlaces(
                           threshold, intersectionRatio,
                           [](int num1, int num2) { return num1 >= num2; }) &&
                       pair.second) {
                pair.second = false;
                result = true;
            }
        }

        return result;
    }

    void Disconnect(std::shared_ptr<TaroRuntime::TaroDOM::TaroElement>& element);
    int32_t Register(std::shared_ptr<TaroRuntime::TaroDOM::TaroElement>& element,
                      ViewportMargin margin,
                      std::unordered_map<float, bool> thresholds,
                      float initialRatio, napi_ref callback_ref);
    VisibilityInfo CalculateNodeVisibility(std::shared_ptr<TaroRuntime::TaroDOM::TaroElement> element,
        const CallbackInfo& callback_info) const;

    VisibilityInfo CalculateNodeVisibility(const ArkUI_NodeComponentEvent* compEvent,
        const CallbackInfo& callback_info) const;

    private:
    void SetViewport(CallbackInfo& callback_info);
    void SetMargin(CallbackInfo& callback_info, ViewportMargin& margin);
    static void updateVisibilityInfo(const CallbackInfo& callback_info,
        float node_left, float node_top,
        float node_width, float node_height,
        VisibilityInfo& visibilityInfo);
};
