/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_source.h"

namespace TaroRuntime {
namespace TaroDOM {
    void EventSource::RemoveNode(std::shared_ptr<TaroNode> child) {
        // TODO: implement this function
        erase(child->nid_);
    }

    void EventSource::RemoveNodeTree(std::shared_ptr<TaroNode> child) {
        erase(child->nid_);
        for (auto& item : child->child_nodes_) {
            RemoveNodeTree(item);
        }
    }
} // namespace TaroDOM
} // namespace TaroRuntime
