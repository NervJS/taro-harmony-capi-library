/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <arkui/native_gesture.h>

namespace TaroRuntime::TaroDOM::TaroEvent {
enum class TaroGestureType {
    LongPress = 1,
    Tap = 2,
    Pan = 3,
    Pinch = 4
};

struct GestureParam {
    public:
    virtual ~GestureParam() = default;
};

struct GestureParam_Tap final : public GestureParam {
    int count_num = 1;
    // 触发长按的最少手指数，最小为1指，最大取值为10指
    int fingers_num = 1;
};

struct GestureParam_LongPress final : public GestureParam {
    // 触发长按的最少手指数，最小为1指，最大取值为10指
    int fingers_num = 1;
    // 是否连续触发事件回调
    bool repeat_result = false;
    // 触发长按的最短时间，单位为毫秒（ms）。设置小于等于0时，按照默认值500处理。
    int duration_num = 500;
};

struct GestureParam_Pan final : public GestureParam {
    // 用于指定触发拖动的最少手指数，最小为1指，最大取值为10指。当设置的值小于1或不设置时，会被转化为默认值 1
    int fingers_num = 1;
    // 用于指定触发拖动的手势方向，此枚举值支持逻辑与(&)和逻辑或（|）运算
    ArkUI_GestureDirectionMask directions;
    // 用于指定触发拖动手势事件的最小拖动距离，单位为px。当设定的值小于等于0时，按默认值5px处理。
    int distance_num = 5;
};

struct GestureParam_Pinch final : public GestureParam {
    // 触发捏合的最少手指数, 最小为2指，最大为5指。默认值：2。
    int fingers_num = 2;
    // 最小识别距离，单位为px。当设置识别距离的值小于等于0时，会被转化为默认值5px处理。
    int distance_num = 5;
};
} // namespace TaroRuntime::TaroDOM::TaroEvent
