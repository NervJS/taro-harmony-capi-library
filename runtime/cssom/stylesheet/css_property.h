//
// Created on 2024/5/29.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TESTNDK_PROPERTY_TYPE_H
#define TESTNDK_PROPERTY_TYPE_H

namespace CSSProperty {
enum Type {

    Invalid = 0,
    AlignContent = 1,
    JustifyContent = 2,
    AlignItems = 3,
    AlignSelf = 4,
    FlexBasis = 5,
    FlexDirection = 6,
    FlexGrow = 7,
    FlexShrink = 8,
    FlexWrap = 9,
    AspectRatio = 10,
    Display = 11,
    ColumnGap = 12,
    RowGap = 13,
    MarginLeft = 14,
    MarginRight = 15,
    MarginTop = 16,
    MarginBottom = 17,
    PaddingLeft = 18,
    PaddingRight = 19,
    PaddingTop = 20,
    PaddingBottom = 21,
    Width = 22,
    MinWidth = 23,
    MaxWidth = 24,
    Height = 25,
    MinHeight = 26,
    MaxHeight = 27,
    Overflow = 28,
    FontSize = 29,
    FontStyle = 30,
    FontFamily = 31,
    FontWeight = 32,
    LineHeight = 33,
    LetterSpacing = 34,
    VerticalAlign = 35,
    TextAlign = 36,
    TextDecoration = 37,
    TextShadow = 38,
    TextOverflow = 39,
    TextTransform = 40,
    Color = 41,
    BackgroundColor = 42,
    BackgroundImage = 43,
    BackgroundPosition = 44,
    BackgroundSize = 45,
    BackgroundRepeat = 46,
    BorderTopColor = 47,
    BorderRightColor = 48,
    BorderBottomColor = 49,
    BorderLeftColor = 50,
    BorderTopStyle = 51,
    BorderRightStyle = 52,
    BorderBottomStyle = 53,
    BorderLeftStyle = 54,
    BorderTopWidth = 55,
    BorderRightWidth = 56,
    BorderBottomWidth = 57,
    BorderLeftWidth = 58,
    BorderTopLeftRadius = 59,
    BorderTopRightRadius = 60,
    BorderBottomLeftRadius = 61,
    BorderBottomRightRadius = 62,
    BoxShadow = 63,
    ZIndex = 64,
    Position = 65,
    Top = 66,
    Right = 67,
    Bottom = 68,
    Left = 69,
    Visibility = 70,
    Opacity = 71,
    Transform = 72,
    TransformOrigin = 73,
    AnimationKeyFrames = 74,
    AnimationDuration = 75,
    AnimationTimingFunction = 76,
    AnimationDelay = 77,
    AnimationIterationCount = 78,
    Content = 79,
    WordBreak = 80,
    WebkitLineClamp = 81,
    AnimationFillMode = 82,
    AnimationPlayState = 83,
    BackgroundPositionX = 83,
    BackgroundPositionY = 84,
    Transition = 85,
    TransitionProperty = 86,
    TransitionDuration = 87,
    TransitionTimingFunction = 88,
    TransitionDelay = 89,
    WhiteSpace = 90,
    TextDecorationLine = 91,
    TextDecorationThickness = 92,
    TextDecorationStyle = 93,
    TextDecorationColor = 94,
    AnimationName = 95,
    BorderWidth = 96,
    BorderColor = 97,
    Margin = 98,
    Padding = 99,
    BorderRadius = 100,
    BoxOrient = 101,
    PointerEvents=102
};

}

namespace PropertyType {

enum class TransformType {
    MATRIX = 0,
    TRANSLATE = 1,
    SCALE = 2,
    ROTATE = 3,
    SKEW = 4,
};

enum class Overflow {
    Visible,
    Hidden,
    Scroll,
    Invalid,
};

enum class Display {
    None,
    Block,
    Flex,
    Box,
    UnKnown,
};

enum class WhiteSpace {
    NoWrap,
    Wrap
};

enum class Position { Static,
                      Relative,
                      Absolute,
                      Fixed,
                      Sticky };

enum class PointerEvents { Auto,
                           None };

enum class LengthUnit { UNKNOWN,
                        PX,
                        VH,
                        VW,
                        PERCENT,
                        VP };

enum class AngleUnit { UNKNOWN,
                       DEGREES,
                       GRADIANS,
                       RADIANS,
                       TURNS };

enum class BoxOrient {
    Horizontal = 0,
    Vertical,
    InlineAxis,
    BlockAxis,
};

} // namespace PropertyType

#endif // TESTNDK_PROPERTY_TYPE_H
