/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_CPP_FILEDOWNLOADER_H
#define TARO_HARMONY_CPP_FILEDOWNLOADER_H

#include <string>

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

    public:
    FileDownloader(const FileDownloader&) = delete;
    FileDownloader& operator=(const FileDownloader&) = delete;

    static FileDownloader* GetInstance();
    bool Download(const std::string& url, const FileDownloaderParam& param, const std::function<void(const Rcp_Response*, uint32_t)>& callback);
};
} // namespace TaroHelper

#endif // TARO_HARMONY_CPP_FILEDOWNLOADER_H
