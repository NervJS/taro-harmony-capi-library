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
class ImagePictureNative {
    public:
        Image_ErrorCode errorCode = IMAGE_SUCCESS;
        OH_DecodingOptionsForPicture *options = nullptr;
        OH_PictureNative *picture = nullptr;
        OH_ImageSourceNative *source = nullptr;
        ImagePictureNative() {}
        ~ImagePictureNative() {}
};

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

std::variant<ResultImageInfo, ErrorImageInfo> loadLocalImage(const LoadRequestOptions &options, char *path, size_t length) {
    ResultImageInfo result = {.url = options.url};
    OH_ImageSourceNative * imageSrc = nullptr;
    Image_ErrorCode errorCode = OH_ImageSourceNative_CreateFromUri(path, length, &imageSrc);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroFileDownloader", "loadImage: OH_ImageSourceNative_CreateFromUri failed.", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    OH_PixelmapNative *pixelmapNative = nullptr;
    OH_DecodingOptions *decodingOptions = nullptr;
    Image_ErrorCode imageErr = OH_ImageSourceNative_CreatePixelmap(imageSrc, decodingOptions, &pixelmapNative);
    if (imageErr != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroFileDownloader", "loadImage: OH_ImageSourceNative_CreatePixelmap failed.", imageErr);
        return ErrorImageInfo{.url = options.url, .errorCode = imageErr};

    }
    OH_Pixelmap_ImageInfo *pixelmapImageInfo = nullptr;
    Image_ErrorCode pixelmapImageInfoCreateRes = OH_PixelmapImageInfo_Create(&pixelmapImageInfo);
    if (pixelmapImageInfoCreateRes != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroFileDownloader", "loadImage: OH_PixelmapImageInfo_Create failed.", pixelmapImageInfoCreateRes);
        return ErrorImageInfo{.url = options.url, .errorCode = pixelmapImageInfoCreateRes};
    }
    Image_ErrorCode getImageInfoRes = OH_PixelmapNative_GetImageInfo(pixelmapNative, pixelmapImageInfo);
    if (getImageInfoRes != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroFileDownloader", "loadImage: OH_PixelmapNative_GetImageInfo failed.", getImageInfoRes);
        return ErrorImageInfo{.url = options.url, .errorCode = getImageInfoRes};
    }
    Image_ErrorCode getWidthRes = OH_PixelmapImageInfo_GetWidth(pixelmapImageInfo, &result.width);
    if (getWidthRes != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroFileDownloader", "loadImage: OH_PixelmapImageInfo_GetWidth failed.", getWidthRes);
        return ErrorImageInfo{.url = options.url, .errorCode = getWidthRes};
    }
    Image_ErrorCode getHeightRes = OH_PixelmapImageInfo_GetHeight(pixelmapImageInfo, &result.height);
    if (getWidthRes != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroFileDownloader", "loadImage: OH_PixelmapImageInfo_GetHeight failed.", getHeightRes);
        return ErrorImageInfo{.url = options.url, .errorCode = getHeightRes};
    }

    result.result_DrawableDescriptor = OH_ArkUI_DrawableDescriptor_CreateFromPixelMap(pixelmapNative);
    if(!options.keepPixelMap) {
        Image_ErrorCode releasePixel = OH_PixelmapNative_Release(pixelmapNative);
        if (releasePixel != IMAGE_SUCCESS) {
            TARO_LOG_ERROR("TaroFileDownloader", "loadImage: OH_PixelmapImageInfo_GetHeight failed.", releasePixel);
            return ErrorImageInfo{.url = options.url, .errorCode = releasePixel};
        }
    }
    return result;
};
}
