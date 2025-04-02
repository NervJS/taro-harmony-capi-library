/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#include "native_logger.h"

#include "helper/TaroLog.h"

#define LOG_DOMAIN 0xBEEF
#define LOG_TAG "#Taro_ARK"
#define LOG_PATTERN "__█ %{public}s"

void nativeLogger(const std::string& message, unsigned int logLevel) {
    switch (logLevel) {
        case 0:
            TARO_LOG_DEBUG(LOG_TAG, LOG_PATTERN, message.c_str());
            break;
        case 1:
            TARO_LOG_INFO(LOG_TAG, LOG_PATTERN, message.c_str());
            break;
        case 2:
            TARO_LOG_WARN(LOG_TAG, LOG_PATTERN, message.c_str());
            break;
        case 3:
            TARO_LOG_ERROR(LOG_TAG, LOG_PATTERN, message.c_str());
            break;
        default:
            TARO_LOG_INFO(LOG_TAG, LOG_PATTERN, message.c_str());
    }
}
