/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_MIXED_PARSER_H
#define HARMONY_LIBRARY_MIXED_PARSER_H

#include <cstdint>
#include <string_view>

#include "runtime/cssom/stylesheet/IAttribute.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

// 解析 padding 混合值
void parsePadding(Stylesheet* ss, std::string_view value);

// 解析 margin 混合值
void parseMargin(Stylesheet* ss, std::string_view value);

// 解析 border 混合值，第三个参数表示设置的方向：0-top | 1-right | 2-bottom | 3-left | 4-all
void parseBorder(Stylesheet* ss, std::string_view value, uint8_t pos);

// 解析 border-width 混合值
void parseBorderWidth(Stylesheet* ss, std::string_view value);

// 解析 border-style 混合值
void parseBorderStyle(Stylesheet* ss, std::string_view value);

// 解析 border-color 混合值
void parseBorderColor(Stylesheet* ss, std::string_view value);

// 解析 background-position 混合值
void parseBackgroundPosition(Stylesheet* ss, std::string_view value);

// 解析 background 混合值
void parseBackground(Stylesheet* ss, std::string_view value);

// 解析 flex 混合值
void parseFlex(Stylesheet* ss, std::string_view value);

// 解析 flex-flow 混合值
void parseFlexFlow(Stylesheet* ss, std::string_view value);

// 解析 text-decoration 混合值
void parseTextDecoration(Stylesheet* ss, std::string_view value);

// 解析 border-radius 混合值
void parseBorderRadius(Stylesheet* ss, std::string_view value);

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // HARMONY_LIBRARY_MIXED_PARSER_H
