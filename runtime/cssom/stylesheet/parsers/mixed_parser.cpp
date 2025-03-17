//
// Created on 2024/6/23.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "mixed_parser.h"

#include <cstdint>
#include <regex>

#include "helper/string.h"
#include "runtime/cssom/stylesheet/types/TRange.h"

// 盒模型
struct Bound {
    std::string_view top;
    std::string_view right;
    std::string_view bottom;
    std::string_view left;
};

/**
 * 解析盒模型字符串
 */
static Bound parseBound(std::string_view str) {
    auto values = TaroHelper::string::splitBySpace(str);

    switch (values.size()) {
        case 1:
            return {values[0], values[0], values[0], values[0]};
        case 2:
            return {values[0], values[1], values[0], values[1]};
        case 3:
            return {values[0], values[1], values[2], values[1]};
        case 4:
            return {values[0], values[1], values[2], values[3]};
        default:
            return {"", "", "", ""};
    }
}

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

/**
 * 解析 padding 混合值
 */
void parsePadding(Stylesheet* ss, std::string_view value) {
    auto bound = parseBound(value);
    if (!bound.top.empty()) ss->paddingTop.setValueFromStringView(bound.top);
    if (!bound.right.empty()) ss->paddingRight.setValueFromStringView(bound.right);
    if (!bound.bottom.empty()) ss->paddingBottom.setValueFromStringView(bound.bottom);
    if (!bound.left.empty()) ss->paddingLeft.setValueFromStringView(bound.left);
}

/**
 * 解析 margin 混合值
 */
void parseMargin(Stylesheet* ss, std::string_view value) {
    auto bound = parseBound(value);
    if (!bound.top.empty()) ss->marginTop.setValueFromStringView(bound.top);
    if (!bound.right.empty()) ss->marginRight.setValueFromStringView(bound.right);
    if (!bound.bottom.empty()) ss->marginBottom.setValueFromStringView(bound.bottom);
    if (!bound.left.empty()) ss->marginLeft.setValueFromStringView(bound.left);
}

/**
 * 解析 border 混合值，第三个参数表示设置的方向：0-top | 1-right | 2-bottom | 3-left | 4-all
 */
void parseBorder(Stylesheet* ss, std::string_view value, uint8_t pos) {
    auto values = TaroHelper::string::splitBySpace(value);
    auto size = values.size();

    if (size < 2) return;

    // 寻找 border-style
    auto styleIter = std::find_if(values.begin(), values.end(), [&](auto& item) {
        return item == "solid" || item == "dashed" || item == "dotted";
    });
    if (styleIter == values.end()) return;

    size_t stylePos = std::distance(values.begin(), styleIter);
    size_t widthPos = size;
    size_t colorPos = size;

    for (size_t i = 0; i < size; ++i) {
        if (i == stylePos) continue;
        if (colorPos != size) {
            widthPos = i;
            break;
        }
        if (widthPos != size) {
            colorPos = i;
            break;
        }

        if (std::regex_match(values[i].begin(), values[i].end(), std::regex(R"(#[0-9a-fA-F]{3,6}|(?:rgba?|hsl|hwb|lab|lch)\(.*?\)|transparent)"))) {
            colorPos = i;
        } else {
            widthPos = i;
        }
    }

    if (widthPos == size) return;

    switch (pos) {
        case 0:
            ss->borderTopStyle.setValueFromStringView(values[stylePos]);
            ss->borderTopWidth.setValueFromStringView(values[widthPos]);
            if (colorPos != size) ss->borderTopColor.setValueFromStringView(values[colorPos]);
            break;
        case 1:
            ss->borderRightStyle.setValueFromStringView(values[stylePos]);
            ss->borderRightWidth.setValueFromStringView(values[widthPos]);
            if (colorPos != size) ss->borderRightColor.setValueFromStringView(values[colorPos]);
            break;
        case 2:
            ss->borderBottomStyle.setValueFromStringView(values[stylePos]);
            ss->borderBottomWidth.setValueFromStringView(values[widthPos]);
            if (colorPos != size) ss->borderBottomColor.setValueFromStringView(values[colorPos]);
            break;
        case 3:
            ss->borderLeftStyle.setValueFromStringView(values[stylePos]);
            ss->borderLeftWidth.setValueFromStringView(values[widthPos]);
            if (colorPos != size) ss->borderLeftColor.setValueFromStringView(values[colorPos]);
            break;
        case 4:
            parseBorderStyle(ss, values[stylePos]);
            parseBorderWidth(ss, values[widthPos]);
            if (colorPos != size) parseBorderColor(ss, values[colorPos]);
            break;
        default:
            break;
    }
}

/**
 * 解析 border-width 混合值
 */
void parseBorderWidth(Stylesheet* ss, std::string_view value) {
    auto bound = parseBound(value);
    if (!bound.top.empty()) ss->borderTopWidth.setValueFromStringView(bound.top);
    if (!bound.right.empty()) ss->borderRightWidth.setValueFromStringView(bound.right);
    if (!bound.bottom.empty()) ss->borderBottomWidth.setValueFromStringView(bound.bottom);
    if (!bound.left.empty()) ss->borderLeftWidth.setValueFromStringView(bound.left);
}

/**
 * 解析 border-style 混合值
 */
void parseBorderStyle(Stylesheet* ss, std::string_view value) {
    auto bound = parseBound(value);
    if (!bound.top.empty()) ss->borderTopStyle.setValueFromStringView(bound.top);
    if (!bound.right.empty()) ss->borderRightStyle.setValueFromStringView(bound.right);
    if (!bound.bottom.empty()) ss->borderBottomStyle.setValueFromStringView(bound.bottom);
    if (!bound.left.empty()) ss->borderLeftStyle.setValueFromStringView(bound.left);
}

/**
 * 解析 border-color 混合值
 */
void parseBorderColor(Stylesheet* ss, std::string_view value) {
    auto bound = parseBound(value);
    if (!bound.top.empty()) ss->borderTopColor.setValueFromStringView(bound.top);
    if (!bound.right.empty()) ss->borderRightColor.setValueFromStringView(bound.right);
    if (!bound.bottom.empty()) ss->borderBottomColor.setValueFromStringView(bound.bottom);
    if (!bound.left.empty()) ss->borderLeftColor.setValueFromStringView(bound.left);
}

/**
 * 解析 background-position 混合值
 * 语法规则：https://developer.mozilla.org/zh-CN/docs/Web/CSS/background-position
 */
void parseBackgroundPosition(Stylesheet* ss, std::string_view value) {
    auto values = TaroHelper::string::splitBySpace(value);
    if (values.empty() || values.size() > 4) return;

    bool reverse = values[0] == "top" || values[0] == "bottom";

    if (values.size() < 3) {
        std::string_view v1 = values.size() == 1 ? "center" : values[1];
        ss->backgroundPositionX.setValueFromStringView(reverse ? v1 : values[0]);
        ss->backgroundPositionY.setValueFromStringView(reverse ? values[0] : v1);
    } else {
        // 3个、4个值的时候，必定有两个关键字，第二个关键字的位置有可能在第2，3位置
        uint8_t secondKeywordIndex = UINT8_MAX;
        for (uint8_t i = 1; i < 3; ++i) {
            auto& item = values[i];
            if (item == "center" || (reverse ? (item == "left" || item == "right") : (item == "top" || item == "bottom"))) {
                secondKeywordIndex = i;
                break;
            }
        }
        if (secondKeywordIndex != UINT8_MAX) {
            auto v0 = value.substr(0, std::distance(values[0].begin(), values[secondKeywordIndex - 1].end()));
            auto v1 = value.substr(
                std::distance(values[0].begin(), values[secondKeywordIndex].begin()),
                std::distance(values[secondKeywordIndex].begin(), values[values.size() - 1].end()));
            ss->backgroundPositionX.setValueFromStringView(reverse ? v1 : v0);
            ss->backgroundPositionY.setValueFromStringView(reverse ? v0 : v1);
        }
    }
}

/**
 * 解析 background 混合值
 * 语法规则：https://developer.mozilla.org/zh-CN/docs/Web/CSS/background
 */
void parseBackground(Stylesheet* ss, std::string_view value) {
    std::cmatch matches;
    const char* searchStart;
    std::vector<TRange> colorRanges;    // 颜色区间集合
    std::vector<TRange> positionRanges; // position区间集合
    TRange imageRange;                  // 图片区间
    TRange repeatRange;                 // repeat区间
    TRange sizeRange;                   // size区间

    // 先解析出所有颜色区间
    searchStart = value.begin();
    while (std::regex_search(searchStart, value.end(), matches, std::regex(R"(#[0-9a-fA-F]{3,6}|(?:rgba?|hsl|hwb|lab|lch)\(.*?\)|transparent)"))) {
        size_t start = matches[0].first - value.begin();
        size_t end = start + matches[0].length();
        colorRanges.push_back({start, end});
        searchStart = matches[0].second;
    }

    // 解析背景图片区间
    if (std::regex_search(value.begin(), value.end(), matches, std::regex(R"((?:linear|radial)-gradient\()"))) {
        // 寻找颜色渐变区间
        auto start = matches[0].first - value.begin();
        size_t end = start + matches[0].length();
        do {
            end = value.find(')', end) + 1;
        } while (end != std::string_view::npos && std::any_of(colorRanges.begin(), colorRanges.end(), [&](TRange r) { return r.contains(end); }));

        if (end != std::string_view::npos) {
            imageRange = TRange(start, end);
        }
    } else if (std::regex_search(value.begin(), value.end(), matches, std::regex(R"(none|url\(.*?\))"))) {
        size_t start = matches[0].first - value.cbegin();
        imageRange = TRange(start, start + matches[0].length());
    }

    // 解析 repeat 区间
    if (std::regex_search(value.begin(), value.end(), matches, std::regex(R"((?:repeat-x|repeat-y|no-repeat|repeat|space|round)(?:\s*(?:repeat-x|repeat-y|no-repeat|repeat|space|round))?)"))) {
        size_t start = matches[0].first - value.cbegin();
        repeatRange = TRange(start, start + matches[0].length());
    }

    // 解析 size 区间
    if (std::regex_search(value.begin(), value.end(), matches, std::regex(R"(/\s*((?:calc\(.*?\)|auto|\d+(?:\.\d+)?(?:px|%|vw|vh|r?em))(?:\s*(?:auto|\d+(?:\.\d+)?(?:px|%|vw|vh|r?em)))?|cover|contain))"))) {
        size_t start = matches[1].first - value.cbegin();
        sizeRange = TRange(start, start + matches[1].length());
    }

    // 解析 position 区间集合
    searchStart = value.cbegin();
    while (std::regex_search(searchStart, value.cend(), matches, std::regex(R"((?:(?:calc\(.*?\)|top|left|center|right|bottom|\d+(?:\.\d+)?(?:px|%|vw|vh|r?em))\s*)+)"))) {
        size_t start = matches[0].first - value.cbegin();
        size_t end = start + matches[0].length();
        positionRanges.push_back({start, end});
        searchStart = matches[0].second;
    }

    // background-image
    if (imageRange.width()) {
        ss->backgroundImage.setValueFromStringView(value.substr(imageRange.start, imageRange.width()));
    }

    // background-color: 排除图片选区内的颜色
    if (!colorRanges.empty()) {
        if (!imageRange.width()) {
            ss->backgroundColor.setValueFromStringView(value.substr(colorRanges[0].start, colorRanges[0].width()));
        } else {
            auto it = std::find_if(colorRanges.begin(), colorRanges.end(), [&](auto item) {
                return !item.intersects(imageRange);
            });
            if (it != colorRanges.end()) {
                ss->backgroundColor.setValueFromStringView(value.substr(it->start, it->width()));
            }
        }
    }

    // background-repeat: 排除图片选区内的相同字符串
    if (repeatRange.width() && !repeatRange.intersects(imageRange)) {
        ss->backgroundRepeat.setValueFromStringView(value.substr(repeatRange.start, repeatRange.width()));
    } else {
        repeatRange.reset();
    }

    // background-size: 排除图片选区内的相同字符串
    if (sizeRange.width() && !sizeRange.intersects(imageRange)) {
        ss->backgroundSize.setValueFromStringView(value.substr(sizeRange.start, sizeRange.width()));
    } else {
        sizeRange.reset();
    }

    // background-position
    if (!positionRanges.empty()) {
        auto it = std::find_if(positionRanges.begin(), positionRanges.end(), [&](auto item) {
            if (sizeRange.width() && item.end > sizeRange.start) return false;   // position 不能在 size 之后
            if (imageRange.width() && item.intersects(imageRange)) return false; // 排除背景图片干扰
            return true;
        });
        if (it != positionRanges.end()) {
            parseBackgroundPosition(ss, value.substr(it->start, it->width()));
        }
    }
}

/**
 * 解析 flex 混合值
 * 语法规则：https://developer.mozilla.org/zh-CN/docs/Web/CSS/flex
 */
void parseFlex(Stylesheet* ss, std::string_view value) {
    if (value == "initial" || value == "inherit" || value == "revert" || value == "revert-layer" || value == "unset") {
        return;
    }
    if (value == "none") {
        ss->flexShrink.setValue(0.0f);
        return;
    }

    auto values = TaroHelper::string::splitBySpace(value);

    if (values.size() == 3) {
        ss->flexGrow.setValueFromStringView(values[0]);
        ss->flexShrink.setValueFromStringView(values[1]);
        ss->flexBasis.setValueFromStringView(values[2]);
        return;
    }

    if (values.size() == 2) {
        ss->flexGrow.setValueFromStringView(values[0]);
        bool isValidFlexShrink = TaroHelper::string::isPositiveNumber(values[1]);
        if (isValidFlexShrink) {
            ss->flexShrink.setValueFromStringView(values[1]);
            ss->flexBasis.setValue(0.0f);
        } else {
            ss->flexShrink.setValue(1.0f);
            ss->flexBasis.setValueFromStringView(values[1]);
        }
        return;
    }

    if (values.size() == 1) {
        bool isValidFlexGrow = TaroHelper::string::isPositiveNumber(values[0]);
        if (isValidFlexGrow) {
            ss->flexGrow.setValueFromStringView(values[0]);
            ss->flexShrink.setValue(1.0f);
            ss->flexBasis.setValue(0.0f);
        } else {
            ss->flexGrow.setValue(1.0f);
            ss->flexShrink.setValue(1.0f);
            ss->flexBasis.setValueFromStringView(values[0]);
        }
    }
}

/**
 * 解析 flex-flow 混合值
 * 语法规则：https://developer.mozilla.org/zh-CN/docs/Web/CSS/flex-flow
 */
void parseFlexFlow(Stylesheet* ss, std::string_view value) {
    auto values = TaroHelper::string::splitBySpace(value);
    if (values.empty() || values.size() > 2) return;

    bool firstItemIsWrapType = values[0] == "nowrap" || values[0] == "wrap" || values[0] == "wrap-reverse";

    if (firstItemIsWrapType) {
        ss->flexWrap.setValueFromStringView(values[0]);
    } else {
        ss->flexDirection.setValueFromStringView(values[0]);
    }

    if (values.size() == 2) {
        if (firstItemIsWrapType) {
            ss->flexDirection.setValueFromStringView(values[1]);
        } else {
            ss->flexWrap.setValueFromStringView(values[1]);
        }
    }
}

/**
 * 解析 text-decoration 混合值
 */
void parseTextDecoration(Stylesheet* ss, std::string_view value) {
    auto values = TaroHelper::string::splitBySpace(value);
    auto size = values.size();

    if (values.empty() || size > 3) return;

    // 寻找 text-decoration-line
    auto lineIter = std::find_if(values.begin(), values.end(), [&](auto& item) {
        return item == "none" || item == "underline" || item == "overline" || item == "line-through";
    });
    if (lineIter == values.end() || *lineIter == "none") return;

    size_t linePos = std::distance(values.begin(), lineIter);
    size_t stylePos = size;
    size_t colorPos = size;

    for (size_t i = 0; i < size; ++i) {
        if (i == linePos) continue;
        if (colorPos != size) {
            stylePos = i;
            break;
        }
        if (stylePos != size) {
            colorPos = i;
            break;
        }

        if (std::regex_match(values[i].begin(), values[i].end(), std::regex(R"(#[0-9a-fA-F]{3,6}|(?:rgba?|hsl|hwb|lab|lch)\(.*?\)|transparent)"))) {
            colorPos = i;
        } else {
            stylePos = i;
        }
    }

    ss->textDecorationLine.setValueFromStringView(values[linePos]);
    if (colorPos != size) ss->textDecorationColor.setValueFromStringView(values[colorPos]);
    if (stylePos != size) ss->textDecorationStyle.setValueFromStringView(values[stylePos]);
}

/**
 * 解析 border-radius 混合值
 */
void parseBorderRadius(Stylesheet* ss, std::string_view value) {
    // 鸿蒙不支持椭圆圆角，排除 / 的写法
    auto linePos = value.find('/');
    if (linePos != std::string_view::npos) {
        value = value.substr(0, linePos);
    }

    // 混合值的规则类似 padding 等属性
    auto bound = parseBound(value);
    if (!bound.top.empty()) ss->borderTopLeftRadius.setValueFromStringView(bound.top);
    if (!bound.right.empty()) ss->borderTopRightRadius.setValueFromStringView(bound.right);
    if (!bound.bottom.empty()) ss->borderBottomRightRadius.setValueFromStringView(bound.bottom);
    if (!bound.left.empty()) ss->borderBottomLeftRadius.setValueFromStringView(bound.left);
}

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
