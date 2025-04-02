/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "harmony_style_setter.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {
#define CREATE_FLOAT1_SETTER_BODY(FUNCTIONNAME, ARKUINAME, STYLENAME)          \
    void HarmonyStyleSetter::FUNCTIONNAME(                                     \
        const ArkUI_NodeHandle& node,                                          \
        const std::shared_ptr<                                                 \
            TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) { \
        auto STYLENAME = stylesheet->STYLENAME;                                \
        FUNCTIONNAME(node, STYLENAME);                                         \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node,        \
                                          const double& val) {                 \
        setFloat(node, ARKUINAME, val);                                        \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node,        \
                                          const Optional<float>& val) {        \
        setFloat(node, ARKUINAME, val);                                        \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node) {      \
        NativeNodeApi::getInstance()->resetAttribute(node, ARKUINAME);         \
    }

#define CREATE_LENGTHVALUE_SETTER_BODY(FUNCTIONNAME, ARKUINAME, STYLENAME)     \
    void HarmonyStyleSetter::FUNCTIONNAME(                                     \
        const ArkUI_NodeHandle& node,                                          \
        const std::shared_ptr<                                                 \
            TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) { \
        auto STYLENAME = stylesheet->STYLENAME;                                \
        FUNCTIONNAME(node, STYLENAME);                                         \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node,        \
                                          const double& val) {                 \
        setFloat(node, ARKUINAME, val);                                        \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(                                     \
        const ArkUI_NodeHandle& node,                                          \
        const Optional<TaroRuntime::Dimension>& val) {                         \
        setFloat(node, ARKUINAME, val);                                        \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node) {      \
        NativeNodeApi::getInstance()->resetAttribute(node, ARKUINAME);         \
    }

#define CREATE_INT_SETTER_BODY(FUNCTIONNAME, ARKUINAME, STYLENAME)             \
    void HarmonyStyleSetter::FUNCTIONNAME(                                     \
        const ArkUI_NodeHandle& node,                                          \
        const std::shared_ptr<                                                 \
            TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) { \
        auto STYLENAME = stylesheet->STYLENAME;                                \
        FUNCTIONNAME(node, STYLENAME);                                         \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node,        \
                                          const int& val) {                    \
        setInt(node, ARKUINAME, val);                                          \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node,        \
                                          const Optional<int>& val) {          \
        setInt(node, ARKUINAME, val);                                          \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node) {      \
        NativeNodeApi::getInstance()->resetAttribute(node, ARKUINAME);         \
    }

#define CREATE_ENUM_SETTER_BODY(FUNCTIONNAME, ARKUINAME, ENUMNAME, STYLENAME)  \
    void HarmonyStyleSetter::FUNCTIONNAME(                                     \
        const ArkUI_NodeHandle& node,                                          \
        const std::shared_ptr<                                                 \
            TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) { \
        auto STYLENAME = stylesheet->STYLENAME;                                \
        FUNCTIONNAME(node, STYLENAME);                                         \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node,        \
                                          const int& val) {                    \
        setInt(node, ARKUINAME, val);                                          \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node,        \
                                          const Optional<ENUMNAME>& val) {     \
        if (Optional<int> intVal; val.has_value()) {                           \
            intVal.set(static_cast<int>(val.value()));                         \
            setInt(node, ARKUINAME, intVal);                                   \
        }                                                                      \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node) {      \
        NativeNodeApi::getInstance()->resetAttribute(node, ARKUINAME);         \
    }

#define CREATE_STRING_SETTER_BODY(FUNCTIONNAME, ARKUINAME, STYLENAME)          \
    void HarmonyStyleSetter::FUNCTIONNAME(                                     \
        const ArkUI_NodeHandle& node,                                          \
        const std::shared_ptr<                                                 \
            TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet) { \
        auto STYLENAME = stylesheet->STYLENAME;                                \
        FUNCTIONNAME(node, STYLENAME);                                         \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node,        \
                                          const std::string& val) {            \
        setString(node, ARKUINAME, val);                                       \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node,        \
                                          const Optional<std::string>& val) {  \
        setString(node, ARKUINAME, val);                                       \
    }                                                                          \
    void HarmonyStyleSetter::FUNCTIONNAME(const ArkUI_NodeHandle& node) {      \
        NativeNodeApi::getInstance()->resetAttribute(node, ARKUINAME);         \
    }

        CREATE_LENGTHVALUE_SETTER_BODY(setLineHeight, NODE_TEXT_LINE_HEIGHT, lineHeight);
        CREATE_LENGTHVALUE_SETTER_BODY(setFontSize, NODE_FONT_SIZE, fontSize);
        CREATE_LENGTHVALUE_SETTER_BODY(setWidth, NODE_WIDTH, width);
        CREATE_LENGTHVALUE_SETTER_BODY(setHeight, NODE_HEIGHT, height);
        CREATE_FLOAT1_SETTER_BODY(setLetterSpacing, NODE_TEXT_LETTER_SPACING, letterSpacing);
        CREATE_INT_SETTER_BODY(setZIndex, NODE_Z_INDEX, zIndex);

        CREATE_INT_SETTER_BODY(setWebkitLineClamp, NODE_TEXT_MAX_LINES, webkitLineClamp);

        CREATE_STRING_SETTER_BODY(setFontFamily, NODE_FONT_FAMILY, fontFamily);

        CREATE_ENUM_SETTER_BODY(setWordBreak, NODE_TEXT_WORD_BREAK, OH_Drawing_WordBreakType, wordBreak);

        CREATE_ENUM_SETTER_BODY(setFontWeight, NODE_FONT_WEIGHT, ArkUI_FontWeight, fontWeight);

        CREATE_ENUM_SETTER_BODY(setVisibility, NODE_VISIBILITY, ArkUI_Visibility, visibility);
        CREATE_ENUM_SETTER_BODY(setFontStyle, NODE_FONT_STYLE, ArkUI_FontStyle, fontStyle);

        CREATE_ENUM_SETTER_BODY(setTextAlign, NODE_TEXT_ALIGN, ArkUI_TextAlignment, textAlign);
        CREATE_ENUM_SETTER_BODY(setTextOverflow, NODE_TEXT_OVERFLOW, ArkUI_TextOverflow, textOverflow);
        CREATE_ENUM_SETTER_BODY(setVerticalAlign, NODE_ALIGNMENT, OH_Drawing_PlaceholderVerticalAlignment, verticalAlign);

        void HarmonyStyleSetter::setFloat(const ArkUI_NodeHandle& node,
                                          ArkUI_NodeAttributeType attributeType,
                                          const double& val) {
            ArkUI_NumberValue arkUI_NumberValue[1] = {};
            ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
            arkUI_NumberValue[0].f32 = val;

            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
        }

        void HarmonyStyleSetter::setFloat(const ArkUI_NodeHandle& node,
                                          ArkUI_NodeAttributeType attributeType,
                                          const double& val1,
                                          const double& val2) {
            ArkUI_NumberValue arkUI_NumberValue[2] = {};
            ArkUI_AttributeItem item = {arkUI_NumberValue, 2};
            arkUI_NumberValue[0].f32 = val1;
            arkUI_NumberValue[1].f32 = val2;

            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
        }

        void HarmonyStyleSetter::setFloat(const ArkUI_NodeHandle& node,
                                          ArkUI_NodeAttributeType attributeType,
                                          const Optional<float>& val) {
            if (val.has_value()) {
                ArkUI_NumberValue arkUI_NumberValue[1] = {};
                ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
                arkUI_NumberValue[0].f32 = val.value();

                TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
            }
        }

        void HarmonyStyleSetter::setFloat(
            const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
            const Optional<TaroRuntime::Dimension>&
                lengthVal) {
            if (lengthVal.has_value()) {
                ArkUI_NumberValue arkUI_NumberValue[1] = {};
                ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
                auto topValue = lengthVal.value();
                if (lengthVal.has_value()) {
                    if (auto val = lengthVal.value().ParseToVp(); val.has_value()) {
                        arkUI_NumberValue[0].f32 = val.value();
                    }
                }

                TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
            }
        }

        void HarmonyStyleSetter::setFloat(
            const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
            const Optional<TaroRuntime::Dimension>&
                val1,
            const Optional<TaroRuntime::Dimension>&
                val2) {
            ArkUI_NumberValue arkUI_NumberValue[2] = {};
            ArkUI_AttributeItem item = {arkUI_NumberValue, 2};
            if (val1.has_value()) {
                if (auto val = val1.value().ParseToVp(); val.has_value()) {
                    arkUI_NumberValue[0].f32 = val.value();
                }
            }

            if (val2.has_value()) {
                if (auto val = val2.value().ParseToVp(); val.has_value()) {
                    arkUI_NumberValue[0].f32 = val.value();
                }
            }

            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
        }

        void HarmonyStyleSetter::setFloat(
            const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
            const Optional<Dimension>&
                val1,
            const Optional<Dimension>&
                val2,
            float val1Base, float val2base) {
            ArkUI_NumberValue arkUI_NumberValue[2] = {};
            ArkUI_AttributeItem item = {arkUI_NumberValue, 2};
            if (val1.has_value()) {
                if (auto val = val1.value().ParseToVp(val1Base); val.has_value()) {
                    arkUI_NumberValue[0].f32 = val.value();
                }
            }

            if (val2.has_value()) {
                if (auto val = val2.value().ParseToVp(val2base); val.has_value()) {
                    arkUI_NumberValue[0].f32 = val.value();
                }
            }

            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
        }

        void HarmonyStyleSetter::setFloat(const ArkUI_NodeHandle& node,
                                          ArkUI_NodeAttributeType attributeType,
                                          double top,
                                          double right,
                                          double bottom,
                                          double left) {
            ArkUI_NumberValue arkUI_NumberValue[4] = {};
            ArkUI_AttributeItem item = {arkUI_NumberValue, 4};
            arkUI_NumberValue[0].f32 = top;
            arkUI_NumberValue[1].f32 = right;
            arkUI_NumberValue[2].f32 = bottom;
            arkUI_NumberValue[3].f32 = left;

            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
        }
        void HarmonyStyleSetter::setFloat(
            const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
            const Optional<TaroRuntime::Dimension>& top,
            const Optional<TaroRuntime::Dimension>& right,
            const Optional<TaroRuntime::Dimension>& bottom,
            const Optional<TaroRuntime::Dimension>& left) {
            ArkUI_NumberValue arkUI_NumberValue[4] = {};
            ArkUI_AttributeItem item = {arkUI_NumberValue, 4};

            if (top.has_value()) {
                if (auto val = top.value().ParseToVp(); val.has_value()) {
                    arkUI_NumberValue[0].f32 = val.value();
                }
            }
            if (right.has_value()) {
                if (auto val = right.value().ParseToVp(); val.has_value()) {
                    arkUI_NumberValue[1].f32 = val.value();
                }
            }
            if (bottom.has_value()) {
                if (auto val = bottom.value().ParseToVp(); val.has_value()) {
                    arkUI_NumberValue[2].f32 = val.value();
                }
            }
            if (left.has_value()) {
                if (auto val = left.value().ParseToVp(); val.has_value()) {
                    arkUI_NumberValue[3].f32 = val.value();
                }
            }

            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(
                node, attributeType, &item);
        }

        void HarmonyStyleSetter::setFloat(
            const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
            const Optional<Dimension>& top,
            const Optional<Dimension>& right,
            const Optional<Dimension>& bottom,
            const Optional<Dimension>& left,
            float baseValue) {
            ArkUI_NumberValue arkUI_NumberValue[4] = {};
            ArkUI_AttributeItem item = {arkUI_NumberValue, 4};

            if (top.has_value()) {
                if (auto val = top.value().ParseToVp(baseValue); val.has_value()) {
                    arkUI_NumberValue[0].f32 = val.value();
                }
            }
            if (right.has_value()) {
                if (auto val = right.value().ParseToVp(baseValue); val.has_value()) {
                    arkUI_NumberValue[1].f32 = val.value();
                }
            }
            if (bottom.has_value()) {
                if (auto val = bottom.value().ParseToVp(baseValue); val.has_value()) {
                    arkUI_NumberValue[2].f32 = val.value();
                }
            }
            if (left.has_value()) {
                if (auto val = left.value().ParseToVp(baseValue); val.has_value()) {
                    arkUI_NumberValue[3].f32 = val.value();
                }
            }

            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(
                node, attributeType, &item);
        }

        void HarmonyStyleSetter::HarmonyStyleSetter::setUint(
            const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
            const uint32_t& val1) {
            ArkUI_NumberValue arkUI_NumberValue[1] = {};
            ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
            arkUI_NumberValue[0].u32 = val1;
            NativeNodeApi::getInstance()->setAttribute(node, attributeType,
                                                       &item);
        }

        void HarmonyStyleSetter::setUint(const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType, const Optional<uint32_t>& val1) {
            if (val1.has_value()) {
                ArkUI_NumberValue arkUI_NumberValue[1] = {};
                ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
                arkUI_NumberValue[0].u32 = val1.value();
                NativeNodeApi::getInstance()->setAttribute(node, attributeType,
                                                           &item);
            }
        }

        void HarmonyStyleSetter::setString(
            const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
            const std::string& val) {
            ArkUI_AttributeItem item = {.string = val.c_str()};
            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
        }

        void HarmonyStyleSetter::setString(
            const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
            const Optional<std::string>& val) {
            ArkUI_AttributeItem item = {.string = val.value_or("").c_str()};
            TaroRuntime::NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
        }

        void HarmonyStyleSetter::HarmonyStyleSetter::setInt(
            const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
            const int& val1) {
            ArkUI_NumberValue arkUI_NumberValue[1] = {};
            ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
            arkUI_NumberValue[0].i32 = val1;
            NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
        }

        void HarmonyStyleSetter::setInt(const ArkUI_NodeHandle& node,
                                        ArkUI_NodeAttributeType attributeType,
                                        const Optional<int>& val1) {
            if (val1.has_value()) {
                ArkUI_NumberValue arkUI_NumberValue[1] = {};
                ArkUI_AttributeItem item = {arkUI_NumberValue, 1};
                arkUI_NumberValue[0].i32 = val1.value();
                NativeNodeApi::getInstance()->setAttribute(node, attributeType, &item);
            }
        }

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime
