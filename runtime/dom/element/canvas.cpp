//
// Created on 2024/8/21.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "canvas.h"

#include <math.h>

#include "arkjs/ArkJS.h"
#include "runtime/dom/ark_nodes/canvas.h"

namespace TaroRuntime {
namespace TaroDOM {
    std::unordered_map<std::string, CANVAS_FUNC_CODE> CANVAS_FUNC_CODE_MAPPING = {
        {"setStrokeStyle", CANVAS_FUNC_CODE::SET_STROKE_STYLE},
        {"setLineWidth", CANVAS_FUNC_CODE::SET_LINE_WIDTH},
        {"moveTo", CANVAS_FUNC_CODE::MOVE_TO},
        {"lineTo", CANVAS_FUNC_CODE::LINE_TO},
        {"stroke", CANVAS_FUNC_CODE::STROKE},
        {"fill", CANVAS_FUNC_CODE::Fill},
        {"rect", CANVAS_FUNC_CODE::RECT},
        {"arc", CANVAS_FUNC_CODE::ARC},
        {"font", CANVAS_FUNC_CODE::Font},
        {"setFontSize", CANVAS_FUNC_CODE::SetFontSize},
        {"fillText", CANVAS_FUNC_CODE::FillText},
        {"fillRect", CANVAS_FUNC_CODE::FillRect},
        {"strokeRect", CANVAS_FUNC_CODE::StrokeRect},
        {"beginPath", CANVAS_FUNC_CODE::BeginPath},
        {"closePath", CANVAS_FUNC_CODE::ClosePath},
        {"setTextAlign", CANVAS_FUNC_CODE::SetTextAlign},
        {"setFillStyle", CANVAS_FUNC_CODE::SetFillStyle},
        {"translate", CANVAS_FUNC_CODE::Translate},
        {"rotate", CANVAS_FUNC_CODE::Rotate},
        {"scale", CANVAS_FUNC_CODE::Scale},
        {"measure", CANVAS_FUNC_CODE::Measure},
        {"drawImage", CANVAS_FUNC_CODE::DrawImage},
        {"reset", CANVAS_FUNC_CODE::RESET},
        {"draw", CANVAS_FUNC_CODE::DRAW},
        {"toDataUrl", CANVAS_FUNC_CODE::ToDataUrl},
        {"drawAble", CANVAS_FUNC_CODE::DRAWABLE},
    };

    TaroCanvas::TaroCanvas(napi_value node)
        : TaroElement(node) {
    }
    TaroCanvas::~TaroCanvas() {}

    void TaroCanvas::Build() {
        if (!is_init_) {
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto node = std::make_shared<TaroCanvasNode>(element);
            node->Build();
            SetRenderNode(node);
        }
    }

    void TaroCanvas::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        GetHeadRenderNode()->SetStyle(style_);
    }

    void TaroCanvas::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);
    }

    int TaroCanvas::GetFuncCode(std::string str) {
        auto it = CANVAS_FUNC_CODE_MAPPING.find(str);
        if (it != CANVAS_FUNC_CODE_MAPPING.end()) {
            return static_cast<int>(it->second);
        }
        return TaroElement::GetFuncCode(str);
    }

    napi_value TaroCanvas::ExecuteFunc(std::shared_ptr<TaroRenderNode> renderNode, napi_value name, napi_value params) {
        auto node = std::static_pointer_cast<TaroCanvasNode>(renderNode);
        NapiGetter paramsGetter(params);
        CANVAS_FUNC_CODE code = static_cast<CANVAS_FUNC_CODE>(TaroElement::GetFuncCode(name));
        switch (code) {
            case CANVAS_FUNC_CODE::SET_STROKE_STYLE:
                node->SetStrokeStyle(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::SET_LINE_WIDTH:
                node->SetLineWidth(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::RECT:
                node->Rect(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::DRAW:
                node->Draw();
                break;
            case CANVAS_FUNC_CODE::STROKE:
                node->Stroke();
                break;
            case CANVAS_FUNC_CODE::Fill:
                node->Fill();
                break;
            case CANVAS_FUNC_CODE::MOVE_TO:
                node->MoveTo(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::ARC:
                node->Arc(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::Font:
                node->Font(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::SetFontSize:
                node->SetFontSize(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::SetFillStyle:
                node->SetFillStyle(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::FillText:
                node->FillText(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::FillRect:
                node->FillRect(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::StrokeRect:
                node->StrokeRect(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::BeginPath:
                node->BeginPath();
                break;
            case CANVAS_FUNC_CODE::ClosePath:
                node->ClosePath();
                break;
            case TaroDOM::CANVAS_FUNC_CODE::SetTextAlign:
                node->TextAlign(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::LINE_TO:
                node->LineTo(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::Translate:
                node->Translate(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::Rotate:
                node->Rotate(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::Scale:
                node->Scale(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::Measure:
                {
                    double width;
                    double height;
                    node->Measure(paramsGetter, width, height);
                    ArkJS arkJs(NativeNodeApi::env);
                    return arkJs.createDouble(width);
                }
            case CANVAS_FUNC_CODE::DrawImage: 
                node->DrawImage(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::DRAWABLE: 
                node->AwaitDrawAble(paramsGetter);
                break;
            case CANVAS_FUNC_CODE::RESET:
                node->Reset();
                break;
            case CANVAS_FUNC_CODE::ToDataUrl:
                {
                    std::string dataUrl;
                    node->ExportImage(paramsGetter, dataUrl);
                    ArkJS arkJs(NativeNodeApi::env);
                    return arkJs.createString(dataUrl);
                }
            default:
                return TaroElement::ExecuteFunc(renderNode, name, params);
        }

        return nullptr;
    }
} // namespace TaroDOM
} // namespace TaroRuntime
