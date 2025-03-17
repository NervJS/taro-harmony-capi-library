//
// Created on 2024/8/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_HARMONY_CPP_FILEMANAGER_H
#define TARO_HARMONY_CPP_FILEMANAGER_H

#include <cstdint>
#include <string>
#include <js_native_api.h>

namespace TaroHelper {
class FileManager {
    private:
    static FileManager* instance;
    FileManager();
    ~FileManager();
    // 应用沙箱文件路径 <路径前缀>/<加密等级>/base/files
    std::string filesDir_ = "";

    public:
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

    static FileManager* GetInstance();

    int32_t Init(const napi_value&);
    std::string GetFilesDir();
    std::string GetFilename(const std::string& file);
    std::string Dirname(const std::string& fullPath);
    bool PathExists(const std::string& fullPath);
    int32_t EnsureDir(const std::string& dirPath);
    int32_t WriteFileToFilesDir(const std::string& filePath, const char* buffer, uint32_t length);
    int32_t ReadFileFromFilesDir(const std::string& filePath, std::vector<char>& buffer);
    int32_t RemoveFile(const std::string& filePath);
    int32_t RemoveAllFilesInDir(const std::string& dirPath);
    std::vector<std::string> ListFilesInDir(const std::string& dirPath);
};
} // namespace TaroHelper
#endif // TARO_HARMONY_CPP_FILEMANAGER_H
