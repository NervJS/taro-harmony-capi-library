//
// Created on 2024/6/23.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_TENUM_H
#define HARMONY_LIBRARY_TENUM_H
#include <typeindex>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <native_drawing/drawing_text_typography.h>

#include "../css_property.h"

#include "string"
#include "unordered_map"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {
class TEnum {
    public:
    static std::unordered_map<std::type_index, std::unordered_map<std::string_view, int>> enumMappings;
    static std::unordered_map<std::type_index, std::unordered_map<int, int>> intEnumMappings;
};
} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // HARMONY_LIBRARY_TENUM_H
