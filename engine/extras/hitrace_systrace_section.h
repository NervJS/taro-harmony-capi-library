//
// Created on 2024/6/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once
#include <sstream>
#include <string>

#include "hitrace/trace.h"

struct HiTraceSystraceSection {
    public:
    template <typename... ConvertsToStringPiece>
    explicit HiTraceSystraceSection(
        const char *name,
        ConvertsToStringPiece &&...args) {
        std::ostringstream oss;
        (oss << ... << args);
        std::string result = std::string(name) + oss.str();
        OH_HiTrace_StartTrace(result.c_str());
    }

    ~HiTraceSystraceSection() {
        OH_HiTrace_FinishTrace();
    }
};
