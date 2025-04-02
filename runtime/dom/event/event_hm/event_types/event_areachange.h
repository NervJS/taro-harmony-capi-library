/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include "../event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroEventAreaChange : public TaroEventBase {
            public:
            TaroEventAreaChange(const std::string& js_event_type, ArkUI_NodeEvent* event);

            virtual ~TaroEventAreaChange() {}

            int parseHmEvent(ArkUI_NodeEvent* event) override;

            virtual int serializeFun(napi_value& ret_obj) override;

            public:
            float old_width_ = 0;    // 表示过去目标元素的宽度，类型为number，单位vp
            float old_height_ = 0;   // 表示过去目标元素的高度，类型为number，单位vp
            float old_parent_x_ = 0; // 表示过去目标元素左上角相对父元素左上角的位置的x轴坐标，类型为number，单位vp
            float old_parent_y_ = 0; // 表示过去目标元素左上角相对父元素左上角的位置的y轴坐标，类型为number，单位vp
            float old_page_x_ = 0;   // 表示过去目标元素目标元素左上角相对页面左上角的位置的x轴坐标，类型为number，单位vp。
            float old_page_y_ = 0;   // 表示过去目标元素目标元素左上角相对页面左上角的位置的y轴坐标，类型为number，单位vp。
            float new_width_ = 0;    // 表示新目标元素的宽度，类型为number，单位vp
            float new_height_ = 0;   // 表示新目标元素的高度，类型为number，单位vp
            float new_parent_x_ = 0; // 表示新目标元素左上角相对父元素左上角的位置的x轴坐标，类型为number，单位vp
            float new_parent_y_ = 0; // 表示新目标元素左上角相对父元素左上角的位置的y轴坐标，类型为number，单位vp
            float new_page_x_ = 0;   // 表示新目标目标元素左上角相对页面左上角的位置的x轴坐标，类型为number，单位vp。
            float new_page_y_ = 0;   // 表示新目标目标元素左上角相对页面左上角的位置的y轴坐标，类型为number，单位vp。
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
