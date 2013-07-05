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

#ifndef COREDATANODEFACTORY_H_INCLUDED
#define COREDATANODEFACTORY_H_INCLUDED

#include <set>

#include <MitkExports.h>
#include "mitkCoreDataNodeFactoryBase.h"

namespace mitk {

class Event;

class MITK_CORE_EXPORT CoreDataNodeFactory : public mitk::CoreDataNodeFactoryBase
{
  public:
    mitkClassMacro(CoreDataNodeFactory, CoreDataNodeFactoryBase);
    itkFactorylessNewMacro(CoreDataNodeFactory);

    virtual void SetDefaultCommonProperties(mitk::DataNode::Pointer &node);
    virtual void RegisterExtraFactory(CoreDataNodeFactoryBase* factory);
    virtual void UnRegisterExtraFactory(CoreDataNodeFactoryBase* factory);

    static Pointer GetInstance();
	
	CoreDataNodeFactoryBase::Pointer GetLastFactory();
	CoreDataNodeFactoryBase::Pointer GetDefaultFactory();
  protected:
    CoreDataNodeFactory();

    typedef std::list<mitk::CoreDataNodeFactoryBase::Pointer> ExtraFactoriesContainer;
    ExtraFactoriesContainer m_ExtraFactories;
};

} // namespace mitk

#endif
