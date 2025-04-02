/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once

#include <cstdint>

#include "runtime/dom/element/form/widget.h"

namespace TaroRuntime {
namespace TaroDOM {
    struct TaroPickerCommonAttributes : public TaroFormAttributes {
        virtual ~TaroPickerCommonAttributes() = default;
        // 公共属性
        TaroHelper::Optional<std::string> mode;
        TaroHelper::Optional<std::string> okText;
        TaroHelper::Optional<std::string> cancelText;
    };

    struct TaroSelectorPickerAttributes : public TaroPickerCommonAttributes {
        public:
        TaroHelper::Optional<std::string> range;
        TaroHelper::Optional<std::uint32_t> value;
        TaroHelper::Optional<std::string> rangeKey;
    };

    struct TaroMultiSelectorPickerAttributes : public TaroPickerCommonAttributes {
        public:
        TaroHelper::Optional<std::string> range;
        TaroHelper::Optional<std::string> rangeKey;
        TaroHelper::Optional<std::vector<uint32_t>> value;
    };

    struct TaroTimePickerAttributes : public TaroPickerCommonAttributes {
        public:
        TaroHelper::Optional<std::string> value;
    };

    struct TaroDatePickerAttributes : public TaroPickerCommonAttributes {
        public:
        TaroHelper::Optional<std::string> value;
        TaroHelper::Optional<std::string> start;
        TaroHelper::Optional<std::string> end;
    };

    struct TaroRegionData {
        TaroHelper::Optional<std::string> code;
        TaroHelper::Optional<std::string> value;
        TaroHelper::Optional<std::string> postcode;
        TaroHelper::Optional<std::vector<TaroRegionData>> children;
    };

    struct TaroRegionPickerAttributes : public TaroPickerCommonAttributes {
        public:
        TaroHelper::Optional<std::vector<TaroRegionData>> regionData;
        TaroHelper::Optional<std::vector<std::string>> value;
        TaroHelper::Optional<std::string> level;
        TaroHelper::Optional<std::string> customItem;
    };

    class TaroPicker;
    class TaroPickerDelegate {
        public:
        TaroPickerDelegate(const std::shared_ptr<TaroPicker> element)
            : element_ref_(element) {};
        virtual ~TaroPickerDelegate() = default;
        virtual void onGetNodeAttributes() = 0;
        virtual void onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode>) = 0;
        virtual void onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter, ArkUI_NodeHandle handle) = 0;
        virtual void onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) = 0;
        virtual void onCancel(napi_value picker) = 0;
        virtual void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) = 0;

        virtual TaroPickerCommonAttributes *GetAttrs() const = 0;

        protected:
        void SetCommonAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode);
        void SetCommonAttributesChanged(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value);
        void SetCommonAttributes();
        std::shared_ptr<TaroPicker> element_ref_;
    };

    class TaroPicker : public FormWidget {
        public:
        TaroPicker(napi_value node);
        ~TaroPicker();
        void Build() override;
        void GetNodeAttributes() override;
        void SetAttributesToRenderNode() override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        void RegisterEvents();

        virtual TaroPickerCommonAttributes *GetAttrs() const override {
            return attributes_.get();
        }
        std::unique_ptr<TaroPickerCommonAttributes> attributes_;

        protected:
        bool bindListenEvent(const std::string &) override;

        private:
        std::shared_ptr<TaroPickerDelegate> delegate;
        std::string mode;
    };

    struct SelectorSource {
        std::string key;
        std::string value;
        uint32_t position;
    };

    class TaroSelectorPickerDelegate : public TaroPickerDelegate {
        public:
        TaroSelectorPickerDelegate(const std::shared_ptr<TaroPicker> element)
            : TaroPickerDelegate(element) {
            element->attributes_.reset(new TaroSelectorPickerAttributes());
            attributes_ = static_cast<TaroSelectorPickerAttributes *>(element->GetAttrs());
        };
        void onGetNodeAttributes() override;
        void onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) override;
        void onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter, ArkUI_NodeHandle handle) override;
        void onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) override;
        void onCancel(napi_value picker) override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        virtual TaroSelectorPickerAttributes *GetAttrs() const override {
            return attributes_;
        }

        private:
        TaroSelectorPickerAttributes *attributes_;

        std::map<std::string, std::vector<SelectorSource>> origin;
        bool isObjArray = false;
        Optional<std::int32_t> columnChanged;

        void SetRangeAttributes(NapiGetter rangeNapiGetter);
        void SetRangeKeyAttributes(std::string rangeKey, bool isUpdateRange);
        void SetValueAttributes(NapiGetter valueNapiGetter);
        void SetObjectRangeAttributes(NapiGetter objRangeNapiGetter);
    };

    class TaroMultiSelectorPickerDelegate : public TaroPickerDelegate {
        public:
        TaroMultiSelectorPickerDelegate(const std::shared_ptr<TaroPicker> element)
            : TaroPickerDelegate(element) {
            element->attributes_.reset(new TaroMultiSelectorPickerAttributes());
            attributes_ = static_cast<TaroMultiSelectorPickerAttributes *>(element->GetAttrs());
        };
        void onGetNodeAttributes() override;
        void onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> rendorNode) override;
        void onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter, ArkUI_NodeHandle handle) override;
        void onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) override;
        void onCancel(napi_value picker) override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        void SetRangeAttribute(NapiGetter rangeNapiGetter);
        void SetRangeKeyAttribute(NapiGetter rangeKeyNapiGetter);
        void SetValueAttribute(NapiGetter rangeKeyNapiGetter);

        virtual TaroMultiSelectorPickerAttributes *GetAttrs() const override {
            return attributes_;
        }

        private:
        TaroMultiSelectorPickerAttributes *attributes_;
        std::map<std::string, std::vector<std::vector<SelectorSource>>> sources;

        bool isObjArray = false;
        std::vector<int32_t> changed;
        std::string ParseRange(std::vector<NapiGetter> napiValues);
        std::string ParseRangeByRangeKey(std::string rangeKey, std::vector<NapiGetter> napiValues);
        void SendColumnChangedEvent(napi_value picker);
        int32_t columnCount;
    };

    class TaroTimePickerDelegate : public TaroPickerDelegate {
        public:
        TaroTimePickerDelegate(const std::shared_ptr<TaroPicker> element)
            : TaroPickerDelegate(element) {
            element->attributes_.reset(new TaroTimePickerAttributes());
            attributes_ = static_cast<TaroTimePickerAttributes *>(element->GetAttrs());
        };
        void onGetNodeAttributes() override;
        void onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) override;
        void onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter, ArkUI_NodeHandle handle) override;
        void onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) override;
        void onCancel(napi_value picker) override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;

        virtual TaroTimePickerAttributes *GetAttrs() const override {
            return attributes_;
        }

        private:
        TaroTimePickerAttributes *attributes_;
        Optional<std::string> changed;
    };

    class TaroDatePickerDelegate : public TaroPickerDelegate {
        public:
        TaroDatePickerDelegate(const std::shared_ptr<TaroPicker> element)
            : TaroPickerDelegate(element) {
            element->attributes_.reset(new TaroDatePickerAttributes());
            attributes_ = static_cast<TaroDatePickerAttributes *>(element->GetAttrs());
        };
        void onGetNodeAttributes() override;
        void onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) override;
        void onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter, ArkUI_NodeHandle handle) override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        void onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) override;
        void onCancel(napi_value picker) override;

        virtual TaroDatePickerAttributes *GetAttrs() const override {
            return attributes_;
        }

        private:
        TaroDatePickerAttributes *attributes_;
        Optional<std::string> changed;
    };

    // 基于multi text picker进行实现
    class TaroRegionPickerDelegate : public TaroPickerDelegate {
        public:
        TaroRegionPickerDelegate(const std::shared_ptr<TaroPicker> element)
            : TaroPickerDelegate(element) {
            element->attributes_.reset(new TaroRegionPickerAttributes());
            attributes_ = static_cast<TaroRegionPickerAttributes *>(element->GetAttrs());
        };
        void onGetNodeAttributes() override;
        void onSetAttributesToRenderNode(std::shared_ptr<TaroRenderNode> renderNode) override;
        void onRegisterEvent(std::shared_ptr<TaroEvent::TaroEventEmitter> emitter, ArkUI_NodeHandle handle) override;
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        void onConfirm(std::shared_ptr<TaroRenderNode> renderNode, napi_value picker) override;
        void onCancel(napi_value picker) override;

        virtual TaroRegionPickerAttributes *GetAttrs() const override {
            return attributes_;
        }

        private:
        TaroRegionPickerAttributes *attributes_;
        std::map<std::string, std::vector<std::string>> cityMap;
        std::map<std::string, std::vector<std::string>> regionMap;
        std::vector<std::string> provinces;
        std::vector<std::string> city;
        std::vector<std::string> region;
        int provinceIndex = 0;
        int cityIndex = 0;
        int regionIndex = 0;
        std::shared_ptr<TaroRenderNode> m_renderNode;
        // like the data description of package/taro-components-rn/src/components/Picker
        std::vector<TaroRegionData> ParseRegionData(std::vector<NapiGetter> data);
        // covert for strings
        void ParseRegionInfos();
        // param：isUsingDefault 是否用value默认的值设置默认索引，首次加载，数据更新 true，滚动场景fasle
        std::string CreateRegionRange(bool isUsingDefault);
        // handleColumChange
        void HandleColumnChanged(int provinceChanged, int cityChanged, int regionChanged);
        void SetValue(std::vector<NapiGetter> valueNapi);
        void SetLevel(std::string level);
        void SetCustomItem(std::string customItem);
    };

} // namespace TaroDOM
} // namespace TaroRuntime
