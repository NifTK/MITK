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


#ifndef MITKLOOKUPTABLES_H_HEADER_INCLUDED
#define MITKLOOKUPTABLES_H_HEADER_INCLUDED

#include "mitkGenericIntLookupTable.h"
#include "mitkGenericStringLookupTable.h"


namespace mitk {
/**Documentation
* \brief specializations of GenericStringLookupTable and GenericIntLookupTable
*
* This file contains specializations of mitk::GenericIntLookupTable and mitk::GenericStringLookupTable
* for bool, float, int and std::string lookuptables
* \WARN: you have to call the mitkSpecializeGeneric<type>LookupTableOperator macro
* in mitkLookupTables.cpp with each specialization to add an ostream << operator
* for that lookuptable specialization.
*/
   mitkSpecializeGenericIntLookupTable(BoolLookupTable, bool);
   mitkSpecializeGenericIntLookupTable(FloatLookupTable, float);
   mitkSpecializeGenericIntLookupTable(IntLookupTable, int);
   mitkSpecializeGenericIntLookupTable(StringLookupTable, std::string);
   
   mitkSpecializeGenericStringLookupTable(StringTaggedStringLookupTable, std::string);
} // namespace mitk

#endif /* MITKLOOKUPTABLES_H_HEADER_INCLUDED*/
