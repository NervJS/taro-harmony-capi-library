//
// Created on 2024/4/20.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".
#include "StringUtils.h"

#include <sstream>
#include <string>
#include <vector>
#include <bits/alltypes.h>

#include "helper/TaroLog.h"

namespace TaroHelper {
static const const std::unordered_map<std::string, uint32_t> TARO_COLOR_MAPPING = {
    {"aliceblue", 0xFFF0F8FF},
    {"antiquewhite", 0xFFFAEBD7},
    {"aqua", 0xFF00FFFF},
    {"aquamarine", 0xFF7FFFD4},
    {"azure", 0xFFF0FFFF},
    {"beige", 0xFFF5F5DC},
    {"bisque", 0xFFFFE4C4},
    {"black", 0xFF000000},
    {"blanchedalmond", 0xFFFFEBCD},
    {"blue", 0xFF0000FF},
    {"blueviolet", 0xFF8A2BE2},
    {"brown", 0xFFA52A2A},
    {"burlywood", 0xFFDEB887},
    {"cadetblue", 0xFF5F9EA0},
    {"chartreuse", 0xFF7FFF00},
    {"chocolate", 0xFFD2691E},
    {"coral", 0xFFFF7F50},
    {"cornflowerblue", 0xFF6495ED},
    {"cornsilk", 0xFFFFF8DC},
    {"crimson", 0xFFDC143C},
    {"cyan", 0xFF00FFFF},
    {"darkblue", 0xFF00008B},
    {"darkcyan", 0xFF008B8B},
    {"darkgoldenrod", 0xFFB8860B},
    {"darkgray", 0xFFA9A9A9},
    {"darkgreen", 0xFF006400},
    {"darkgrey", 0xFFA9A9A9},
    {"darkkhaki", 0xFFBDB76B},
    {"darkmagenta", 0xFF8B008B},
    {"darkolivegreen", 0xFF556B2F},
    {"darkorange", 0xFFFF8C00},
    {"darkorchid", 0xFF9932CC},
    {"darkred", 0xFF8B0000},
    {"darksalmon", 0xFFE9967A},
    {"darkseagreen", 0xFF8FBC8F},
    {"darkslateblue", 0xFF483D8B},
    {"darkslategray", 0xFF2F4F4F},
    {"darkslategrey", 0xFF2F4F4F},
    {"darkturquoise", 0xFF00CED1},
    {"darkviolet", 0xFF9400D3},
    {"deeppink", 0xFFFF1493},
    {"deepskyblue", 0xFF00BFFF},
    {"dimgray", 0xFF696969},
    {"dimgrey", 0xFF696969},
    {"dodgerblue", 0xFF1E90FF},
    {"firebrick", 0xFFB22222},
    {"floralwhite", 0xFFFFFAF0},
    {"forestgreen", 0xFF228B22},
    {"fuchsia", 0xFFFF00FF},
    {"gainsboro", 0xFFDCDCDC},
    {"ghostwhite", 0xFFF8F8FF},
    {"gold", 0xFFFFD700},
    {"goldenrod", 0xFFDAA520},
    {"gray", 0xFF808080},
    {"green", 0xFF008000},
    {"greenyellow", 0xFFADFF2F},
    {"grey", 0xFF808080},
    {"honeydew", 0xFFF0FFF0},
    {"hotpink", 0xFFFF69B4},
    {"indianred", 0xFFCD5C5C},
    {"indigo", 0xFF4B0082},
    {"ivory", 0xFFFFFFF0},
    {"khaki", 0xFFF0E68C},
    {"lavender", 0xFFE6E6FA},
    {"lavenderblush", 0xFFFFF0F5},
    {"lawngreen", 0xFF7CFC00},
    {"lemonchiffon", 0xFFFFFACD},
    {"lightblue", 0xFFADD8E6},
    {"lightcoral", 0xFFF08080},
    {"lightcyan", 0xFFE0FFFF},
    {"lightgoldenrodyellow", 0xFFFAFAD2},
    {"lightgray", 0xFFD3D3D3},
    {"lightgreen", 0xFF90EE90},
    {"lightgrey", 0xFFD3D3D3},
    {"lightpink", 0xFFFFB6C1},
    {"lightsalmon", 0xFFFFA07A},
    {"lightseagreen", 0xFF20B2AA},
    {"lightskyblue", 0xFF87CEFA},
    {"lightslategray", 0xFF778899},
    {"lightslategrey", 0xFF778899},
    {"lightsteelblue", 0xFFB0C4DE},
    {"lightyellow", 0xFFFFFFE0},
    {"lime", 0xFF00FF00},
    {"limegreen", 0xFF32CD32},
    {"linen", 0xFFFAF0E6},
    {"magenta", 0xFFFF00FF},
    {"maroon", 0xFF800000},
    {"mediumaquamarine", 0xFF66CDAA},
    {"mediumblue", 0xFF0000CD},
    {"mediumorchid", 0xFFBA55D3},
    {"mediumpurple", 0xFF9370DB},
    {"mediumseagreen", 0xFF3CB371},
    {"mediumslateblue", 0xFF7B68EE},
    {"mediumspringgreen", 0xFF00FA9A},
    {"mediumturquoise", 0xFF48D1CC},
    {"mediumvioletred", 0xFFC71585},
    {"midnightblue", 0xFF191970},
    {"mintcream", 0xFFF5FFFA},
    {"mistyrose", 0xFFFFE4E1},
    {"moccasin", 0xFFFFE4B5},
    {"navajowhite", 0xFFFFDEAD},
    {"navy", 0xFF000080},
    {"oldlace", 0xFFFDF5E6},
    {"olive", 0xFF808000},
    {"olivedrab", 0xFF6B8E23},
    {"orange", 0xFFFFA500},
    {"orangered", 0xFFFF4500},
    {"orchid", 0xFFDA70D6},
    {"palegoldenrod", 0xFFEEE8AA},
    {"palegreen", 0xFF98FB98},
    {"paleturquoise", 0xFFAFEEEE},
    {"palevioletred", 0xFFDB7093},
    {"papayawhip", 0xFFFFEFD5},
    {"peachpuff", 0xFFFFDAB9},
    {"peru", 0xFFCD853F},
    {"pink", 0xFFFFC0CB},
    {"plum", 0xFFDDA0DD},
    {"powderblue", 0xFFB0E0E6},
    {"purple", 0xFF800080},
    {"rebeccapurple", 0xFF663399},
    {"red", 0xFFFF0000},
    {"rosybrown", 0xFFBC8F8F},
    {"royalblue", 0xFF4169E1},
    {"saddlebrown", 0xFF8B4513},
    {"salmon", 0xFFFA8072},
    {"sandybrown", 0xFFF4A460},
    {"seagreen", 0xFF2E8B57},
    {"seashell", 0xFFFFF5EE},
    {"sienna", 0xFFA0522D},
    {"silver", 0xFFC0C0C0},
    {"skyblue", 0xFF87CEEB},
    {"slateblue", 0xFF6A5ACD},
    {"slategray", 0xFF708090},
    {"slategrey", 0xFF708090},
    {"snow", 0xFFFFFAFA},
    {"springgreen", 0xFF00FF7F},
    {"steelblue", 0xFF4682B4},
    {"tan", 0xFFD2B48C},
    {"teal", 0xFF008080},
    {"thistle", 0xFFD8BFD8},
    {"tomato", 0xFFFF6347},
    {"turquoise", 0xFF40E0D0},
    {"violet", 0xFFEE82EE},
    {"wheat", 0xFFF5DEB3},
    {"white", 0xFFFFFFFF},
    {"whitesmoke", 0xFFF5F5F5},
    {"yellow", 0xFFFFFF00},
    {"yellowgreen", 0xFF9ACD32}};

// 解析十六进制字符为整数
static int hexCharToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    return 0;
}

std::vector<std::string> StringUtils::split(
    const std::string &str, const std::string &delimiter) {
    std::vector<std::string> tokens;
    split(tokens, str, delimiter);
    return tokens;
}

void StringUtils::split(std::vector<std::string> &tokens,
                        const std::string &str, const std::string &delimiter) {
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        // Add the substring from 'start' to 'end' (not including 'end')
        tokens.push_back(str.substr(start, end - start));
        // Move 'start' position past the delimiter
        start = end + delimiter.length();
        // Find next occurrence of delimiter
        end = str.find(delimiter, start);
    }

    // Add the last token
    tokens.push_back(str.substr(start, end - start));
}

std::vector<std::string> StringUtils::splitWithEscape(
    const std::string &str, const std::string &delimiter, const char &escape) {
    std::vector<std::string> tokens;
    splitWithEscape(tokens, str, delimiter, escape);
    return tokens;
}

void StringUtils::splitWithEscape(std::vector<std::string> &tokens,
                                  const std::string &str, const std::string &delimiter, const char &escape) {
    size_t start = 0;
    size_t end = str.find(delimiter);
    std::string token;

    while (end != std::string::npos) {
        // 判断 end 是否为转义字符
        if (end > 0 && str[end - 1] == escape) {
            token += str.substr(start, end - start - 1) + delimiter;
        } else {
            token += str.substr(start, end - start);
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }

        // Move 'start' position past the delimiter
        start = end + delimiter.length();
        // Find next occurrence of delimiter
        end = str.find(delimiter, start);
    }

    token += str.substr(start, end - start);
    // Add the last token
    if (!token.empty()) {
        tokens.push_back(token);
    }
}

std::string StringUtils::join(const std::vector<std::string> &tokens,
                              const std::string &delimiter) {
    std::string result;
    join(tokens, delimiter, result);
    return result;
}

void StringUtils::join(const std::vector<std::string> &tokens,
                       const std::string &delimiter, std::string &result) {
    std::ostringstream oss;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i > 0) {
            oss << delimiter;
        }
        oss << tokens[i];
    }
    result = oss.str();
}

std::string StringUtils::toUpperCase(const std::string &str) {
    std::string result(str.size(), '\0');
    std::transform(str.begin(), str.end(), result.begin(), ::toupper);
    return result;
}

std::string StringUtils::toLowerCase(const std::string &str) {
    std::string result(str.size(), '\0');
    std::transform(str.begin(), str.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtils::toCamelCase(const std::string &str) {
    std::string result;
    bool capitalize = false;
    for (char c : str) {
        if (c != '-') {
            result += capitalize ? std::toupper(c) : c;
            capitalize = false;
        } else {
            capitalize = true;
        }
    }
    return result;
}

// 将 #RGB 或 #RRGGBB 转换为十六进制字符串
uint32_t StringUtils::parseHexColor(const std::string &hex) {
    int r = 0, g = 0, b = 0;
    if (hex.size() == 4) { // #RGB
        r = hexCharToInt(hex[1]);
        g = hexCharToInt(hex[2]);
        b = hexCharToInt(hex[3]);
        r = (r << 4) | r;
        g = (g << 4) | g;
        b = (b << 4) | b;
    } else if (hex.size() == 7) { // #RRGGBB
        r = (hexCharToInt(hex[1]) << 4) | hexCharToInt(hex[2]);
        g = (hexCharToInt(hex[3]) << 4) | hexCharToInt(hex[4]);
        b = (hexCharToInt(hex[5]) << 4) | hexCharToInt(hex[6]);
    } else {
        throw std::invalid_argument("Invalid color format");
    }

    return (0xFF << 24) | (r << 16) | (g << 8) | b; // Alpha 默认为 0xFF
}

// 将 rgba(r, g, b, a) 转换为十六进制字符串
uint32_t StringUtils::parseRgbaColor(const std::string &color) {
    int r = 0, g = 0, b = 0;
    float a = 1.0; // 默认 alpha 值为 1.0 (完全不透明)
    unsigned int alpha;

    if (color.find("rgba") != std::string::npos) {
        // 处理 rgba 格式
        sscanf(color.c_str(), "rgba(%d, %d, %d, %f)", &r, &g, &b, &a);
    } else if (color.find("rgb") != std::string::npos) {
        // 处理 rgb 格式
        sscanf(color.c_str(), "rgb(%d, %d, %d)", &r, &g, &b);
    } else {
        throw std::invalid_argument("Invalid color format");
    }

    // 将浮点数 a 转换为 0-255 范围的整数
    alpha = static_cast<unsigned int>(a * 255.0f + 0.5f); // 加0.5用于四舍五入

    // 将 rgba 转换为一个 16 进制的整数
    return (alpha << 24) | (r << 16) | (g << 8) | b;
}

std::optional<uint32_t> StringUtils::parseColor(
    const std::string &colorString) {
    std::optional<uint32_t> val;
    if (colorString[0] == '#') {
        val = parseHexColor(colorString);
    } else if (colorString.find("rgb") != std::string::npos) {
        val = parseRgbaColor(colorString);
    } else {
        auto it = TARO_COLOR_MAPPING.find(colorString);
        if (it != TARO_COLOR_MAPPING.end()) {
            val = it->second;
        }
    }
    return val; // 默认返回值，也可视为错误处理
}

double StringUtils::percentageToDecimal(const std::string &str) {
    double percentage = 0.0;
    size_t len = str.size();

    for (size_t i = 0; i < len - 1; ++i) {
        if (!isdigit(str[i])) {
            return -1.0;
        }
        percentage = percentage * 10 + (str[i] - '0');
    }

    if (str[len - 1] != '%') {
        return -1.0;
    }

    return percentage;
}

float StringUtils::parseSize(const std::string &sourceStr) {
    // 去除字符串前后的空格
    std::string str = sourceStr;
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);

    // 判断字符串是否为空
    if (str.empty()) {
        return 0.0f;
    }

    // 判断字符串是否以数字开头
    if (!isdigit(str[0]) && str[0] != '.' && str[0] != '-' && str[0] != '+') {
        return 0.0f;
    }

    // 查找单位字符的位置
    size_t unitPos = str.find_first_not_of("0123456789.-+");

    // 提取数值部分
    std::string numStr = str.substr(0, unitPos);

    // 将数值字符串转换为浮点数
    float value = std::stof(numStr);

    return value;
}

bool StringUtils::isStartWith(const std::string &str, const std::string &prefix,
                              const int32_t prefixLen) {
    return str.compare(0, prefixLen, prefix) == 0;
}

double percentageToDecimal(const std::string &str) {
    double percentage = 0.0;
    if (str.empty()) {
        return -1.0;
    }

    size_t len = str.size();

    std::string numberStr = str.substr(0, len - 1);
    try {
        percentage = std::stod(numberStr);
        return percentage / 100.0;
    } catch (...) {
        return -1.0;
    }
}
} // namespace TaroHelper
