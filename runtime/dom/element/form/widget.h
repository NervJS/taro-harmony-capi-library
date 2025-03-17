//
// Created by zhutianjian on 24-8-31.
//

#ifndef TARO_HARMONY_WIDGET_H
#define TARO_HARMONY_WIDGET_H

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    enum class FORM_FUNC_CODE {
        FOCUS = 100,
        BLUR,
    };

    extern std::unordered_map<std::string, FORM_FUNC_CODE> FORM_FUNC_CODE_MAPPING;

    struct TaroFormAttributes : public CommonAttributes {
        virtual ~TaroFormAttributes() = default;

        TaroHelper::Optional<std::string> name;
        TaroHelper::Optional<std::string> value;
    };

    class FormWidget : public TaroElement {
        public:
        FormWidget(napi_value node);
        virtual ~FormWidget() = default;

        virtual void PostBuild() override;

        virtual TaroFormAttributes* GetAttrs() const override = 0;
        virtual void bindSelfEventHandle();

        void GetNodeAttributes() override;
        void GetNameAttribute(const napi_value& napiValue);
        virtual void GetValueAttribute(const napi_value& napiValue);
        virtual void SetValueAttribute() {};
        void SetAttribute(std::shared_ptr<TaroRenderNode> renderNode, ATTRIBUTE_NAME name, napi_value value) override;
        void SetAttributesToRenderNode() override;
        virtual std::variant<bool, int, std::string, StylesheetRef, napi_value> GetAttribute(ATTRIBUTE_NAME name, napi_value result) override;

        int GetFuncCode(std::string str) override;
        napi_value ExecuteFunc(std::shared_ptr<TaroRenderNode> renderNode, napi_value name, napi_value params) override;

        bool is_focus = false;
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_WIDGET_H
