#include "event_areachange.h"
#include "event_change.h"
#include "event_checkbox.h"
#include "event_click.h"
#include "event_image.h"
#include "event_input.h"
#include "event_picker.h"
#include "event_radio.h"
#include "event_scroll.h"
#include "event_slider.h"
#include "event_swiper.h"
#include "event_touch.h"
#include "event_type.h"
#include "runtime/dom/event/event_hm/event_factory.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        // 通用事件 click、touch
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_CLICK, NODE_ON_CLICK, TaroEventClick);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_TOUCH_START, NODE_TOUCH_EVENT, TaroEventTouchStart);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_TOUCH_MOVE, NODE_TOUCH_EVENT, TaroEventTouchMove);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_TOUCH_END, NODE_TOUCH_EVENT, TaroEventTouchEnd);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_TOUCH_CANCEL, NODE_TOUCH_EVENT, TaroEventTouchCancel);

        REGISTER_TAROEVENT(TARO_EVENT_TYPE_NODE_APPEAR, NODE_EVENT_ON_APPEAR, TaroEventBase);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_NODE_DISAPPEAR, NODE_EVENT_ON_DISAPPEAR, TaroEventBase);

        REGISTER_TAROEVENT(TARO_EVENT_TYPE_AREA_CHANGE, NODE_EVENT_ON_AREA_CHANGE, TaroEventAreaChange);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_VISIBLE_AREA_CHANGE, NODE_EVENT_ON_VISIBLE_AREA_CHANGE, TaroEventBase);

        // scroll 相关事件
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_SCROLL_ON_SCROLL, NODE_SCROLL_EVENT_ON_SCROLL, TaroEventScroll);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_SCROLL_START_ON_SCROLL, NODE_SCROLL_EVENT_ON_SCROLL_START, TaroEventScroll);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_SCROLL_END_ON_SCROLL, NODE_SCROLL_EVENT_ON_SCROLL_STOP, TaroEventScroll);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_REACH_END_ON_SCROLL, NODE_SCROLL_EVENT_ON_REACH_END, TaroEventScroll);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_REACH_START_ON_SCROLL, NODE_SCROLL_EVENT_ON_REACH_START, TaroEventScroll);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_DID_SCROLL_ON_SCROLL, NODE_SCROLL_EVENT_ON_DID_SCROLL, TaroEventDidScroll);

        // image 相关事件
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_COMPLETE_ON_IMAGE, NODE_IMAGE_ON_COMPLETE, TaroEventCompleteOnImage);

        // swiper 相关事件
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_SWIPER_ON_CHANGE, NODE_SWIPER_EVENT_ON_CHANGE, TaroElementChangedInSwiper);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_SWIPER_ON_TRANSITION, NODE_SWIPER_EVENT_ON_CONTENT_DID_SCROLL, TaroEventTransitionInSwiper);

        // 表单相关事件
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_FOCUS, NODE_ON_FOCUS, TaroInputFocus);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_BLUR, NODE_ON_BLUR, TaroInputBlur);
        // input 相关事件
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_INPUT_ON_INPUT, NODE_TEXT_INPUT_ON_CHANGE, TaroTextInputEventInput);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_INPUT_ON_CHANGE, NODE_TEXT_INPUT_ON_CHANGE, TaroTextInputEventChange);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_INPUT_ON_CONFIRM, NODE_TEXT_INPUT_ON_SUBMIT, TaroInputSubmit);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_TEXT_AREA_ON_INPUT, NODE_TEXT_AREA_ON_CHANGE, TaroTextInputEventInput);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_TEXT_AREA_ON_CHANGE, NODE_TEXT_AREA_ON_CHANGE, TaroTextInputEventChange);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_TEXT_AREA_ON_CONFIRM, NODE_TEXT_AREA_ON_SUBMIT, TaroInputSubmit);

        REGISTER_TAROEVENT(TARO_EVENT_TYPE_CHECKBOX_ON_CHANGE, NODE_CHECKBOX_EVENT_ON_CHANGE, TaroCheckboxEventChange);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_RADIO_ON_CHANGE, NODE_RADIO_EVENT_ON_CHANGE, TaroRadioEventChange);

        // picker 相关事件
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_TEXT_PICKER_ON_CHANGE, NODE_TEXT_PICKER_EVENT_ON_CHANGE, TaroPickerChangeEvent);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_DATE_PICKER_ON_CHANGE, NODE_DATE_PICKER_EVENT_ON_DATE_CHANGE, TaroPickerChangeEvent);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_TIME_PICKER_ON_CHANGE, NODE_TIME_PICKER_EVENT_ON_CHANGE, TaroPickerChangeEvent);

        // WaterFlow 相关事件
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_DID_WATER_FLOW_ON_SCROLL, NODE_WATER_FLOW_ON_DID_SCROLL, TaroEventDidWaterFlowScroll);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_WATER_FLOW_ON_SCROLL_INDEX, NODE_WATER_FLOW_ON_SCROLL_INDEX, TaroEventLazyLoadScrollIndex);

        // list 相关事件
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_DID_LIST_ON_SCROLL, NODE_LIST_ON_DID_SCROLL, TaroEventDidWaterFlowScroll);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_LIST_ON_SCROLL_INDEX, NODE_LIST_ON_SCROLL_INDEX, TaroEventLazyLoadScrollIndex);
        // slider节点相关事件
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_SLIDER_ON_CHANGE, NODE_SLIDER_EVENT_ON_CHANGE, TaroSliderEventChange);
        REGISTER_TAROEVENT(TARO_EVENT_TYPE_SLIDER_ON_CHANGING, NODE_SLIDER_EVENT_ON_CHANGE, TaroSliderEventChanging);
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
