/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "attribute.h"

#include "arkjs/ArkJS.h"
#include "arkjs/Scope.h"
#include "helper/TaroTimer.h"
#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"

namespace TaroRuntime {
namespace TaroDOM {
    TaroAttribute::TaroAttribute(napi_value node)
        : TaroNode(node),
          Attribute::Id(),
          Attribute::ClassName(),
          Attribute::Disabled(),
          Attribute::Style() {
        node_owner_ = this;
        render_node_ = nullptr;
        attributes_ = std::make_unique<CommonAttributes>();
    }

    TaroAttribute::~TaroAttribute() {
        ArkJS arkJs(NativeNodeApi::env);
        for (auto& attribute : attributes_ref_) {
            arkJs.deleteReference(attribute.second);
        }
        attributes_ref_.clear();
        if (dataset_ref_ != nullptr) {
            arkJs.deleteReference(dataset_ref_);
        }
    }

    ATTRIBUTE_NAME TaroAttribute::GetAttributeName(std::string name) {
        ATTRIBUTE_NAME attrName = ATTRIBUTE_NAME::UNKNOWN;
        auto it = ATTRIBUTE_NAME_MAP.find(name);
        if (it != ATTRIBUTE_NAME_MAP.end()) {
            attrName = it->second;
        }

        return attrName;
    }

    void TaroAttribute::SetAttributeNodeValue(std::string key, napi_value value) {
        if (attributes_ref_[key]) {
            ArkJS(NativeNodeApi::env).deleteReference(attributes_ref_[key]);
            attributes_ref_[key] = nullptr;
        }
        attributes_ref_[key] = ArkJS(NativeNodeApi::env).createReference(value);

        if (key.starts_with("data-")) {
            SetDataset(StringUtils::toCamelCase(key.substr(5)), value);
        }
    }

    napi_value TaroAttribute::GetAttributeNodeValue(std::string key) {
        return ArkJS(NativeNodeApi::env).getReferenceValue(attributes_ref_[key]);
    }

    void TaroAttribute::SetNodeValue(napi_value node) {
        attrs_ = nullptr;
        render_node_ = nullptr;
        GetNodeAttributes();
    }

    void TaroAttribute::GetNodeAttributes() {
        // 初始化属性
        id_ = GetIdAttribute(GetAttributeNodeValue(Id::ATTR_NAME.data()));
        class_name_ = GetClassNameAttribute(GetAttributeNodeValue(ClassName::ATTR_NAME.data()));
        inline_style_ = GetStyleAttribute(GetAttributeNodeValue(Style::ATTR_NAME.data()));

        HandleAttributeChanged(ATTRIBUTE_NAME::STYLE, "", "");
        HandleAttributeChanged(ATTRIBUTE_NAME::CLASS, Attribute::ClassName::DEFAULT_VALUE, class_name_);
    }

    void TaroAttribute::SetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) {
        render_node_ = renderNode;
        Id::SetValueToNode(!id_.empty() ? id_ : "Taro nid:" + std::to_string(nid_) + "; class:" + class_name_);
    }

    void TaroAttribute::SetAttribute(std::string name, napi_value value) {
        SetAttribute(GetAttributeName(name), value);
        SetAttributeNodeValue(name, value);
    }

    void TaroAttribute::SetAttribute(ATTRIBUTE_NAME name, napi_value value) {
        SetAttribute(render_node_, name, value);
    }

    void TaroAttribute::SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) {
        TIMER_US_FUNCTION();
        switch (name) {
            case ATTRIBUTE_NAME::CLASS:
                SetClassNameAttribute(renderNode, value);
                break;
            case ATTRIBUTE_NAME::ID:
                SetIdAttribute(renderNode, value);
                break;
            case ATTRIBUTE_NAME::STYLE:
                SetStyleAttribute(renderNode, value);
                break;
            case ATTRIBUTE_NAME::DISABLED:
                SetDisabledAttribute(renderNode, value);
                break;
            case ATTRIBUTE_NAME::ANIMATION:
                SetAnimationAttribute(renderNode, value);
                break;
            case ATTRIBUTE_NAME::REUSE_ID:
                SetReuseId(value);
                break;
            default:
                break;
        }
    }

    void TaroAttribute::RemoveAttribute(std::string name) {
        attributes_ref_.erase(name);
        RemoveAttribute(GetAttributeName(name));
    }

    void TaroAttribute::RemoveAttribute(ATTRIBUTE_NAME name) {
        RemoveAttribute(render_node_, name);
    }

    void TaroAttribute::RemoveAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name) {
        TIMER_US_FUNCTION();
        switch (name) {
            case ATTRIBUTE_NAME::CLASS:
                RemoveClassNameAttribute(renderNode);
                break;
            case ATTRIBUTE_NAME::ID:
                RemoveIdAttribute(renderNode);
                break;
            case ATTRIBUTE_NAME::STYLE:
                RemoveStyleAttribute(renderNode);
                break;
            case ATTRIBUTE_NAME::DISABLED:
                RemoveDisabledAttribute(renderNode);
                break;
            default:
                break;
        }
    }

    bool TaroAttribute::HasAttribute(std::string key) {
        return attributes_ref_.find(key) != attributes_ref_.end();
    }

    bool TaroAttribute::HasAttributes() {
        return !attributes_ref_.empty();
    }

    std::variant<bool, int, std::string, StylesheetRef, napi_value>
    TaroAttribute::GetAttribute(TaroRuntime::ATTRIBUTE_NAME name, napi_value value) {
        switch (name) {
            case ATTRIBUTE_NAME::ID:
                return id_;
            case ATTRIBUTE_NAME::STYLE:
                return inline_style_;
            case ATTRIBUTE_NAME::DISABLED:
                return disabled_;
            default:
                return value;
        }
    }

    std::string TaroAttribute::GetIdAttribute(napi_value value) {
        Id::SetValueFromNapi(value);
        return Id::value_or("");
    }

    void TaroAttribute::SetIdAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value) {
        std::string preId = id_;
        Id::SetValueFromNapi(value);
        id_ = Id::value_or("");
        Id::SetValueToNode(renderNode, (!id_.empty() ? id_ : "Taro nid:" + std::to_string(nid_) + "; class:" + class_name_));
        HandleAttributeChanged(ATTRIBUTE_NAME::ID, preId, id_);
    }

    void TaroAttribute::RemoveIdAttribute(std::shared_ptr<TaroRenderNode> renderNode) {
        std::string preId = id_;
        Id::ResetValueToNode(renderNode, (!id_.empty() ? id_ : "Taro nid:" + std::to_string(nid_) + "; class:" + class_name_));
        HandleAttributeChanged(ATTRIBUTE_NAME::ID, preId, id_);
    }

    std::string TaroAttribute::GetClassNameAttribute(napi_value value) {
        ClassName::SetValueFromNapi(value);
        return ClassName::value_or("");
    }

    void TaroAttribute::SetClassNameAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value) {
        std::string preClassName = class_name_;
        ClassName::SetValueFromNapi(value);
        class_name_ = ClassName::value_or("");
        HandleAttributeChanged(ATTRIBUTE_NAME::CLASS, preClassName, class_name_);
    }

    void TaroAttribute::RemoveClassNameAttribute(std::shared_ptr<TaroRenderNode> renderNode) {
        std::string preClassName = class_name_;
        class_name_ = ClassName::DEFAULT_VALUE;
        ClassName::ResetValueToNode(renderNode, ClassName::DEFAULT_VALUE);
        HandleAttributeChanged(ATTRIBUTE_NAME::CLASS, preClassName, class_name_);
    }

    StylesheetRef TaroAttribute::GetStyleAttribute(napi_value value) {
        return TaroCSSOM::TaroStylesheet::Stylesheet::makeFromInlineStyle(value);
    }

    void TaroAttribute::SetStyleAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value) {
        Style::SetValueFromNapi(value);
        inline_style_ = TaroCSSOM::TaroStylesheet::Stylesheet::makeFromInlineStyle(value);
        HandleAttributeChanged(ATTRIBUTE_NAME::STYLE, "", "");
    }

    void TaroAttribute::SetAnimationAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value) {
        auto option = std::make_shared<TaroAnimate::TaroJsAnimationOption>();
        int ret = option->setFromNode(value);
        if (ret != 0) {
            TARO_LOG_DEBUG("TaroAnimation", "set TaroJsAnimationOption failed, ret=%{public}d", ret);
            return;
        }
        js_animation_ = option;
        HandleAttributeChanged(ATTRIBUTE_NAME::ANIMATION, "", "");
    }

    void TaroAttribute::RemoveStyleAttribute(std::shared_ptr<TaroRenderNode> renderNode) {
        Style::ResetValueToNode(renderNode, Style::DEFAULT_VALUE);
        StylesheetRef emptyStyle;
        inline_style_ = emptyStyle;
        HandleAttributeChanged(ATTRIBUTE_NAME::STYLE, "", "");
    }

    bool TaroAttribute::GetDisabledAttribute(napi_value value) {
        Disabled::SetValueFromNapi(value);
        return Disabled::value_or(false);
    }

    void TaroAttribute::SetDisabledAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value) {
        Disabled::SetValueFromNapi(value);
    }

    void TaroAttribute::RemoveDisabledAttribute(std::shared_ptr<TaroRenderNode> renderNode) {
        Disabled::ResetValueToNode(renderNode);
    }

    void TaroAttribute::GetCatchMoveAttribute(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<bool> catchMove = getter.BoolNull();
        if (catchMove.has_value()) {
            attributes_->catchMove.set(catchMove.value());
        }
    }

    void TaroAttribute::SetCatchMoveAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value) {
        NapiGetter getter(value);
        TaroHelper::Optional<bool> catchMove = getter.BoolNull();
        if (catchMove.has_value()) {
            attributes_->catchMove.set(catchMove.value());
        }
    }

    void TaroAttribute::RemoveCatchMoveAttribute() {
        attributes_->catchMove.reset();
    }

    void TaroAttribute::SetReuseId(const napi_value& value) {
        NapiGetter getter(value);
        TaroHelper::Optional<std::string> reuseId = getter.String();
        if (reuseId.has_value()) {
            reuse_id_ = reuseId.value();
            reusable_ = true;
        }
    }

    napi_value TaroAttribute::GetDataset() {
        ArkJS arkJs(NativeNodeApi::env);
        if (dataset_ref_ == nullptr) {
            NapiHandleScopeWrapper wrapper(NativeNodeApi::env);
            dataset_ref_ = arkJs.createReference(arkJs.createObject());
        }
        return arkJs.getReferenceValue(dataset_ref_);
    }

    void TaroAttribute::SetDataset(std::string key, napi_value value) {
        ArkJS arkJs(NativeNodeApi::env);
        napi_value dataset = GetDataset();
        if (dataset != nullptr && value != nullptr) {
            arkJs.setObjectProperty(dataset, key, value);
        }
    }
} // namespace TaroDOM
} // namespace TaroRuntime
