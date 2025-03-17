//
// Created on 2024/6/19.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "icon.h"

#include <arkui/native_node.h>

#include "runtime/cssom/CSSStyleSheet.h"
#include "runtime/dom/ark_nodes/image.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroIcon::TaroIcon(napi_value node)
        : TaroElement(node),
          attributes_(std::make_unique<TaroIconAttributes>()) {}

    TaroIcon::~TaroIcon() {}

    static const std::unordered_map<std::string, uint32_t> ICON_COLOR_MAP = {
        {"success", 0xFF008000},
        {"success_no_circle", 0xFF008000},
        {"info", 0xFF0000FF},
        {"warn", 0xFFFF0000},
        {"waiting", 0xFF0000FF},
        {"cancel", 0xFFFF0000},
        {"download", 0xFF808080},
        {"search", 0xFF808080},
        {"clear", 0xFF808080},
        {"info_circle", 0xFF0000FF},
        {"circle", 0xFF808080}};

    static const std::unordered_map<std::string, std::string> ICON_PATH_MAP = {
        {"success", "resource://base/media/success.svg"},
        {"success_no_circle", "resource://base/media/success_no_circle.svg"},
        {"info", "resource://base/media/info.svg"},
        {"warn", "resource://base/media/warn.svg"},
        {"waiting", "resource://base/media/waiting.svg"},
        {"cancel", "resource://base/media/cancel.svg"},
        {"download", "resource://base/media/download.svg"},
        {"search", "resource://base/media/search.svg"},
        {"clear", "resource://base/media/clear.svg"},
        {"info_circle", "resource://base/media/info_circle.svg"},
        {"circle", "resource://base/media/circle.svg"}};

    void TaroIcon::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TaroAttribute::SetAttribute(renderNode, name, value);
        switch (name) {
            case ATTRIBUTE_NAME::SIZE:
                GetSizeAttribute(value);
                if (is_init_) {
                    SetSizeAttribute();
                }
                break;
            case ATTRIBUTE_NAME::TYPE:
                GetTypeAttribute(value);
                if (is_init_) {
                    SetTypeAttribute();
                }
                break;
            case ATTRIBUTE_NAME::COLOR:
                GetColorAttribute(value);
                if (is_init_) {
                    SetColorAttribute();
                }
                break;
            default:
                break;
        }
    }

    void TaroIcon::GetTypeAttribute(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        // Icon init attributes
        TaroHelper::Optional<std::string> type = getter.String();
        if (type.has_value()) {
            attributes_->type = type.value();
            auto iter = ICON_PATH_MAP.find(attributes_->type);
            if (iter != ICON_PATH_MAP.end()) {
                iconPath = iter->second;
                isValidIconType = true;
            }
        }
        uint32_t defaultColor = 0xFF000000;
        auto iter = ICON_COLOR_MAP.find(attributes_->type);
        if (iter != ICON_COLOR_MAP.end()) {
            defaultColor = iter->second;
        }
        attributes_->color.set(defaultColor);
    }

    void TaroIcon::SetTypeAttribute() {
        if (!isValidIconType) {
            TARO_LOG_DEBUG("TaroIcon", "Invalid icon type we can't set attributes to icon node");
            return;
        }
        std::shared_ptr<TaroImageNode> render_image = std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode());
        render_image->setImageSrc(iconPath);
    }

    void TaroIcon::GetSizeAttribute(const napi_value& napiValue) {
        float defaultSize = 23.0f;
        NapiGetter getter(napiValue);
        TaroHelper::Optional<float> size = TaroCSSOM::TaroStylesheet::getFloat(getter);
        if (size.has_value()) {
            defaultSize = size.value();
        }

        attributes_->size.set(Dimension{defaultSize, DimensionUnit::DESIGN_PX});
    }

    void TaroIcon::SetSizeAttribute() {
        if (attributes_->size.has_value()) {
            std::shared_ptr<TaroImageNode> render_image = std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode());
            render_image->SetHeight(attributes_->size.value());
            render_image->SetWidth(attributes_->size.value());
        }
    }

    void TaroIcon::GetColorAttribute(const napi_value& napiValue) {
        uint32_t defaultColor = 0xFF000000;
        NapiGetter getter(napiValue);
        TaroHelper::Optional<std::string> color = getter.String();
        if (color.has_value()) {
            std::optional<uint32_t> colorValue = TaroHelper::StringUtils::parseColor(color.value());
            if (colorValue.has_value()) {
                defaultColor = colorValue.value();
            }
        } else {
            auto iter = ICON_COLOR_MAP.find(attributes_->type);
            if (iter != ICON_COLOR_MAP.end()) {
                defaultColor = iter->second;
            }
        }
        attributes_->color.set(defaultColor);
    }

    void TaroIcon::SetColorAttribute() {
        if (attributes_->color.has_value()) {
            std::shared_ptr<TaroImageNode> render_image = std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode());
            render_image->setImageFillColor(attributes_->color.value());
        }
    }

    void TaroIcon::Build() {
        if (!is_init_) {
            if (!isValidIconType) {
                TARO_LOG_DEBUG("TaroIcon", "Invalid icon type we can't build icon node");
                return;
            }

            // create icon render node
            auto element = std::static_pointer_cast<TaroElement>(shared_from_this());
            auto render_icon = std::make_shared<TaroImageNode>(element);
            render_icon->Build();

            SetRenderNode(render_icon);
        }
    }

    void TaroIcon::Build(std::shared_ptr<TaroElement> &reuse_element) {}

    bool TaroIcon::bindListenEvent(const std::string& event_name) {
        if (event_name == "click") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_CLICK, event_name);
        } else if (event_name == "complete") {
            event_emitter_->registerEvent(TaroEvent::TARO_EVENT_TYPE_COMPLETE_ON_IMAGE, event_name);
        } else {
            return false;
        }
        return true;
    }

    void TaroIcon::SetAttributesToRenderNode() {
        if (!isValidIconType) {
            TARO_LOG_DEBUG("TaroIcon", "Invalid icon type we can't set attributes to icon node");
            return;
        }
        TaroElement::SetAttributesToRenderNode();
        std::shared_ptr<TaroImageNode> render_image = std::static_pointer_cast<TaroImageNode>(GetHeadRenderNode());
        render_image->SetStyle(style_);
        SetTypeAttribute();
        SetSizeAttribute();
        SetColorAttribute();
    }
} // namespace TaroDOM
} // namespace TaroRuntime