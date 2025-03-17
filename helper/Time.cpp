#include "Time.h"

namespace TaroHelper {

int64_t TaroTime::getCurrentMsTime() {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int64_t TaroTime::getCurrentUsTime() {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return tv.tv_sec * 10000000 + tv.tv_usec;
}

} // namespace TaroHelper
