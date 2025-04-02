/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "event_generator.h"

#include "runtime/dom/element/element.h"

namespace TaroRuntime::TaroDOM::TaroEvent {
EventGenerator::EventGenerator(EventGeneratorType gen_type, std::shared_ptr<TaroElement> node)
    : gen_type_(gen_type), node_owner_(node), nid_(node->nid_) {}
} // namespace TaroRuntime::TaroDOM::TaroEvent