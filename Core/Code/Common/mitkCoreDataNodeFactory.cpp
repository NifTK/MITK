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

#include "mitkConfig.h"
#include "mitkCoreDataNodeFactory.h"
#include "mitkCoreDataNodeFactoryBase.h"
#include <mitkDataNodeFactory.h>

void mitk::CoreDataNodeFactory::RegisterExtraFactory(CoreDataNodeFactoryBase* factory) {
  MITK_DEBUG << "CoreDataNodeFactory: registering extra factory of type " << factory->GetNameOfClass();
  m_ExtraFactories.push_back(CoreDataNodeFactoryBase::Pointer(factory));
}

void mitk::CoreDataNodeFactory::UnRegisterExtraFactory(CoreDataNodeFactoryBase *factory)
{
  MITK_DEBUG << "CoreDataNodeFactory: un-registering extra factory of type " << factory->GetNameOfClass();
  try
  {
    m_ExtraFactories.remove(factory);
  }
  catch( std::exception const& e)
  {
    MITK_ERROR << "Caught exception while unregistering: " << e.what();
  }
}

mitk::CoreDataNodeFactory::Pointer mitk::CoreDataNodeFactory::GetInstance() 
{
  static mitk::CoreDataNodeFactory::Pointer instance;
  if (instance.IsNull())
  {
     instance = mitk::CoreDataNodeFactory::New();
  }
  return instance;
}

void mitk::CoreDataNodeFactory::SetDefaultCommonProperties(mitk::DataNode::Pointer &node)
{
  if(node.IsNull())
    return;

  mitk::DataNode::Pointer nodePointer = node;

  for (ExtraFactoriesContainer::iterator it = m_ExtraFactories.begin(); it != m_ExtraFactories.end() ; it++ ) 
  {
    (*it)->SetDefaultCommonProperties(node);
  }
}

mitk::CoreDataNodeFactory::CoreDataNodeFactory()
{

}

mitk::CoreDataNodeFactoryBase::Pointer mitk::CoreDataNodeFactory::GetLastFactory()
{ 
  if (!m_ExtraFactories.empty())
  {
    ExtraFactoriesContainer::iterator last = m_ExtraFactories.end();
    --last;
	  return *last;
  }
  return mitk::DataNodeFactory::New();
}

mitk::CoreDataNodeFactoryBase::Pointer mitk::CoreDataNodeFactory::GetDefaultFactory()
{
  return mitk::DataNodeFactory::New();
}
