/*
 * Copyright (c) 2018 O2Team. All Rights Reserved.
 */

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
