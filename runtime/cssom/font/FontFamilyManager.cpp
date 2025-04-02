/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "FontFamilyManager.h"

#include <cstdint>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_register_font.h>

#include "helper/TaroLog.h"
namespace TaroRuntime {
namespace TaroCSSOM {
    FontFamilyManager* FontFamilyManager::instance = nullptr;
    FontFamilyManager* FontFamilyManager::GetInstance() {
        if (instance == nullptr) {
            instance = new FontFamilyManager();
        }
        return instance;
    }

    FontFamilyManager::FontFamilyManager() {
        m_FontCollection = OH_Drawing_CreateSharedFontCollection();
    }

    FontFamilyManager::~FontFamilyManager() {
        DestroyFontCollection();
    }

    void FontFamilyManager::SetFont(const std::string& fontName, const std::string& fontPath) {
        int ret = OH_Drawing_RegisterFont(m_FontCollection, fontName.c_str(), fontPath.c_str());
        if (ret != 0) {
            // 注册字体失败
            TARO_LOG_ERROR("FontFamilyManager", "Regist Font %{public}s Path %{public}s error %{public}d", fontName.c_str(), fontPath.c_str(), ret);
        } else {
            m_Fonts.insert_or_assign(fontName, fontPath);
        }
    }

    const FontsMap& FontFamilyManager::GetFonts() {
        return m_Fonts;
    }

    bool FontFamilyManager::HasFont(const std::string& fontName) {
        return m_Fonts.find(fontName) != m_Fonts.end();
    }

    OH_Drawing_FontCollection* FontFamilyManager::GetFontCollection() {
        return m_FontCollection;
    }

    void FontFamilyManager::DestroyFontCollection() {
        if (m_FontCollection != nullptr) {
            OH_Drawing_DestroyFontCollection(m_FontCollection);
            m_FontCollection = nullptr;
        }
    }

    void FontFamilyManager::RegisterFonts() {
        if (m_Fonts.size() > 0) {
            for (const auto& pair : m_Fonts) {
                int ret = OH_Drawing_RegisterFont(m_FontCollection, pair.first.c_str(), pair.second.c_str());
                if (ret != 0) {
                    // 注册字体失败
                    TARO_LOG_ERROR("FontFamilyManager", "Regist Font %{public}s Path %{public}s error %{public}d", pair.first.c_str(), pair.second.c_str(), ret);
                }
            }
        }
    }

    void FontFamilyManager::AppendToCurrentRequestUrls(const std::string& url) {
        std::lock_guard<std::mutex> lock(m_CurrentRequestUrlsMutex);
        m_CurrentRequestUrls.push_back(url);
    }

    bool FontFamilyManager::IsInCurrentRequestUrls(const std::string& url) {
        std::lock_guard<std::mutex> lock(m_CurrentRequestUrlsMutex);
        return std::find(m_CurrentRequestUrls.begin(), m_CurrentRequestUrls.end(), url) != m_CurrentRequestUrls.end();
    }

    void FontFamilyManager::DeleteUrlFromCurrentRequestUrls(const std::string& url) {
        std::lock_guard<std::mutex> lock(m_CurrentRequestUrlsMutex);
        auto it = std::find(m_CurrentRequestUrls.begin(), m_CurrentRequestUrls.end(), url);
        if (it != m_CurrentRequestUrls.end()) {
            m_CurrentRequestUrls.erase(it);
        }
    }
} // namespace TaroCSSOM
} // namespace TaroRuntime
