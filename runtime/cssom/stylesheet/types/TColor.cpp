//
// Created on 2024/6/22.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "TColor.h"

#include <string>

#include "helper/TaroLog.h"
#include "helper/string.h"
#include "runtime/NapiGetter.h"

/**
 * 解析十六进制字符为整数
 */
static int hexCharToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    return 0;
}

/**
 * 将颜色名转换为数值 :
 * https://developer.mozilla.org/zh-CN/docs/Web/CSS/named-color
 */
static Optional<uint32_t> hex2Int(std::string_view hex) {
    int r = 0, g = 0, b = 0, a = 0xff;
    auto size = hex.size();
    if (size == 4 || size == 5) { // #RGB
        r = hexCharToInt(hex[1]);
        g = hexCharToInt(hex[2]);
        b = hexCharToInt(hex[3]);
        r = (r << 4) | r;
        g = (g << 4) | g;
        b = (b << 4) | b;
    } else if (size == 7 || size == 9) { // #RRGGBB
        r = (hexCharToInt(hex[1]) << 4) | hexCharToInt(hex[2]);
        g = (hexCharToInt(hex[3]) << 4) | hexCharToInt(hex[4]);
        b = (hexCharToInt(hex[5]) << 4) | hexCharToInt(hex[6]);
    } else {
        Optional<uint32_t> res;
        return res;
    }

    if (size == 5) { // #RGBA
        a = hexCharToInt(hex[4]);
        a = (a << 4) | a;
    } else if (size == 9) { //  #RRGGBBAA
        a = (hexCharToInt(hex[7]) << 4) | hexCharToInt(hex[8]);
    }

    // 将 rgba 转换为一个 16 进制的整数
    return (a << 24) | (r << 16) | (g << 8) | b;
}

static std::unordered_map<std::string_view, uint32_t> namedColor = {
    {"black", 0xff000000},
    {"silver", 0xffc0c0c0},
    {"gray", 0xff808080},
    {"white", 0xffffffff},
    {"maroon", 0xff800000},
    {"red", 0xffff0000},
    {"purple", 0xff800080},
    {"fuchsia", 0xffff00ff},
    {"green", 0xff008000},
    {"lime", 0xff00ff00},
    {"olive", 0xff808000},
    {"yellow", 0xffffff00},
    {"navy", 0xff000080},
    {"blue", 0xff0000ff},
    {"teal", 0xff008080},
    {"aqua", 0xff00ffff},

    {"aliceblue", 0xfff0f8ff},
    {"antiquewhite", 0xfffaebd7},
    {"aqua", 0xff00ffff},
    {"aquamarine", 0xff7fffd4},
    {"azure", 0xfff0ffff},
    {"beige", 0xfff5f5dc},
    {"bisque", 0xffffe4c4},
    {"black", 0xff000000},
    {"blanchedalmond", 0xffffebcd},
    {"blue", 0xff0000ff},
    {"blueviolet", 0xff8a2be2},
    {"brown", 0xffa52a2a},
    {"burlywood", 0xffdeb887},
    {"cadetblue", 0xff5f9ea0},
    {"chartreuse", 0xff7fff00},
    {"chocolate", 0xffd2691e},
    {"coral", 0xffff7f50},
    {"cornflowerblue", 0xff6495ed},
    {"cornsilk", 0xfffff8dc},
    {"crimson", 0xffdc143c},
    {"cyan", 0xff00ffff},
    {"darkblue", 0xff00008b},
    {"darkcyan", 0xff008b8b},
    {"darkgoldenrod", 0xffb8860b},
    {"darkgray", 0xffa9a9a9},
    {"darkgreen", 0xff006400},
    {"darkgrey", 0xffa9a9a9},
    {"darkkhaki", 0xffbdb76b},
    {"darkmagenta", 0xff8b008b},
    {"darkolivegreen", 0xff556b2f},
    {"darkorange", 0xffff8c00},
    {"darkorchid", 0xff9932cc},
    {"darkred", 0xff8b0000},
    {"darksalmon", 0xffe9967a},
    {"darkseagreen", 0xff8fbc8f},
    {"darkslateblue", 0xff483d8b},
    {"darkslategray", 0xff2f4f4f},
    {"darkslategrey", 0xff2f4f4f},
    {"darkturquoise", 0xff00ced1},
    {"darkviolet", 0xff9400d3},
    {"deeppink", 0xffff1493},
    {"deepskyblue", 0xff00bfff},
    {"dimgray", 0xff696969},
    {"dimgrey", 0xff696969},
    {"dodgerblue", 0xff1e90ff},
    {"firebrick", 0xffb22222},
    {"floralwhite", 0xfffffaf0},
    {"forestgreen", 0xff228b22},
    {"fuchsia", 0xffff00ff},
    {"gainsboro", 0xffdcdcdc},
    {"ghostwhite", 0xfff8f8ff},
    {"gold", 0xffffd700},
    {"goldenrod", 0xffdaa520},
    {"gray", 0xff808080},
    {"green", 0xff008000},
    {"greenyellow", 0xffadff2f},
    {"grey", 0xff808080},
    {"honeydew", 0xfff0fff0},
    {"hotpink", 0xffff69b4},
    {"indianred", 0xffcd5c5c},
    {"indigo", 0xff4b0082},
    {"ivory", 0xfffffff0},
    {"khaki", 0xfff0e68c},
    {"lavender", 0xffe6e6fa},
    {"lavenderblush", 0xfffff0f5},
    {"lawngreen", 0xff7cfc00},
    {"lemonchiffon", 0xfffffacd},
    {"lightblue", 0xffadd8e6},
    {"lightcoral", 0xfff08080},
    {"lightcyan", 0xffe0ffff},
    {"lightgoldenrodyellow", 0xfffafad2},
    {"lightgray", 0xffd3d3d3},
    {"lightgreen", 0xff90ee90},
    {"lightgrey", 0xffd3d3d3},
    {"lightpink", 0xffffb6c1},
    {"lightsalmon", 0xffffa07a},
    {"lightseagreen", 0xff20b2aa},
    {"lightskyblue", 0xff87cefa},
    {"lightslategray", 0xff778899},
    {"lightslategrey", 0xff778899},
    {"lightsteelblue", 0xffb0c4de},
    {"lightyellow", 0xffffffe0},
    {"lime", 0xff00ff00},
    {"limegreen", 0xff32cd32},
    {"linen", 0xfffaf0e6},
    {"magenta", 0xffff00ff},
    {"maroon", 0xff800000},
    {"mediumaquamarine", 0xff66cdaa},
    {"mediumblue", 0xff0000cd},
    {"mediumorchid", 0xffba55d3},
    {"mediumpurple", 0xff9370db},
    {"mediumseagreen", 0xff3cb371},
    {"mediumslateblue", 0xff7b68ee},
    {"mediumspringgreen", 0xff00fa9a},
    {"mediumturquoise", 0xff48d1cc},
    {"mediumvioletred", 0xffc71585},
    {"midnightblue", 0xff191970},
    {"mintcream", 0xfff5fffa},
    {"mistyrose", 0xffffe4e1},
    {"moccasin", 0xffffe4b5},
    {"navajowhite", 0xffffdead},
    {"navy", 0xff000080},
    {"oldlace", 0xfffdf5e6},
    {"olive", 0xff808000},
    {"olivedrab", 0xff6b8e23},
    {"orange", 0xffffa500},
    {"orangered", 0xffff4500},
    {"orchid", 0xffda70d6},
    {"palegoldenrod", 0xffeee8aa},
    {"palegreen", 0xff98fb98},
    {"paleturquoise", 0xffafeeee},
    {"palevioletred", 0xffdb7093},
    {"papayawhip", 0xffffefd5},
    {"peachpuff", 0xffffdab9},
    {"peru", 0xffcd853f},
    {"pink", 0xffffc0cb},
    {"plum", 0xffdda0dd},
    {"powderblue", 0xffb0e0e6},
    {"purple", 0xff800080},
    {"rebeccapurple", 0xff663399},
    {"red", 0xffff0000},
    {"rosybrown", 0xffbc8f8f},
    {"royalblue", 0xff4169e1},
    {"saddlebrown", 0xff8b4513},
    {"salmon", 0xfffa8072},
    {"sandybrown", 0xfff4a460},
    {"seagreen", 0xff2e8b57},
    {"seashell", 0xfffff5ee},
    {"sienna", 0xffa0522d},
    {"silver", 0xffc0c0c0},
    {"skyblue", 0xff87ceeb},
    {"slateblue", 0xff6a5acd},
    {"slategray", 0xff708090},
    {"slategrey", 0xff708090},
    {"snow", 0xfffffafa},
    {"springgreen", 0xff00ff7f},
    {"steelblue", 0xff4682b4},
    {"tan", 0xffd2b48c},
    {"teal", 0xff008080},
    {"thistle", 0xffd8bfd8},
    {"tomato", 0xffff6347},
    {"transparent", 0x00000000},
    {"turquoise", 0xff40e0d0},
    {"violet", 0xffee82ee},
    {"wheat", 0xfff5deb3},
    {"white", 0xffffffff},
    {"whitesmoke", 0xfff5f5f5},
    {"yellow", 0xffffff00},
    {"yellowgreen", 0xff9acd32}};

/**
 * 将 rgba(r, g, b, a) 转换为数值
 * TODO 还能进一步优化
 */
static Optional<uint32_t> rgba2Int(std::string_view color) {
    int r = 0, g = 0, b = 0;
    float a = 1.0f; // 默认 alpha 值为 1.0 (完全不透明)
    unsigned int alpha;
    Optional<uint32_t> res;

    if (color.find("rgba") != std::string_view::npos) {
        // 处理 rgba 格式
        if (sscanf(std::string(color).c_str(), "rgba(%d, %d, %d, %f)", &r, &g, &b,
                   &a) != 4) {
            return res;
        }
    } else if (color.find("rgb") != std::string::npos) {
        // 处理 rgb 格式
        if (sscanf(std::string(color).c_str(), "rgb(%d, %d, %d)", &r, &g, &b) != 3) {
            return res;
        }
    } else {
        return res;
    }

    // 将浮点数 a 转换为 0-255 范围的整数
    alpha = static_cast<unsigned int>(a * 255.0f + 0.5f); // 加0.5用于四舍五入

    // 将 rgba 转换为一个 16 进制的整数
    return (alpha << 24) | (r << 16) | (g << 8) | b;
}

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

void TColor::setValue(std::string_view str) {
    try {
        str = TaroHelper::string::trim(str);
        if (str.size() == 0) return;
        if (str[0] == '#') {
            if (auto hexColor = hex2Int(str); hexColor.has_value()) {
                setValue(hexColor.value());
            }
        } else if (str.find("rgb") != std::string_view::npos) {
            if (auto rgbaColor = rgba2Int(str); rgbaColor.has_value()) {
                setValue(rgbaColor.value());
            }
        } else if (auto color = namedColor.find(str); color != namedColor.end()) {
            setValue(color->second);
        }
    } catch (...) {
        TARO_LOG_ERROR("TaroStyle", "TColor 颜色解析失败");
    }
}

TaroHelper::Optional<uint32_t> TColor::getValue() {
    TaroHelper::Optional<uint32_t> res;
    if (has_value_) {
        res.set(value_);
    }
    return res;
}

void TColor::setValue(napi_value value) {
    NapiGetter getter(value);
    auto type = getter.Type();
    if (type == napi_string) {
        auto strValue = getter.String();
        if (strValue.has_value()) {
            setValue(strValue.value());
        }
    } else if (type == napi_number) {
        auto intValue = getter.UInt32();
        if (intValue.has_value()) {
            setValue(intValue.value());
        }
    }
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet