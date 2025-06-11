/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

#ifndef TARO_CAPI_HARMONY_DEMO_ATTRIBUTE_BASE_H
#define TARO_CAPI_HARMONY_DEMO_ATTRIBUTE_BASE_H

#include <arkui/native_node.h>
#include <arkui/native_type.h>

#include "./utils.h"
#include "common.h"
#include "engine/react_common/systrace_section.h"
#include "helper/ColorUtils.h"
#include "helper/Optional.h"
#include "helper/StringUtils.h"
#include "helper/TaroLog.h"
#include "runtime/NapiGetter.h"
#include "runtime/NativeNodeApi.h"
#include "runtime/cssom/dimension/dimension.h"
#include "runtime/cssom/stylesheet/css_property.h"
#include "runtime/cssom/stylesheet/types/TColor.h"
#include "runtime/cssom/stylesheet/types/TEnum.h"

namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

#define ATTRIBUTE_ASSIGN                                                                          \
    if (item.has_value()) {                                                                       \
        bool is_import = flag_.test(static_cast<size_t>(CSS_PROPERTY_FLAG::IMPORTANT));           \
                                                                                                  \
        bool item_is_import = item.flag_.test(static_cast<size_t>(CSS_PROPERTY_FLAG::IMPORTANT)); \
        if (is_import && !item_is_import) {                                                       \
            return;                                                                               \
        }                                                                                         \
        this->set(item);                                                                          \
        if (item_is_import) {                                                                     \
            flag_.set(static_cast<size_t>(CSS_PROPERTY_FLAG::IMPORTANT));                         \
        }                                                                                         \
    }

template <typename T, typename Enable = void>
class AttributeBase : public TaroHelper::Optional<T> {
    public:
    virtual void setValueFromNapi(const napi_value& value) = 0;
    virtual void setValueFromStringView(std::string_view value) = 0;
    virtual void assign(const AttributeBase<T>& item) {
        ATTRIBUTE_ASSIGN
    };
    std::bitset<8> flag_;
};

// 针对 color 特化
template <>
class AttributeBase<TColor> : public TaroHelper::Optional<uint32_t> {
    public:
    // 设置字符串
    virtual void setValueFromStringView(std::string_view value) {
        try {
            auto color = TColor::MakeFromString(value);
            this->set(color.getValue());
        } catch (...) {
        }
    }

    void setValueFromNapi(const napi_value& napiValue) {
        try {
            auto color = TColor::MakeFromNapi(napiValue);
            this->set(color.getValue());
        } catch (...) {
        }
    }

    void assign(const AttributeBase<TColor>& item){
        ATTRIBUTE_ASSIGN} std::bitset<8> flag_;
};

// 针对 int 类型的特化
template <typename T>
class AttributeBase<
    T, typename std::enable_if<std::is_same<T, int>::value ||
                               std::is_same<T, uint32_t>::value>::type>
    : public TaroHelper::Optional<T> {
    public:
    virtual void setValueFromStringView(std::string_view value) {
        try {
            this->set(std::stof(std::string(value)));
        } catch (...) {
            // stof转化报错
        }
    }

    void setValue(float value) {
        this->set(value);
    }
    void setValueFromNapi(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        napi_valuetype type;
        getter.GetType(type);
        if (type == napi_number) {
            auto getterValue = getter.Int32();
            if (getterValue.has_value()) {
                this->set(getterValue.value());
            }
        }
    }

    void assign(const AttributeBase<T>& item){
        ATTRIBUTE_ASSIGN} std::bitset<8> flag_;
};

// 针对 float 类型的特化
template <>
class AttributeBase<float> : public TaroHelper::Optional<float> {
    public:
    virtual void setValueFromStringView(std::string_view value) {
        std::string val{value};
        try {
            float len = std::stof(val);
            this->set(len);
        } catch (...) {
            TARO_LOG_ERROR("TaroStyle", "不能转换的字符串数字");
        }
    }

    void setValue(float value) {
        this->set(value);
    }

    void setValueFromNapi(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        napi_valuetype type;
        getter.GetType(type);

        if (type == napi_number) {
            auto getterValue = getter.Double();
            if (getterValue.has_value()) {
                this->set(getterValue.value());
            }
        } else if (type == napi_string) {
            setValueFromStringView(getter.StringOr("0"));
        }
    }

    void assign(const AttributeBase<float>& item){
        ATTRIBUTE_ASSIGN} std::bitset<8> flag_;
};

// 针对 Dimension 类型的特化
template <>
class AttributeBase<Dimension> : public TaroHelper::Optional<Dimension> {
    public:
    virtual void setValueFromStringView(std::string_view value) {
        auto dimension = Dimension::FromString(value);
        if (dimension.Unit() != DimensionUnit::INVALID) {
            this->set(dimension);
        }
    }

    void setValue(float value) {
        this->set(Dimension{value, DimensionUnit::VP});
    }

    void setValueFromNapi(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        napi_valuetype type;
        getter.GetType(type);
        if (type == napi_number) {
            auto getterValue = getter.Double();
            if (getterValue.has_value()) {
                this->set(Dimension{getterValue.value(), DimensionUnit::DESIGN_PX});
            }
        } else if (type == napi_string) {
            auto getterString = getter.String();
            if (getterString.has_value()) {
                setValueFromStringView(getterString.value());
            }
        }
    }

    void assign(const AttributeBase<Dimension>& item){
        ATTRIBUTE_ASSIGN} std::bitset<8> flag_;
};

// 针对 enum 类型的特化
template <typename T>
class AttributeBase<T, typename std::enable_if<std::is_enum<T>::value>::type>
    : public TaroHelper::Optional<T> {
    public:
    virtual void setValueFromStringView(std::string_view str) {
        if (auto outerItem = TEnum::enumMappings.find(std::type_index(typeid(T)));
            outerItem != TEnum::enumMappings.end()) {
            if (auto innerItem = outerItem->second.find(str);
                innerItem != outerItem->second.end()) {
                this->set(static_cast<T>(innerItem->second));
            }
        }
    };

    void setValue(T value) {
        this->set(value);
    }

    void setValueFromNapi(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        napi_valuetype type;
        getter.GetType(type);
        if (type == napi_number) {
            auto getterValue = getter.Int32();
            if (getterValue.has_value()) {
                if (auto outerItem = TEnum::intEnumMappings.find(std::type_index(typeid(T)));
                    outerItem != TEnum::intEnumMappings.end()) {
                    if (auto innerItem = outerItem->second.find(getterValue.value());
                        innerItem != outerItem->second.end()) {
                        this->set(static_cast<T>(innerItem->second));
                    }
                } else {
                    this->set(static_cast<T>(getterValue.value()));
                }
            }
        }
    };

    void assign(const AttributeBase<T>& item){
        ATTRIBUTE_ASSIGN}

    AttributeBase& operator=(T value) {
        this->setValue(value);
        return *this;
    }
    std::bitset<8> flag_;
};

// 针对 std::string 类型的特化
template <>
class AttributeBase<std::string> : public TaroHelper::Optional<std::string> {
    public:
    virtual void setValueFromStringView(std::string_view str) {
        std::string s(str);
        this->set(s);
    };
    void setValueFromNapi(const napi_value& napiValue) {
        NapiGetter getter(napiValue);
        napi_valuetype type;
        getter.GetType(type);
        if (type == napi_string) {
            auto getterValue = getter.String();
            if (getterValue.has_value()) {
                this->set(getterValue.value());
            }
        }
    }

    void assign(const AttributeBase<std::string>& item){
        ATTRIBUTE_ASSIGN} std::bitset<8> flag_;
};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // TARO_CAPI_HARMONY_DEMO_ATTRIBUTE_BASE_H
