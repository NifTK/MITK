/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/


#include "mitkLookupTables.h"

template class mitk::GenericIntLookupTable<bool>;
template class mitk::GenericIntLookupTable<float>;
template class mitk::GenericIntLookupTable<int>;
template class mitk::GenericIntLookupTable<std::string>;

template class mitk::GenericStringLookupTable<std::string>;

mitkSpecializeGenericIntLookupTableOperator(mitk::BoolLookupTable);
mitkSpecializeGenericIntLookupTableOperator(mitk::FloatLookupTable);
mitkSpecializeGenericIntLookupTableOperator(mitk::IntLookupTable);
mitkSpecializeGenericIntLookupTableOperator(mitk::StringLookupTable);

mitkSpecializeGenericStringLookupTableOperator(mitk::StringTaggedStringLookupTable);
