/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef HARMONY_LIBRARY_CHECKBOX_GROUP_ELEMENT_H
#define HARMONY_LIBRARY_CHECKBOX_GROUP_ELEMENT_H

#include <set>
#include <arkui/native_node.h>

#include "runtime/dom/element/form/group_manager.h"
#include "runtime/dom/element/form/widget.h"

namespace TaroRuntime {
namespace TaroDOM {
    class TaroCheckboxGroup : public FormWidget {
        public:
        TaroCheckboxGroup(napi_value node);
        ~TaroCheckboxGroup() = default;
        void callEventHandler(std::string value, bool selected);
        void SetAttributesToRenderNode() override;
        void Build() override;
        void onAppendChild(std::shared_ptr<TaroNode> child) override;
        void onRemoveChild(std::shared_ptr<TaroNode> child, bool is_detach_render_node = true) override;

        virtual TaroFormAttributes* GetAttrs() const override {
            return attributes_.get();
        }

        protected:
        bool bindListenEvent(const std::string& event_name) override;

        private:
        std::unique_ptr<TaroFormAttributes> attributes_;
        std::set<std::string> list_;
        TaroHelper::Optional<std::string> name_;
        std::shared_ptr<FormGroup> group_;
    };
} // namespace TaroDOM
} // namespace TaroRuntime
#endif // HARMONY_LIBRARY_CHECKBOX_GROUP_ELEMENT_H
