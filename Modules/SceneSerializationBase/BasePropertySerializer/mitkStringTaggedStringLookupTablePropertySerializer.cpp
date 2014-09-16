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

#ifndef mitkStringTaggedStringLookupTablePropertySerializer_h_included
#define mitkStringTaggedStringLookupTablePropertySerializer_h_included

#include "mitkBasePropertySerializer.h"

#include "mitkProperties.h"

#include <MitkSceneSerializationBaseExports.h>

namespace mitk
{

class MitkSceneSerializationBase_EXPORT StringTaggedStringLookupTablePropertySerializer : public BasePropertySerializer
{
  public:

    mitkClassMacro( StringTaggedStringLookupTablePropertySerializer, BasePropertySerializer );
    itkNewMacro(Self);

    virtual TiXmlElement* Serialize()
    {
      const StringTaggedStringLookupTableProperty* prop = dynamic_cast<const StringTaggedStringLookupTableProperty*>(m_Property.GetPointer());
      if (prop == NULL)
        return NULL;
      StringTaggedStringLookupTable lut = prop->GetValue();
      //if (lut.IsNull())
      //  return NULL; // really?
      const StringTaggedStringLookupTable::LookupTableType& map = lut.GetLookupTable();

      TiXmlElement* element = new TiXmlElement("StringTaggedStringLookupTable");
      for (StringTaggedStringLookupTable::LookupTableType::const_iterator it = map.begin(); it != map.end(); ++it)
        {
          TiXmlElement* tableEntry = new TiXmlElement("LUTValue");
          tableEntry->SetAttribute("id", it->first);
          tableEntry->SetAttribute("value", it->second);
          element->LinkEndChild( tableEntry );
        }
        return element;
    }

    virtual BaseProperty::Pointer Deserialize(TiXmlElement* element)
    {
      if (!element)
        return NULL;

      StringTaggedStringLookupTable lut;
      for( TiXmlElement* child = element->FirstChildElement("LUTValue"); child != NULL; child = child->NextSiblingElement("LUTValue"))
      {

        std::string temp;
        if (child->QueryStringAttribute("id", &temp) == TIXML_WRONG_TYPE)
          return NULL; // TODO: can we do a better error handling?
        StringTaggedStringLookupTable::IdentifierType id = static_cast<StringTaggedStringLookupTable::IdentifierType>(temp);

        if (child->Attribute("value") == NULL)
          return NULL; // TODO: can we do a better error handling?
        StringTaggedStringLookupTable::ValueType val = child->Attribute("value");
        lut.SetTableValue(id, val);
      }
      return StringTaggedStringLookupTableProperty::New(lut).GetPointer();
    }
  protected:
    StringTaggedStringLookupTablePropertySerializer() {}
    virtual ~StringTaggedStringLookupTablePropertySerializer() {}
};
} // namespace
// important to put this into the GLOBAL namespace (because it starts with 'namespace mitk')
MITK_REGISTER_SERIALIZER(StringTaggedStringLookupTablePropertySerializer);
#endif
