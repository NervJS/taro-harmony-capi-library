/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_MARCOS_H
#define HARMONY_LIBRARY_MARCOS_H

#include "Debug.h"

#ifdef IS_DEBUG
#include <cassert>
#define TARO_DCHECK(expr) assert(expr)
#else
#define TARO_DCHECK(expr) ((void)0)
#endif // TARO_DEBUG

#endif // HARMONY_LIBRARY_MARCOS_H
