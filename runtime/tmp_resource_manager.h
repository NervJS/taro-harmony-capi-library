/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_CPP_TMP_RESOURCE_MANAGER_H
#define TARO_HARMONY_CPP_TMP_RESOURCE_MANAGER_H

#include <filesystem>
#include <string>
#include <multimedia/image_framework/image/pixelmap_native.h>

#include "helper/TaroLog.h"

namespace TaroRuntime {
namespace TaroDOM {
    // 临时文件路径
    //     const static std::string TMP_RESOURCE_PATH = "/data/storage/el2/base/files/";
    const static std::string TMP_CANVAS_RESOURCE_PATH = "/data/storage/el2/base/files/taro/canvas_tmp/";

    class TaroTmpResource {
        public:
        static TaroTmpResource* GetInstance() {
            static TaroTmpResource* instance = new TaroTmpResource();
            return instance;
        }

        std::unordered_map<std::string, OH_PixelmapNative*> tmp_pixels_manager_;

        // 检查并创建目录
        bool ensureDirectoryExists(std::string path) {
            if (!std::filesystem::exists(path)) {
                try {
                    // 创建目录
                    std::filesystem::create_directories(path);
                    return true;
                } catch (const std::filesystem::filesystem_error& e) {
                    TARO_LOG_ERROR("TaroTmpResource", "临时目录不存在：%{public}s", e.what());
                    return false;
                }
            }
            return true; // 目录已经存在
        }

        bool Clear(std::string path) {
            if (TMP_CANVAS_RESOURCE_PATH == path) {
                tmp_pixels_manager_.clear();
            }
            try {
                if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
                    return false;
                }
                // 遍历目录并删除所有内容
                for (const auto& entry : std::filesystem::directory_iterator(path)) {
                    std::filesystem::remove_all(entry.path()); // 删除文件或目录
                }
                return true;
            } catch (const std::filesystem::filesystem_error& e) {
                return false;
            }
        }

        void ReleasePixels(std::string path = "") {
            if (path.size()) {
                if (auto pixels = tmp_pixels_manager_[path]) {
                    OH_PixelmapNative_Release(pixels);
                    tmp_pixels_manager_.erase(path);
                }
            } else {
                for (auto it : tmp_pixels_manager_) {
                    OH_PixelmapNative_Release(it.second);
                }
            }
        }
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // TARO_HARMONY_CPP_TMP_RESOURCE_MANAGER_H
