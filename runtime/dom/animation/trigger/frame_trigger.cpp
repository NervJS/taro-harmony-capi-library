/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "frame_trigger.h"

#include "frame_trigger_master.h"

namespace TaroRuntime {
namespace TaroAnimate {
    TaroFrameTrigger::TaroFrameTrigger() {
        static uint32_t g_id_no = 0;
        ++g_id_no;
        id_ = g_id_no;
    }

    TaroFrameTrigger::~TaroFrameTrigger() {
        disable();
    }

    void TaroFrameTrigger::enable(uint32_t xid) {
        xid_ = xid;
        TaroFrameTriggerMaster::instance()->registerTrigger(id_, shared_from_this(),
                                                            xid);
    }

    void TaroFrameTrigger::disable() {
        TaroFrameTriggerMaster::instance()->unregisterTrigger(id_, xid_);
    }

} // namespace TaroAnimate
} // namespace TaroRuntime
