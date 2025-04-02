/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "FileDownloader.h"

#include "helper/TaroLog.h"

namespace TaroHelper {
FileDownloader* FileDownloader::instance = nullptr;
FileDownloader* FileDownloader::GetInstance() {
    if (instance == nullptr) {
        instance = new FileDownloader();
    }
    return instance;
}

FileDownloader::FileDownloader() {}

FileDownloader::~FileDownloader() {}

bool FileDownloader::Download(const std::string& url, const FileDownloaderParam& param, const std::function<void(const Rcp_Response*, uint32_t)>& callback) {
    std::lock_guard<std::mutex> lock(mutex);
    uint32_t errorCode;
    Rcp_Session* session = HMS_Rcp_CreateSession(nullptr, &errorCode);
    if (session == nullptr || errorCode != 0) {
        TARO_LOG_DEBUG("Download core", "Failed to initialize HTTP session.");
        return false;
    }

    Rcp_Request* request = HMS_Rcp_CreateRequest(url.c_str());
    if (!request) {
        HMS_Rcp_CancelSession(session);
        HMS_Rcp_CloseSession(&session);
        TARO_LOG_DEBUG("Download core", "Failed to create HTTP request.");
        return false;
    }

    request->method = RCP_METHOD_GET;
    Rcp_Headers* headers = HMS_Rcp_CreateHeaders();
    if (!headers) {
        TARO_LOG_DEBUG("Download core", "Failed to create headers.");
        HMS_Rcp_CancelSession(session);
        HMS_Rcp_CloseSession(&session);
        HMS_Rcp_DestroyRequest(request);
        return false;
    }
    if (param.headers.size() > 0) {
        for (const auto& pair : param.headers) {
            auto status = HMS_Rcp_SetHeaderValue(headers, pair.first.c_str(), pair.second.c_str());
            if (status != 0) {
                TARO_LOG_ERROR("Download core", "Set Header key %{public}s value %{public}s error %{public}d", pair.first.c_str(), pair.second.c_str(), status);
            }
        }
    }

    // 将头部添加到请求中
    request->headers = headers;
    Rcp_ResponseCallbackObject* responseCallback = new Rcp_ResponseCallbackObject;

    auto callbackWrapper = new std::function<void(Rcp_Response*, uint32_t)>([callback, request, session, responseCallback](Rcp_Response* response, uint32_t errCode) mutable {
        callback(response, errCode);
        HMS_Rcp_DestroyRequest(request);
        if (response) {
            response->destroyResponse(response);
        }
        HMS_Rcp_CancelSession(session);
        HMS_Rcp_CloseSession(&session);
        delete responseCallback;
    });
    *responseCallback = {
        [](void* context, Rcp_Response* response, uint32_t errCode) {
            auto userCallback = reinterpret_cast<std::function<void(const Rcp_Response*, uint32_t)>*>(context);
            (*userCallback)(response, errCode);
            delete userCallback;
        },
        callbackWrapper};

    auto result = HMS_Rcp_Fetch(session, request, responseCallback);
    if (result != 0) {
        HMS_Rcp_CancelSession(session);
        HMS_Rcp_CloseSession(&session);
        HMS_Rcp_DestroyRequest(request);
        delete responseCallback;
        delete callbackWrapper;
        TARO_LOG_DEBUG("Download core", "Failed to Send HTTP fetch.");
        return false;
    }
    return true;
}
} // namespace TaroHelper
