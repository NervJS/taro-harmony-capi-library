/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <variant>
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

class ImagePixels {
    private:
    ArkUI_DrawableDescriptor* descriptor = nullptr;

    public:
    ImagePixels(ArkUI_DrawableDescriptor* p)
        : descriptor(p) {}
    ~ImagePixels() {
        if (descriptor) {
            OH_ArkUI_DrawableDescriptor_Dispose(descriptor);
        }
    }
    ArkUI_DrawableDescriptor* get() {
        return descriptor;
    }
};

// 定义结果图片信息结构体
struct ResultImageInfo {
    std::string url;                                        // 图片URL
    std::shared_ptr<ImagePixels> result_DrawableDescriptor; // 鸿蒙图片DrawableDescriptor
    uint32_t width = 0;                                     // 图片宽度,单位 px
    uint32_t height = 0;                                    // 图片高度,单位 px
};

// 定义异常图片信息结构体
struct ErrorImageInfo {
    std::string url;                // 图片URL
    bool isUnsupportedType = false; // 是否为不支持的格式
    uint32_t errorCode = -1;        // 错误码
};

// 定义缓存项结构
struct CacheItem {
    std::string url;
    std::shared_ptr<ImagePixels> drawable;
    uint32_t width;
    uint32_t height;
    size_t size; // 估计的内存大小（字节）
};

class ImageLoader {
    private:
    ImageLoader();
    ~ImageLoader();

    // 缓存相关
    size_t maxMemorySize = 100 * 1024 * 1024; // 默认最大内存占用（100MB）
    size_t currentMemorySize = 0;             // 当前内存占用

    std::list<CacheItem> cacheList;                                           // 双向链表，最近使用的在前面
    std::unordered_map<std::string, std::list<CacheItem>::iterator> cacheMap; // 快速查找缓存项

    mutable std::mutex cacheMutex; // 保证缓存操作的线程安全

    // 文件写入锁
    std::unordered_map<std::string, std::unique_ptr<std::mutex>> fileWriteMutexes;
    std::mutex fileWriteMutexLock;

    public:
    static ImageLoader& GetInstance();
    ImageLoader(const ImageLoader&) = delete;
    ImageLoader& operator=(const ImageLoader&) = delete;

    // 缓存
    void setCacheSize(size_t maxMemory = 100 * 1024 * 1024);
    void clearCache();
    bool getCachedImage(const std::string& url, ResultImageInfo& result);
    void cacheImage(const ResultImageInfo& result);
    void removeFromCache(const std::string& url);

    // 获取文件写入锁
    std::mutex& getFileWriteMutex(const std::string& fileName);
};

// 定义图片请求回调函数类型
using ImageRequestCallback = std::function<void(const std::variant<ResultImageInfo, ErrorImageInfo>)>;

void loadImage(const LoadRequestOptions& options, ImageRequestCallback&& onCallback) noexcept;
static Image_ErrorCode getImageInfo(OH_ImageSourceNative* imageSrc, ResultImageInfo& result);
static Image_ErrorCode getPixelmapWithOneFrame(OH_ImageSourceNative* imageSrc, OH_PixelmapNative** pixelmapNative);
static Image_ErrorCode getDrawableDescriptor(OH_ImageSourceNative* source, const LoadRequestOptions& options, ResultImageInfo& result);
static std::variant<ResultImageInfo, ErrorImageInfo> loadImageFromUri(const LoadRequestOptions& options, char* path, size_t length);
static std::variant<ResultImageInfo, ErrorImageInfo> loadImageFromData(const LoadRequestOptions& options, uint8_t* data, size_t dataSize);

} // namespace TaroHelper

#endif // IMAGELOADER_H
