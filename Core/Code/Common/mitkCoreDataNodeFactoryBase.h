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

#ifndef COREDATANODEFACTORYBASE_H_INCLUDED
#define COREDATANODEFACTORYBASE_H_INCLUDED

// the mbilog header is necessary for CMake test drivers.
// Since the EXTRA_INCLUDE parameter of CREATE_TEST_SOURCELIST only
// allows one extra include file, we specify mitkLog.h here so it will
// be available to all classes implementing this interface.
#include "mitkLog.h"

#include <MitkExports.h>
#include "mitkMapper.h"
#include <mitkDataNodeSource.h>
#include <itkObjectFactoryBase.h>
#include <itkVersion.h>

namespace mitk {

class DataNode;

//## @brief base-class for factories of all DataNodeFactory classes
//## @ingroup Algorithms

class MITK_CORE_EXPORT CoreDataNodeFactoryBase : public mitk::DataNodeSource
{
  public:
    mitkClassMacro(CoreDataNodeFactoryBase, mitk::DataNodeSource);

    virtual void SetDefaultCommonProperties(mitk::DataNode::Pointer &node) = 0;

    virtual const char* GetITKSourceVersion() const
    {
      return ITK_SOURCE_VERSION;
    }
    virtual const char* GetDescription() const
    {
      return "Core DataNode Factory";
    }

  protected:
};
}
#endif
