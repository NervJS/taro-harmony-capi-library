//
// Created on 2024/6/13.
//
// Node APIs are not fully supported. To solve the compilation error of the
// interface cannot be found, please include "napi/native_api.h".

#ifndef TARO_CAPI_HARMONY_DEMO_BUTTON_H
#define TARO_CAPI_HARMONY_DEMO_BUTTON_H

#include <arkui/native_node.h>

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {

    struct TaroButtonThemeColor {
        uint32_t text;
        uint32_t background;
        uint32_t plainText;
    };

    enum TARO_BUTTON_TYPE {
        TARO_BUTTON_TYPE_PRIMARY,
        TARO_BUTTON_TYPE_DEFAULT,
        TARO_BUTTON_TYPE_WARN
    };

    enum TARO_BUTTON_SIZE {
        TARO_BUTTON_SIZE_DEFAULT,
        TARO_BUTTON_SIZE_MINI,
    };

    static std::unordered_map<std::string, TARO_BUTTON_TYPE>
        TARO_BUTTON_TYPE_MAPPING = {
            {"primary", TARO_BUTTON_TYPE_PRIMARY},
            {"default", TARO_BUTTON_TYPE_DEFAULT},
            {"warn", TARO_BUTTON_TYPE_WARN},
    };

    static std::unordered_map<std::string, TARO_BUTTON_SIZE>
        TARO_BUTTON_SIZE_MAPPING = {
            {"default", TARO_BUTTON_SIZE_DEFAULT},
            {"mini", TARO_BUTTON_SIZE_MINI},
    };

    static std::unordered_map<TARO_BUTTON_TYPE, TaroButtonThemeColor>
        TARO_BUTTON_THEME_MAPPING = {
            {TARO_BUTTON_TYPE_PRIMARY,
             TaroButtonThemeColor{.text = 0xffffffff,
                                  .background = 0xff1aad19,
                                  .plainText = 0xff1aad19}},
            {TARO_BUTTON_TYPE_WARN, TaroButtonThemeColor{.text = 0xffffffff,
                                                         .background = 0xffE64340,
                                                         .plainText = 0xffE64340}},
            {TARO_BUTTON_TYPE_DEFAULT,
             TaroButtonThemeColor{.text = 0xff000000,
                                  .background = 0xfff7f7f7,
                                  .plainText = 0xff000000}}};

    struct TaroButtonAttributes : public CommonAttributes {
        // Button attributes
        TaroHelper::Optional<TARO_BUTTON_TYPE> type;
        TaroHelper::Optional<TARO_BUTTON_SIZE> size;
        TaroHelper::Optional<bool> plain;
    };

    class TaroButton : public TaroElement {
        public:
        TaroButton(napi_value node);

        ~TaroButton() = default;

        void GetNodeAttributes() override;

        void SetAttributesToRenderNode() override;

        void Build() override;

        bool bindListenEvent(const std::string& event_name) override;

        std::unique_ptr<TaroButtonAttributes> attributes_;

        private:
        Dimension default_height_;
        Dimension default_mini_height_;
        Dimension default_width_;
        Dimension default_mini_width_;
        Dimension default_radius_;
        TARO_BUTTON_TYPE default_type_ = TARO_BUTTON_TYPE_DEFAULT;
        TARO_BUTTON_SIZE default_size_ = TARO_BUTTON_SIZE_DEFAULT;
        Dimension default_border_width_ = Dimension{1.0};
        bool default_plain_ = false;

        TARO_BUTTON_TYPE TypeString2Enum(const std::string& val);
        TARO_BUTTON_SIZE SizeString2Enum(const std::string& val);
        TaroButtonThemeColor GetThemeByType(const TARO_BUTTON_TYPE& type);
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_CAPI_HARMONY_DEMO_BUTTON_H
