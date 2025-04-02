/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "./IAttribute.h"

#include "./utils.h"
#include "helper/string.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/parsers/mixed_parser.h"

namespace TaroRuntime {
namespace TaroCSSOM {
    namespace TaroStylesheet {

#define ASSIGN_PROPERTY(PROP)   \
    res->PROP.assign(s1->PROP); \
    res->PROP.assign(s2->PROP);

#define SWITCH_CASE(PROPERTY_TYPE, PROPERTY_NAME)                                  \
    case CSSProperty::PROPERTY_TYPE: {                                             \
        if (change.type == ChangeType::Modified ||                                 \
            change.type == ChangeType::Added) {                                    \
            PROPERTY_TYPE::staticSetValueToNode(node,                              \
                                                new_style->PROPERTY_NAME.value()); \
        } else {                                                                   \
            PROPERTY_TYPE::staticResetValueToNode(node);                           \
        }                                                                          \
        break;                                                                     \
    }

#define SET_PROPERTY_IF_NEED(PROPERTY_TYPE, PROPERTY_NAME) \
    if (new_style->PROPERTY_NAME.has_value())              \
        PROPERTY_TYPE::staticSetValueToNode(node, new_style->PROPERTY_NAME.value());

#define SET_PROPERTY_IF_NEED_WITH_PARAM1(PROPERTY_TYPE, PROPERTY_NAME, \
                                         PARAMS1)                      \
    if (new_style->PROPERTY_NAME.has_value())                          \
        PROPERTY_TYPE::staticSetValueToNode(                           \
            node, new_style->PROPERTY_NAME.value(), PARAMS1);

        Stylesheet::Stylesheet() {}

        template <typename T>
        void Stylesheet::compareAttributes(const CSSProperty::Type& name, const T& attr1, const T& attr2, std::vector<Change>& changes) {
            if (attr1.has_value() && !attr2.has_value()) {
                changes.emplace_back(Change{name, ChangeType::Removed});
            } else if (!attr1.has_value() && attr2.has_value()) {
                changes.emplace_back(Change{name, ChangeType::Added});
            } else if (attr1.has_value() && attr2.has_value() && !(attr1.isEqual(attr2))) {
                changes.emplace_back(Change{name, ChangeType::Modified});
            }
        }

        std::shared_ptr<Stylesheet> Stylesheet::assign(
            const std::shared_ptr<Stylesheet>& s1,
            const std::shared_ptr<Stylesheet>& s2) {
            if (!s1) return s2;
            if (!s2) return s1;
            std::shared_ptr<Stylesheet> res = std::make_shared<Stylesheet>();
            ASSIGN_PROPERTY(height);
            ASSIGN_PROPERTY(width);
            ASSIGN_PROPERTY(minWidth);
            ASSIGN_PROPERTY(minHeight);
            ASSIGN_PROPERTY(maxWidth);
            ASSIGN_PROPERTY(maxHeight);
            ASSIGN_PROPERTY(display);
            ASSIGN_PROPERTY(flexDirection);
            ASSIGN_PROPERTY(flexWrap);
            ASSIGN_PROPERTY(alignContent);
            ASSIGN_PROPERTY(justifyContent);
            ASSIGN_PROPERTY(alignItems);
            ASSIGN_PROPERTY(alignSelf);
            ASSIGN_PROPERTY(flexGrow);
            ASSIGN_PROPERTY(flexShrink);
            ASSIGN_PROPERTY(flexBasis);
            ASSIGN_PROPERTY(marginTop);
            ASSIGN_PROPERTY(marginRight);
            ASSIGN_PROPERTY(marginBottom);
            ASSIGN_PROPERTY(marginLeft);
            ASSIGN_PROPERTY(paddingTop);
            ASSIGN_PROPERTY(paddingRight);
            ASSIGN_PROPERTY(paddingBottom);
            ASSIGN_PROPERTY(paddingLeft);
            ASSIGN_PROPERTY(backgroundColor);
            ASSIGN_PROPERTY(backgroundRepeat);
            ASSIGN_PROPERTY(backgroundImage);
            ASSIGN_PROPERTY(borderTopLeftRadius);
            ASSIGN_PROPERTY(borderTopRightRadius);
            ASSIGN_PROPERTY(borderBottomLeftRadius);
            ASSIGN_PROPERTY(borderBottomRightRadius);
            ASSIGN_PROPERTY(opacity);
            ASSIGN_PROPERTY(fontSize);
            ASSIGN_PROPERTY(fontWeight);
            ASSIGN_PROPERTY(fontStyle);
            ASSIGN_PROPERTY(color);
            ASSIGN_PROPERTY(textDecorationStyle);
            ASSIGN_PROPERTY(textDecorationColor);
            ASSIGN_PROPERTY(textDecorationLine);
            ASSIGN_PROPERTY(fontFamily);
            ASSIGN_PROPERTY(visibility);
            ASSIGN_PROPERTY(overflow);
            ASSIGN_PROPERTY(boxShadow);
            ASSIGN_PROPERTY(borderLeftWidth);
            ASSIGN_PROPERTY(borderTopWidth);
            ASSIGN_PROPERTY(borderRightWidth);
            ASSIGN_PROPERTY(borderBottomWidth);
            ASSIGN_PROPERTY(borderTopColor);
            ASSIGN_PROPERTY(borderLeftColor);
            ASSIGN_PROPERTY(borderRightColor);
            ASSIGN_PROPERTY(borderBottomColor);
            ASSIGN_PROPERTY(borderLeftStyle);
            ASSIGN_PROPERTY(borderTopStyle);
            ASSIGN_PROPERTY(borderRightStyle);
            ASSIGN_PROPERTY(borderBottomStyle);
            ASSIGN_PROPERTY(position);
            ASSIGN_PROPERTY(top);
            ASSIGN_PROPERTY(left);
            ASSIGN_PROPERTY(right);
            ASSIGN_PROPERTY(bottom);
            ASSIGN_PROPERTY(backgroundSize);
            ASSIGN_PROPERTY(backgroundPositionX);
            ASSIGN_PROPERTY(backgroundPositionY);
            ASSIGN_PROPERTY(transformOrigin);
            ASSIGN_PROPERTY(transform);
            ASSIGN_PROPERTY(textAlign);
            ASSIGN_PROPERTY(letterSpacing);
            ASSIGN_PROPERTY(lineHeight);
            ASSIGN_PROPERTY(wordBreak);
            ASSIGN_PROPERTY(whiteSpace);
            ASSIGN_PROPERTY(textOverflow);
            ASSIGN_PROPERTY(verticalAlign);
            ASSIGN_PROPERTY(webkitLineClamp);
            ASSIGN_PROPERTY(zIndex);
            ASSIGN_PROPERTY(animationMulti);
            ASSIGN_PROPERTY(transition);
            ASSIGN_PROPERTY(boxShadow);
            ASSIGN_PROPERTY(boxOrient);
            ASSIGN_PROPERTY(pointerEvents);

            return res;
        }

        bool Stylesheet::compare(const std::shared_ptr<Stylesheet>& s1, const std::shared_ptr<Stylesheet>& s2) {
            if (!s1 && !s2) {
                return true;
            }
            if (!s1 || !s2) {
                return false;
            }
            return (
                s1->display == s2->display &&
                s1->width == s2->width &&
                s1->minWidth == s2->minWidth &&
                s1->maxWidth == s2->maxWidth &&
                s1->height == s2->height &&
                s1->minHeight == s2->minHeight &&
                s1->maxHeight == s2->maxHeight &&
                s1->flexDirection == s2->flexDirection &&
                s1->flexWrap == s2->flexWrap &&
                s1->alignContent == s2->alignContent &&
                s1->justifyContent == s2->justifyContent &&
                s1->alignItems == s2->alignItems &&
                s1->alignSelf == s2->alignSelf &&
                s1->flexGrow == s2->flexGrow &&
                s1->flexShrink == s2->flexShrink &&
                s1->flexBasis == s2->flexBasis &&
                s1->marginBottom == s2->marginBottom &&
                s1->marginTop == s2->marginTop &&
                s1->marginLeft == s2->marginLeft &&
                s1->marginRight == s2->marginRight &&
                s1->paddingBottom == s2->paddingBottom &&
                s1->paddingTop == s2->paddingTop &&
                s1->paddingLeft == s2->paddingLeft &&
                s1->paddingRight == s2->paddingRight &&
                s1->backgroundColor == s2->backgroundColor &&
                s1->backgroundImage == s2->backgroundImage &&
                s1->borderBottomLeftRadius == s2->borderBottomLeftRadius &&
                s1->borderBottomRightRadius == s2->borderBottomRightRadius &&
                s1->borderTopLeftRadius == s2->borderTopLeftRadius &&
                s1->borderTopRightRadius == s2->borderTopRightRadius &&
                s1->opacity == s2->opacity &&
                s1->fontSize == s2->fontSize &&
                s1->fontWeight == s2->fontWeight &&
                s1->fontStyle == s2->fontStyle &&
                s1->color == s2->color &&
                s1->textDecorationStyle == s2->textDecorationStyle &&
                s1->textDecorationLine == s2->textDecorationLine &&
                s1->textDecorationColor == s2->textDecorationColor &&
                s1->fontFamily == s2->fontFamily &&
                s1->visibility == s2->visibility &&
                s1->overflow == s2->overflow &&
                s1->boxShadow == s2->boxShadow &&
                s1->borderTopWidth == s2->borderTopWidth &&
                s1->borderRightWidth == s2->borderRightWidth &&
                s1->borderBottomWidth == s2->borderBottomWidth &&
                s1->borderLeftWidth == s2->borderLeftWidth &&
                s1->borderBottomColor == s2->borderBottomColor &&
                s1->borderTopColor == s2->borderTopColor &&
                s1->borderLeftColor == s2->borderLeftColor &&
                s1->borderRightColor == s2->borderRightColor &&
                s1->borderTopStyle == s2->borderTopStyle &&
                s1->borderRightStyle == s2->borderRightStyle &&
                s1->borderBottomStyle == s2->borderBottomStyle &&
                s1->borderLeftStyle == s2->borderLeftStyle &&
                s1->backgroundRepeat == s2->backgroundRepeat &&
                s1->position == s2->position &&
                s1->top == s2->top &&
                s1->left == s2->left &&
                s1->right == s2->right &&
                s1->bottom == s2->bottom &&
                s1->backgroundSize == s2->backgroundSize &&
                s1->backgroundPositionX == s2->backgroundPositionX &&
                s1->backgroundPositionY == s2->backgroundPositionY &&
                s1->transformOrigin == s2->transformOrigin &&
                s1->transform == s2->transform &&
                s1->textAlign == s2->textAlign &&
                s1->letterSpacing == s2->letterSpacing &&
                s1->lineHeight == s2->lineHeight &&
                s1->whiteSpace == s2->whiteSpace &&
                s1->wordBreak == s2->wordBreak &&
                s1->textOverflow == s2->textOverflow &&
                s1->verticalAlign == s2->verticalAlign &&
                s1->webkitLineClamp == s2->webkitLineClamp &&
                s1->zIndex == s2->zIndex &&
                s1->animationMulti == s2->animationMulti &&
                s1->transition == s2->transition &&
                s1->boxOrient == s2->boxOrient && s1->pointerEvents == s2->pointerEvents);
        }

        /**
         * 根据节点的 style 属性生成自身
         */
        std::shared_ptr<Stylesheet> Stylesheet::makeFromInlineStyle(napi_value node) {
            auto result = std::make_shared<Stylesheet>();
            NapiGetter getter(node);
            auto type = getter.Type();

            if (type == napi_object) {
                auto names = getter.GetAllPropertyNames();
                for (auto& iter : names) {
                    auto name = iter.String();
                    if (name.has_value()) {
                        auto valueGetter = getter.GetProperty(name.value().c_str());
                        if (valueGetter.GetType() == napi_string) {
                            auto value = valueGetter.String();
                            if (value.has_value()) {
                                result->parseCssProperty(name.value(), TaroHelper::string::trim(value.value()));
                            }
                        } else if (valueGetter.GetType() == napi_number) {
                            auto value = std::to_string(valueGetter.Double(0.0f));
                            result->parseCssProperty(name.value(), TaroHelper::string::trim(value));
                        }
                    }
                }
            } else if (type == napi_string) {
                auto styleStr = getter.String();
                if (!styleStr.has_value()) return result;

                std::string_view sv(styleStr.value());
                size_t start = 0;

                while (start < sv.size()) {
                    size_t end = sv.find(';', start);
                    if (end == std::string_view::npos) {
                        end = sv.size();
                    }

                    std::string_view property = sv.substr(start, end - start);
                    size_t colonPos = property.find(':');
                    if (colonPos != std::string_view::npos) {
                        std::string_view key = property.substr(0, colonPos);
                        std::string_view value = property.substr(colonPos + 1);
                
                        // 去除键和值两端的空格
                        key = TaroHelper::string::trim(key);
                        value = TaroHelper::string::trim(value);

                        result->parseCssProperty(key, value);
                    }

                    start = end + 1;
                }
            }

            return result;
        }

        // 映射到 Stylesheet 每个属性的解析函数
        static std::unordered_map<std::string_view, std::function<void(Stylesheet*, std::string_view)>> parseHandles = {
            {"opacity", [](Stylesheet* ss, std::string_view value) { ss->opacity.setValueFromStringView(value); }},
            {"overflow", [](Stylesheet* ss, std::string_view value) { ss->overflow.setValueFromStringView(value); }},
            {"visibility", [](Stylesheet* ss, std::string_view value) { ss->visibility.setValueFromStringView(value); }},
            {"display", [](Stylesheet* ss, std::string_view value) { ss->display.setValueFromStringView(value); }},
            {"width", [](Stylesheet* ss, std::string_view value) { ss->width.setValueFromStringView(value); }},
            {"minWidth", [](Stylesheet* ss, std::string_view value) { ss->minWidth.setValueFromStringView(value); }},
            {"min-width", [](Stylesheet* ss, std::string_view value) { ss->minWidth.setValueFromStringView(value); }},
            {"maxWidth", [](Stylesheet* ss, std::string_view value) { ss->maxWidth.setValueFromStringView(value); }},
            {"max-width", [](Stylesheet* ss, std::string_view value) { ss->maxWidth.setValueFromStringView(value); }},
            {"height", [](Stylesheet* ss, std::string_view value) { ss->height.setValueFromStringView(value); }},
            {"minHeight", [](Stylesheet* ss, std::string_view value) { ss->minHeight.setValueFromStringView(value); }},
            {"min-height", [](Stylesheet* ss, std::string_view value) { ss->minHeight.setValueFromStringView(value); }},
            {"maxHeight", [](Stylesheet* ss, std::string_view value) { ss->maxHeight.setValueFromStringView(value); }},
            {"max-height", [](Stylesheet* ss, std::string_view value) { ss->maxHeight.setValueFromStringView(value); }},

            {"box-shadow", [](Stylesheet* ss, std::string_view value) { ss->boxShadow.setValueFromStringView(value); }},
            {"boxShadow", [](Stylesheet* ss, std::string_view value) { ss->boxShadow.setValueFromStringView(value); }},

            {"position", [](Stylesheet* ss, std::string_view value) { ss->position.setValueFromStringView(value); }},
            {"top", [](Stylesheet* ss, std::string_view value) { ss->top.setValueFromStringView(value); }},
            {"right", [](Stylesheet* ss, std::string_view value) { ss->right.setValueFromStringView(value); }},
            {"bottom", [](Stylesheet* ss, std::string_view value) { ss->bottom.setValueFromStringView(value); }},
            {"left", [](Stylesheet* ss, std::string_view value) { ss->left.setValueFromStringView(value); }},
            {"zIndex", [](Stylesheet* ss, std::string_view value) { ss->zIndex.setValueFromStringView(value); }},
            {"z-index", [](Stylesheet* ss, std::string_view value) { ss->zIndex.setValueFromStringView(value); }},

            {"padding", [](Stylesheet* ss, std::string_view value) { parsePadding(ss, value); }},
            {"paddingTop", [](Stylesheet* ss, std::string_view value) { ss->paddingTop.setValueFromStringView(value); }},
            {"padding-top", [](Stylesheet* ss, std::string_view value) { ss->paddingTop.setValueFromStringView(value); }},
            {"paddingRight", [](Stylesheet* ss, std::string_view value) { ss->paddingRight.setValueFromStringView(value); }},
            {"padding-right", [](Stylesheet* ss, std::string_view value) { ss->paddingRight.setValueFromStringView(value); }},
            {"paddingBottom", [](Stylesheet* ss, std::string_view value) { ss->paddingBottom.setValueFromStringView(value); }},
            {"padding-bottom", [](Stylesheet* ss, std::string_view value) { ss->paddingBottom.setValueFromStringView(value); }},
            {"paddingLeft", [](Stylesheet* ss, std::string_view value) { ss->paddingLeft.setValueFromStringView(value); }},
            {"padding-left", [](Stylesheet* ss, std::string_view value) { ss->paddingLeft.setValueFromStringView(value); }},

            {"margin", [](Stylesheet* ss, std::string_view value) { parseMargin(ss, value); }},
            {"marginTop", [](Stylesheet* ss, std::string_view value) { ss->marginTop.setValueFromStringView(value); }},
            {"margin-top", [](Stylesheet* ss, std::string_view value) { ss->marginTop.setValueFromStringView(value); }},
            {"marginRight", [](Stylesheet* ss, std::string_view value) { ss->marginRight.setValueFromStringView(value); }},
            {"margin-right", [](Stylesheet* ss, std::string_view value) { ss->marginRight.setValueFromStringView(value); }},
            {"marginBottom", [](Stylesheet* ss, std::string_view value) { ss->marginBottom.setValueFromStringView(value); }},
            {"margin-bottom", [](Stylesheet* ss, std::string_view value) { ss->marginBottom.setValueFromStringView(value); }},
            {"marginLeft", [](Stylesheet* ss, std::string_view value) { ss->marginLeft.setValueFromStringView(value); }},
            {"margin-left", [](Stylesheet* ss, std::string_view value) { ss->marginLeft.setValueFromStringView(value); }},

            {"border", [](Stylesheet* ss, std::string_view value) { parseBorder(ss, value, 4); }},
            {"borderTop", [](Stylesheet* ss, std::string_view value) { parseBorder(ss, value, 0); }},
            {"border-top", [](Stylesheet* ss, std::string_view value) { parseBorder(ss, value, 0); }},
            {"borderRight", [](Stylesheet* ss, std::string_view value) { parseBorder(ss, value, 1); }},
            {"border-right", [](Stylesheet* ss, std::string_view value) { parseBorder(ss, value, 1); }},
            {"borderBottom", [](Stylesheet* ss, std::string_view value) { parseBorder(ss, value, 2); }},
            {"border-bottom", [](Stylesheet* ss, std::string_view value) { parseBorder(ss, value, 2); }},
            {"borderLeft", [](Stylesheet* ss, std::string_view value) { parseBorder(ss, value, 3); }},
            {"border-left", [](Stylesheet* ss, std::string_view value) { parseBorder(ss, value, 3); }},

            {"borderWidth", [](Stylesheet* ss, std::string_view value) { parseBorderWidth(ss, value); }},
            {"border-width", [](Stylesheet* ss, std::string_view value) { parseBorderWidth(ss, value); }},
            {"borderTopWidth", [](Stylesheet* ss, std::string_view value) { ss->borderTopWidth.setValueFromStringView(value); }},
            {"border-top-width", [](Stylesheet* ss, std::string_view value) { ss->borderTopWidth.setValueFromStringView(value); }},
            {"borderRightWidth", [](Stylesheet* ss, std::string_view value) { ss->borderRightWidth.setValueFromStringView(value); }},
            {"border-right-width", [](Stylesheet* ss, std::string_view value) { ss->borderRightWidth.setValueFromStringView(value); }},
            {"borderBottomWidth", [](Stylesheet* ss, std::string_view value) { ss->borderBottomWidth.setValueFromStringView(value); }},
            {"border-bottom-width", [](Stylesheet* ss, std::string_view value) { ss->borderBottomWidth.setValueFromStringView(value); }},
            {"borderLeftWidth", [](Stylesheet* ss, std::string_view value) { ss->borderLeftWidth.setValueFromStringView(value); }},
            {"border-left-width", [](Stylesheet* ss, std::string_view value) { ss->borderLeftWidth.setValueFromStringView(value); }},

            {"borderStyle", [](Stylesheet* ss, std::string_view value) { parseBorderStyle(ss, value); }},
            {"border-style", [](Stylesheet* ss, std::string_view value) { parseBorderStyle(ss, value); }},
            {"borderTopStyle", [](Stylesheet* ss, std::string_view value) { ss->borderTopStyle.setValueFromStringView(value); }},
            {"border-top-style", [](Stylesheet* ss, std::string_view value) { ss->borderTopStyle.setValueFromStringView(value); }},
            {"borderRightStyle", [](Stylesheet* ss, std::string_view value) { ss->borderRightStyle.setValueFromStringView(value); }},
            {"border-right-style", [](Stylesheet* ss, std::string_view value) { ss->borderRightStyle.setValueFromStringView(value); }},
            {"borderBottomStyle", [](Stylesheet* ss, std::string_view value) { ss->borderBottomStyle.setValueFromStringView(value); }},
            {"border-bottom-style", [](Stylesheet* ss, std::string_view value) { ss->borderBottomStyle.setValueFromStringView(value); }},
            {"borderLeftStyle", [](Stylesheet* ss, std::string_view value) { ss->borderLeftStyle.setValueFromStringView(value); }},
            {"border-left-style", [](Stylesheet* ss, std::string_view value) { ss->borderLeftStyle.setValueFromStringView(value); }},

            {"borderColor", [](Stylesheet* ss, std::string_view value) { parseBorderColor(ss, value); }},
            {"border-color", [](Stylesheet* ss, std::string_view value) { parseBorderColor(ss, value); }},
            {"borderTopColor", [](Stylesheet* ss, std::string_view value) { ss->borderTopColor.setValueFromStringView(value); }},
            {"border-top-color", [](Stylesheet* ss, std::string_view value) { ss->borderTopColor.setValueFromStringView(value); }},
            {"borderRightColor", [](Stylesheet* ss, std::string_view value) { ss->borderRightColor.setValueFromStringView(value); }},
            {"border-right-color", [](Stylesheet* ss, std::string_view value) { ss->borderRightColor.setValueFromStringView(value); }},
            {"borderBottomColor", [](Stylesheet* ss, std::string_view value) { ss->borderBottomColor.setValueFromStringView(value); }},
            {"border-bottom-color", [](Stylesheet* ss, std::string_view value) { ss->borderBottomColor.setValueFromStringView(value); }},
            {"borderLeftColor", [](Stylesheet* ss, std::string_view value) { ss->borderLeftColor.setValueFromStringView(value); }},
            {"border-left-color", [](Stylesheet* ss, std::string_view value) { ss->borderLeftColor.setValueFromStringView(value); }},

            {"borderRadius", [](Stylesheet* ss, std::string_view value) { parseBorderRadius(ss, value); }},
            {"border-radius", [](Stylesheet* ss, std::string_view value) { parseBorderRadius(ss, value); }},
            {"borderTopLeftRadius", [](Stylesheet* ss, std::string_view value) { ss->borderTopLeftRadius.setValueFromStringView(value); }},
            {"border-top-left-radius", [](Stylesheet* ss, std::string_view value) { ss->borderTopLeftRadius.setValueFromStringView(value); }},
            {"borderTopRightRadius", [](Stylesheet* ss, std::string_view value) { ss->borderTopRightRadius.setValueFromStringView(value); }},
            {"border-top-right-radius", [](Stylesheet* ss, std::string_view value) { ss->borderTopRightRadius.setValueFromStringView(value); }},
            {"borderBottomLeftRadius", [](Stylesheet* ss, std::string_view value) { ss->borderBottomLeftRadius.setValueFromStringView(value); }},
            {"border-bottom-left-radius", [](Stylesheet* ss, std::string_view value) { ss->borderBottomLeftRadius.setValueFromStringView(value); }},
            {"borderBottomRightRadius", [](Stylesheet* ss, std::string_view value) { ss->borderBottomRightRadius.setValueFromStringView(value); }},
            {"border-bottom-right-radius", [](Stylesheet* ss, std::string_view value) { ss->borderBottomRightRadius.setValueFromStringView(value); }},

            {"transformOrigin", [](Stylesheet* ss, std::string_view value) { ss->transformOrigin.setValueFromStringView(value); }},
            {"transform-origin", [](Stylesheet* ss, std::string_view value) { ss->transformOrigin.setValueFromStringView(value); }},
            {"transform", [](Stylesheet* ss, std::string_view value) { ss->transform.setValueFromStringView(value); }},

            {"background", [](Stylesheet* ss, std::string_view value) { parseBackground(ss, value); }},
            {"backgroundPosition", [](Stylesheet* ss, std::string_view value) { parseBackgroundPosition(ss, value); }},
            {"background-position", [](Stylesheet* ss, std::string_view value) { parseBackgroundPosition(ss, value); }},
            {"backgroundColor", [](Stylesheet* ss, std::string_view value) { ss->backgroundColor.setValueFromStringView(value); }},
            {"background-color", [](Stylesheet* ss, std::string_view value) { ss->backgroundColor.setValueFromStringView(value); }},
            {"backgroundImage", [](Stylesheet* ss, std::string_view value) { ss->backgroundImage.setValueFromStringView(value); }},
            {"background-image", [](Stylesheet* ss, std::string_view value) { ss->backgroundImage.setValueFromStringView(value); }},
            {"backgroundSize", [](Stylesheet* ss, std::string_view value) { ss->backgroundSize.setValueFromStringView(value); }},
            {"background-size", [](Stylesheet* ss, std::string_view value) { ss->backgroundSize.setValueFromStringView(value); }},
            {"backgroundPositionX", [](Stylesheet* ss, std::string_view value) { ss->backgroundPositionX.setValueFromStringView(value); }},
            {"background-position-x", [](Stylesheet* ss, std::string_view value) { ss->backgroundPositionX.setValueFromStringView(value); }},
            {"backgroundPositionY", [](Stylesheet* ss, std::string_view value) { ss->backgroundPositionY.setValueFromStringView(value); }},
            {"background-position-y", [](Stylesheet* ss, std::string_view value) { ss->backgroundPositionY.setValueFromStringView(value); }},
            {"backgroundRepeat", [](Stylesheet* ss, std::string_view value) { ss->backgroundRepeat.setValueFromStringView(value); }},
            {"background-repeat", [](Stylesheet* ss, std::string_view value) { ss->backgroundRepeat.setValueFromStringView(value); }},

            {"text-decoration", [](Stylesheet* ss, std::string_view value) { parseTextDecoration(ss, value); }},
            {"textDecoration", [](Stylesheet* ss, std::string_view value) { parseTextDecoration(ss, value); }},
            {"text-decoration-line", [](Stylesheet* ss, std::string_view value) { ss->textDecorationLine.setValueFromStringView(value); }},
            {"textDecorationLine", [](Stylesheet* ss, std::string_view value) { ss->textDecorationLine.setValueFromStringView(value); }},
            {"text-decoration-color", [](Stylesheet* ss, std::string_view value) { ss->textDecorationColor.setValueFromStringView(value); }},
            {"textDecorationColor", [](Stylesheet* ss, std::string_view value) { ss->textDecorationColor.setValueFromStringView(value); }},
            {"text-decoration-style", [](Stylesheet* ss, std::string_view value) { ss->textDecorationStyle.setValueFromStringView(value); }},
            {"textDecorationStyle", [](Stylesheet* ss, std::string_view value) { ss->textDecorationStyle.setValueFromStringView(value); }},

            {"pointerEvents", [](Stylesheet* ss, std::string_view value) { ss->pointerEvents.setValueFromStringView(value); }},
            {"pointer-events", [](Stylesheet* ss, std::string_view value) { ss->pointerEvents.setValueFromStringView(value); }},
            {"color", [](Stylesheet* ss, std::string_view value) { ss->color.setValueFromStringView(value); }},
            {"fontSize", [](Stylesheet* ss, std::string_view value) { ss->fontSize.setValueFromStringView(value); }},
            {"font-size", [](Stylesheet* ss, std::string_view value) { ss->fontSize.setValueFromStringView(value); }},
            {"fontWeight", [](Stylesheet* ss, std::string_view value) { ss->fontWeight.setValueFromStringView(value); }},
            {"font-weight", [](Stylesheet* ss, std::string_view value) { ss->fontWeight.setValueFromStringView(value); }},
            {"fontStyle", [](Stylesheet* ss, std::string_view value) { ss->fontStyle.setValueFromStringView(value); }},
            {"font-style", [](Stylesheet* ss, std::string_view value) { ss->fontStyle.setValueFromStringView(value); }},
            {"fontFamily", [](Stylesheet* ss, std::string_view value) { ss->fontFamily.setValueFromStringView(value); }},
            {"font-family", [](Stylesheet* ss, std::string_view value) { ss->fontFamily.setValueFromStringView(value); }},
            {"textAlign", [](Stylesheet* ss, std::string_view value) { ss->textAlign.setValueFromStringView(value); }},
            {"text-align", [](Stylesheet* ss, std::string_view value) { ss->textAlign.setValueFromStringView(value); }},
            {"letterSpacing", [](Stylesheet* ss, std::string_view value) { ss->letterSpacing.setValueFromStringView(value); }},
            {"letter-spacing", [](Stylesheet* ss, std::string_view value) { ss->letterSpacing.setValueFromStringView(value); }},
            {"lineHeight", [](Stylesheet* ss, std::string_view value) { ss->lineHeight.setValueFromStringView(value); }},
            {"line-height", [](Stylesheet* ss, std::string_view value) { ss->lineHeight.setValueFromStringView(value); }},
            {"whiteSpace", [](Stylesheet* ss, std::string_view value) { ss->whiteSpace.setValueFromStringView(value); }},
            {"white-space", [](Stylesheet* ss, std::string_view value) { ss->whiteSpace.setValueFromStringView(value); }},
            {"wordBreak", [](Stylesheet* ss, std::string_view value) { ss->wordBreak.setValueFromStringView(value); }},
            {"word-break", [](Stylesheet* ss, std::string_view value) { ss->wordBreak.setValueFromStringView(value); }},
            {"textOverflow", [](Stylesheet* ss, std::string_view value) { ss->textOverflow.setValueFromStringView(value); }},
            {"text-overflow", [](Stylesheet* ss, std::string_view value) { ss->textOverflow.setValueFromStringView(value); }},
            {"verticalAlign", [](Stylesheet* ss, std::string_view value) { ss->verticalAlign.setValueFromStringView(value); }},
            {"vertical-align", [](Stylesheet* ss, std::string_view value) { ss->verticalAlign.setValueFromStringView(value); }},
            {"webkitLineClamp", [](Stylesheet* ss, std::string_view value) { ss->webkitLineClamp.setValueFromStringView(value); }},
            {"webkit-line-clamp", [](Stylesheet* ss, std::string_view value) { ss->webkitLineClamp.setValueFromStringView(value); }},

            {"flex", [](Stylesheet* ss, std::string_view value) { parseFlex(ss, value); }},
            {"flexFlow", [](Stylesheet* ss, std::string_view value) { parseFlexFlow(ss, value); }},
            {"flexDirection", [](Stylesheet* ss, std::string_view value) { ss->flexDirection.setValueFromStringView(value); }},
            {"flex-direction", [](Stylesheet* ss, std::string_view value) { ss->flexDirection.setValueFromStringView(value); }},
            {"flexWrap", [](Stylesheet* ss, std::string_view value) { ss->flexWrap.setValueFromStringView(value); }},
            {"flex-wrap", [](Stylesheet* ss, std::string_view value) { ss->flexWrap.setValueFromStringView(value); }},
            {"alignItems", [](Stylesheet* ss, std::string_view value) { ss->alignItems.setValueFromStringView(value); }},
            {"align-items", [](Stylesheet* ss, std::string_view value) { ss->alignItems.setValueFromStringView(value); }},
            {"alignSelf", [](Stylesheet* ss, std::string_view value) { ss->alignSelf.setValueFromStringView(value); }},
            {"align-self", [](Stylesheet* ss, std::string_view value) { ss->alignSelf.setValueFromStringView(value); }},
            {"alignContent", [](Stylesheet* ss, std::string_view value) { ss->alignContent.setValueFromStringView(value); }},
            {"align-content", [](Stylesheet* ss, std::string_view value) { ss->alignContent.setValueFromStringView(value); }},
            {"justifyContent", [](Stylesheet* ss, std::string_view value) { ss->justifyContent.setValueFromStringView(value); }},
            {"justify-content", [](Stylesheet* ss, std::string_view value) { ss->justifyContent.setValueFromStringView(value); }},
            {"flexGrow", [](Stylesheet* ss, std::string_view value) { ss->flexGrow.setValueFromStringView(value); }},
            {"flex-grow", [](Stylesheet* ss, std::string_view value) { ss->flexGrow.setValueFromStringView(value); }},
            {"flexShrink", [](Stylesheet* ss, std::string_view value) { ss->flexShrink.setValueFromStringView(value); }},
            {"flex-shrink", [](Stylesheet* ss, std::string_view value) { ss->flexShrink.setValueFromStringView(value); }},
            {"flexBasis", [](Stylesheet* ss, std::string_view value) { ss->flexBasis.setValueFromStringView(value); }},
            {"flex-basis", [](Stylesheet* ss, std::string_view value) { ss->flexBasis.setValueFromStringView(value); }},
            {"box-orient", [](Stylesheet* ss, std::string_view value) { ss->boxOrient.setValueFromStringView(value); }},
            {"-webkit-box-orient", [](Stylesheet* ss, std::string_view value) { ss->boxOrient.setValueFromStringView(value); }},
        };

        /**
         * 属性键值对解析
         * 注：方法内部做去除字符串两端空格的处理
         */
        void Stylesheet::parseCssProperty(std::string_view name, std::string_view value) {
            if (name.empty() || value.empty()) return;

            auto it = parseHandles.find(name);
            if (it != parseHandles.end()) {
                it->second(this, value);
            }
        }

    } // namespace TaroStylesheet
} // namespace TaroCSSOM
} // namespace TaroRuntime