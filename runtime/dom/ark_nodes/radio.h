/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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