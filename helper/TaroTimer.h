#pragma once

#include <sys/time.h>

#include "./Debug.h"
#include "./TaroLog.h"

#if IS_DEBUG == 1
#define TIMER_SCOPE(name, unit) TaroHelper::FunctionTimer timer##__LINE__(name, unit)
#define TIMER_FUNCTION() TIMER_SCOPE(__FUNCTION__, TaroHelper::TimeUnit::MS)
#define TIMER_US_FUNCTION() TIMER_SCOPE(__FUNCTION__, TaroHelper::TimeUnit::US)
#else
#define TIMER_SCOPE(name, unit) do {} while(0)
#define TIMER_FUNCTION() do {} while(0)
#define TIMER_US_FUNCTION() do {} while(0)
#endif

namespace TaroHelper {
// 精确到微妙（千分之一毫秒）
class MsTimer {
    private:
    struct timeval tvafter, tvpre;
    struct timezone tz;
    bool is_start_;

    public:
    MsTimer()
        : is_start_(false) {}

    // 开始计时
    void Start() {
        is_start_ = true;
        gettimeofday(&tvpre, &tz);
    }
    // 结束计时
    void End() {
        if (is_start_) {
            gettimeofday(&tvafter, &tz);
        }
    }

    // 临时计时
    int ClockUs() {
        End();
        return CostUs();
    }

    // 临时计时
    int ClockMs() {
        End();
        return CostMs();
    }

    int CostUs() const {
        if (is_start_) {
            return (tvafter.tv_sec - tvpre.tv_sec) * 1000000 + (tvafter.tv_usec - tvpre.tv_usec);
        }
        return 0;
    }

    int CostMs() const {
        if (is_start_) {
            return (tvafter.tv_sec - tvpre.tv_sec) * 1000 + (tvafter.tv_usec - tvpre.tv_usec) / 1000;
        }
        return 0;
    }
};

enum class TimeUnit {
    MS,
    US
};

class FunctionTimer {
    private:
    const char* m_Name;
    MsTimer m_Timer;
    bool m_Stop;
    TimeUnit m_Unit;

    public:
    FunctionTimer(const char* name, TimeUnit unit = TimeUnit::MS)
        : m_Name(name), m_Timer(TaroHelper::MsTimer()), m_Unit(unit), m_Stop(false) {
        m_Timer.Start();
    }

    ~FunctionTimer() {
        if (!m_Stop) {
            Stop();
        }
    }

    void Stop() {
        if (m_Unit == TimeUnit::MS) {
            TARO_LOG_DEBUG("TaroTimer", "Function %{public}s take %{public}d ms", m_Name, m_Timer.ClockMs());
        } else {
            TARO_LOG_DEBUG("TaroTimer", "Function %{public}s take %{public}d us", m_Name, m_Timer.ClockUs());
        }
        m_Stop = true;
    }
};
}; // namespace TaroHelper
