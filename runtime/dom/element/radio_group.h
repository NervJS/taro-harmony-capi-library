/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_RADIO_GROUP_ELEMENT_H
#define HARMONY_LIBRARY_RADIO_GROUP_ELEMENT_H
#include <arkui/native_node.h>

#include "runtime/dom/element/form/widget.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroRadioGroup : public FormWidget {
        public:
        TaroRadioGroup(napi_value node);
        ~TaroRadioGroup() = default;
        void SetAttributesToRenderNode() override;
        void Build() override;
        void onAppendChild(std::shared_ptr<TaroNode> child) override;
        void onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node = true) override;

        virtual TaroFormAttributes* GetAttrs() const override {
            return attributes_.get();
        }

        bool bindListenEvent(const std::string& event_name) override;

        private:
        std::unique_ptr<TaroFormAttributes> attributes_;
        std::string name_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // HARMONY_LIBRARY_RADIO_GROUP_ELEMENT_H
