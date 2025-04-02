/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "utils.h"

namespace TaroRuntime {
namespace TaroAnimate {
    bool TaroUtils::nearEqual(const double& left, const double& right) {
        constexpr double epsilon = 0.001f;
        return nearEqual(left, right, epsilon);
    }

    bool TaroUtils::nearEqual(const double left, const double right,
                              const double epsilon) {
        return (std::abs(left - right) <= epsilon);
    }

    int64_t TaroUtils::getCurrentMsTime() {
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    void TaroUtils::removeHeadTailSpace(std::string& str) {
        if (!str.empty()) {
            auto start = str.find_first_not_of(' ');
            if (start == std::string::npos) {
                str.clear();
            } else {
                str = str.substr(start, str.find_last_not_of(' ') - start + 1);
            }
        }
    }

    float TaroUtils::stringToFloat(const std::string& str) {
        char* pEnd = nullptr;
        errno = 0;
        double result = std::strtof(str.c_str(), &pEnd);
        if (pEnd == str.c_str() || errno == ERANGE) {
            return 0.0;
        } else {
            return result;
        }
    }

    int32_t TaroUtils::stringToInt(const std::string& value) {
        errno = 0;
        char* pEnd = nullptr;
        int64_t result = std::strtol(value.c_str(), &pEnd, 10);
        if (pEnd == value.c_str() || (result < INT_MIN || result > INT_MAX) ||
            errno == ERANGE) {
            return 0;
        } else {
            return result;
        }
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
