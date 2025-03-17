//
// Created on 2024/6/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_TSCALAR_H
#define HARMONY_LIBRARY_TSCALAR_H

#include <algorithm>

namespace TaroHelper {

#define FLOAT_MAX           3.402823466e+38f
#define FLOAT_MIN           (-Taro_ScalarMax)
#define EMPTY_FLOAT         FLOAT_MAX
#define EMPTY_INDEX         std::numeric_limits<size_t>::max()

} // namespace TaroHelper
#endif //HARMONY_LIBRARY_TSCALAR_H
