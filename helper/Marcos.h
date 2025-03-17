//
// Created on 2024/7/11.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

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
