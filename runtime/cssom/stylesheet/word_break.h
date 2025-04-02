/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_WORDBREAK_H
#define TARO_CAPI_HARMONY_DEMO_WORDBREAK_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <napi/native_api.h>

#include "./attribute_base.h"
#include "helper/Optional.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

// NODE_TEXT_WORD_BREAK
class WordBreak : public AttributeBase<OH_Drawing_WordBreakType> {};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet

#endif // TARO_CAPI_HARMONY_DEMO_WORDBREAK_H
