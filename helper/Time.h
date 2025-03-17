#pragma once

#include <cstdint>
#include <string>
#include <sys/time.h>

namespace TaroHelper {
class TaroTime {
    public:
    static int64_t getCurrentMsTime();
    static int64_t getCurrentUsTime();
};
} // namespace TaroHelper