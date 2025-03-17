//
// Created on 2024/6/13.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#include "button.h"

#include <arkui/native_node.h>

#include "arkjs/ArkJS.h"
#include "helper/TaroLog.h"
#include "helper/TaroTimer.h"
#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/dom/ark_nodes/button.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroButton::TaroButton(napi_value node)
        : TaroElement(node),
          default_height_(Dimension{92, DimensionUnit::DESIGN_PX}),
          default_mini_height_(Dimension{60, DimensionUnit::DESIGN_PX}),
          default_width_(Dimension{1, DimensionUnit::PERCENT}),
          default_mini_width_(Dimension{120, DimensionUnit::DESIGN_PX}),
          default_radius_(Dimension{10, DimensionUnit::DESIGN_PX}) {
        attributes_ = std::make_unique<TaroButtonAttributes>();
    }

    void TaroButton::Build() {
        if (!is_init_) {
            // 构建button render node
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_button = std::make_shared<TaroButtonNode>(element);
            render_button->Build();
            SetRenderNode(render_button);
        }
    }

    bool TaroButton::bindListenEvent(const std::string& event_name) {
        if (event_name == "click") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else if (event_name == "touchstart") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_START, event_name);
        } else if (event_name == "touchmove") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_MOVE, event_name);
        } else if (event_name == "touchend") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_END, event_name);
        } else if (event_name == "touchcancel") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_TOUCH_CANCEL, event_name);
        } else {
            return false;
        }
        return true;
    }

    void TaroButton::GetNodeAttributes() {
        TaroElement::GetNodeAttributes();

        ArkJS arkJs(NativeNodeApi::env);
        auto typeValue = GetAttributeNodeValue("type");
        if (typeValue != nullptr) {
            attributes_->type.set(TypeString2Enum(arkJs.getString(typeValue)));
        }
        auto sizeValue = GetAttributeNodeValue("size");
        if (sizeValue != nullptr) {
            attributes_->size.set(SizeString2Enum(arkJs.getString(sizeValue)));
        }
        auto plainValue = GetAttributeNodeValue("plain");
        if (plainValue != nullptr) {
            attributes_->plain.set(arkJs.getBoolean(plainValue));
        }
    }

    void TaroButton::SetAttributesToRenderNode() {
        TaroElement::SetAttributesToRenderNode();
        auto new_style =
            std::make_shared<TaroCSSOM::TaroStylesheet::Stylesheet>(*style_);

        bool isPlain = attributes_->plain.has_value() && attributes_->plain.value();

        // 覆盖样式
        std::shared_ptr<TaroButtonNode> render_button =
            std::static_pointer_cast<TaroButtonNode>(GetHeadRenderNode());

        if (attributes_->size.has_value() &&
            attributes_->size.value() == TARO_BUTTON_SIZE_MINI) {
            if (!new_style->height.has_value()) {
                new_style->height.set(default_mini_height_);
            }
            if (!new_style->width.has_value()) {
                new_style->width.set(default_mini_width_);
            }
            if (!new_style->fontSize.has_value()) {
                new_style->fontSize.set(13);
            }
        } else {
            if (!new_style->fontSize.has_value()) {
                new_style->fontSize.set(18);
            }
        }
        if (!new_style->height.has_value()) {
            new_style->height.set(default_height_);
        }
        if (!new_style->width.has_value()) {
            new_style->width.set(default_width_);
        }

        new_style->alignItems.set(ArkUI_ItemAlignment::ARKUI_ITEM_ALIGNMENT_CENTER);
        new_style->justifyContent.set(ArkUI_FlexAlignment::ARKUI_FLEX_ALIGNMENT_CENTER);
        new_style->display.set(PropertyType::Display::Flex);

        // 主题色
        const TaroButtonThemeColor& theme =
            GetThemeByType(attributes_->type.has_value() ? attributes_->type.value()
                                                         : TARO_BUTTON_TYPE_DEFAULT);
        if (isPlain) {
            if (!new_style->borderTopColor.has_value())
                new_style->borderTopColor.set(theme.plainText);
            if (!new_style->borderLeftColor.has_value())
                new_style->borderLeftColor.set(theme.plainText);
            if (!new_style->borderBottomColor.has_value())
                new_style->borderBottomColor.set(theme.plainText);
            if (!new_style->borderRightColor.has_value())
                new_style->borderRightColor.set(theme.plainText);

            if (!new_style->backgroundColor.has_value())
                new_style->backgroundColor.set(0x00000000);
            if (!new_style->color.has_value()) new_style->color.set(theme.plainText);
        } else {
            if (!new_style->borderTopColor.has_value())
                new_style->borderTopColor.set(theme.background);
            if (!new_style->borderLeftColor.has_value())
                new_style->borderLeftColor.set(theme.background);
            if (!new_style->borderBottomColor.has_value())
                new_style->borderBottomColor.set(theme.background);
            if (!new_style->borderRightColor.has_value())
                new_style->borderRightColor.set(theme.background);

            if (!new_style->backgroundColor.has_value())
                new_style->backgroundColor.set(theme.background);
            if (!new_style->color.has_value()) new_style->color.set(theme.text);
        }

        if (!new_style->borderTopLeftRadius.has_value())
            new_style->borderTopLeftRadius.set(default_radius_);
        if (!new_style->borderTopRightRadius.has_value())
            new_style->borderTopRightRadius.set(default_radius_);
        if (!new_style->borderBottomRightRadius.has_value())
            new_style->borderBottomRightRadius.set(default_radius_);
        if (!new_style->borderBottomLeftRadius.has_value())
            new_style->borderBottomLeftRadius.set(default_radius_);

        if (!new_style->borderBottomWidth.has_value())
            new_style->borderBottomWidth.set(default_border_width_);
        if (!new_style->borderTopWidth.has_value())
            new_style->borderTopWidth.set(default_border_width_);
        if (!new_style->borderRightWidth.has_value())
            new_style->borderRightWidth.set(default_border_width_);
        if (!new_style->borderLeftWidth.has_value())
            new_style->borderLeftWidth.set(default_border_width_);

        if (!new_style->opacity.has_value()) {
            if (disabled_) {
                new_style->opacity.set(0.4);
            }
        }

        style_ = new_style;
        render_button->SetStyle(new_style);
    }

    TARO_BUTTON_SIZE TaroButton::SizeString2Enum(const std::string& val) {
        auto it = TARO_BUTTON_SIZE_MAPPING.find(val);
        if (it != TARO_BUTTON_SIZE_MAPPING.end()) {
            return it->second;
        }
        return default_size_;
    }

    TARO_BUTTON_TYPE TaroButton::TypeString2Enum(const std::string& val) {
        auto it = TARO_BUTTON_TYPE_MAPPING.find(val);
        if (it != TARO_BUTTON_TYPE_MAPPING.end()) {
            return it->second;
        }
        return default_type_;
    }

    TaroButtonThemeColor TaroButton::GetThemeByType(const TARO_BUTTON_TYPE& type) {
        auto it = TARO_BUTTON_THEME_MAPPING.find(type);
        if (it != TARO_BUTTON_THEME_MAPPING.end()) {
            return it->second;
        } else {
            return TARO_BUTTON_THEME_MAPPING[TARO_BUTTON_TYPE_DEFAULT];
        }
    }
} // namespace TaroDOM
} // namespace TaroRuntime