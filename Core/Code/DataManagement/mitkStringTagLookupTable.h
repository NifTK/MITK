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


#ifndef MITKSTRINGTAGLOOKUPTABLE_H_HEADER_INCLUDED
#define MITKSTRINGTAGLOOKUPTABLE_H_HEADER_INCLUDED

#include <string>
#include <sstream>
#include <stdlib.h>
#include <map>

#include <itkDataObject.h>

#include <mitkVector.h>
#include <mitkBaseProperty.h>
#include <mitkProperties.h>

#include <MitkExports.h>

namespace mitk 
{
/**
 * @brief Class for string indexed string lookup table 
 *
 * The main purpose of this class is to be used for storing DICOM tags as:
 * (String ID) (String Value).
 */

  class MITK_EXPORT StringTagLookupTable : public mitk::BaseProperty
  {

  public:
    mitkClassMacro(StringTagLookupTable, BaseProperty); 
    itkNewMacro(StringTagLookupTable);

    typedef std::string                         IdentifierType;
    typedef std::string                         ValueType;
    
    typedef std::map<IdentifierType, ValueType> LookupTableType;
    typedef LookupTableType::iterator           LookupTableIterator;
    typedef LookupTableType::const_iterator     LookupTableConstIterator;

    StringTagLookupTable() {}
    virtual ~StringTagLookupTable() {}

    //virtual const char *GetNameOfClass() const
    //{
    //  return "StringTagLookupTable";
    //}

    void SetTableValue(IdentifierType id, ValueType value)
    {
      m_LookupTable[id] = value;
    }

    bool ValueExists(IdentifierType id) const
    {
      LookupTableConstIterator it = m_LookupTable.find(id);
      return (it != m_LookupTable.end());
    }

    ValueType GetTableValue(IdentifierType id) const
    {
      LookupTableConstIterator it = m_LookupTable.find(id);
      if (it != m_LookupTable.end())
        return it->second;
      else
        return std::string("Not Available");
    }

    const LookupTableType& GetLookupTable() const
    {
      return m_LookupTable;
    }

    bool operator==(const Self& lookupTable) const
    {
      return (m_LookupTable == lookupTable.m_LookupTable);
    }
    
    bool operator!=(const Self& lookupTable) const
    {
      return !(m_LookupTable == lookupTable.m_LookupTable);
    }

    virtual Self& operator=(const Self& other)  // \TODO: this needs to be unit tested!
    {
      if (this == &other)
      {
        return *this;
      }
      else
      {
        m_LookupTable.clear();
        m_LookupTable = other.m_LookupTable;
        return *this;
      }
    }

    virtual std::string GetValueAsString() const
    {
      std::stringstream sstream;

      LookupTableConstIterator iter = this->m_LookupTable.begin();

      for (iter = m_LookupTable.begin(); iter != m_LookupTable.end(); iter++)
      {
        sstream << "(" << iter->first << ")";
        sstream << " = " << iter->second << std::endl;
      }
      return sstream.str();
    }

  protected:
    LookupTableType m_LookupTable;

  private:
    virtual bool IsEqual(const BaseProperty& other) const
    {
      return (this->m_LookupTable == static_cast<const Self&>(other).m_LookupTable);
    }

    virtual bool Assign(const BaseProperty& other)
    {
      this->m_LookupTable = static_cast<const Self&>(other).m_LookupTable;
      return true;
    }

    itk::LightObject::Pointer InternalClone() const
    {
      itk::LightObject::Pointer result(new Self(*this));
      return result;
    }

  };
} // namespace mitk

#endif /* MITKSTRINGTAGLOOKUPTABLE_H_HEADER_INCLUDED */
