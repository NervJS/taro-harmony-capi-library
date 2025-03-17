//
// Created on 2024/6/19.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TESTNDK_MAX_WIDTH_H
#define TESTNDK_MAX_WIDTH_H

#include "attribute_base.h"
#include "common.h"
#include "napi/native_api.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
// NODE_BORDER_WIDTH
class MaxWidth : public AttributeBase<Dimension> {};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TESTNDK_MAX_WIDTH_H
