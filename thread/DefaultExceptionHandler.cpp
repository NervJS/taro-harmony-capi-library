/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "DefaultExceptionHandler.h"

#include "helper/TaroLog.h"

void TaroThread::defaultExceptionHandler(std::exception_ptr e) {
    try {
        std::rethrow_exception(e);
    } catch (std::exception const& e) {
        TARO_LOG_ERROR("TARO_THREAD", "Exception thrown in task");
        TARO_LOG_ERROR("TARO_THREAD", e.what());
        try {
            std::rethrow_if_nested(e);
        } catch (const std::exception& nested) {
            TARO_LOG_ERROR("TARO_THREAD", nested.what());
        }
    }
};