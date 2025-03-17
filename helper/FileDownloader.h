//
// Created on 2024/8/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_HARMONY_CPP_FILEDOWNLOADER_H
#define TARO_HARMONY_CPP_FILEDOWNLOADER_H

#include <mutex>

#include "RemoteCommunicationKit/rcp.h"

namespace TaroHelper {
struct FileDownloaderParam {
    char* method = RCP_METHOD_GET;
    std::unordered_map<std::string, std::string> headers;
};
class FileDownloader {
    private:
    static FileDownloader* instance;
    FileDownloader();
    ~FileDownloader();

    std::mutex mutex;

    public:
    FileDownloader(const FileDownloader&) = delete;
    FileDownloader& operator=(const FileDownloader&) = delete;

    static FileDownloader* GetInstance();
    bool Download(const std::string& url, const FileDownloaderParam& param, const std::function<void(const Rcp_Response*, uint32_t)>& callback);
};
} // namespace TaroHelper

#endif // TARO_HARMONY_CPP_FILEDOWNLOADER_H
