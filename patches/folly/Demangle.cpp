/*
 * Modify from https://github.com/facebook/folly/
 * Apache-2.0 License https://github.com/facebook/folly/blob/main/LICENSE
 */

#include <algorithm>
#include <cstring>
#include <folly/Demangle.h>
#include <folly/lang/CString.h>

#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#endif

//  The headers <libiberty.h> (binutils) and <string.h> (glibc) both declare the
//  symbol basename. Unfortunately, the declarations are different. So including
//  both headers in the same translation unit fails due to the two conflicting
//  declarations. Since <demangle.h> includes <libiberty.h> we must be careful.

// TARO: patch
// #if __has_include(<demangle.h>)
// #pragma push_macro("HAVE_DECL_BASENAME")
// #define HAVE_DECL_BASENAME 1
// #include <demangle.h> // @manual
// #pragma pop_macro("HAVE_DECL_BASENAME")
// #endif

//  try to find cxxabi demangle
//
//  prefer using a weakref

#if __has_include(<cxxabi.h>)

[[gnu::weakref("__cxa_demangle")]] static char* cxxabi_demangle(
    char const*, char*, size_t*, int*);

#else // __has_include(<cxxabi.h>)

static constexpr auto cxxabi_demangle = static_cast<char* (*)(...)>(nullptr);

#endif // __has_include(<cxxabi.h>)

//  try to find liberty demangle
//
//  cannot use a weak symbol since it may be the only referenced symbol in
//  liberty
//
//  in contrast with cxxabi, where there are certainly other referenced symbols

// TARO: patch
// #if __has_include(<demangle.h>)
//
// static constexpr auto liberty_demangle = cplus_demangle_v3_callback;
//
// #if defined(DMGL_NO_RECURSE_LIMIT)
// static constexpr auto liberty_demangle_options_no_recurse_limit =
// DMGL_NO_RECURSE_LIMIT;
// #else
// static constexpr auto liberty_demangle_options_no_recurse_limit = 0;
// #endif
//
// static constexpr auto liberty_demangle_options = //
// DMGL_PARAMS | DMGL_ANSI | DMGL_TYPES | //
// liberty_demangle_options_no_recurse_limit;
//
// #else // __has_include(<demangle.h>)

static constexpr auto liberty_demangle = static_cast<int (*)(...)>(nullptr);
static constexpr auto liberty_demangle_options = 0;

// #endif // __has_include(<demangle.h>)

//  implementations

namespace folly {

//  reinterpret-cast currently evades -Waddress
bool const demangle_build_has_cxxabi = cxxabi_demangle;
bool const demangle_build_has_liberty =
    reinterpret_cast<void*>(liberty_demangle);

fbstring demangle(const char* name) {
    if (!name) {
        return fbstring();
    }

    if (demangle_max_symbol_size) {
        // GCC's __cxa_demangle() uses on-stack data structures for the
        // parser state which are linear in the number of components of the
        // symbol. For extremely long symbols, this can cause a stack
        // overflow. We set an arbitrary symbol length limit above which we
        // just return the mangled name.
        size_t mangledLen = strlen(name);
        if (mangledLen > demangle_max_symbol_size) {
            return fbstring(name, mangledLen);
        }
    }

    if (cxxabi_demangle) {
        int status;
        size_t len = 0;
        // malloc() memory for the demangled type name
        char* demangled = cxxabi_demangle(name, nullptr, &len, &status);
        if (status != 0) {
            return name;
        }
        // len is the length of the buffer (including NUL terminator and maybe
        // other junk)
        return fbstring(
            demangled, strlen(demangled), len, AcquireMallocatedString());
    } else {
        return fbstring(name);
    }
}

namespace {

    struct DemangleBuf {
        char* dest;
        size_t remaining;
        size_t total;
    };

    void demangleCallback(const char* str, size_t size, void* p) {
        DemangleBuf* buf = static_cast<DemangleBuf*>(p);
        size_t n = std::min(buf->remaining, size);
        memcpy(buf->dest, str, n);
        buf->dest += n;
        buf->remaining -= n;
        buf->total += size;
    }

} // namespace

size_t demangle(const char* name, char* out, size_t outSize) {
    if (demangle_max_symbol_size) {
        size_t mangledLen = strlen(name);
        if (mangledLen > demangle_max_symbol_size) {
            if (outSize) {
                size_t n = std::min(mangledLen, outSize - 1);
                memcpy(out, name, n);
                out[n] = '\0';
            }
            return mangledLen;
        }
    }

    if (reinterpret_cast<void*>(liberty_demangle)) {
        DemangleBuf dbuf;
        dbuf.dest = out;
        dbuf.remaining = outSize ? outSize - 1 : 0; // leave room for null term
        dbuf.total = 0;

        // Unlike most library functions, this returns 1 on success and 0 on failure
        int status = liberty_demangle(
            name, liberty_demangle_options, demangleCallback, &dbuf);
        if (status == 0) { // failed, return original
            return folly::strlcpy(out, name, outSize);
        }
        if (outSize != 0) {
            *dbuf.dest = '\0';
        }
        return dbuf.total;
    } else {
        return folly::strlcpy(out, name, outSize);
    }
}

} // namespace folly
