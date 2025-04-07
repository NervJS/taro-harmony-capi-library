/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "ImageLoader.h"

#include <cstddef>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <list>

#include "helper/FileDownloader.h"
#include "helper/FileManager.h"
#include "helper/TaroLog.h"
#include "runtime/render.h"

namespace TaroHelper {

ImageLoader::ImageLoader() {}

ImageLoader::~ImageLoader() {
    clearCache();
}

ImageLoader& ImageLoader::GetInstance() {
    static ImageLoader instance;
    return instance;
}

void ImageLoader::setCacheSize(size_t maxMemory) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    maxMemorySize = maxMemory;

    while (!cacheList.empty() && currentMemorySize > maxMemorySize) {
        auto& item = cacheList.back();
        currentMemorySize -= item.size;
        cacheMap.erase(item.url);
        cacheList.pop_back();
    }
}

void ImageLoader::clearCache() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    cacheMap.clear();
    cacheList.clear();
    currentMemorySize = 0;
}

bool ImageLoader::getCachedImage(const std::string& url, ResultImageInfo& result) {
    std::lock_guard<std::mutex> lock(cacheMutex);

    auto it = cacheMap.find(url);
    if (it == cacheMap.end()) {
        return false;
    }

    auto listIt = it->second;
    CacheItem cacheItem = *listIt;
    cacheList.erase(listIt);
    cacheList.push_front(cacheItem);
    cacheMap[url] = cacheList.begin();

    result.url = cacheItem.url;
    result.result_DrawableDescriptor = cacheItem.drawable;
    result.width = cacheItem.width;
    result.height = cacheItem.height;

    return true;
}

void ImageLoader::cacheImage(const ResultImageInfo& result) {
    if (!result.result_DrawableDescriptor || !result.result_DrawableDescriptor->get()) {
        return;
    }
    size_t estimatedSize = result.width * result.height * 4 + 1024;

    CacheItem newItem = {
        .url = result.url,
        .drawable = result.result_DrawableDescriptor,
        .width = result.width,
        .height = result.height,
        .size = estimatedSize
    };

    std::lock_guard<std::mutex> lock(cacheMutex);
    removeFromCache(result.url);

    while (!cacheList.empty() && currentMemorySize + estimatedSize > maxMemorySize) {
        auto& item = cacheList.back();
        currentMemorySize -= item.size;
        cacheMap.erase(item.url);
        cacheList.pop_back();
    }

    cacheList.push_front(newItem);
    cacheMap[result.url] = cacheList.begin();
    currentMemorySize += estimatedSize;
}

void ImageLoader::removeFromCache(const std::string& url) {
    auto it = cacheMap.find(url);
    if (it != cacheMap.end()) {
        auto listIt = it->second;
        currentMemorySize -= listIt->size;
        cacheList.erase(listIt);
        cacheMap.erase(it);
    }
}

std::mutex& ImageLoader::getFileWriteMutex(const std::string& fileName) {
    std::lock_guard<std::mutex> lock(fileWriteMutexLock);
    if (fileWriteMutexes.find(fileName) == fileWriteMutexes.end()) {
        fileWriteMutexes[fileName] = std::make_unique<std::mutex>();
    }
    return *fileWriteMutexes[fileName];
}

void loadImage(const LoadRequestOptions& options, ImageRequestCallback&& onCallback) noexcept {
    if (options.url.empty()) {
        ErrorImageInfo errorInfo = {.url = options.url};
        onCallback(errorInfo);
        return;
    }

    auto& imageLoader = ImageLoader::GetInstance();
    ResultImageInfo cachedResult;
    if (imageLoader.getCachedImage(options.url, cachedResult)) {
        onCallback(cachedResult);
        return;
    }

    auto fileManagerInstance = FileManager::GetInstance();
    auto fileName = "/images/" + fileManagerInstance->GetFilename(options.url);
    auto filePath = fileManagerInstance->GetFilesDir() + fileName;

    if (!options.url.compare("http") || fileManagerInstance->PathExists(filePath)) {
        auto path = const_cast<char *>(filePath.c_str());
        auto loadRes = loadImageFromUri(options, path, filePath.length());

        if (std::holds_alternative<ResultImageInfo>(loadRes)) {
            imageLoader.cacheImage(std::get<ResultImageInfo>(loadRes));
        }

        onCallback(loadRes);
        return;
    }

    if (fileManagerInstance->PathExists(filePath)) {
        auto path = const_cast<char *>(filePath.c_str());
        auto loadRes = loadImageFromUri(options, path, filePath.length());

        if (std::holds_alternative<ResultImageInfo>(loadRes)) {
            imageLoader.cacheImage(std::get<ResultImageInfo>(loadRes));
        }

        onCallback(loadRes);
        return;
    }

    auto downloader = FileDownloader::GetInstance();
    auto fileDownloaderParam = FileDownloaderParam{.headers = {{"Content-Type", "application/octet-stream"}}};

    auto downloadCallback = [options, callback = std::move(onCallback), fileManagerInstance,
                             filePath = filePath, fileName = fileName, &imageLoader]
                            (const Rcp_Response* response, uint32_t errCode) {
        auto runner = TaroRuntime::Render::GetInstance()->GetTaskRunner();

        if (!response || errCode != 0) {
            ErrorImageInfo errorInfo = {.url = options.url, .errorCode = errCode};
            runner->runTask(TaroThread::TaskThread::MAIN, [errorInfo, callback = std::move(callback)] {
                callback(errorInfo);
            });
            return;
        }

        auto src = response->body.buffer;
        auto length = response->body.length;

        auto data = reinterpret_cast<uint8_t *>(const_cast<char *>(src));
        auto loadRes = loadImageFromData(options, data, length);
        runner->runTask(TaroThread::TaskThread::MAIN, [loadRes, callback = std::move(callback)] {
            callback(loadRes);
        });

        std::unique_lock<std::mutex> fileWriteLock(imageLoader.getFileWriteMutex(fileName));
        if (fileManagerInstance->WriteFileToFilesDir(fileName, src, length) != 0) {
            ErrorImageInfo errorInfo = {.url = options.url};
            fileWriteLock.unlock();

            runner->runTask(TaroThread::TaskThread::MAIN, [errorInfo, callback = std::move(callback)] {
                callback(errorInfo);
            });
            return;
        }
        fileWriteLock.unlock();

        if (std::holds_alternative<ResultImageInfo>(loadRes)) {
            imageLoader.cacheImage(std::get<ResultImageInfo>(loadRes));
        }
    };

    downloader->Download(options.url, fileDownloaderParam, downloadCallback);
}

static Image_ErrorCode getImageInfo(OH_ImageSourceNative* imageSrc, ResultImageInfo& result) {
    OH_ImageSource_Info* imageSourceInfo;
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

static Image_ErrorCode getPixelmapWithOneFrame(OH_ImageSourceNative* imageSrc, OH_PixelmapNative** pixelmapNative) {
    OH_DecodingOptions* decodingOpts;
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

static Image_ErrorCode getPixelmapListWithMultiFrame(OH_ImageSourceNative* imageSrc, std::vector<OH_PixelmapNative*>& pixelmaps, uint32_t frameCount) {
    OH_DecodingOptions* decodingOpts;
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

static std::variant<ResultImageInfo, ErrorImageInfo> loadImageFromData(const LoadRequestOptions& options, uint8_t *data, size_t dataSize) {
    ResultImageInfo result = {.url = options.url};
    OH_ImageSourceNative* imageSrc = nullptr;
    Image_ErrorCode errorCode;
    errorCode = OH_ImageSourceNative_CreateFromData(data, dataSize, &imageSrc);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceNative_CreateFromData failed. errorCode: %{public}d", errorCode);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }

    errorCode = getImageInfo(imageSrc, result);
    if (errorCode != IMAGE_SUCCESS) {
        OH_ImageSourceNative_Release(imageSrc);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }

    errorCode = getDrawableDescriptor(imageSrc, options, result);

    auto releaseCode = OH_ImageSourceNative_Release(imageSrc);
    if (errorCode != IMAGE_SUCCESS) {
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    if (releaseCode != IMAGE_SUCCESS) {
        return ErrorImageInfo{.url = options.url, .errorCode = releaseCode};
    }
    return result;
}

static std::variant<ResultImageInfo, ErrorImageInfo> loadImageFromUri(const LoadRequestOptions& options, char* path, size_t length) {
    ResultImageInfo result = {.url = options.url};
    OH_ImageSourceNative* imageSrc = nullptr;
    Image_ErrorCode errorCode;
    errorCode = OH_ImageSourceNative_CreateFromUri(path, length, &imageSrc);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceNative_CreateFromUri failed."
            "errorCode: %{public}d, path: %{public}s, length: %{public}d", errorCode, path, length);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }

    errorCode = getImageInfo(imageSrc, result);
    if (errorCode != IMAGE_SUCCESS) {
        OH_ImageSourceNative_Release(imageSrc);
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    errorCode = getDrawableDescriptor(imageSrc, options, result);
    auto releaseCode = OH_ImageSourceNative_Release(imageSrc);

    if (errorCode != IMAGE_SUCCESS) {
        return ErrorImageInfo{.url = options.url, .errorCode = errorCode};
    }
    if (releaseCode != IMAGE_SUCCESS) {
        return ErrorImageInfo{.url = options.url, .errorCode = releaseCode};
    }
    return result;
};

static Image_ErrorCode getDrawableDescriptor(OH_ImageSourceNative *source, const LoadRequestOptions& options, ResultImageInfo& result){
    uint32_t frameCount;
    auto errorCode = OH_ImageSourceNative_GetFrameCount(source, &frameCount);
    if (errorCode != IMAGE_SUCCESS) {
        TARO_LOG_ERROR("TaroImageLoader", "OH_ImageSourceNative_GetFrameCount failed. errorCode: %{public}d", errorCode);
        return errorCode;
    }
    if (frameCount == 0) {
        TARO_LOG_ERROR("TaroImageLoader", "got 'frameCount == 0'. errorCode: %{public}d", IMAGE_UNKNOWN_ERROR);
        return errorCode;
    } else if (frameCount == 1) {
        OH_PixelmapNative* pixelmapNative = nullptr;
        errorCode = getPixelmapWithOneFrame(source, &pixelmapNative);
        if (errorCode != IMAGE_SUCCESS) {
            TARO_LOG_ERROR("TaroImageLoader", "getPixelmapWitπhOneFrame failed, errorCode: %{public}d", errorCode);
            return errorCode;
        }
        auto image_drawabledescriptor = OH_ArkUI_DrawableDescriptor_CreateFromPixelMap(pixelmapNative);
        result.result_DrawableDescriptor = std::make_shared<ImagePixels>(image_drawabledescriptor);
        if (!options.keepPixelMap) {
            errorCode = OH_PixelmapNative_Release(pixelmapNative);
            if (errorCode != IMAGE_SUCCESS) {
                TARO_LOG_ERROR("TaroImageLoader", "OH_PixelmapNative_Release failed. errorCode: %{public}d", errorCode);
                return errorCode;
            }
        }
    } else {
        std::vector<OH_PixelmapNative*> pixelmaps(frameCount);
        errorCode = getPixelmapListWithMultiFrame(source, pixelmaps, frameCount);
        if (errorCode != IMAGE_SUCCESS) {
            TARO_LOG_ERROR("TaroImageLoader", "getPixelmapListWithMultiFrame failed, errorCode: %{public}d", errorCode);
            return errorCode;
        }
        auto image_drawabledescriptor = OH_ArkUI_DrawableDescriptor_CreateFromAnimatedPixelMap(pixelmaps.data(), frameCount);
        result.result_DrawableDescriptor = std::make_shared<ImagePixels>(image_drawabledescriptor);
        if (!options.keepPixelMap) {
            for (int index = 0; index < frameCount; index++) {
                errorCode = OH_PixelmapNative_Release(pixelmaps[index]);
                if (errorCode != IMAGE_SUCCESS) {
                    TARO_LOG_ERROR("TaroImageLoader", "OH_PixelmapNative_Release failed. errorCode: %{public}d", errorCode);
                    return errorCode;
                }
            }
        }
    }
    return IMAGE_SUCCESS;
}

} // namespace TaroHelper
