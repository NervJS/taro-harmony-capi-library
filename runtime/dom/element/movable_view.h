/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_MOVABLE_VIEW_H
#define HARMONY_LIBRARY_MOVABLE_VIEW_H

#include <arkui/native_gesture.h>

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TaroMovableViewAttributes : public CommonAttributes {
        TaroHelper::Optional<std::string> direction;
        TaroHelper::Optional<bool> outOfBounds;
        TaroHelper::Optional<bool> scale;
        TaroHelper::Optional<float> scaleMin;
        TaroHelper::Optional<float> scaleMax;

        TaroMovableViewAttributes() {
            direction = TaroHelper::Optional<std::string>("none");
            outOfBounds = TaroHelper::Optional<bool>(false);
            scale = TaroHelper::Optional<bool>(false);
            scaleMin = TaroHelper::Optional<float>(0.5);
            scaleMax = TaroHelper::Optional<float>(10.0);
        }
    };

    class TaroMovableView : public TaroElement {
        public:
        TaroMovableView(napi_value node);

        ~TaroMovableView();

        void SetAttributesToRenderNode() override;

        void Build() override;
        void Build(std::shared_ptr<TaroElement>& reuse_element) override;

        // 实现Attribute，监听属性的变化
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        void onPanStart(ArkUI_GestureEvent* event);
        void onPanUpdate(ArkUI_GestureEvent* event);
        void onPanEnd(ArkUI_GestureEvent* event);

        void onPinchStart(ArkUI_GestureEvent* event);
        void onPinchUpdate(ArkUI_GestureEvent* event);
        void onPinchEnd(ArkUI_GestureEvent* event);

        void callJSFunc(std::string eventType);

        bool bindListenEvent(const std::string& event_name) override;

        private:
        std::unique_ptr<TaroMovableViewAttributes> attributes_;
        float position_x;
        float position_y;
        float position_temp_x;
        float position_temp_y;
        float out_of_bounds;
        float scale_value;
        float scale_value_temp;

        float area_width;
        float area_height;
        float self_width;
        float self_height;

        void bindSelfEventHandle();
        void updatePanGesture(std::string direction);
        float calcPosition(float position, float start, float end);
        void checkPositionBoundary();
        bool checkScaleValueInBounds(float currentScale);
        void serializeForPan(napi_value& ret_obj);
        void serializeForPinch(napi_value& ret_obj);
    };

} // namespace TaroDOM
} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_MOVABLE_VIEW_H
