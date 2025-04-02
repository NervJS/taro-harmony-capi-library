/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_HARMONY_EVENT_SOURCE_H
#define TARO_HARMONY_EVENT_SOURCE_H

#include <unordered_map>

#include "runtime/dom/element/element.h"

namespace TaroRuntime {
namespace TaroDOM {
    class EventSource : public std::unordered_map<int32_t, std::shared_ptr<TaroElement>> {
        public:
        void RemoveNode(std::shared_ptr<TaroNode> child);

        void RemoveNodeTree(std::shared_ptr<TaroNode> child);
    };
} // namespace TaroDOM
} // namespace TaroRuntime

#endif // TARO_HARMONY_EVENT_SOURCE_H
