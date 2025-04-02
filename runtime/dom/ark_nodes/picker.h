/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include <cstdint>

#include "runtime/cssom/CSSStyleSheet.h"

namespace TaroRuntime {
namespace TaroDOM {
    // parent
    class TaroPickerNode : public TaroRenderNode {
        public:
        TaroPickerNode(const TaroElementRef element);
        ~TaroPickerNode();

        void Build() override;
        void Show();
        void Prepare();

        virtual void onConfirm() = 0;
        virtual void onCancel() = 0;

        ArkUI_NodeHandle GetPickerHandle() {
            return pickerHandle;
        }
        ArkUI_NodeHandle GetCancelHandle() {
            return cancelHandle;
        }
        ArkUI_NodeHandle GetConfirmHandle() {
            return confirmHandle;
        }

        void SetOKText(std::string okText);
        void SetCancelText(std::string cancelText);

        protected:
        bool isPickerCreated = false;

        private:
        ArkUI_NodeHandle CreateDialogContent();
        virtual ArkUI_NodeHandle CreatePickerContent() = 0;
        ArkUI_NodeHandle confirmHandle;
        ArkUI_NodeHandle cancelHandle;

        protected:
        ArkUI_NativeDialogHandle dialogHandle;
        ArkUI_NodeHandle pickerHandle;
        // common style
        std::string okText = "确认";
        std::string cancelText = "取消";
        std::string selectTextStyle = "#ff000000;20;normal;Arial;normal";
    };

    // Selector
    class TaroSelectorPickerNode : public TaroPickerNode {
        public:
        TaroSelectorPickerNode(const TaroElementRef element);
        ArkUI_NodeHandle CreatePickerContent() override;
        void onConfirm() override;
        void onCancel() override;

        void SetSelectorRange(Optional<std::string> selectorValues);
        void SetSelect(Optional<uint32_t> select);
    };

    // MultiSelector
    class TaroMultiSelectorPickerNode : public TaroPickerNode {
        public:
        TaroMultiSelectorPickerNode(const TaroElementRef element);
        ArkUI_NodeHandle CreatePickerContent() override;
        void onConfirm() override;
        void onCancel() override;

        void SetMultiSelectorRange(Optional<std::string> values);
        void SetMultiSelectorValues(std::vector<uint32_t> indexes);
    };

    class TaroTimePickerNode : public TaroPickerNode {
        public:
        TaroTimePickerNode(const TaroElementRef element);
        ArkUI_NodeHandle CreatePickerContent() override;
        void onConfirm() override;
        void onCancel() override;

        void SetTime(std::string time);

        private:
        TaroHelper::Optional<std::string> entryTime;
        TaroHelper::Optional<std::string> currentTime;
    };

    class TaroDatePickerNode : public TaroPickerNode {
        public:
        TaroDatePickerNode(const TaroElementRef element);
        ArkUI_NodeHandle CreatePickerContent() override;
        void onConfirm() override;
        void onCancel() override;

        void SetDate(std::string dateValue);
        void SetStart(std::string start);
        void SetEnd(std::string end);
    };

    class TaroRegionPickerNode : public TaroPickerNode {
        public:
        TaroRegionPickerNode(const TaroElementRef element);
        ArkUI_NodeHandle CreatePickerContent() override;
        void onConfirm() override;
        void onCancel() override;

        void SetRegionData(std::string regionData, uint32_t province, uint32_t city, uint32_t region);
    };

} // namespace TaroDOM
} // namespace TaroRuntime
