/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include <cstdint>
#include <native_window/external_window.h>

#include "runtime/dom/ark_nodes/arkui_node.h"

namespace TaroRuntime {
namespace TaroDOM {

    // 定义图片绘制结构体
    struct TaroCanvasImageData {
        std::string url;     // 图片URL
        double dx = 0;       // 目标画布的绘制x坐标，单位 vp
        double dy = 0;       // 目标画布的绘制y坐标，单位 vp
        double dWidth = -1;  // 图片宽度，单位 vp
        double dHeight = -1; // 图片高度，单位 vp
        double sx = 0;       // 源图像的裁剪起始坐标，单位 vp
        double sy = 0;       // 源图像的裁剪起始坐标，单位 vp
        double sWidth = -1;  // 裁切宽度，单位 vp
        double sHeight = -1; // 裁切高度，单位 vp
    };
    // 定义图片绘制结构体
    struct TaroCanvasDataUrlOpts {
        std::string fileType;                    // 图片类型
        double quality;                          // 图片质量0～1
        double x = 0;                            // 源图像的裁剪起始坐标，单位 vp
        double y = 0;                            // 源图像的裁剪起始坐标，单位 vp
        TaroHelper::Optional<double> width;      // 裁切宽度，单位 vp
        TaroHelper::Optional<double> height;     // 裁切高度，单位 vp
        TaroHelper::Optional<double> destWidth;  // 图片宽度，单位 vp
        TaroHelper::Optional<double> destHeight; // 图片高度，单位 vp
    };

    struct TaroCanvasImageResource {
        OH_PixelmapNativeHandle pixelMap;
        OH_Drawing_SamplingOptions* samplingOpts;
        OH_Drawing_Rect* dstRect;
        OH_Drawing_Rect* srcRect;
    };

    struct TaroCanvasTextResource {
        OH_Drawing_TextStyle* txtStyle;
        OH_Drawing_TypographyStyle* typoStyle;
        OH_Drawing_Typography* typography;
    };

    struct TaroCanvasData {
        OH_Drawing_Pen* pen;
        OH_Drawing_Brush* brush;
        std::vector<OH_Drawing_Rect*> rects;
        TaroHelper::Optional<OH_Drawing_Path*> path;
        TaroHelper::Optional<OH_Drawing_TextStyle*> textStyle;
        uint32_t strokeStyle = 0xff000000;
        uint32_t strokeWidth = 1;
        uint32_t fillStyle = 0x00000000;
    };

    class TaroCanvasNode : public TaroRenderNode {
        public:
        TaroCanvasNode(const TaroElementRef element);
        ~TaroCanvasNode();

        static std::unordered_map<OH_NativeXComponent*, std::weak_ptr<BaseRenderNode>> canvas_manager_;
        OHNativeWindow* nativeWindow_;
        OH_Drawing_Bitmap* cBitmap_ = nullptr;
        bool drawable = false;
        std::vector<napi_ref> draw_cb_refs;
        uint64_t height_ = 0;
        uint64_t width_ = 0;

        void Build() override;

        void SetStrokeStyle(NapiGetter colorNapi);
        void SetLineWidth(NapiGetter widthNapi);
        void Rect(NapiGetter rectNapi);
        void MoveTo(NapiGetter move);
        void Arc(NapiGetter arc);
        void LineTo(NapiGetter arc);
        void Stroke();
        void Fill();
        void BeginPath();
        void ClosePath();
        void DrawImage(NapiGetter imageNapi);
        void Font(NapiGetter font);
        void SetFontSize(NapiGetter size);
        void TextAlign(NapiGetter size);
        void SetFillStyle(NapiGetter fillStyle);
        void FillText(NapiGetter textNapi);
        void FillRect(NapiGetter rectNapi);
        void StrokeRect(NapiGetter rectNapi);
        void Translate(NapiGetter translate);
        void Rotate(NapiGetter rotate);
        void Scale(NapiGetter scale);

        void Measure(NapiGetter textNapi, double& width, double& height);
        void Reset();
        void Draw(bool runTask = true);
        void StartDraw();
        void AwaitDrawAble(NapiGetter cbNapi);
        void ExportImage(NapiGetter optsNapi, std::string& dataUrl);
        void DrawImageFromPixel(const TaroCanvasImageData& imageData, OH_PixelmapNative* pixelmap, double width, double height);

        private:
        std::vector<std::function<void(OH_Drawing_Canvas* canvas, TaroCanvasData*)>> tasks_;

        std::vector<TaroCanvasImageResource> imageResources_;
        std::vector<TaroCanvasTextResource> textResources_;
        OH_Drawing_TypographyCreate* typoHandler_;

        // 当前文字
        double current_font_size_ = 16;
        double current_font_style_ = FONT_STYLE_NORMAL;
        double current_font_weight_ = FONT_WEIGHT_400;
        double current_font_color_ = 0xff000000;
        std::vector<std::string> current_font_family_;
        OH_Drawing_TextAlign current_text_align_ = TEXT_ALIGN_LEFT;
        std::string current_text = "";

        static constexpr double HALF_CIRCLE_ANGLE = 180.0;
        static constexpr double FULL_CIRCLE_ANGLE = 360.0;
        static constexpr double FULL_TEXT_MAX_WIDTH = 100000;
        void DrawRect(OH_Drawing_Canvas* canvas, TaroCanvasData* data);
        void DrawPath(OH_Drawing_Canvas* canvas, TaroCanvasData* data);
        void StartTask(TaroCanvasData* data);
        void DestroyTask(TaroCanvasData* data);

        inline bool NearEqual(const double left, const double right, const double epsilon) {
            return (std::abs(left - right) <= epsilon);
        }

        inline bool NearEqual(const double& left, const double& right) {
            constexpr double epsilon = 0.00001f;
            return NearEqual(left, right, epsilon);
        }

        inline bool GreatNotEqual(double left, double right) {
            constexpr double epsilon = 0.001f;
            return (left - right) > epsilon;
        }

        OH_NativeXComponent* xComponent_;
        OH_NativeXComponent_Callback xComponentCb_;
        OH_Drawing_Canvas* cCanvas_ = nullptr;
        uint32_t* mappedAddr_ = nullptr;
        BufferHandle* bufferHandle_ = nullptr;
        struct NativeWindowBuffer* buffer_ = nullptr;
        int fenceFd_ = 0;
        std::vector<std::string> canvasCachePaths_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
