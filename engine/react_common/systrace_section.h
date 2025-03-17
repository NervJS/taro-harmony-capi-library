//
// Created on 2024/6/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "engine/extras/hitrace_systrace_section.h"
/**
 * Allow providing an fbsystrace implementation that can short-circuit out
 * quickly and can throttle too frequent events so we can get useful traces even
 * if rendering etc. is spinning. For throttling we'll need file/line info so we
 * use a macro.
 */
#if defined(WITH_LOOM_TRACE)
#define SystraceSection                                           \
    static constexpr const char systraceSectionFile[] = __FILE__; \
    fbsystrace::FbSystraceSection<systraceSectionFile, __LINE__>
/**
 * This is a convenience class to avoid lots of verbose profiling
 * #ifdefs.  If WITH_FBSYSTRACE is not defined, the optimizer will
 * remove this completely.  If it is defined, it will behave as
 * FbSystraceSection, with the right tag provided. Use two separate classes to
 * to ensure that the ODR rule isn't violated, that is, if WITH_FBSYSTRACE has
 * different values in different files, there is no inconsistency in the sizes
 * of defined symbols.
 */
#elif defined(WITH_FBSYSTRACE)
struct ConcreteSystraceSection {
    public:
    template <typename... ConvertsToStringPiece>
    explicit ConcreteSystraceSection(
        const char *name,
        ConvertsToStringPiece &&...args)
        : m_section(TRACE_TAG_REACT_CXX_BRIDGE, name, args...) {}

    private:
    fbsystrace::FbSystraceSection m_section;
};
using SystraceSection = ConcreteSystraceSection;
#elif defined(WITH_HITRACE_SYSTRACE)
using SystraceSection = HiTraceSystraceSection;
#else
struct DummySystraceSection {
    public:
    template <typename... ConvertsToStringPiece>
    explicit DummySystraceSection(
        // TARO patch: remove unsupported `__unused` qualifier
        const char *name,
        ConvertsToStringPiece &&...args) {}
};
using SystraceSection = DummySystraceSection;
#endif
