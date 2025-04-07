/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "canvas.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <fcntl.h>
#include <multimedia/image_framework/image/image_packer_native.h>
#include <multimedia/image_framework/image/image_source_native.h>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_pixel_map.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_window/external_window.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "arkjs/ArkJS.h"
#include "helper/ColorUtils.h"
#include "helper/FontParser.h"
#include "runtime/cssom/font/FontFamilyManager.h"
#include "runtime/render.h"
#include "runtime/tmp_resource_manager.h"

namespace TaroRuntime {
namespace TaroDOM {
    // 临时文件路径

    std::unordered_map<int32_t, OH_Drawing_FontWeight> CANVAS_FONT_WEIGHT_MAPPER = {
        {100, FONT_WEIGHT_100},
        {200, FONT_WEIGHT_200},
        {300, FONT_WEIGHT_300},
        {400, FONT_WEIGHT_400},
        {500, FONT_WEIGHT_500},
        {600, FONT_WEIGHT_600},
        {700, FONT_WEIGHT_700},
        {800, FONT_WEIGHT_800},
        {900, FONT_WEIGHT_900},
    };

    std::unordered_map<OH_NativeXComponent*, std::weak_ptr<BaseRenderNode>> TaroCanvasNode::canvas_manager_;

    namespace {
        void OnSurfaceCreatedCB(OH_NativeXComponent* component, void* window) {
            // 可获取 OHNativeWindow 实例
            OHNativeWindow* nativeWindow = static_cast<OHNativeWindow*>(window);
            auto it = TaroCanvasNode::canvas_manager_.find(component);
            if (it != TaroCanvasNode::canvas_manager_.end()) {
                auto canvasNode = std::dynamic_pointer_cast<TaroCanvasNode>(it->second.lock());
                if (canvasNode != nullptr) {
                    canvasNode->drawable = true;
                    canvasNode->nativeWindow_ = nativeWindow;
                    int32_t xSize = OH_NativeXComponent_GetXComponentSize(component, window, &canvasNode->width_, &canvasNode->height_);
                    if ((xSize == OH_NATIVEXCOMPONENT_RESULT_SUCCESS)) {
                        TARO_LOG_ERROR("xComponent", "width = %{public}llu, height = %{public}llu", canvasNode->width_, canvasNode->height_);
                    }
                    canvasNode->StartDraw();
                }
            }
        }
        void OnSurfaceChangedCB(OH_NativeXComponent* component, void* window) {
            // 可获取 OHNativeWindow 实例
            OHNativeWindow* nativeWindow = static_cast<OHNativeWindow*>(window);
            auto it = TaroCanvasNode::canvas_manager_.find(component);
            if (it != TaroCanvasNode::canvas_manager_.end()) {
                auto canvasNode = std::dynamic_pointer_cast<TaroCanvasNode>(it->second.lock());
                if (canvasNode != nullptr) {
                    canvasNode->nativeWindow_ = nativeWindow;
                    OH_Drawing_BitmapDestroy(canvasNode->cBitmap_);
                    canvasNode->cBitmap_ = nullptr;
                }
            }
        }
        void OnSurfaceDestroyedCB(OH_NativeXComponent* component, void* window) {
            // 可获取 OHNativeWindow 实例
            OHNativeWindow* nativeWindow = static_cast<OHNativeWindow*>(window);
            if ((component == nullptr) || (window == nullptr)) {
                return;
            }
            TaroCanvasNode::canvas_manager_.erase(component);
        }

    } // namespace

    TaroCanvasNode::TaroCanvasNode(const TaroElementRef element)
        : TaroRenderNode(element) {
    }

    TaroCanvasNode::~TaroCanvasNode() {
        for (auto i = 0; i < canvasCachePaths_.size(); i++) {
            TaroTmpResource::GetInstance()->ReleasePixels(canvasCachePaths_[i]);
        }

        // 图片资源释放
        for (auto imageResource : imageResources_) {
            // Tips: 释放会导致再次加载时读取不到pixelMap
            OH_PixelmapNative_Release(imageResource.pixelMap);
            OH_Drawing_SamplingOptionsDestroy(imageResource.samplingOpts);
            OH_Drawing_RectDestroy(imageResource.srcRect);
            OH_Drawing_RectDestroy(imageResource.dstRect);
        }
        imageResources_.clear();

        // 文本资源销毁
        for (auto textResources : textResources_) {
            OH_Drawing_DestroyTypography(textResources.typography);
            OH_Drawing_DestroyTextStyle(textResources.txtStyle);
            OH_Drawing_DestroyTypographyStyle(textResources.typoStyle);
        }

        textResources_.clear();

        OH_Drawing_CanvasDestroy(cCanvas_);
        cCanvas_ = nullptr;
        OH_Drawing_BitmapDestroy(cBitmap_);
        cBitmap_ = nullptr;
    }

    void TaroCanvasNode::Build() {
        SetArkUINodeHandle(NativeNodeApi::getInstance()->createNode(ARKUI_NODE_XCOMPONENT));
        xComponent_ = OH_NativeXComponent_GetNativeXComponent(GetArkUINodeHandle());
        xComponentCb_.OnSurfaceCreated = OnSurfaceCreatedCB;
        xComponentCb_.OnSurfaceChanged = OnSurfaceChangedCB;
        xComponentCb_.OnSurfaceDestroyed = OnSurfaceDestroyedCB;
        xComponentCb_.DispatchTouchEvent = nullptr;
        TaroCanvasNode::canvas_manager_[xComponent_] = weak_from_this();
        OH_NativeXComponent_RegisterCallback(xComponent_, &xComponentCb_);
        // 创建一个canvas对象
        cCanvas_ = OH_Drawing_CanvasCreate();
    }

    void TaroCanvasNode::SetStrokeStyle(NapiGetter colorNapi) {
        auto data = ColorUtils::getInt32FromNapiGetter(colorNapi);
        if (!data.has_value()) {
            return;
        }
        uint32_t color = data.value();
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task =
            [color, this](OH_Drawing_Canvas* canvas, TaroCanvasData* canvasData) -> void {
            OH_Drawing_PenSetColor(canvasData->pen, color);
            canvasData->strokeStyle = color;
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::SetLineWidth(NapiGetter widthNapi) {
        auto width = vp2Px(widthNapi.UInt32().value());
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task = [width, this](OH_Drawing_Canvas* canvas, TaroCanvasData* canvasData) -> void {
            OH_Drawing_PenSetWidth(canvasData->pen, width);
            canvasData->strokeWidth = width;
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::Rect(NapiGetter rectNapi) {
        std::vector<NapiGetter> params = rectNapi.Vector().value();
        auto x = vp2Px(params[0].DoubleOr(0));
        auto y = vp2Px(params[1].DoubleOr(0));
        auto width = vp2Px(params[2].DoubleOr(0));
        auto height = vp2Px(params[3].DoubleOr(0));
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task = [this, x, y, width, height](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            data->rects.push_back(OH_Drawing_RectCreate(x, y, x + width, y + height));
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::FillRect(NapiGetter rectNapi) {
        std::vector<NapiGetter> params = rectNapi.Vector().value();
        auto x = vp2Px(params[0].DoubleOr(0));
        auto y = vp2Px(params[1].DoubleOr(0));
        auto width = vp2Px(params[2].DoubleOr(0));
        auto height = vp2Px(params[3].DoubleOr(0));
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task = [this, x, y, width, height](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            OH_Drawing_CanvasAttachBrush(canvas, data->brush);
            OH_Drawing_CanvasDrawRect(canvas, OH_Drawing_RectCreate(x, y, x + width, y + height));
            OH_Drawing_CanvasDetachBrush(canvas);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::StrokeRect(NapiGetter rectNapi) {
        std::vector<NapiGetter> params = rectNapi.Vector().value();
        auto x = vp2Px(params[0].DoubleOr(0));
        auto y = vp2Px(params[1].DoubleOr(0));
        auto width = vp2Px(params[2].DoubleOr(0));
        auto height = vp2Px(params[3].DoubleOr(0));
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task = [this, x, y, width, height](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            OH_Drawing_CanvasAttachPen(canvas, data->pen);
            OH_Drawing_CanvasDrawRect(canvas, OH_Drawing_RectCreate(x, y, x + width, y + height));
            OH_Drawing_CanvasDetachPen(canvas);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::MoveTo(NapiGetter move) {
        std::vector<NapiGetter> params = move.Vector().value();
        auto x = vp2Px(params[0].DoubleOr(0));
        auto y = vp2Px(params[1].DoubleOr(0));
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task = [this, x, y](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            if (!data->path.has_value()) {
                data->path.set(OH_Drawing_PathCreate());
            }
            OH_Drawing_PathMoveTo(data->path.value(), x, y);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::Arc(NapiGetter arcGetter) {
        std::vector<NapiGetter> params = arcGetter.Vector().value();
        auto x = vp2Px(params[0].DoubleOr(0));
        auto y = vp2Px(params[1].DoubleOr(0));
        auto r = vp2Px(params[2].DoubleOr(0));
        auto sAngle = params[3].DoubleOr(0);
        auto eAngle = params[4].DoubleOr(0);
        auto counterclockwise = params[5].BoolOr(false);

        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task =
            [this, x, y, r, sAngle, eAngle, counterclockwise](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            if (!data->path.has_value()) {
                data->path.set(OH_Drawing_PathCreate());
            }
            double left = x - r;
            double top = y - r;
            double right = x + r;
            double bottom = y + r;
            double startAngle = sAngle * HALF_CIRCLE_ANGLE / M_PI;
            double endAngle = eAngle * HALF_CIRCLE_ANGLE / M_PI;
            double sweepAngle = endAngle - startAngle;
            if (counterclockwise) {
                sweepAngle =
                    endAngle > startAngle ? (std::fmod(sweepAngle, FULL_CIRCLE_ANGLE) - FULL_CIRCLE_ANGLE) : sweepAngle;
            } else {
                sweepAngle =
                    endAngle > startAngle ? sweepAngle : (std::fmod(sweepAngle, FULL_CIRCLE_ANGLE) + FULL_CIRCLE_ANGLE);
            }
            if (!NearEqual(startAngle, endAngle) &&
                (NearEqual(std::abs(std::fmod(sweepAngle, FULL_CIRCLE_ANGLE)), 0.0) ||
                 NearEqual(std::abs(std::fmod(sweepAngle, FULL_CIRCLE_ANGLE)), FULL_CIRCLE_ANGLE))) {
                // draw circle
                double half = GreatNotEqual(sweepAngle, 0.0) ? HALF_CIRCLE_ANGLE : -HALF_CIRCLE_ANGLE;
                OH_Drawing_PathArcTo(data->path.value(), left, top, right, bottom, startAngle, half);
                OH_Drawing_PathArcTo(data->path.value(), left, top, right, bottom, (half + startAngle), half);
            } else if (!NearEqual(std::fmod(sweepAngle, FULL_CIRCLE_ANGLE), 0.0) &&
                       std::abs(sweepAngle) > FULL_CIRCLE_ANGLE) {
                double half = GreatNotEqual(sweepAngle, 0.0) ? HALF_CIRCLE_ANGLE : -HALF_CIRCLE_ANGLE;
                OH_Drawing_PathArcTo(data->path.value(), left, top, right, bottom, startAngle, half);
                OH_Drawing_PathArcTo(data->path.value(), left, top, right, bottom, half + startAngle, half);
                OH_Drawing_PathArcTo(data->path.value(), left, top, right, bottom, (half + half + startAngle), sweepAngle);
            } else {
                OH_Drawing_PathArcTo(data->path.value(), left, top, right, bottom, startAngle, sweepAngle);
            }
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::LineTo(NapiGetter line) {
        std::vector<NapiGetter> params = line.Vector().value();
        auto x = vp2Px(params[0].DoubleOr(0));
        auto y = vp2Px(params[1].DoubleOr(0));
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task =
            [this, x, y](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            if (!data->path.has_value()) {
                data->path.set(OH_Drawing_PathCreate());
            }
            OH_Drawing_PathLineTo(data->path.value(), x, y);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::Translate(NapiGetter translate) {
        std::vector<NapiGetter> params = translate.Vector().value();
        auto x = vp2Px(params[0].DoubleOr(0));
        auto y = vp2Px(params[1].DoubleOr(0));
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task =
            [this, x, y](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            OH_Drawing_CanvasTranslate(canvas, x, y);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::Rotate(NapiGetter rotate) {
        auto r = rotate.DoubleOr(0);
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task =
            [this, r](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            OH_Drawing_CanvasRotate(canvas, r, width_ / 2, height_ / 2);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::Scale(NapiGetter scale) {
        std::vector<NapiGetter> params = scale.Vector().value();
        auto x = vp2Px(params[0].DoubleOr(1));
        auto y = vp2Px(params[1].DoubleOr(1));
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task =
            [this, x, y](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            OH_Drawing_CanvasScale(canvas, x, y);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::Stroke() {
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task = [this](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            OH_Drawing_CanvasAttachPen(canvas, data->pen);
            DrawRect(canvas, data);
            DrawPath(canvas, data);
            OH_Drawing_CanvasDetachPen(canvas);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::Fill() {
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task = [this](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            OH_Drawing_CanvasAttachBrush(canvas, data->brush);
            DrawRect(canvas, data);
            DrawPath(canvas, data);
            OH_Drawing_CanvasDetachBrush(canvas);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::BeginPath() {
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task = [this](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            for (OH_Drawing_Rect* rect : data->rects) {
                OH_Drawing_RectDestroy(rect);
            }
            data->rects.clear();
            if (data->path.has_value()) {
                OH_Drawing_PathDestroy(data->path.value());
                data->path.reset();
            }
            data->path.set(OH_Drawing_PathCreate());

            OH_Drawing_CanvasDetachPen(canvas);
            OH_Drawing_CanvasDetachBrush(canvas);

            // 换新的笔和笔刷
            OH_Drawing_BrushReset(data->brush);
            OH_Drawing_PenReset(data->pen);
            OH_Drawing_PenSetAntiAlias(data->pen, true);
            // 应用当前笔和笔刷的状态
            OH_Drawing_PenSetColor(data->pen, data->strokeStyle);
            OH_Drawing_PenSetWidth(data->pen, data->strokeWidth);
            OH_Drawing_BrushSetColor(data->brush, data->fillStyle);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::ClosePath() {
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task = [this](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            if (data->path.has_value()) {
                OH_Drawing_PathClose(data->path.value());
            }
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::Font(NapiGetter font) {
        if (font.String().has_value()) {
            auto fontInfo = TaroHelper::FontParser::parse(font.StringOr(""));
            if (fontInfo.size > 0) {
                current_font_size_ = fontInfo.size;
            }
            current_font_style_ = fontInfo.isItalic ? FONT_STYLE_ITALIC : FONT_STYLE_NORMAL;
            if (fontInfo.numericWeight) {
                auto it = CANVAS_FONT_WEIGHT_MAPPER.find(fontInfo.numericWeight);
                if (it != CANVAS_FONT_WEIGHT_MAPPER.end()) {
                    current_font_weight_ = static_cast<int>(it->second);
                }
            }
            if (fontInfo.family != "") {
                auto fontFamilyVec = TaroHelper::string::split(fontInfo.family, ",");
                std::vector<std::string> validFontFamilies;
                for (const auto& familyView : fontFamilyVec) {
                    std::string family(familyView);
                    auto fontFamilyManager = TaroCSSOM::FontFamilyManager::GetInstance();
                    if (fontFamilyManager->HasFont(family)) {
                        validFontFamilies.push_back(std::move(family));
                    }
                }
                if (validFontFamilies.empty()) {
                    current_font_family_ = {"HarmonyOS Sans"};
                } else {
                    current_font_family_.clear();
                    for (const auto& family : validFontFamilies) {
                        current_font_family_.push_back(family);
                    }
                    current_font_family_.push_back("HarmonyOS Sans");
                }
            }
        }
    }

    void TaroCanvasNode::SetFontSize(NapiGetter size) {
        current_font_size_ = size.DoubleOr(16);
    }

    void TaroCanvasNode::TextAlign(NapiGetter textAlign) {
        std::string align = textAlign.StringOr("left");
        if (align == "left") {
            current_text_align_ = OH_Drawing_TextAlign::TEXT_ALIGN_LEFT;
        } else if (align == "center") {
            current_text_align_ = OH_Drawing_TextAlign::TEXT_ALIGN_CENTER;
        } else if (align == "right") {
            current_text_align_ = OH_Drawing_TextAlign::TEXT_ALIGN_RIGHT;
        }
    }

    void TaroCanvasNode::SetFillStyle(NapiGetter fillStyle) {
        auto data = ColorUtils::getInt32FromNapiGetter(fillStyle);
        if (!data.has_value()) {
            return;
        }
        uint32_t color = data.value();
        current_font_color_ = color;
        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task =
            [color, this](OH_Drawing_Canvas* canvas, TaroCanvasData* canvasData) -> void {
            // 笔刷颜色
            OH_Drawing_BrushSetColor(canvasData->brush, color);
            canvasData->fillStyle = color;
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::FillText(NapiGetter textNapi) {
        std::vector<NapiGetter> params = textNapi.Vector().value();
        auto text = params[0].StringOr("");
        auto x = vp2Px(params[1].DoubleOr(0));
        auto y = vp2Px(params[2].DoubleOr(0));
        current_text = text;

        OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
        OH_Drawing_SetTypographyTextDirection(typoStyle, TEXT_DIRECTION_LTR);
        OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_LEFT);
        OH_Drawing_SetTypographyTextFontStyle(typoStyle, TEXT_ALIGN_LEFT);

        typoHandler_ = OH_Drawing_CreateTypographyHandler(typoStyle, TaroCSSOM::FontFamilyManager::GetInstance()->GetFontCollection());

        OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
        OH_Drawing_SetTextStyleFontStyle(txtStyle, current_font_style_);
        OH_Drawing_SetTextStyleFontSize(txtStyle, vp2Px(current_font_size_));
        OH_Drawing_SetTextStyleColor(txtStyle, current_font_color_);
        OH_Drawing_SetTextStyleFontWeight(txtStyle, current_font_weight_);
        std::vector<const char*> fontFamilyCStrings;
        for (const auto& family : current_font_family_) {
            fontFamilyCStrings.push_back(family.c_str());
        }
        OH_Drawing_SetTextStyleFontFamilies(txtStyle, fontFamilyCStrings.size(), fontFamilyCStrings.data());

        OH_Drawing_TypographyHandlerPushTextStyle(typoHandler_, txtStyle);
        OH_Drawing_TypographyHandlerAddText(typoHandler_, text.c_str());
        OH_Drawing_TypographyHandlerPopTextStyle(typoHandler_);

        OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(typoHandler_);

        OH_Drawing_TypographyLayout(typography, FULL_TEXT_MAX_WIDTH);

        float textWidth = OH_Drawing_TypographyGetLongestLine(typography);
        switch (current_text_align_) {
            case TEXT_ALIGN_CENTER:
                x = x - textWidth / 2;
                break;
            case TEXT_ALIGN_RIGHT:
                x = x - textWidth;
                break;
            default:
                break;
        }

        TaroCanvasTextResource textResource = {
            .txtStyle = txtStyle,
            .typoStyle = typoStyle,
            .typography = typography};
        textResources_.push_back(textResource);

        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task =
            [this, typography, x, y](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            // 将文本绘制到画布上
            OH_Drawing_TypographyPaint(typography, canvas, x, y);
        };
        tasks_.push_back(task);
        OH_Drawing_DestroyTypographyHandler(typoHandler_);
    }

    void TaroCanvasNode::Measure(NapiGetter textNapi, double& width, double& height) {
        std::string text = textNapi.StringOr("");

        OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
        OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);

        OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
        OH_Drawing_SetTypographyTextDirection(typoStyle, TEXT_DIRECTION_LTR);
        OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_LEFT);

        // 当前尺寸
        OH_Drawing_SetTextStyleFontSize(txtStyle, vp2Px(current_font_size_));

        OH_Drawing_TypographyCreate* typoHandler = OH_Drawing_CreateTypographyHandler(typoStyle, TaroCSSOM::FontFamilyManager::GetInstance()->GetFontCollection());

        OH_Drawing_TypographyHandlerPushTextStyle(typoHandler, txtStyle);
        OH_Drawing_TypographyHandlerAddText(typoHandler, text.c_str());

        OH_Drawing_TypographyHandlerPopTextStyle(typoHandler);
        OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(typoHandler);

        OH_Drawing_TypographyLayout(typography, FULL_TEXT_MAX_WIDTH);

        width = px2Vp(OH_Drawing_TypographyGetLongestLine(typography));
        height = px2Vp(OH_Drawing_TypographyGetHeight(typography));
        // 确认typography不再使用时销毁
        OH_Drawing_DestroyTypography(typography);
        // 确认已经完成layout时销毁
        OH_Drawing_DestroyTypographyHandler(typoHandler);
        // 确认已经完成layout时销毁
        OH_Drawing_DestroyTextStyle(txtStyle);
        // 确认已经完成layout时销毁
        OH_Drawing_DestroyTypographyStyle(typoStyle);
    }

    void TaroCanvasNode::Reset() {
        tasks_.clear();
    }

    void TaroCanvasNode::DrawImageFromPixel(const TaroCanvasImageData& imageData, OH_PixelmapNative* pixelmap, double width, double height) {
        double sourceWidth = width;
        double sourceHeight = height;
        OH_Drawing_PixelMap* pixels = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelmap);
        OH_Drawing_Rect* dstRect = OH_Drawing_RectCreate(
            vp2Px(imageData.dx),
            vp2Px(imageData.dy),
            (imageData.dWidth == -1 ? sourceWidth : vp2Px(imageData.dWidth)) + vp2Px(imageData.dx),
            (imageData.dHeight == -1 ? sourceHeight : vp2Px(imageData.dHeight)) + vp2Px(imageData.dy));
        OH_Drawing_Rect* srcRect = OH_Drawing_RectCreate(
            vp2Px(imageData.sx),
            vp2Px(imageData.sy),
            (imageData.sWidth == -1 ? sourceWidth : vp2Px(imageData.sWidth)) + vp2Px(imageData.sx),
            (imageData.sHeight == -1 ? sourceHeight : vp2Px(imageData.sHeight)) + vp2Px(imageData.sy));
        OH_Drawing_SamplingOptions* samplingOpts = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST, OH_Drawing_MipmapMode::MIPMAP_MODE_NONE);

        TaroCanvasImageResource imageResource = {
            .srcRect = srcRect,
            .dstRect = dstRect,
            .pixelMap = pixelmap,
            .samplingOpts = samplingOpts};
        imageResources_.push_back(imageResource);

        std::function<void(OH_Drawing_Canvas*, TaroCanvasData*)> task = [this, pixels, srcRect, dstRect, samplingOpts](OH_Drawing_Canvas* canvas, TaroCanvasData* data) -> void {
            OH_Drawing_CanvasDrawPixelMapRect(canvas, pixels, srcRect, dstRect, samplingOpts);
        };
        tasks_.push_back(task);
    }

    void TaroCanvasNode::DrawImage(NapiGetter imageNapi) {
        TaroCanvasImageData imageData = {
            .dx = imageNapi.GetProperty("dx").DoubleOr(0),
            .dy = imageNapi.GetProperty("dy").DoubleOr(0),
            .dWidth = imageNapi.GetProperty("dWidth").DoubleOr(-1),
            .dHeight = imageNapi.GetProperty("dHeight").DoubleOr(-1),
            .sx = imageNapi.GetProperty("sx").DoubleOr(0),
            .sy = imageNapi.GetProperty("sy").DoubleOr(0),
            .sWidth = imageNapi.GetProperty("sWidth").DoubleOr(-1),
            .sHeight = imageNapi.GetProperty("sHeight").DoubleOr(-1)};

        auto src = imageNapi.GetProperty("src");
        if (src.String().has_value()) {
            // 从资源路径获取
            imageData.url = src.String().value();
            OH_PixelmapNativeHandle pixelmapNative = TaroRuntime::TaroDOM::TaroTmpResource::GetInstance()->tmp_pixels_manager_[imageData.url];
            if (pixelmapNative) {
                // 对有已经通过canvas.createImage创建过的图片，从缓存里面取出来
                OH_Pixelmap_ImageInfo* imageInfo;
                OH_PixelmapImageInfo_Create(&imageInfo);
                Image_ErrorCode error = OH_PixelmapNative_GetImageInfo(pixelmapNative, imageInfo);
                if (error != Image_ErrorCode::IMAGE_SUCCESS) {
                    TARO_LOG_ERROR("DrawImage", "OH_Drawing_PixelMapGetFromOhPixelMapNative Error: %{public}d", error);
                    return;
                }
                uint32_t sourceWidth;
                uint32_t sourceHeight;
                OH_PixelmapImageInfo_GetWidth(imageInfo, &sourceWidth);
                OH_PixelmapImageInfo_GetHeight(imageInfo, &sourceHeight);
                DrawImageFromPixel(imageData, pixelmapNative, sourceWidth, sourceHeight);
            } else {
                TaroHelper::LoadRequestOptions opts = {.url = imageData.url.c_str()};
                if (imageData.dWidth != -1) {
                    opts.width = imageData.dWidth;
                }
                if (imageData.dHeight != -1) {
                    opts.height = imageData.dHeight;
                }
                opts.keepPixelMap = true;
                TaroHelper::loadImage(
                    opts,
                    [this, imageData](const std::variant<TaroHelper::ResultImageInfo, TaroHelper::ErrorImageInfo>& result) {
                        auto res = std::get_if<TaroHelper::ResultImageInfo>(&result);
                        if (res) {
                            OH_PixelmapNativeHandle pixelmap = OH_ArkUI_DrawableDescriptor_GetStaticPixelMap(res->result_DrawableDescriptor->get());
                            DrawImageFromPixel(imageData, pixelmap, res->width, res->height);
                            Draw();
                            relatedImageDrawableDescriptors.push_back(res->result_DrawableDescriptor);
                        }
                    });
            }
        } else {
            // 从napi的imagePixelmap获取
            OH_PixelmapNative* pixelmapNative;
            Image_ErrorCode error = OH_PixelmapNative_ConvertPixelmapNativeFromNapi(NativeNodeApi::getInstance()->env, src.GetNapiValue(), &pixelmapNative);
            if (error != Image_ErrorCode::IMAGE_SUCCESS) {
                TARO_LOG_ERROR("DrawImage", "OH_PixelmapNative_ConvertPixelmapNativeFromNapi Error: %{public}d", error);
                return;
            }
            OH_Pixelmap_ImageInfo* imageInfo;
            OH_PixelmapImageInfo_Create(&imageInfo);
            error = OH_PixelmapNative_GetImageInfo(pixelmapNative, imageInfo);
            if (error != Image_ErrorCode::IMAGE_SUCCESS) {
                TARO_LOG_ERROR("DrawImage", "OH_Drawing_PixelMapGetFromOhPixelMapNative Error: %{public}d", error);
                return;
            }
            uint32_t sourceWidth;
            uint32_t sourceHeight;
            OH_PixelmapImageInfo_GetWidth(imageInfo, &sourceWidth);
            OH_PixelmapImageInfo_GetHeight(imageInfo, &sourceHeight);
            DrawImageFromPixel(imageData, pixelmapNative, sourceWidth, sourceHeight);
        }
    }

    void TaroCanvasNode::StartTask(TaroCanvasData* taskData) {
        for (std::function<void(OH_Drawing_Canvas * canvas, TaroCanvasData*)> f : tasks_) {
            f(cCanvas_, taskData);
        }
    }

    void TaroCanvasNode::StartDraw() {
        ArkJS arkJs(NativeNodeApi::getInstance()->env);
        for (auto cb : draw_cb_refs) {
            std::vector<napi_value> args;
            arkJs.call(arkJs.getReferenceValue(cb), args);
            arkJs.deleteReference(cb);
        }
        draw_cb_refs.clear();
    }

    void TaroCanvasNode::AwaitDrawAble(NapiGetter cbNapi) {
        ArkJS arkJs(NativeNodeApi::getInstance()->env);
        std::vector<napi_value> args;
        napi_value callback = cbNapi.GetNapiValue();
        auto valuetype = arkJs.getType(callback);
        if (valuetype == napi_function) {
            draw_cb_refs.push_back(arkJs.createReference(callback));
        }
        if (drawable) {
            StartDraw();
        }
    }

    void TaroCanvasNode::Draw(bool runTask) {
        if (!drawable || !nativeWindow_)
            return;
        OH_NativeXComponent_GetXComponentSize(xComponent_, nativeWindow_, &width_, &height_);
        // 通过 OH_NativeWindow_NativeWindowRequestBuffer 获取 OHNativeWindowBuffer 实例
        int32_t ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow_, &buffer_, &fenceFd_);
        if (ret != 0)
            return;
        bufferHandle_ = OH_NativeWindow_GetBufferHandleFromNative(buffer_);
        if (!bufferHandle_)
            return;

        uint32_t drawWidth = bufferHandle_->stride / 4;

        // 创建一个bitmap对象
        if (!cBitmap_) {
            cBitmap_ = OH_Drawing_BitmapCreate();
            // 定义bitmap的像素格式
            OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL};
            // 构造对应格式的bitmap，width的值必须为 bufferHandle->stride / 4
            OH_Drawing_BitmapBuild(cBitmap_, drawWidth, height_, &cFormat);
            // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
            OH_Drawing_CanvasBind(cCanvas_, cBitmap_);
            // 使用白色清除画布内容
            OH_Drawing_CanvasClear(cCanvas_, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
        }
        if (runTask) {
            TaroCanvasData taskData;
            auto brush = OH_Drawing_BrushCreate();
            auto pen = OH_Drawing_PenCreate();
            OH_Drawing_PenSetAntiAlias(pen, true);
            taskData.pen = pen;
            taskData.brush = brush;
            StartTask(&taskData);
            DestroyTask(&taskData);
            OH_Drawing_BrushDestroy(taskData.brush);
            OH_Drawing_PenDestroy(taskData.pen);
        }

        mappedAddr_ = static_cast<uint32_t*>(
            // 使用内存映射函数mmap将bufferHandle对应的共享内存映射到用户空间，可以通过映射出来的虚拟地址向bufferHandle中写入图像数据
            // bufferHandle->virAddr是bufferHandle在共享内存中的起始地址，bufferHandle->size是bufferHandle在共享内存中的内存占用大小
            mmap(bufferHandle_->virAddr, bufferHandle_->size, PROT_READ | PROT_WRITE, MAP_SHARED, bufferHandle_->fd, 0));
        if (mappedAddr_ == MAP_FAILED) {
            return;
        }
        // 画完后获取像素地址，地址指向的内存包含画布画的像素数据
        void* bitmapAddr = OH_Drawing_BitmapGetPixels(cBitmap_);
        if (!bitmapAddr)
            return;
        uint32_t* value = static_cast<uint32_t*>(bitmapAddr);

        // 使用mmap获取到的地址来访问内存
        uint32_t* pixel = static_cast<uint32_t*>(mappedAddr_);
        for (uint32_t x = 0; x < drawWidth; x++) {
            for (uint32_t y = 0; y < height_; y++) {
                *pixel++ = *value++;
            }
        }

        // 如果Region中的Rect为nullptr,或者rectNumber为0，则认为OHNativeWindowBuffer全部有内容更改。
        Region region{nullptr, 0};
        // 通过OH_NativeWindow_NativeWindowFlushBuffer 提交给消费者使用，例如：显示在屏幕上。
        OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow_, buffer_, fenceFd_, region);

        // 释放内存
        if (mappedAddr_ && bufferHandle_) {
            munmap(mappedAddr_, bufferHandle_->size);
        }
    }

    void TaroCanvasNode::DrawRect(OH_Drawing_Canvas* canvas, TaroCanvasData* data) {
        if (data->rects.size() == 0) {
            return;
        }
        for (OH_Drawing_Rect* rect : data->rects) {
            OH_Drawing_CanvasDrawRect(canvas, rect);
        }
    }

    void TaroCanvasNode::DrawPath(OH_Drawing_Canvas* canvas, TaroCanvasData* data) {
        if (!data->path.has_value()) {
            return;
        }
        OH_Drawing_CanvasDrawPath(canvas, data->path.value());
    }

    void TaroCanvasNode::DestroyTask(TaroCanvasData* data) {
        // rects
        for (OH_Drawing_Rect* rect : data->rects) {
            OH_Drawing_RectDestroy(rect);
        }
        data->rects.clear();
        //         // pen
        //         if (data->pen) {
        //             OH_Drawing_PenDestroy(data->pen);
        //             data->pen = nullptr;
        //         }
        // path
        if (data->path.has_value()) {
            OH_Drawing_PathDestroy(data->path.value());
            data->path.reset();
        }
    }

    Image_ErrorCode packToFileFromPixelmapTest(OH_PixelmapNative* pixelmap, int fd, std::string& mimeType) {
        // 创建ImagePacker实例
        OH_ImagePackerNative* imagePacker = nullptr;
        Image_ErrorCode errCode = OH_ImagePackerNative_Create(&imagePacker);
        // 指定打包参数，将PixelMap图片源编码后直接打包进文件
        OH_PackingOptions* option = nullptr;
        OH_PackingOptions_Create(&option);
        Image_MimeType image_MimeType = {mimeType.data(), mimeType.size()};
        OH_PackingOptions_SetQuality(option, 100);
        OH_PackingOptions_SetMimeType(option, &image_MimeType);
        errCode = OH_ImagePackerNative_PackToFileFromPixelmap(imagePacker, option, pixelmap, fd);
        if (errCode != IMAGE_SUCCESS) {
            TARO_LOG_ERROR("TaroCanvas", "ImagePackerNativeCTest OH_ImagePackerNative_PackToFileFromPixelmap  failed, errCode: %{public}d.", errCode);
            return errCode;
        }
        // 释放ImagePacker实例
        errCode = OH_ImagePackerNative_Release(imagePacker);
        if (errCode != IMAGE_SUCCESS) {
            TARO_LOG_ERROR("TaroCanvas", "ImagePackerNativeCTest ReleasePacker OH_ImagePackerNative_Release failed, errCode: %{public}d.", errCode);
            return errCode;
        }
        return IMAGE_SUCCESS;
    }

    std::string getFileTypeFromMime(const std::string& mimeType) {
        // 查找第一斜杠的位置
        size_t slashPos = mimeType.find('/');
        if (slashPos != std::string::npos) {
            // 提取斜杠后面的部分
            return mimeType.substr(slashPos + 1);
        }
        return ""; // 如果没有找到斜杠，返回空字符串
    }

    OH_PixelmapNative_AntiAliasingLevel getAntiAliasingLevel(float quality) {
        // 确保quality在0~1范围内
        quality = std::min(1.0f, std::max(0.0f, quality));
        if (quality < 0.25f) {
            return OH_PixelmapNative_AntiAliasing_NONE; // [0.0, 0.25)
        } else if (quality < 0.5f) {
            return OH_PixelmapNative_AntiAliasing_LOW; // [0.25, 0.5)
        } else if (quality < 0.75f) {
            return OH_PixelmapNative_AntiAliasing_MEDIUM; // [0.5, 0.75)
        } else {
            return OH_PixelmapNative_AntiAliasing_HIGH; // [0.75, 1.0]
        }
    }

    // 生成随机文件名
    std::string generateRandomFileName(std::string extension) {
        std::string fileName;
        static char hash[16];
        sprintf(hash, "%lx", time(NULL));
        fileName += hash;
        // 随机选择扩展名
        fileName = fileName + '.' + extension;
        return fileName;
    }

    void TaroCanvasNode::ExportImage(NapiGetter optsNapi, std::string& dataUrl) {
        if (!xComponent_ || !nativeWindow_ || !bufferHandle_ || !cBitmap_)
            return;
        OH_NativeXComponent_GetXComponentSize(xComponent_, nativeWindow_, &width_, &height_);
        TaroCanvasDataUrlOpts dataUrlOpts = {
            .fileType = optsNapi.GetProperty("x").StringOr("image/jpeg"),
            .quality = optsNapi.GetProperty("x").DoubleOr(1),
            .x = optsNapi.GetProperty("x").DoubleOr(0),
            .y = optsNapi.GetProperty("y").DoubleOr(0),
            .width = optsNapi.GetProperty("width").Double(),
            .height = optsNapi.GetProperty("height").Double(),
            .destWidth = optsNapi.GetProperty("destWidth").Double(),
            .destHeight = optsNapi.GetProperty("destHeight").Double(),
        };

        void* pixels = OH_Drawing_BitmapGetPixels(cBitmap_);
        size_t dataLength = bufferHandle_->stride / 4 * height_;
        OH_Pixelmap_InitializationOptions* opts;
        OH_PixelmapInitializationOptions_Create(&opts);
        OH_PixelmapInitializationOptions_SetWidth(opts, width_);
        OH_PixelmapInitializationOptions_SetHeight(opts, height_);
        OH_PixelmapInitializationOptions_SetSrcPixelFormat(opts, PIXEL_FORMAT_RGBA_8888);
        OH_PixelmapInitializationOptions_SetPixelFormat(opts, PIXEL_FORMAT_BGRA_8888);
        OH_PixelmapInitializationOptions_SetAlphaType(opts, ALPHA_FORMAT_PREMUL);
        OH_PixelmapInitializationOptions_SetRowStride(opts, bufferHandle_->stride);
        OH_PixelmapNative* pixelmap;
        Image_ErrorCode err = OH_PixelmapNative_CreatePixelmap(static_cast<uint8_t*>(pixels), dataLength, opts, &pixelmap);
        if (err != Image_ErrorCode::IMAGE_SUCCESS || pixelmap == nullptr) {
            return;
        }
        OH_PixelmapInitializationOptions_Release(opts);

        // 对图片进行裁剪
        Image_Region region;
        region.x = vp2Px(dataUrlOpts.x);
        region.y = vp2Px(dataUrlOpts.y);
        region.width = dataUrlOpts.width.has_value() ? vp2Px(dataUrlOpts.width.value()) : width_ - vp2Px(dataUrlOpts.x);     // 默认canvas宽度-x
        region.height = dataUrlOpts.height.has_value() ? vp2Px(dataUrlOpts.height.value()) : height_ - vp2Px(dataUrlOpts.y); // canvas高度-y
        OH_PixelmapNative_Crop(pixelmap, &region);

        if (dataUrlOpts.destWidth.has_value() || dataUrlOpts.destHeight.has_value()) {
            float scaleX = 1;
            float scaleY = 1;
            if (dataUrlOpts.destWidth.has_value()) {
                scaleX = vp2Px(dataUrlOpts.destWidth.value()) / region.width;
            }
            if (dataUrlOpts.destHeight.has_value()) {
                scaleY = vp2Px(dataUrlOpts.destHeight.value()) / region.height;
            }
            OH_PixelmapNative_ScaleWithAntiAliasing(pixelmap, scaleX, scaleY, getAntiAliasingLevel(dataUrlOpts.quality));
        }

        TaroTmpResource* resourceInst = TaroTmpResource::GetInstance();
        // 确保目录存在
        if (!resourceInst->ensureDirectoryExists(TMP_CANVAS_RESOURCE_PATH)) {
            OH_PixelmapNative_Release(pixelmap);
            return;
        }

        std::string filePath = TMP_CANVAS_RESOURCE_PATH + generateRandomFileName(getFileTypeFromMime(dataUrlOpts.fileType));
        filePath = std::filesystem::absolute(std::filesystem::path(filePath)).lexically_normal();

        resourceInst->tmp_pixels_manager_[filePath] = pixelmap; // 存入缓存
        canvasCachePaths_.push_back(filePath);

        int fd = open(filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        packToFileFromPixelmapTest(pixelmap, fd, dataUrlOpts.fileType);
        close(fd);

        dataUrl = filePath;
    }

} // namespace TaroDOM
} // namespace TaroRuntime
