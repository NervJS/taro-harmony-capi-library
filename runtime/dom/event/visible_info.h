//
// Created on 2024/8/1.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_HARMONY_CPP_VISIBLE_INFO_H
#define TARO_HARMONY_CPP_VISIBLE_INFO_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <napi/native_api.h>

struct Viewport {
    float x;
    float y;
};

struct ViewportMargin {
    float left;
    float top;
    float right;
    float bottom;
};

struct VisibilityInfo {
    float intersectionRatio = -1.0f;

    struct Rect {
        struct IntersectionRect {
            float left = 0.0f;
            float right = 0.0f;
            float top = 0.0f;
            float bottom = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
        } intersectionRect;

        struct BoundingClientRect {
            float left = 0.0f;
            float right = 0.0f;
            float top = 0.0f;
            float bottom = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
        } boundingClientRect;

        struct RelativeRect {
            float left = 0.0f;
            float right = 0.0f;
            float top = 0.0f;
            float bottom = 0.0f;
        } relativeRect;
    } rect;
};

struct CallbackInfo {
    Viewport viewport;
    napi_ref callback_ref;
    ViewportMargin margin;
    float initialRatio = 0;
    bool registerd = false;
    bool registerdToArk = false;
    std::unordered_map<float, bool> triggeredThresholds_;
};

template <typename T>
void SetNamedProperty(napi_env env, napi_value object, const char* name,
                      T value) {
    napi_value property;
    napi_create_double(env, static_cast<double>(value), &property);
    napi_set_named_property(env, object, name, property);
}

template <typename T>
void SetRectProperties(napi_env env, napi_value object, const char* prefix,
                       const T& rect) {
    SetNamedProperty(env, object, (std::string(prefix) + "left").c_str(),
                     rect.left);
    SetNamedProperty(env, object, (std::string(prefix) + "right").c_str(),
                     rect.right);
    SetNamedProperty(env, object, (std::string(prefix) + "top").c_str(),
                     rect.top);
    SetNamedProperty(env, object, (std::string(prefix) + "bottom").c_str(),
                     rect.bottom);
}

template <typename T>
void SetRectPropertiesWithWidthHeight(napi_env env, napi_value object,
                                      const char* prefix, const T& rect) {
    SetRectProperties(env, object, prefix, rect);
    SetNamedProperty(env, object, (std::string(prefix) + "width").c_str(),
                     rect.width);
    SetNamedProperty(env, object, (std::string(prefix) + "height").c_str(),
                     rect.height);
}

#endif // TARO_HARMONY_CPP_VISIBLE_INFO_H
