//
// Created by zhutianjian on 24-8-4.
//

#include "./TaroLog.h"

#include <boost/stacktrace.hpp>
#include <sstream>

namespace TaroLog {
  std::string GetCallStack() {
    std::ostringstream oss;
    oss << boost::stacktrace::stacktrace();
    return oss.str();
  }
} // namespace TaroLog
