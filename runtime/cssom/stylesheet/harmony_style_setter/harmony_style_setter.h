/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_HARMONY_STYLE_SETTER_H
#define TARO_CAPI_HARMONY_DEMO_HARMONY_STYLE_SETTER_H

#include "../IAttribute.h"
#include "../common.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/dom/ark_nodes/arkui_node.h"
#include "runtime/dom/ark_nodes/differ/layout_differ.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {

#define CREATE_FLOAT1_SETTER_HEAD(FUNCTIONNAME)                                         \
    static void FUNCTIONNAME(                                                           \
        const ArkUI_NodeHandle& node,                                                   \
        const std::shared_ptr<                                                          \
            TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet);           \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node, const double& val);          \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node, const Optional<float>& val); \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node)

#define CREATE_LENGTHVALUE_SETTER_HEAD(FUNCTIONNAME)                           \
    static void FUNCTIONNAME(                                                  \
        const ArkUI_NodeHandle& node,                                          \
        const std::shared_ptr<                                                 \
            TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet);  \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node, const double& val); \
    static void FUNCTIONNAME(                                                  \
        const ArkUI_NodeHandle& node,                                          \
        const Optional<TaroRuntime::Dimension>& val);                          \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node)

#define CREATE_ENUM_SETTER_HEAD(FUNCTIONNAME, ENUMNAME)                       \
    static void FUNCTIONNAME(                                                 \
        const ArkUI_NodeHandle& node,                                         \
        const std::shared_ptr<                                                \
            TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet); \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node, const int& val);   \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node,                    \
                             const Optional<ENUMNAME>& val);                  \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node)

#define CREATE_INT_SETTER_HEAD(FUNCTIONNAME)                                  \
    static void FUNCTIONNAME(                                                 \
        const ArkUI_NodeHandle& node,                                         \
        const std::shared_ptr<                                                \
            TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet); \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node, const int& val);   \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node,                    \
                             const Optional<int>& val);                       \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node)

#define CREATE_STRING_SETTER_HEAD(FUNCTIONNAME)                               \
    static void FUNCTIONNAME(                                                 \
        const ArkUI_NodeHandle& node,                                         \
        const std::shared_ptr<                                                \
            TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet); \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node,                    \
                             const std::string& val);                         \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node,                    \
                             const Optional<std::string>& val);               \
    static void FUNCTIONNAME(const ArkUI_NodeHandle& node)

        class HarmonyStyleSetter {
            public:
            CREATE_LENGTHVALUE_SETTER_HEAD(setLineHeight);
            CREATE_LENGTHVALUE_SETTER_HEAD(setFontSize);
            CREATE_LENGTHVALUE_SETTER_HEAD(setWidth);
            CREATE_LENGTHVALUE_SETTER_HEAD(setHeight);

            CREATE_FLOAT1_SETTER_HEAD(setLetterSpacing);
            CREATE_INT_SETTER_HEAD(setZIndex);

            CREATE_INT_SETTER_HEAD(setWebkitLineClamp);

            CREATE_STRING_SETTER_HEAD(setFontFamily);
            CREATE_ENUM_SETTER_HEAD(setWordBreak, OH_Drawing_WordBreakType);

            static void setBackgroundImage(const ArkUI_NodeHandle& node,
                                           const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
                                               stylesheet);
            static void setBackgroundImage(const ArkUI_NodeHandle& node, const Optional<BackgroundImageItem>& val, const Optional<ArkUI_ImageRepeat>& repeatVal = ARKUI_IMAGE_REPEAT_NONE);
            static void setBackgroundImage(const ArkUI_NodeHandle& node);

            static void setOpacity(
                const ArkUI_NodeHandle& node,
                const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
                    stylesheet);
            static void setOpacity(const ArkUI_NodeHandle& node, const double& val);
            static void setOpacity(const ArkUI_NodeHandle& node,
                                   const Optional<float>& val);
            static void setOpacity(const ArkUI_NodeHandle& node);

            static void setTransform(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet);
            static void setTransform(const ArkUI_NodeHandle& node, const TransformParam& value, float widthBase, float heightBase);
            static void setTransform(const ArkUI_NodeHandle& node, const TransformParam& value);
            static void setTransform(const ArkUI_NodeHandle& node, const Optional<TransformParam>& value);
            static void setTransform(const ArkUI_NodeHandle& node, const Optional<TransformParam>& value, float widthBase, float heightBase);
            static void setTransform(const ArkUI_NodeHandle& node, const std::vector<float>& value);
            static void setTransform(const ArkUI_NodeHandle& node);

            static void setTransformOrigin(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet);
            static void setTransformOrigin(const ArkUI_NodeHandle& node,
                                           const TransformOriginData& val);
            static void setTransformOrigin(const ArkUI_NodeHandle& node,
                                           const Optional<TransformOriginData>& val);
            static void setTransformOrigin(const ArkUI_NodeHandle& node);

            static void setBackgroundSize(
                const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet,
                const TaroDOM::ComputedStyle& computed_style);
            static void setBackgroundSize(
                const ArkUI_NodeHandle& node, const BackgroundSizeParam& value, const float& width, const float& height);
            static void setBackgroundSize(const ArkUI_NodeHandle& node, const Optional<BackgroundSizeParam>& value, const float& width, const float& heigh);
            static void setBackgroundSize(const ArkUI_NodeHandle& node);

            CREATE_ENUM_SETTER_HEAD(setOverflow, PropertyType::Overflow);
            CREATE_ENUM_SETTER_HEAD(setPointerEvents, PropertyType::PointerEvents);
            CREATE_ENUM_SETTER_HEAD(setVisibility, ArkUI_Visibility);
            CREATE_ENUM_SETTER_HEAD(setFontWeight, ArkUI_FontWeight);
            CREATE_ENUM_SETTER_HEAD(setFontStyle, ArkUI_FontStyle);

            CREATE_ENUM_SETTER_HEAD(setTextAlign, ArkUI_TextAlignment);
            CREATE_ENUM_SETTER_HEAD(setTextOverflow, ArkUI_TextOverflow);
            CREATE_ENUM_SETTER_HEAD(setVerticalAlign,
                                    OH_Drawing_PlaceholderVerticalAlignment);

            static void setBoxShadow(
                const ArkUI_NodeHandle& node,
                const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
                    stylesheet);
            static void setBoxShadow(const ArkUI_NodeHandle& node, const BoxShadowItem& val);
            static void setBoxShadow(const ArkUI_NodeHandle& node,
                                     const Optional<BoxShadowItem>& val);
            static void setBoxShadow(const ArkUI_NodeHandle& node);

            static void setBackgroundPosition(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet);
            static void setBackgroundPosition(const ArkUI_NodeHandle& node, const double& x, const double y);
            static void setBackgroundPosition(const ArkUI_NodeHandle& node, const Optional<Dimension>& x, const Optional<Dimension>& y);
            static void setBackgroundPosition(const ArkUI_NodeHandle& node, const Optional<Dimension>& x, const Optional<Dimension>& y, float widthBase, float heightBase, const Optional<TaroCSSOM::TaroStylesheet::BackgroundSizeParam>& backgroundSize);
            static void setBackgroundPosition(const ArkUI_NodeHandle& node);

            static void setColor(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet);
            static void setColor(const ArkUI_NodeHandle& node, const uint32_t& val);
            static void setColor(const ArkUI_NodeHandle& node, const Optional<uint32_t>& val);
            static void setColor(const ArkUI_NodeHandle& node);

            static void setBackgroundColor(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet);
            static void setBackgroundColor(const ArkUI_NodeHandle& node, const uint32_t& val);
            static void setBackgroundColor(const ArkUI_NodeHandle& node, const Optional<uint32_t>& val);
            static void setBackgroundColor(const ArkUI_NodeHandle& node);

            static void setBorderColor(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet);
            static void setBorderColor(const ArkUI_NodeHandle& node, const uint32_t& top, const uint32_t right, const uint32_t& bottom, const uint32_t& left);
            static void setBorderColor(const ArkUI_NodeHandle& node, const Optional<uint32_t>& top, const Optional<uint32_t>& right, const Optional<uint32_t>& bottom, const Optional<uint32_t>& left);
            static void setBorderColor(const ArkUI_NodeHandle& node);

            static void setBorderRadius(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet);
            static void setBorderRadius(const ArkUI_NodeHandle& node, double topLeft, double topRight, double bottomLeft, double bottomRight);

            static void setBorderRadius(const ArkUI_NodeHandle& node, const Optional<Dimension>& topLeft, const Optional<Dimension>& topRight, const Optional<Dimension>& bottomLeft, const Optional<Dimension>& bottomRight);
            static void setBorderRadius(const ArkUI_NodeHandle& node, const Optional<Dimension>& topLeft, const Optional<Dimension>& topRight, const Optional<Dimension>& bottomLeft, const Optional<Dimension>& bottomRight, float baseValue);
            static void setBorderRadius(const ArkUI_NodeHandle& node);

            static void setBorderStyle(const ArkUI_NodeHandle& node, const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>& stylesheet);
            static void setBorderStyle(const ArkUI_NodeHandle& node,
                                       const ArkUI_BorderStyle& top,
                                       const ArkUI_BorderStyle right,
                                       const ArkUI_BorderStyle& bottom,
                                       const ArkUI_BorderStyle& left);
            static void setBorderStyle(const ArkUI_NodeHandle& node,
                                       const Optional<ArkUI_BorderStyle>& top,
                                       const Optional<ArkUI_BorderStyle>& right,
                                       const Optional<ArkUI_BorderStyle>& bottom,
                                       const Optional<ArkUI_BorderStyle>& left);
            static void setBorderStyle(const ArkUI_NodeHandle& node);

            static void setBorderWidth(
                const ArkUI_NodeHandle& node,
                const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
                    stylesheet);
            static void setBorderWidth(const ArkUI_NodeHandle& node, const double& top,
                                       const double& right, const double& bottom,
                                       const double& left);
            static void setBorderWidth(
                const ArkUI_NodeHandle& node,
                const Optional<TaroRuntime::Dimension>& top,
                const Optional<TaroRuntime::Dimension>& right,
                const Optional<TaroRuntime::Dimension>& bottom,
                const Optional<TaroRuntime::Dimension>& left);
            static void setBorderWidth(const ArkUI_NodeHandle& node);

            static void setMargin(
                const ArkUI_NodeHandle& node,
                const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
                    stylesheet);
            static void setMargin(const ArkUI_NodeHandle& node, const double& top,
                                  const double& right, const double& bottom,
                                  const double& left);
            static void setMargin(
                const ArkUI_NodeHandle& node,
                const Optional<TaroRuntime::Dimension>& top,
                const Optional<TaroRuntime::Dimension>& right,
                const Optional<TaroRuntime::Dimension>& bottom,
                const Optional<TaroRuntime::Dimension>& left);
            static void setMargin(const ArkUI_NodeHandle& node);

            static void setPadding(
                const ArkUI_NodeHandle& node,
                const std::shared_ptr<TaroRuntime::TaroCSSOM::TaroStylesheet::Stylesheet>&
                    stylesheet);
            static void setPadding(const ArkUI_NodeHandle& node, const double& top,
                                   const double& right, const double& bottom,
                                   const double& left);
            static void setPadding(
                const ArkUI_NodeHandle& node,
                const Optional<TaroRuntime::Dimension>& top,
                const Optional<TaroRuntime::Dimension>& right,
                const Optional<TaroRuntime::Dimension>& bottom,
                const Optional<TaroRuntime::Dimension>& left);
            static void setPadding(const ArkUI_NodeHandle& node);

            static void setFloat(const ArkUI_NodeHandle& node,
                                 ArkUI_NodeAttributeType attributeType,
                                 const double& val);
            static void setFloat(const ArkUI_NodeHandle& node,
                                 ArkUI_NodeAttributeType attributeType,
                                 const double& val1, const double& val2);
            static void setFloat(const ArkUI_NodeHandle& node,
                                 ArkUI_NodeAttributeType attributeType,
                                 const Optional<float>& val);

            static void setFloat(
                const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
                const Optional<TaroRuntime::Dimension>& val);

            static void setFloat(
                const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
                const Optional<TaroRuntime::Dimension>& val1, const Optional<TaroRuntime::Dimension>& val2);



            static void setFloat(
                const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
                const Optional<Dimension>& val1,
                const Optional<Dimension>& val2, float val1Base, float val2base);

            static void setFloat(const ArkUI_NodeHandle& node,
                                 ArkUI_NodeAttributeType attributeType,
                                 double top,
                                 double right,
                                 double bottom,
                                 double left);
            static void setFloat(
                const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
                const Optional<TaroRuntime::Dimension>& top,
                const Optional<TaroRuntime::Dimension>& right,
                const Optional<TaroRuntime::Dimension>& bottom,
                const Optional<TaroRuntime::Dimension>& left);



            static void setFloat(
                const ArkUI_NodeHandle& node, ArkUI_NodeAttributeType attributeType,
                const Optional<Dimension>& top,
                const Optional<Dimension>& right,
                const Optional<Dimension>& bottom,
                const Optional<Dimension>& left,
                float baseValue);

            static void setUint(const ArkUI_NodeHandle& node,
                                ArkUI_NodeAttributeType attributeType,
                                const uint32_t& val1);

            static void setUint(const ArkUI_NodeHandle& node,
                                ArkUI_NodeAttributeType attributeType,
                                const Optional<uint32_t>& val1);

            static void setString(const ArkUI_NodeHandle& node,
                                  ArkUI_NodeAttributeType attributeType,
                                  const std::string& val);

            static void setString(const ArkUI_NodeHandle& node,
                                  ArkUI_NodeAttributeType attributeType,
                                  const Optional<std::string>& val);

            static void setInt(const ArkUI_NodeHandle& node,
                               ArkUI_NodeAttributeType attributeType, const int& val1);

            static void setInt(const ArkUI_NodeHandle& node,
                               ArkUI_NodeAttributeType attributeType,
                               const Optional<int>& val1);
        };

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_HARMONY_STYLE_SETTER_H
