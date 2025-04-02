/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./TaroLog.h"

#include <sstream>
#include <boost/stacktrace.hpp>

namespace TaroLog {
std::string GetCallStack() {
    std::ostringstream oss;
    oss << boost::stacktrace::stacktrace();
    return oss.str();
}
} // namespace TaroLog
