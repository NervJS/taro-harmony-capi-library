//
// Created on 2024/7/22.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#ifndef HARMONY_LIBRARY_RADIO_H
#define HARMONY_LIBRARY_RADIO_H
#include "arkui_node.h"
#include "runtime/cssom/CSSStyleSheet.h"
namespace TaroRuntime {
namespace TaroDOM {
    class TaroRadioNode : public TaroRenderNode {
        public:
        TaroRadioNode(const TaroElementRef element);
        ~TaroRadioNode();
        void Build() override;
        void setValue(const char *value);
        void setChecked(bool checked);
        void ChangeChecked();
        void setColor(uint32_t color);
        void setZeroMargin();
        void setNodeGroup(const char *groupName);

        private:
        std::string name_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // HARMONY_LIBRARY_RADIO_H