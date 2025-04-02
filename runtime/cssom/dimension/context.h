/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_DIMENSION_CONTEXT_H
#define HARMONY_LIBRARY_DIMENSION_CONTEXT_H

#include <memory>
namespace TaroRuntime {

struct SafeArea {
    double top = 0;
    double left = 0;
    double right = 0;
    double bottom = 0;
};

enum Orientation {
    PORTRAIT = 0,
    LANDSCAPE = 1,
    PORTRAIT_INVERTED = 2,
    LANDSCAPE_INVERTED = 3
};

class DimensionContext {
    public:
    static std::shared_ptr<DimensionContext> GetInstance() {
        static std::shared_ptr<DimensionContext> instance(new DimensionContext());
        return instance;
    }

    // 当前的context上下文
    static std::shared_ptr<DimensionContext> GetCurrentContext() {
        static std::shared_ptr<DimensionContext> currentInstance(new DimensionContext());
        return currentInstance;
    }

    static void UpdateCurrentContext(std::shared_ptr<DimensionContext> instance) {
        auto currentContext = GetCurrentContext();
        currentContext->design_ratio_ = instance->design_ratio_;
        currentContext->density_pixels_ = instance->density_pixels_;
        currentContext->scaled_density = instance->scaled_density;
        currentContext->device_width_ = instance->device_width_;
        currentContext->device_height_ = instance->device_height_;
        currentContext->viewport_width_ = instance->viewport_width_;
        currentContext->viewport_height_ = instance->viewport_height_;
        currentContext->density_dpi_ = instance->density_dpi_;
        currentContext->orientation = instance->orientation;
        currentContext->safeArea = instance->safeArea;
    }

    // 屏幕与设计稿的比值
    float design_ratio_ = 1.0f;
    // px 转 vp的比值
    // 显示设备逻辑像素的密度，代表物理像素与逻辑像素的缩放系数，该参数为浮点数，受densityDPI范围限制，取值范围在[0.5，4.0]。一般取值1.0、3.0等，实际取值取决于不同设备提供的densityDPI。
    float density_pixels_ = 1.0f;
    // 显示设备的显示字体的缩放因子。该参数为浮点数，通常与densityPixels相同
    float scaled_density = 1.0f;
    // 设备宽度：单位vp
    float device_width_ = 1.0f;
    // 设备高度：单位vp
    float device_height_ = 1.0f;
    // 视窗宽度：单位vp
    float viewport_width_ = 1.0f;
    // 视窗高度：单位vp
    float viewport_height_ = 1.0f;
    // 显示设备屏幕的物理像素密度，表示每英寸上的像素点数。该参数为浮点数，单位为px，支持的范围为[80.0，640.0]。一般取值160.0、480.0等，实际能取到的值取决于不同设备设置里
    float density_dpi_ = 1.0f;
    // 表示屏幕当前显示的方向
    Orientation orientation = Orientation::PORTRAIT;
    // 安全区域
    SafeArea safeArea;
};

using DimensionContextRef = std::shared_ptr<DimensionContext>;

} // namespace TaroRuntime

#endif // HARMONY_LIBRARY_DIMENSION_CONTEXT_H
