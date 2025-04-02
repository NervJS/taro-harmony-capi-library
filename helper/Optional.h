/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_OPTIONAL_H
#define TARO_CAPI_HARMONY_DEMO_OPTIONAL_H

#include <stdexcept>
namespace TaroHelper {
template <typename T>
class Optional {
    public:
    Optional()
        : is_set(false) {}

    Optional(const T& value)
        : data(value), is_set(true) {}

    void set(const T& value) {
        data = value;
        is_set = true;
    }

    void set(const Optional<T>& value) {
        if (value.has_value()) {
            data = value.value();
            is_set = true;
        } else {
            this->reset();
        }
    }

    void reset() {
        is_set = false;
    }

    bool has_value() const {
        return is_set;
    }

    const T& value() const {
        if (!is_set) {
            throw std::logic_error("Accessing value from an empty Optional");
        }
        return data;
    }

    T& value() {
        if (!is_set) {
            throw std::logic_error("Accessing value from an empty Optional");
        }
        return data;
    }

    const T& value_or(const T& default_value) const {
        return is_set ? data : default_value;
    }

    bool operator==(const Optional<T>& other) const {
        bool hasValue = this->has_value();
        if (hasValue == other.has_value()) {
            if (hasValue) {
                return this->value() == other.value();
            }
            return true;
        }
        return false;
    }

    bool operator!=(const Optional<T>& other) const {
        return !(*this == other);
    }

    bool isEqual(const Optional<T>& other) const {
        return *this == other;
    }

    private:
    T data;
    bool is_set;
};
} // namespace TaroHelper

#endif // TARO_CAPI_HARMONY_DEMO_OPTIONAL_H
