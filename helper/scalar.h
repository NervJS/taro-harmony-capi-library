/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_TSCALAR_H
#define HARMONY_LIBRARY_TSCALAR_H

#include <algorithm>

namespace TaroHelper {

#define FLOAT_MAX 3.402823466e+38f
#define FLOAT_MIN (-Taro_ScalarMax)
#define EMPTY_FLOAT FLOAT_MAX
#define EMPTY_INDEX std::numeric_limits<size_t>::max()

} // namespace TaroHelper
#endif // HARMONY_LIBRARY_TSCALAR_H
