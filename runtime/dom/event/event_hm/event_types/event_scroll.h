/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <cstdint>

#include "runtime/dom/event/event_hm/event_base.h"

namespace TaroRuntime {
namespace TaroDOM {
    namespace TaroEvent {
        class TaroEventScroll : public TaroEventBase {
            public:
            TaroEventScroll(const std::string &js_event_type, ArkUI_NodeEvent *event);

            virtual ~TaroEventScroll() = default;

            int parseHmEvent(ArkUI_NodeEvent *event) override;

            int serializeFun(napi_value &ret_obj) override;

            public:
            float deltaX_ = 0;
            float deltaY_ = 0;
        };

        class TaroEventDidScroll : public TaroEventBase {
            public:
            TaroEventDidScroll(const std::string &js_event_type, ArkUI_NodeEvent *event);

            virtual ~TaroEventDidScroll() = default;

            int parseHmEvent(ArkUI_NodeEvent *event) override;

            virtual int serializeFun(napi_value &ret_obj) override;

            public:
            float deltaX_ = 0;
            float deltaY_ = 0;
            ArkUI_ScrollState scrollState_;
        };

        class TaroEventDidWaterFlowScroll : public TaroEventBase {
            public:
            TaroEventDidWaterFlowScroll(const std::string &js_event_type, ArkUI_NodeEvent *event);

            virtual ~TaroEventDidWaterFlowScroll() = default;

            int parseHmEvent(ArkUI_NodeEvent *event) override;

            virtual int serializeFun(napi_value &ret_obj) override;

            public:
            float deltaY_ = 0;
            ArkUI_ScrollState scrollState_;
        };

        class TaroEventLazyLoadScrollIndex : public TaroEventBase {
            public:
            TaroEventLazyLoadScrollIndex(const std::string &js_event_type, ArkUI_NodeEvent *event);

            virtual ~TaroEventLazyLoadScrollIndex() = default;

            int parseHmEvent(ArkUI_NodeEvent *event) override;

            virtual int serializeFun(napi_value &ret_obj) override;

            public:
            int32_t startIndex_;
            int32_t endIndex_;
        };
    } // namespace TaroEvent
} // namespace TaroDOM
} // namespace TaroRuntime
