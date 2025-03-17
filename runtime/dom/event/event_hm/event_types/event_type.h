#pragma once

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        enum TaroEventType {
            TARO_EVENT_TYPE_BASE,
            // 点击事件
            TARO_EVENT_TYPE_CLICK,
            // touch 事件
            TARO_EVENT_TYPE_TOUCH_START,
            TARO_EVENT_TYPE_TOUCH_MOVE,
            TARO_EVENT_TYPE_TOUCH_END,
            TARO_EVENT_TYPE_TOUCH_CANCEL,
            // common event
            TARO_EVENT_TYPE_NODE_APPEAR,
            TARO_EVENT_TYPE_NODE_DISAPPEAR,
            // area change
            TARO_EVENT_TYPE_AREA_CHANGE,
            TARO_EVENT_TYPE_VISIBLE_AREA_CHANGE,

            // scroll 节点相关事件
            TARO_EVENT_TYPE_SCROLL_ON_SCROLL,
            TARO_EVENT_TYPE_SCROLL_START_ON_SCROLL,
            TARO_EVENT_TYPE_SCROLL_END_ON_SCROLL,
            TARO_EVENT_TYPE_REACH_END_ON_SCROLL,
            TARO_EVENT_TYPE_REACH_START_ON_SCROLL,
            TARO_EVENT_TYPE_DID_SCROLL_ON_SCROLL,

            // 表单事件
            TARO_EVENT_TYPE_FOCUS,
            TARO_EVENT_TYPE_BLUR,
            // input 节点事件
            TARO_EVENT_TYPE_INPUT_ON_INPUT,
            TARO_EVENT_TYPE_INPUT_ON_CHANGE,
            TARO_EVENT_TYPE_INPUT_ON_CONFIRM,
            // textarea 节点事件
            TARO_EVENT_TYPE_TEXT_AREA_ON_INPUT,
            TARO_EVENT_TYPE_TEXT_AREA_ON_CHANGE,
            TARO_EVENT_TYPE_TEXT_AREA_ON_CONFIRM,
            // checkbox 节点相关事件
            TARO_EVENT_TYPE_CHECKBOX_ON_CHANGE,
            // radio 节点相关事件
            TARO_EVENT_TYPE_RADIO_ON_CHANGE,

            // image 节点相关事件
            TARO_EVENT_TYPE_COMPLETE_ON_IMAGE,

            // Swiper节点相关事件
            TARO_EVENT_TYPE_SWIPER_ON_CHANGE,
            TARO_EVENT_TYPE_SWIPER_ON_TRANSITION,

            // picker onChange
            TARO_EVENT_TYPE_TEXT_PICKER_ON_CHANGE,

            TARO_EVENT_TYPE_DATE_PICKER_ON_CHANGE,

            TARO_EVENT_TYPE_TIME_PICKER_ON_CHANGE,
            // WaterFlow 节点相关事件
            TARO_EVENT_TYPE_DID_WATER_FLOW_ON_SCROLL,
            TARO_EVENT_TYPE_WATER_FLOW_ON_SCROLL_INDEX,
            // list 组件相关事件
            TARO_EVENT_TYPE_DID_LIST_ON_SCROLL,
            TARO_EVENT_TYPE_LIST_ON_SCROLL_INDEX,

            // slider节点相关事件
            TARO_EVENT_TYPE_SLIDER_ON_CHANGE,
            TARO_EVENT_TYPE_SLIDER_ON_CHANGING,

            // textarea节点相关事件
            TARO_EVENT_TYPE_AREA_ON_FOCUS,
            TARO_EVENT_TYPE_AREA_ON_BLUR,
            // TARO_EVENT_TYPE_AREA_ON_CONFIRM,
            TARO_EVENT_TYPE_AREA_ON_INPUT,

            TARO_EVENT_TYPE_MAX
        };
    }
} // namespace TaroDOM
} // namespace TaroRuntime
