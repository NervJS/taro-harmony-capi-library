/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_STRINGUTILS_H
#define TARO_CAPI_HARMONY_DEMO_STRINGUTILS_H

#include <optional>
#include <vector>

namespace TaroHelper {
class StringUtils {
    public:
    static std::vector<std::string>
    split(const std::string &str, const std::string &delimiter);

    static void split(std::vector<std::string> &token,
                      const std::string &str, const std::string &delimiter);

    static std::vector<std::string>
    splitWithEscape(const std::string &str, const std::string &delimiter,
                    const char &escape = '\\');

    static void
    splitWithEscape(std::vector<std::string> &token, const std::string &str,
                    const std::string &delimiter, const char &escape = '\\');

    static std::string join(const std::vector<std::string> &tokens,
                            const std::string &delimiter);

    static void join(const std::vector<std::string> &tokens,
                     const std::string &delimiter, std::string &result);

    static std::string toUpperCase(const std::string &str);

    static std::string toLowerCase(const std::string &str);

    static std::string toCamelCase(const std::string &str);

    static uint32_t parseHexColor(const std::string &hex);

    static uint32_t parseRgbaColor(const std::string &rgba);

    static std::optional<uint32_t> parseColor(const std::string &colorString);

    static double percentageToDecimal(const std::string &str);

    static float parseSize(const std::string &sourceStr);

    static bool isStartWith(const std::string &, const std::string &,
                            const int32_t);
};
} // namespace TaroHelper

#endif // TARO_CAPI_HARMONY_DEMO_STRINGUTILS_H
