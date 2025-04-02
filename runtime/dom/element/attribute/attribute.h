/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TESTNDK_ATTRIBUTE_H
#define TESTNDK_ATTRIBUTE_H

#include "helper/TaroLog.h"
#include "runtime/constant.h"
#include "runtime/cssom/stylesheet/IAttribute.h"
#include "runtime/dom/animation/js_animation_param.h"
#include "runtime/dom/element/attribute/class_name.h"
#include "runtime/dom/element/attribute/disabled.h"
#include "runtime/dom/element/attribute/id.h"
#include "runtime/dom/element/attribute/style.h"
#include "runtime/dom/node.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct CommonAttributes {
        virtual ~CommonAttributes() = default;

        TaroHelper::Optional<bool> catchMove;
    };

    class TaroAttribute : public TaroNode,
                          public Attribute::Id,
                          protected Attribute::ClassName,
                          protected Attribute::Disabled,
                          protected Attribute::Style {
        public:
        TaroAttribute(napi_value node);

        virtual ~TaroAttribute();

        std::map<std::string, napi_ref> attributes_ref_;

        std::string id_;
        std::string class_name_ = "";
        StylesheetRef inline_style_;
        bool disabled_ = false;

        // FIXME 改为 element 自己声明 attributes_ 并重写 GetAttrs 方法
        virtual CommonAttributes *GetAttrs() const {
            return attributes_.get();
        }

        TaroAnimate::TaroJsAnimationOptionRef js_animation_ = nullptr;
        std::string reuse_id_;

        static ATTRIBUTE_NAME GetAttributeName(std::string name);
        void SetAttributeNodeValue(std::string key, napi_value value);
        napi_value GetAttributeNodeValue(std::string key);

        void SetReuseId(const napi_value &value);

        void SetNodeValue(napi_value node);

        virtual void GetNodeAttributes();

        void SetAttribute(std::string name, napi_value value);
        void SetAttribute(ATTRIBUTE_NAME name, napi_value value);
        virtual void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value);

        void RemoveAttribute(std::string name);
        void RemoveAttribute(ATTRIBUTE_NAME name);
        virtual void RemoveAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name);

        virtual bool HasAttribute(std::string key);
        virtual bool HasAttributes();

        virtual std::variant<bool, int, std::string, StylesheetRef, napi_value> GetAttribute(ATTRIBUTE_NAME name, napi_value result);

        void SetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode);

        std::string GetIdAttribute(napi_value value);

        void SetIdAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value);

        void RemoveIdAttribute(std::shared_ptr<TaroRenderNode> renderNode);

        std::string GetClassNameAttribute(napi_value value);

        void SetClassNameAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value);

        void RemoveClassNameAttribute(std::shared_ptr<TaroRenderNode> renderNode);

        StylesheetRef GetStyleAttribute(napi_value value);

        void SetStyleAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value);

        void RemoveStyleAttribute(std::shared_ptr<TaroRenderNode> renderNode);

        bool GetDisabledAttribute(napi_value value);

        void SetDisabledAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value);

        void RemoveDisabledAttribute(std::shared_ptr<TaroRenderNode> renderNode);

        void GetCatchMoveAttribute(const napi_value &value);

        napi_value GetDataset();

        void SetDataset(std::string key, napi_value value);

        void SetCatchMoveAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value);

        void RemoveCatchMoveAttribute();

        void SetAnimationAttribute(std::shared_ptr<TaroRenderNode> renderNode, napi_value value);

        protected:
        std::unique_ptr<CommonAttributes> attributes_;
        virtual void HandleAttributeChanged(ATTRIBUTE_NAME name,
                                            const std::string &preValue,
                                            const std::string &curValue) {};

        private:
        napi_ref dataset_ref_ = nullptr;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TESTNDK_ATTRIBUTE_H
