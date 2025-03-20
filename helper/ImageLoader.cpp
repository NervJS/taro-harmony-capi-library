//
// Created on 2025/3/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "ImageLoader.h"
#include <cstddef>
#include <multimedia/image_framework/image/image_source_native.h>

#include "helper/FileDownloader.h"
#include "helper/FileManager.h"
#include "helper/TaroLog.h"

namespace TaroHelper {
void loadImage(const LoadRequestOptions &options, ImageRequestCallback &&onCallback) noexcept {
    if (options.url.empty()) {
        ErrorImageInfo errorInfo = {.url = options.url};
        onCallback(errorInfo);
        return;
    }
    if (options.url.compare("http")) {
        auto downloader = FileDownloader::GetInstance();
        auto fileDownloaderParam = FileDownloaderParam{.headers = {{"Content-Type", "application/octet-stream"}}};
        auto downloadCallback = [options, callback = std::move(onCallback)](const Rcp_Response* response, uint32_t errCode){
            if(!response || errCode != 0) {
                ErrorImageInfo errorInfo = {.url = options.url, .errorCode = errCode};
                callback(errorInfo);
                return;
            }
            auto src = response->body.buffer;
            auto length = response->body.length;
            auto fileManagerInstance = FileManager::GetInstance();
            auto fileName = "/images/" + fileManagerInstance->GetFilename(options.url);
            if (fileManagerInstance->WriteFileToFilesDir(fileName, src, length) != 0) {
                ErrorImageInfo errorInfo = {.url = options.url};
                callback(errorInfo);
                return;
            }
            std::string filePath = fileManagerInstance->GetFilesDir() + fileName;
            auto loadRes = loadLocalImage(options, filePath.data(), filePath.length());
            callback(loadRes);
        };
        downloader->Download(options.url, fileDownloaderParam, downloadCallback);
    } else {
        char path[options.url.length() + 1];
        strcpy(path, options.url.data());
        auto loadRes = loadLocalImage(options, path, options.url.length());
        onCallback(loadRes);
    }
}

static std::variant<ResultImageInfo, ErrorImageInfo> loadLocalImage(const LoadRequestOptions &options, char *path, size_t length) {
    ResultImageInfo result = {.url = options.url};
    OH_ImageSourceNative * imageSrc = nullptr;
    Image_ErrorCode errorCode;
    errorCode = OH_ImageSourceNative_CreateFromUri(path, length, &imageSrc);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "loadImage: OH_ImageSourceNative_CreateFromUri failed.", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    OH_PixelmapNative *pixelmapNative = nullptr;
    OH_DecodingOptions *ops = nullptr;
    errorCode = OH_ImageSourceNative_CreatePixelmap(imageSrc, ops, &pixelmapNative);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "loadImage: OH_ImageSourceNative_CreatePixelmap failed.", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    errorCode = OH_ImageSourceNative_Release(imageSrc);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "loadImage: OH_PixelmapImageInfo_Create failed.", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    OH_Pixelmap_ImageInfo *pixelmapImageInfo = nullptr;
    errorCode = OH_PixelmapImageInfo_Create(&pixelmapImageInfo);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "loadImage: OH_PixelmapImageInfo_Create failed.", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    errorCode = OH_PixelmapNative_GetImageInfo(pixelmapNative, pixelmapImageInfo);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "loadImage: OH_PixelmapNative_GetImageInfo failed.", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    Image_ErrorCode getWidthRes = OH_PixelmapImageInfo_GetWidth(pixelmapImageInfo, &result.width);
    Image_ErrorCode getHeightRes = OH_PixelmapImageInfo_GetHeight(pixelmapImageInfo, &result.height);
    errorCode = OH_PixelmapImageInfo_Release(pixelmapImageInfo);
    if (getWidthRes != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "loadImage: OH_PixelmapImageInfo_GetWidth failed.", getWidthRes);
        return ErrorImageInfo{.url = options.url, .errorCode = getWidthRes};
    }
    if (getWidthRes != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "loadImage: OH_PixelmapImageInfo_GetHeight failed.", getHeightRes);
        return ErrorImageInfo{.url = options.url, .errorCode = getHeightRes};
    }
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "loadImage: OH_PixelmapImageInfo_Release failed.", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    
    result.result_DrawableDescriptor = OH_ArkUI_DrawableDescriptor_CreateFromPixelMap(pixelmapNative);
    if(!options.keepPixelMap) {
        errorCode = OH_PixelmapNative_Release(pixelmapNative);
        if (errorCode != IMAGE_SUCCESS) {
            TARO_LOG_ERROR("TaroImageLoader", "loadImage: OH_PixelmapImageInfo_GetHeight failed.", errorCode);
            return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
        }
    }
    return result;
};
}
