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


#ifndef __mitkOclResourceServiceImpl_h
#define __mitkOclResourceServiceImpl_h

#include <map>

//Micro Services
#include <usModuleActivator.h>
#include <usModuleContext.h>
#include <usGetModuleContext.h>
#include <usServiceProperties.h>

//ocl
#include "mitkOclResourceService.h"
#include "mitkOclUtils.h"
#include "mitkOclImageFormats.h"
#include "mitkOclContextCollection.h"

#include <itkFastMutexLock.h>

US_USE_NAMESPACE

//todo add docu!

class OclResourceServiceImpl
    : public OclResourceService
{

private:
  // define programmdata private class
  struct ProgramData
  {
    int counter;
    cl_program program;
    itk::FastMutexLock::Pointer mutex;
    ProgramData() :counter(1), program(NULL)
    {}
  };

  typedef std::map< std::string, ProgramData > ProgramMapType;
  //typedef std::map< std::string, std::pair< int, cl_program> > ProgramMapType;

  mutable OclContextCollection* m_ContextCollection;

  /** Map containing all available (allready compiled) OpenCL Programs */
  ProgramMapType m_ProgramStorage;
  /** mutex for manipulating the program storage */
  itk::FastMutexLock::Pointer m_ProgramStorageMutex;

  cl_uint m_CurrentPlatformNum;
  cl_uint m_CurrentDeviceNum;

public:

  OclResourceServiceImpl();
  ~OclResourceServiceImpl();

  virtual void SpecifyPlatformAndDevice(cl_uint platformNum = 0, cl_uint deviceNum = 0, bool sharedCLGL = false);

  cl_context GetContext() const;

  cl_command_queue GetCommandQueue() const;

  cl_device_id   GetCurrentDevice() const;

  cl_platform_id GetCurrentPlatform() const;

  bool GetIsFormatSupported(cl_image_format *);

  void PrintContextInfo() const;

  void InsertProgram(cl_program _program_in, std::string name, bool forceOverride=true);

  cl_program GetProgram(const std::string&name);

  void RemoveProgram(const std::string&name);

  void InvalidateStorage();

  unsigned int GetMaximumImageSize(unsigned int dimension, cl_mem_object_type _imagetype);

  virtual inline cl_uint GetNumOfPlatforms() const { return m_ContextCollection->GetNumOfPlatforms(); }

  virtual inline cl_uint GetNumOfDevicesOnPlatform(cl_uint pid) const { return m_ContextCollection->GetNumOfDevicesOnPlatform(pid); }

};

#endif // __mitkOclResourceServiceImpl_h
