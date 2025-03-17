//
// Created on 2024/8/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "CSSFont.h"

#include <cstdint>
#include <functional>

#include "helper/FileDownloader.h"
#include "helper/FileManager.h"
#include "runtime/NapiGetter.h"
#include "runtime/cssom/font/FontFamilyManager.h"
#include "runtime/render.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    void DownloadCallback(const std::string& fontFamilyValue, const std::string& src, const Rcp_Response* response, uint32_t errCode) {
        auto fontFamilyManager = FontFamilyManager::GetInstance();
        if (!response) {
            fontFamilyManager->DeleteUrlFromCurrentRequestUrls(src);
            return;
        }
        if (errCode == 0) {
            auto fileManagerInstance = FileManager::GetInstance();
            auto fileName = "/fonts/" + fileManagerInstance->GetFilename(src);
            auto filePath = fileManagerInstance->GetFilesDir() + fileName;
            int32_t ret = fileManagerInstance->WriteFileToFilesDir(fileName, response->body.buffer, response->body.length);
            fontFamilyManager->DeleteUrlFromCurrentRequestUrls(src);
            if (ret != 0) {
                return;
            }
            std::shared_ptr<TaroThread::TaskExecutor> runner = Render::GetInstance()->GetTaskRunner();
            runner->runTask(TaroThread::TaskThread::MAIN, [filePath, fontFamilyValue]() {
                if (FileManager::GetInstance()->PathExists(filePath)) {
                    FontFamilyManager::GetInstance()->SetFont(fontFamilyValue, filePath);
                }
            });
        } else {
            fontFamilyManager->DeleteUrlFromCurrentRequestUrls(src);
        }
    }
    int CSSFont::init(const napi_value& config) {
        auto fileDownloaderParam = FileDownloaderParam{.headers = {{"Content-Type", "application/octet-stream"}}};
        NapiGetter::ForEachInArray(config, [&](const napi_value& napi_val, const uint32_t&) {
            auto fontFamilyManager = FontFamilyManager::GetInstance();
            auto fontFamily = NapiGetter::GetProperty(napi_val, "fontFamily").String();
            if (fontFamily.has_value()) {
                auto src = NapiGetter::GetProperty(napi_val, "src").String();
                if (src.has_value()) {
                    auto fontFamilyValue = fontFamily.value();
                    auto fileManagerInstance = FileManager::GetInstance();
                    // 如果没有注册该字体，可以从本地获取，如果本地没有则从网络获取
                    if (!fontFamilyManager->HasFont(fontFamilyValue)) {
                        if (fileManagerInstance->GetFilesDir().empty()) {
                            return;
                        }
                        auto fontSrc = src.value();
                        auto fontFileName = "/fonts/" + fileManagerInstance->GetFilename(fontSrc);
                        auto fontFilePath = fileManagerInstance->GetFilesDir() + fontFileName;
                        if (fileManagerInstance->PathExists(fontFilePath)) {
                            fontFamilyManager->SetFont(fontFamilyValue, fontFilePath);
                        } else {
                            // 没有下载过的字体需要进行字体下载
                            auto fileDownloader = FileDownloader::GetInstance();
                            auto srcVal = src.value();
                            if (!fontFamilyManager->IsInCurrentRequestUrls(srcVal)) {
                                fontFamilyManager->AppendToCurrentRequestUrls(srcVal);
                                auto callback = std::bind(DownloadCallback, fontFamilyValue, srcVal, std::placeholders::_1, std::placeholders::_2);
                                auto isSuccess = fileDownloader->Download(srcVal, fileDownloaderParam, callback);
                                if (!isSuccess) {
                                    fontFamilyManager->DeleteUrlFromCurrentRequestUrls(srcVal);
                                }
                            }
                        }
                    }
                }
            }
        });
        return 0;
    }
} // namespace TaroCSSOM
} // namespace TaroRuntime