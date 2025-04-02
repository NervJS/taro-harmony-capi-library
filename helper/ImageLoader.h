/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <string>
#include <arkui/drawable_descriptor.h>
#include <multimedia/image_framework/image/image_source_native.h>

namespace TaroHelper {
// 定义加载请求选项结构体
struct LoadRequestOptions {
    std::string url;           // 图片URL
    double width = 0;          // 图片宽度，单位 vp
    double height = 0;         // 图片高度，单位 vp
    bool keepPixelMap = false; // 是否持有pixelMap，手动释放
};

// 定义结果图片信息结构体
struct ResultImageInfo {
    std::string url;                                     // 图片URL
    ArkUI_DrawableDescriptor* result_DrawableDescriptor; // 鸿蒙图片DrawableDescriptor
    uint32_t width = 0;                                  // 图片宽度,单位 px
    uint32_t height = 0;                                 // 图片高度,单位 px
};

// 定义异常图片信息结构体
struct ErrorImageInfo {
    std::string url;                // 图片URL
    bool isUnsupportedType = false; // 是否为不支持的格式
    uint32_t errorCode = -1;        // 错误码
};

// 定义图片请求回调函数类型
using ImageRequestCallback = std::function<void(const std::variant<ResultImageInfo, ErrorImageInfo>)>;

void loadImage(const LoadRequestOptions& options, ImageRequestCallback&& onCallback) noexcept;
static std::variant<ResultImageInfo, ErrorImageInfo> loadImageFromUri(const LoadRequestOptions& options, char* path, size_t length);
static Image_ErrorCode getImageInfo(OH_ImageSourceNative* imageSrc, ResultImageInfo& result);
static Image_ErrorCode getPixelmapWithOneFrame(OH_ImageSourceNative* imageSrc, OH_PixelmapNative** pixelmapNative);
} // namespace TaroHelper

#endif // IMAGELOADER_H
