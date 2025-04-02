/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#pragma once
#include <sstream>
#include <string>

#include "hitrace/trace.h"

struct HiTraceSystraceSection {
    public:
    template <typename... ConvertsToStringPiece>
    explicit HiTraceSystraceSection(
        const char* name,
        ConvertsToStringPiece&&... args) {
        std::ostringstream oss;
        (oss << ... << args);
        std::string result = std::string(name) + oss.str();
        OH_HiTrace_StartTrace(result.c_str());
    }

    ~HiTraceSystraceSection() {
        OH_HiTrace_FinishTrace();
    }
};
