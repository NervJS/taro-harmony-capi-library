/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_CPP_FONTFAMILYMANAGER_H
#define TARO_HARMONY_CPP_FONTFAMILYMANAGER_H
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <native_drawing/drawing_text_declaration.h>

namespace TaroRuntime {
namespace TaroCSSOM {
    using FontsMap = std::unordered_map<std::string, std::string>;
    class FontFamilyManager {
        private:
        static FontFamilyManager* instance;
        FontFamilyManager();
        ~FontFamilyManager();
        FontsMap m_Fonts;
        OH_Drawing_FontCollection* m_FontCollection = nullptr;
        std::vector<std::string> m_CurrentRequestUrls;
        std::mutex m_CurrentRequestUrlsMutex;

        public:
        FontFamilyManager(const FontFamilyManager&) = delete;
        FontFamilyManager& operator=(const FontFamilyManager&) = delete;

        static FontFamilyManager* GetInstance();
        void SetFont(const std::string& fontName, const std::string& fontPath);
        const FontsMap& GetFonts();
        bool HasFont(const std::string& fontName);
        OH_Drawing_FontCollection* GetFontCollection();
        void DestroyFontCollection();
        void RegisterFonts();
        void AppendToCurrentRequestUrls(const std::string& url);
        bool IsInCurrentRequestUrls(const std::string& url);
        void DeleteUrlFromCurrentRequestUrls(const std::string& url);
    };
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_CPP_FONTFAMILYMANAGER_H
