//
// Created on 2024/8/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONY_LIBRARY_BOX_ORIENT_H
#define HARMONY_LIBRARY_BOX_ORIENT_H

#include "./css_property.h"
#include "attribute_base.h"
namespace TaroRuntime::TaroCSSOM::TaroStylesheet {

class BoxOrient : public AttributeBase<PropertyType::BoxOrient> {
    public:
    using AttributeBase<PropertyType::BoxOrient>::operator=;
};

} // namespace TaroRuntime::TaroCSSOM::TaroStylesheet
#endif // HARMONY_LIBRARY_BOX_ORIENT_H
