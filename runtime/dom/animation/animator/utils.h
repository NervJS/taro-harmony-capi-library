#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <sys/time.h>

namespace TaroRuntime {
namespace TaroAnimate {
    class TaroUtils {
        public:
        static bool nearEqual(const double &left, const double &right);

        static bool nearEqual(const double left, const double right,
                              const double epsilon);

        static int64_t getCurrentMsTime();

        static void removeHeadTailSpace(std::string &str);

        static float stringToFloat(const std::string &str);

        static int32_t stringToInt(const std::string &value);
    };
} // namespace TaroAnimate
} // namespace TaroRuntime
