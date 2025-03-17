//
// Created on 2024/8/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "FileManager.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "helper/TaroLog.h"
#include "runtime/NapiGetter.h"

namespace TaroHelper {
FileManager* FileManager::instance = nullptr;
FileManager* FileManager::GetInstance() {
    if (instance == nullptr) {
        instance = new FileManager();
    }
    return instance;
}

FileManager::FileManager() {}

FileManager::~FileManager() {}

int32_t FileManager::Init(const napi_value& val) {
    auto napiFilesDir = TaroRuntime::NapiGetter::GetProperty(val, "filesDir").String();
    if (napiFilesDir.has_value()) {
        filesDir_ = napiFilesDir.value();
    }
    return 0;
}

std::string FileManager::GetFilesDir() {
    return filesDir_;
}

std::string FileManager::GetFilename(const std::string& file) {
    if (file.empty()) {
        return "";
    }
    std::filesystem::path filePath(file);
    return filePath.filename().string();
}

std::string FileManager::Dirname(const std::string& fullPath) {
    namespace fs = std::filesystem;
    fs::path filePath(fullPath);
    return filePath.parent_path().string();
}

bool FileManager::PathExists(const std::string& path) {
    return std::filesystem::exists(path);
}

int32_t FileManager::EnsureDir(const std::string& dirPath) {
    namespace fs = std::filesystem;
    try {
        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }
        return 0;
    } catch (const fs::filesystem_error& e) {
        TARO_LOG_ERROR("FileManager", "Error creating directory: %{public}s", e.what());
        return -1;
    }
}

int32_t FileManager::WriteFileToFilesDir(const std::string& filePath, const char* buffer, uint32_t length) {
    if (filesDir_.empty()) {
        TARO_LOG_ERROR("FileManager", "FileManager not inited correctly");
        return -1;
    }
    auto destPath = filesDir_ + filePath;
    if (EnsureDir(Dirname(destPath)) != 0) {
        return -1;
    }
    std::ofstream outFile(destPath, std::ios::binary);
    if (!outFile.is_open()) {
        TARO_LOG_ERROR("FileManager", "Unable to open file for writing");
        return -1;
    }

    outFile.write(buffer, length);
    if (!outFile) {
        TARO_LOG_ERROR("FileManager", "Error occurred while writing to file");
        outFile.close();
        return -1;
    }

    outFile.close();
    return 0;
}

int32_t FileManager::ReadFileFromFilesDir(const std::string& filePath, std::vector<char>& buffer) {
    if (filesDir_.empty()) {
        TARO_LOG_ERROR("FileManager", "FileManager not inited correctly");
        return -1;
    }

    auto destPath = filesDir_ + filePath;

    std::ifstream inFile(destPath, std::ios::binary);
    if (!inFile.is_open()) {
        TARO_LOG_ERROR("FileManager", "Unable to open file for reading");
        return -1;
    }

    inFile.seekg(0, std::ios::end);
    std::streamsize fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    if (fileSize <= 0) {
        TARO_LOG_ERROR("FileManager", "File is empty or error reading size");
        inFile.close();
        return -1;
    }

    buffer.resize(fileSize);
    if (!inFile.read(buffer.data(), fileSize)) {
        TARO_LOG_ERROR("FileManager", "Error occurred while reading from file");
        inFile.close();
        return -1;
    }

    inFile.close();
    return 0;
}

int32_t FileManager::RemoveFile(const std::string& filePath) {
    namespace fs = std::filesystem;
    try {
        if (fs::exists(filePath)) {
            fs::remove(filePath);
            return 0;
        }
        return -1;
    } catch (const fs::filesystem_error& e) {
        TARO_LOG_ERROR("FileManager", "Error removing file: %{public}s", e.what());
        return -1;
    }
}

int32_t FileManager::RemoveAllFilesInDir(const std::string& dirPath) {
    namespace fs = std::filesystem;
    try {
        if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
            for (const auto& entry : fs::directory_iterator(dirPath)) {
                if (fs::is_regular_file(entry.path())) {
                    fs::remove(entry.path());
                }
            }
            return 0;
        }
        return -1;
    } catch (const fs::filesystem_error& e) {
        TARO_LOG_ERROR("FileManager", "Error removing files: %{public}s", e.what());
        return -1;
    }
}

std::vector<std::string> FileManager::ListFilesInDir(const std::string& dirPath) {
    namespace fs = std::filesystem;
    std::vector<std::string> files;
    try {
        if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
            for (const auto& entry : fs::directory_iterator(dirPath)) {
                if (fs::is_regular_file(entry.path())) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        TARO_LOG_ERROR("FileManager", "Error listing files: %{public}s", e.what());
    }
    return files;
}
} // namespace TaroHelper