//
// Created on 2024/6/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_STRING_H
#define HARMONY_LIBRARY_STRING_H

#include <string_view>

namespace TaroHelper::string {

/**
 * 去除字符串两端的空格
 */
std::string_view trim(std::string_view str);

/**
 * 判断字符串是否为一个正数
 */
bool isPositiveNumber(std::string_view str);

/**
 * 基于空格分割字符串
 */
std::vector<std::string_view> splitBySpace(std::string_view str);

/**
 * 分割字符串（无视空格）
 */
std::vector<std::string_view> split(std::string_view str, const char* delimiter);

/**
 * 获取url的路径（不带参数）
 */
std::string extractPathWithoutParams(std::string url);


/**
 * 检查字符串中是否包含数字
 * @param str
 * @return
 */
bool containsDigit(const std::string_view& str);

} // namespace TaroHelper::string

#endif // HARMONY_LIBRARY_STRING_H
