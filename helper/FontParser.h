//
// Created on 2024/11/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TARO_HARMONY_CPP_FONTPARSER_H
#define TARO_HARMONY_CPP_FONTPARSER_H

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <regex>
#include "helper/TaroLog.h"

namespace TaroHelper {

struct FontInfo {
    std::string weight = "normal"; // 设置默认值
    int numericWeight = 400;
    int size = 16;
    std::string family = "Arial";
    bool isItalic = false;
};

class FontParser {
private:
    static const std::map<std::string, int> weightMap;
    static const int MIN_FONT_SIZE = 1;
    static const int MAX_FONT_SIZE = 1000;
    
    static bool isNumericWeight(const std::string& str) {
        if(str.empty()) return false;
        return std::all_of(str.begin(), str.end(), ::isdigit);
    }
    
    static bool endsWith(const std::string& str, const std::string& suffix) {
        if (str.empty() || suffix.empty()) return false;
        if (str.length() < suffix.length()) return false;
        return str.compare(str.length() - suffix.length(), 
                         suffix.length(), suffix) == 0;
    }
    
    static std::vector<std::string> splitString(const std::string& str) {
        std::vector<std::string> result;
        if(str.empty()) return result;
        
        std::string temp;
        bool inQuotes = false;
        
        for(char c : str) {
            if(c == '"') {
                inQuotes = !inQuotes;
            } else if(c == ' ' && !inQuotes) {
                if(!temp.empty()) {
                    result.push_back(temp);
                    temp.clear();
                }
            } else {
                temp += c;
            }
        }
        
        if(!temp.empty()) {
            result.push_back(temp);
        }
        
        return result;
    }

    static int parseSize(const std::string& sizeStr) {
        try {
            std::string numStr = sizeStr;
            if(endsWith(sizeStr, "px")) {
                numStr = sizeStr.substr(0, sizeStr.length() - 2);
            }
            
            int size = std::stoi(numStr);
            if(size < MIN_FONT_SIZE || size > MAX_FONT_SIZE) {
                TARO_LOG_WARN("FontParser", "Font size %{public}d out of range [%{public}d, %{public}d], using default",  size, MIN_FONT_SIZE, MAX_FONT_SIZE);
                return 16; // 返回默认值
            }
            return size;
        } catch(const std::exception& e) {
            TARO_LOG_ERROR("FontParser", "Failed to parse font size: %{public}s", e.what());
            return 16; // 返回默认值
        }
    }

public:
    static FontInfo parse(const std::string& fontStr) {
        FontInfo info;
        if(fontStr.empty()) {
            TARO_LOG_WARN("FontParser", "Empty font string, using defaults");
            return info;
        }

        try {
            auto parts = splitString(fontStr);
            if(parts.empty()) return info;
            
            size_t currentPart = 0;
            
            // 解析字型和字重
            while(currentPart < parts.size() - 2) {
                const std::string& part = parts[currentPart];
                
                if(part == "italic") {
                    info.isItalic = true;
                    currentPart++;
                    continue;
                }
                
                if(isNumericWeight(part)) {
                    try {
                        info.numericWeight = std::stoi(part);
                        info.weight = part;
                        currentPart++;
                    } catch(const std::exception& e) {
                        TARO_LOG_ERROR("FontParser", "Invalid numeric weight: %{public}s", e.what());
                    }
                    continue;
                }
                
                auto it = weightMap.find(part);
                if(it != weightMap.end()) {
                    info.weight = part;
                    info.numericWeight = it->second;
                    currentPart++;
                    continue;
                }
                
                break;
            }
            
            // 解析字号
            if(currentPart < parts.size()) {
                info.size = parseSize(parts[currentPart]);
                currentPart++;
            }
            
            // 解析字体名
            std::string family;
            while(currentPart < parts.size()) {
                if(!family.empty()) family += " ";
                family += parts[currentPart++];
            }
            
            if(!family.empty()) {
                info.family = family;
            }
            
        } catch(const std::exception& e) {
            TARO_LOG_ERROR("FontParser", "Font parsing failed: %{public}s", e.what());
        }
        
        return info;
    }
};

const std::map<std::string, int> FontParser::weightMap = {
    {"normal", 400},
    {"bold", 700},
    {"lighter", 300},
    {"bolder", 800}
};

}

#endif //TARO_HARMONY_CPP_FONTPARSER_H
