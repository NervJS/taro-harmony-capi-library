//
// Created on 2024/6/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <folly/Exception.h>

#define EXPORT __attribute__((visibility("default")))

// JSExecutor functions sometimes take large strings, on the order of
// megabytes.  Copying these can be expensive.  Introducing a
// move-only, non-CopyConstructible type will let the compiler ensure
// that no copies occur.  folly::MoveWrapper should be used when a
// large string needs to be curried into a std::function<>, which must
// by CopyConstructible.

class JSBigString {
    public:
    JSBigString() = default;

    // Not copyable
    JSBigString(const JSBigString &) = delete;
    JSBigString &operator=(const JSBigString &) = delete;

    virtual ~JSBigString() {}

    virtual bool isAscii() const = 0;

    // This needs to be a \0 terminated string
    virtual const char *c_str() const = 0;

    // Length of the c_str without the NULL byte.
    virtual size_t size() const = 0;
};

// Concrete JSBigString implementation which holds a std::string
// instance.
class JSBigStdString : public JSBigString {
    public:
    JSBigStdString(std::string str, bool isAscii = false)
        : m_isAscii(isAscii), m_str(std::move(str)) {}

    bool isAscii() const override {
        return m_isAscii;
    }

    const char *c_str() const override {
        return m_str.c_str();
    }

    size_t size() const override {
        return m_str.size();
    }

    private:
    bool m_isAscii;
    std::string m_str;
};

// Concrete JSBigString implementation which holds a heap-allocated
// buffer, and provides an accessor for writing to it.  This can be
// used to construct a JSBigString in place, such as by reading from a
// file.
class EXPORT JSBigBufferString : public JSBigString {
    public:
    JSBigBufferString(size_t size)
        : m_data(new char[size + 1]), m_size(size) {
        // Guarantee nul-termination.  The caller is responsible for
        // filling in the rest of m_data.
        m_data[m_size] = '\0';
    }

    ~JSBigBufferString() override {
        delete[] m_data;
    }

    bool isAscii() const override {
        return true;
    }

    const char *c_str() const override {
        return m_data;
    }

    size_t size() const override {
        return m_size;
    }

    char *data() {
        return m_data;
    }

    private:
    char *m_data;
    size_t m_size;
};

// JSBigString interface implemented by a file-backed mmap region.
class EXPORT JSBigFileString : public JSBigString {
    public:
    JSBigFileString(int fd, size_t size, off_t offset = 0);
    ~JSBigFileString() override;

    bool isAscii() const override {
        return true;
    }

    const char *c_str() const override;

    size_t size() const override;
    int fd() const;

    static std::unique_ptr<const JSBigFileString> fromPath(
        const std::string &sourceURL);

    private:
    int m_fd;                   // The file descriptor being mmapped
    size_t m_size;              // The size of the mmapped region
    mutable off_t m_pageOff;    // The offset in the mmapped region to the data.
    off_t m_mapOff;             // The offset in the file to the mmapped region.
    mutable const char *m_data; // Pointer to the mmapped region.
};
