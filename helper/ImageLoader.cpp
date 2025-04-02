/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "ImageLoader.h"
#include <cstddef>

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
    auto fileManagerInstance = FileManager::GetInstance();
    auto fileName = "/images/" + fileManagerInstance->GetFilename(options.url);
    if (options.url.compare("http") && !fileManagerInstance->PathExists(fileName)) {
        auto downloader = FileDownloader::GetInstance();
        auto fileDownloaderParam = FileDownloaderParam{.headers = {{"Content-Type", "application/octet-stream"}}};
        auto downloadCallback = [options, callback = std::move(onCallback), fileManagerInstance, fileName](const Rcp_Response* response, uint32_t errCode){
            if(!response || errCode != 0) {
                ErrorImageInfo errorInfo = {.url = options.url, .errorCode = errCode};
                callback(errorInfo);
                return;
            }
            auto src = response->body.buffer;
            auto length = response->body.length;
            if (fileManagerInstance->WriteFileToFilesDir(fileName, src, length) != 0) {
                ErrorImageInfo errorInfo = {.url = options.url};
                callback(errorInfo);
                return;
            }
            std::string filePath = fileManagerInstance->GetFilesDir() + fileName;
            auto loadRes = loadImageFromUri(options, filePath.data(), filePath.length());
            callback(loadRes);
        };
        downloader->Download(options.url, fileDownloaderParam, downloadCallback);
    } else {
        char path[options.url.length() + 1];
        strcpy(path, options.url.data());
        auto loadRes = loadImageFromUri(options, path, options.url.length());
        onCallback(loadRes);
    }
}

static Image_ErrorCode getImageInfo(OH_ImageSourceNative *imageSrc, ResultImageInfo &result) {
    OH_ImageSource_Info *imageSourceInfo;
    auto errorCode = OH_ImageSourceInfo_Create(&imageSourceInfo);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceInfo_Create failed. errorCode: %{public}d", errorCode);
        return errorCode;
    }
    errorCode = OH_ImageSourceNative_GetImageInfo(imageSrc, 0, imageSourceInfo);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceNative_GetImageInfo failed. errorCode: %{public}d", errorCode);
        OH_ImageSourceInfo_Release(imageSourceInfo);
        return errorCode;
    }
    auto heightErrorCode = OH_ImageSourceInfo_GetHeight(imageSourceInfo, &result.height);
    auto widthErrorCode = OH_ImageSourceInfo_GetWidth(imageSourceInfo, &result.width);
    errorCode = OH_ImageSourceInfo_Release(imageSourceInfo);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceInfo_Release failed. errorCode: %{public}d", errorCode);
        return errorCode;
    }
    if (heightErrorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceInfo_GetHeight failed. errorCode: %{public}d", heightErrorCode);
        return heightErrorCode;
    }
    if (widthErrorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceInfo_GetWidth failed. errorCode: %{public}d", widthErrorCode);
        return widthErrorCode;
    }
    return IMAGE_SUCCESS;
}

static Image_ErrorCode getPixelmapWithOneFrame(OH_ImageSourceNative *imageSrc, OH_PixelmapNative **pixelmapNative) {
    OH_DecodingOptions *decodingOpts;
    auto errorCode = OH_DecodingOptions_Create(&decodingOpts);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_DecodingOptions_Create failed. errorCode: %{public}d", errorCode);
        return errorCode;
    }
    errorCode = OH_DecodingOptions_SetPixelFormat(decodingOpts, PIXEL_FORMAT_BGRA_8888);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_DecodingOptions_SetPixelFormat failed. errorCode: %{public}d", errorCode);
        OH_DecodingOptions_Release(decodingOpts);
        return errorCode;
    }
    errorCode = OH_ImageSourceNative_CreatePixelmap(imageSrc, decodingOpts, pixelmapNative);
    auto releaseCode = OH_DecodingOptions_Release(decodingOpts);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceNative_CreatePixelmap failed. errorCode: %{public}d", errorCode);
        return errorCode;
    }
    if (releaseCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_DecodingOptions_Release failed. errorCode: %{public}d", releaseCode);
        return errorCode;
    }
    return IMAGE_SUCCESS;
}

static Image_ErrorCode getPixelmapListWithMultiFrame(OH_ImageSourceNative *imageSrc, std::vector<OH_PixelmapNative*> &pixelmaps, uint32_t frameCount) {
    OH_DecodingOptions *decodingOpts;
    auto errorCode = OH_DecodingOptions_Create(&decodingOpts);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_DecodingOptions_Create failed. errorCode: %{public}d", errorCode);
        return errorCode;
    }
    errorCode = OH_DecodingOptions_SetPixelFormat(decodingOpts, PIXEL_FORMAT_BGRA_8888);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_DecodingOptions_SetPixelFormat failed. errorCode: %{public}d", errorCode);
        OH_DecodingOptions_Release(decodingOpts);
        return errorCode;
    }
    errorCode = OH_ImageSourceNative_CreatePixelmapList(imageSrc, decodingOpts, pixelmaps.data(), frameCount);
    OH_DecodingOptions_Release(decodingOpts);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceNative_CreatePixelmapList failed. errorCode: %{public}d", errorCode);
        return errorCode;
    }
    return IMAGE_SUCCESS;
}


static std::variant<ResultImageInfo, ErrorImageInfo> loadImageFromUri(const LoadRequestOptions &options, char *path, size_t length) {
    ResultImageInfo result = {.url = options.url};
    OH_ImageSourceNative *imageSrc = nullptr;
    Image_ErrorCode errorCode;
    errorCode = OH_ImageSourceNative_CreateFromUri(path, length, &imageSrc);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceNative_CreateFromUri failed. errorCode: %{public}d", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }

    errorCode = getImageInfo(imageSrc, result);
    if (errorCode != IMAGE_SUCCESS) {
        OH_ImageSourceNative_Release(imageSrc);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }

    uint32_t frameCount;
    errorCode = OH_ImageSourceNative_GetFrameCount(imageSrc, &frameCount);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceNative_GetFrameCount failed. errorCode: %{public}d", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    if (frameCount == 0) {
        TARO_LOG_ERROR("TaroImageLoader", "got 'frameCount == 0'. errorCode: %{public}d", IMAGE_UNKNOWN_ERROR);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    } else if (frameCount == 1) {
        OH_PixelmapNative *pixelmapNative = nullptr;
        errorCode = getPixelmapWithOneFrame(imageSrc, &pixelmapNative);
        if (errorCode != IMAGE_SUCCESS) {
            OH_ImageSourceNative_Release(imageSrc);
            return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
        }
        result.result_DrawableDescriptor = OH_ArkUI_DrawableDescriptor_CreateFromPixelMap(pixelmapNative);
        if(!options.keepPixelMap) {
            errorCode = OH_PixelmapNative_Release(pixelmapNative);
            if (errorCode != IMAGE_SUCCESS) {
                TARO_LOG_ERROR("TaroImageLoader", "OH_PixelmapNative_Release failed. errorCode: %{public}d", errorCode);
        }
    }
    } else {
        std::vector<OH_PixelmapNative*> pixelmaps(frameCount);
        errorCode = getPixelmapListWithMultiFrame(imageSrc, pixelmaps, frameCount);
        if (errorCode != IMAGE_SUCCESS) {
            OH_ImageSourceNative_Release(imageSrc);
            return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
        }
        result.result_DrawableDescriptor = OH_ArkUI_DrawableDescriptor_CreateFromAnimatedPixelMap(pixelmaps.data(), frameCount);
        if(!options.keepPixelMap) {
            for (int index = 0; index < frameCount; index++) {
                errorCode = OH_PixelmapNative_Release(pixelmaps[index]);
                if (errorCode != IMAGE_SUCCESS) {
                    TARO_LOG_ERROR("TaroImageLoader", "OH_PixelmapNative_Release failed. errorCode: %{public}d", errorCode);
                }
            }
        }
    }

    errorCode = OH_ImageSourceNative_Release(imageSrc);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceNative_Release failed. errorCode: %{public}d", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    return result;
};
}
