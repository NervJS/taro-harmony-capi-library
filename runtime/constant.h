/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TESTNDK_CONSTANT_H
#define TESTNDK_CONSTANT_H

#include <cstdint>
#include <map>
#include <string>

namespace TaroRuntime {
enum NODE_TYPE {
    ELEMENT_NODE = 1,
    ATTRIBUTE_NODE = 2,
    TEXT_NODE = 3,
    CDATA_SECTION_NODE = 4,
    ENTITY_REFERENCE_NODE = 5,
    PROCESSING_INSTRUCTION_NODE = 7,
    COMMENT_NODE = 8,
    DOCUMENT_NODE = 9,
    DOCUMENT_TYPE_NODE = 10,
    DOCUMENT_FRAGMENT_NODE = 11
};

enum class TAG_NAME {
    UNKNOWN = 0,
    DOCUMENT = 1,
    HTML = 2,
    HEAD = 3,
    BODY = 4,
    CONTAINER = 5,
    APP = 6,
    PAGE = 7,
    VIEW = 8,
    SCROLL_VIEW = 9,
    TEXT = 10,
    IMAGE = 11,
    BUTTON = 12,
    INPUT = 13,
    TEXTAREA = 14,
    SWIPER = 15,
    SPAN = 16,
    ICON = 17,
    MOVABLE_AREA = 18,
    MOVABLE_VIEW = 19,
    LIST = 20,
    LIST_ITEM = 21,
    WATER_FLOW = 22,
    FLOW_ITEM = 23,
    SWIPER_ITEM = 24,
    FLOW_SECTION = 25,
    PROGRESS = 26,
    PICKER = 27,
    STICKY_SECTION = 28,
    STICKY_HEADER = 29,
    CHECKBOX = 30,
    CHECKBOX_GROUP = 31,
    RADIO = 32,
    RADIO_GROUP = 33,
    LABEL = 34,
    FORM = 35,
    ENTRY_ASYNC = 36,
    CANVAS = 37,
    SLIDER = 38,
};

extern std::map<std::string, TAG_NAME> TAG_NAME_MAP;

enum class ATTRIBUTE_NAME {
    UNKNOWN = 0,
    ID = 1,
    CLASS = 2,
    STYLE = 3,
    SRC = 4,
    ALT = 5,
    WIDTH = 6,
    HEIGHT = 7,
    VALUE = 8,
    PLACEHOLDER = 9,
    TYPE = 10,
    NAME = 11,
    DISABLED = 12,
    READONLY = 13,
    REQUIRED = 14,
    TEXT_CONTENT = 15,
    SIZE = 16,
    COLOR = 17,
    ANIMATION = 18,
    REUSE_ID = 19,

    // attribute for swiper 21~40
    INDEX = 21,
    LOOP = 22,
    DURATION = 23,
    INTERVAL = 24,
    VERTICAL = 25,
    AUTOPLAY = 26,
    INDICATOR = 27,
    INDICATOR_COLOR = 28,
    INDICATOR_ACTIVE_COLOR = 29,
    NEXT_MARGIN = 30,
    PREV_MARGIN = 31,
    DISPLAY_COUNT = 32,

    // attribute for movable component 41~60
    DIRECTION = 41,
    OUT_OF_BOUNDS = 42,
    POSITION_X = 43,
    POSITION_Y = 44,
    SCALE = 45,
    SCALE_MIN = 46,
    SCALE_MAX = 47,
    SCALE_VALUE = 48,
    SCALE_AREA = 49,
    CHECKED = 50,
    SHAPE = 51,

    // attribute for scroll-view component 61~80
    SCROLL_X = 61,
    SCROLL_Y = 62,
    SHOW_SCROLL_BAR = 63,
    SCROLL_TOP = 64,
    SCROLL_LEFT = 65,
    SCROLL_WITH_ANIMATION = 66,
    SCROLL_ANIMATION_DURATION = 67,
    COLUMN = 68,
    LOWER_THRESHOLD_COUNT = 69,
    UPPER_THRESHOLD_COUNT = 70,
    CACHE_COUNT = 71,
    ROW_GAP = 72,
    COLUMN_GAP = 73,
    STICKY_HEADER = 74,
    SPACE = 75,

    // attribute for text input component 81~100
    PASSWORD = 81,
    CONFIRM_TYPE = 82,
    MAX_LENGTH = 83,
    FOCUS = 84,
    PLACEHOLDER_STYLE = 85,
    PLACEHOLDER_TEXT_COLOR = 86,
    ADJUST_POSITION = 87,
    EDITING = 88,
    AUTO_HEIGHT,

    // attribute for slider component 101~120
    STEP = 101,
    SHOW_VALUE = 102,
    MIN = 103,
    MAX = 104,
    ACTIVE_COLOR = 105,
    BLOCK_COLOR = 106,
    BLOCK_SIZE = 108,

    LAZY_LOAD,
    ERROR_HOLDER,
    MODE,
    LABEL_FOR,
    CATCH_MOVE,

    // attribute for progress component 121~140
    PERCENT = 121,
    SHOW_INFO,
    BORDER_RADIUS,
    FONT_SIZE,
    STROKE_WIDTH,
    BACKGROUND_COLOR,
    ACTIVE,
    ACTIVE_MODE,

    // attribute for picker component 141~160
    RANGE = 141,
    RANGE_KEY,
    OKText,
    CANCELText,
    START,
    END,
    LEVEL,
    MARGIN,

    DISABLE_TOUCH,
    DISABLE_PROGRAMMATIC_ANIMATION
};

extern std::map<std::string, ATTRIBUTE_NAME> ATTRIBUTE_NAME_MAP;

enum class PROPERTY_NAME {
    UNKNOWN = 0,
    CLIENT_TOP = 1,
    CLIENT_LEFT = 2,
    CLIENT_HEIGHT = 3,
    CLIENT_WIDTH = 4,
    SCROLL_TOP = 5,
    SCROLL_LEFT = 6,
    SCROLL_HEIGHT = 7,
    SCROLL_WIDTH = 8,
    OFFSET_TOP = 9,
    OFFSET_LEFT = 10,
    OFFSET_HEIGHT = 11,
    OFFSET_WIDTH = 12,
    OFFSET_PARENT = 13,
    INNER_TEXT = 14,
    INNER_HTML = 15,
    OUTER_HTML = 16,
    TEXT_CONTENT = 17,
    CHILD_NODES = 18,
    CHILDREN = 19,
    DATASET = 20,
    FIRST_CHILD = 21,
    FIRST_ELEMENT_CHILD = 22,
    LAST_CHILD = 23,
    LAST_ELEMENT_CHILD = 24,
    PARENT_ELEMENT = 25,
    PARENT_NODE = 26,
    PREVIOUS_ELEMENT_SIBLING = 27,
    PREVIOUS_SIBLING = 28,
    NEXT_ELEMENT_SIBLING = 29,
    NEXT_SIBLING = 30,
    OWNER_DOCUMENT = 31,
};

extern std::map<std::string, PROPERTY_NAME> PROPERTY_NAME_MAP;

enum class FUNC_CODE {
    UNKNOWN = 0,
    CLASSLIST_ADD = 1,
    CLASSLIST_REMOVE = 2,
    CLASSLIST_TOGGLE = 3,
    CLASSLIST_CONTAINS = 4,
    CLASSLIST_REPLACE = 5,
    CLASSLIST_TO_STRING = 6,
    CLASSLIST_LENGTH = 7,
};

extern std::map<std::string, FUNC_CODE> FUNC_CODE_MAP;

enum X_COMPONENT_STAGE {
    LOAD = 0,
    SHOW = 1,
    HIDE = 2,
    UNLOAD = 3,
};

enum class STATE_FLAG : std::uint8_t {
    // 表示节点的计算样式失效
    IS_COMPUTED_STYLE_INVALID_FLAG = 1 << 0,
    // 表示整个子树的计算样式失效
    IS_SUBTREE_COMPUTED_STYLE_INVALID_FLAG = 2 << 0,
    // 表示后代需要进行样式失效处理
    DESCENDANT_NEED_STYLE_RESOLUTION = 3 << 0,
    // 表示节点需要马上插入
    IMMEDIATE_ATTACH_TO_TREE = 4 << 0,
};

namespace KEY_FRAME_VSYNC {
    static std::string DIRTY = "dirty";
}

enum class TaroEdge : uint8_t {
    TaroEdgeTop = 0,
    TaroEdgeRight = 1,
    TaroEdgeBottom = 2,
    TaroEdgeLeft = 3,
    TaroEdgeAll = 4
};

enum class TaroChange : uint8_t {
    Added,
    Modified,
    Removed,
    NotChange
};

// 延迟执行器，用于等待布局绘制完成后，操作的根节点挂上树
enum class OperationType {
    AddChild,
    InsertChildAt,
    RemoveChild
};

struct DelayedCall {
    OperationType opType;
    std::function<void()> func;

    DelayedCall(OperationType type, std::function<void()> f)
        : opType(type), func(f) {}
};

} // namespace TaroRuntime

#endif // TESTNDK_CONSTANT_H
