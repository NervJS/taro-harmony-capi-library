/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    enum class CANVAS_FUNC_CODE {
        SET_STROKE_STYLE = 100,
        SET_LINE_WIDTH,
        MOVE_TO,
        LINE_TO,
        STROKE,
        Fill,
        RECT,
        ARC,
        Font,
        SetFontSize,
        SetFillStyle,
        SetTextAlign,
        FillText,
        FillRect,
        StrokeRect,
        BeginPath,
        ClosePath,
        Translate,
        Rotate,
        Scale,
        Measure,
        DrawImage,
        ToDataUrl,
        RESET,
        DRAW,
        DRAWABLE,
    };

    extern std::unordered_map<std::string, CANVAS_FUNC_CODE> CANVAS_FUNC_CODE_MAPPING;

    class TaroCanvas : public TaroElement {
        public:
        TaroCanvas(napi_value node);
        ~TaroCanvas();

        void Build() override;
        void SetAttributesToRenderNode() override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        int GetFuncCode(std::string str) override;
        napi_value ExecuteFunc(std::shared_ptr<TaroRenderNode> renderNode, napi_value name, napi_value params) override;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
