//
// Created on 2024/6/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "string.h"

#include <cstddef>
#include <regex>

#include "runtime/cssom/stylesheet/types/TRange.h"

namespace TaroHelper::string {

bool containsDigit(const std::string_view& str) {
    for (char c : str) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            return true;
        }
    }
    return false;
}

/**
 * 去除字符串两端的空格
 */
std::string_view trim(std::string_view str) {
    size_t first = str.find_first_not_of(" ");
    if (first == std::string_view::npos) return "";
    size_t last = str.find_last_not_of(" ");
    return str.substr(first, last - first + 1);
}

/**
 * 判断字符串是否为一个正数
 */
bool isPositiveNumber(std::string_view str) {
    return std::regex_match(str.begin(), str.end(), std::regex(R"(^\d+(?:\.\d+)?|\.\d+$)"));
}

/**
 * 基于空格分割字符串（括号里面的空格不算）
 */
std::vector<std::string_view> splitBySpace(std::string_view str) {
    TaroRuntime::TaroCSSOM::TaroStylesheet::TRange bracketRange; // 括号的范围
    size_t bracketStart = str.find_first_of('(');
    if (bracketStart != std::string_view::npos) {
        size_t bracketEnd = str.find_last_of(')');
        if (bracketEnd != std::string_view::npos && bracketStart < bracketEnd) {
            bracketRange = {bracketStart, bracketEnd};
        }
    }

    std::vector<std::string_view> result;
    std::size_t start = 0;
    std::size_t end = 0;

    while (start != std::string_view::npos) {
        // 跳过开头的空格
        start = str.find_first_not_of(" ", end);
        if (start == std::string_view::npos) {
            break;
        }

        // 查找下一个空格的位置
        if (bracketRange.empty()) {
            end = str.find(" ", start);
        } else {
            std::size_t _start = start;
            do {
                end = str.find(" ", _start);
                _start = end + 1;
            } while (end != std::string_view::npos && bracketRange.contains(end));
        }

        // 提取子字符串并添加到结果中
        if (end == std::string_view::npos) {
            result.emplace_back(str.substr(start));
        } else {
            result.emplace_back(str.substr(start, end - start));
        }
    }

    return result;
}

/**
 * 基于空格分割字符串
 */
std::vector<std::string_view> split(std::string_view str, const char* delimiter) {
    size_t delimiterLength = std::strlen(delimiter);
    std::vector<std::string_view> result;
    std::size_t start = 0;
    std::size_t end = 0;

    while (end != std::string_view::npos) {
        end = str.find(delimiter, start);

        if (end == std::string_view::npos) {
            auto item = trim(str.substr(start));
            if (!item.empty()) result.emplace_back(item);
            break;
        }

        auto item = trim(str.substr(start, end - start));
        if (!item.empty()) result.emplace_back(item);

        start = end + delimiterLength;
    }

    return result;
}

/**
 * 获取url的路径（不带参数）
 */
std::string extractPathWithoutParams(std::string url) {
    size_t pos = url.find('?');

    if (pos != std::string_view::npos) {
        return url.substr(0, pos);
    }

    return url;
}

} // namespace TaroHelper::string
