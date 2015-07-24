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

#include "mitkOclResourceServiceImpl_p.h"

OclResourceService::~OclResourceService()
{
}

OclResourceServiceImpl::OclResourceServiceImpl()
: m_ContextCollection(NULL),
  m_ProgramStorage(),
  m_CurrentPlatformNum(0),
  m_CurrentDeviceNum(0),
  m_ContextInitialized(false)
{
  m_ProgramStorageMutex = itk::FastMutexLock::New();
}

OclResourceServiceImpl::~OclResourceServiceImpl()
{
  // if map non-empty, release all remaining
  if( m_ProgramStorage.size() )
  {
    ProgramMapType::iterator it = m_ProgramStorage.begin();
    while(it != m_ProgramStorage.end() )
    {
      clReleaseProgram( it->second.program );
      m_ProgramStorage.erase( it++ );
    }
  }

  if( m_ContextCollection )
    delete m_ContextCollection;
}

bool OclResourceServiceImpl::InitContext()
{
  if (m_ContextInitialized)
    return true;

  m_ContextCollection = new OclContextCollection();
  m_ContextCollection->GetPreferredPlatformAndDeviceNum(m_CurrentPlatformNum, m_CurrentDeviceNum);

  m_ContextInitialized = true;
}

void OclResourceServiceImpl::SpecifyPlatformAndDevice(cl_uint platformNum, cl_uint deviceNum, bool sharedCLGL)
{
  if (!m_ContextInitialized)
    InitContext();
  
  //oclPrintFullCLInfo();

  m_CurrentPlatformNum = platformNum;
  m_CurrentDeviceNum   = deviceNum;

  bool supported = false;

  // Check if the sharing extension is supported by the device
  if (sharedCLGL)
    supported = IsCLExtensionSupported(CL_GL_SHARING_EXT, GetCurrentDevice());

  if( m_ContextCollection == NULL )
    m_ContextCollection = new OclContextCollection();

  if (sharedCLGL && supported)
    m_ContextCollection->EnableCLGLContextSharing(true);
  else
    m_ContextCollection->EnableCLGLContextSharing(false);
}

cl_context OclResourceServiceImpl::GetContext()
{
  if (!m_ContextInitialized)
    InitContext();

  return m_ContextCollection->GetContext(m_CurrentPlatformNum, m_CurrentDeviceNum);
}

cl_command_queue OclResourceServiceImpl::GetCommandQueue()
{
  // check if queue valid
  cl_context clQueueContext;

  // check if there is a context available
  // if not create one
  if (!m_ContextInitialized)
    InitContext();

  cl_int clErr = clGetCommandQueueInfo( m_ContextCollection->GetCommandQueue(m_CurrentPlatformNum, m_CurrentDeviceNum), CL_QUEUE_CONTEXT, sizeof(clQueueContext), &clQueueContext, NULL );
  if( clErr != CL_SUCCESS || clQueueContext != m_ContextCollection->GetContext(m_CurrentPlatformNum, m_CurrentDeviceNum))
  {
    MITK_WARN << "Have no valid command queue. Query returned : " << GetOclErrorAsString( clErr );
    return NULL;
  }

  return m_ContextCollection->GetCommandQueue(m_CurrentPlatformNum, m_CurrentDeviceNum);
}

cl_device_id OclResourceServiceImpl::GetCurrentDevice()
{
  if (!m_ContextInitialized)
    InitContext();

  return m_ContextCollection->GetDeviceID(m_CurrentPlatformNum, m_CurrentDeviceNum);
}

cl_platform_id OclResourceServiceImpl::GetCurrentPlatform()
{
  if (!m_ContextInitialized)
    InitContext();
  return m_ContextCollection->GetPlatformID(m_CurrentPlatformNum);
}

bool OclResourceServiceImpl::GetIsFormatSupported(cl_image_format *fmt)
{
  if (!m_ContextInitialized)
    InitContext();
  
  cl_image_format temp;
  temp.image_channel_data_type = fmt->image_channel_data_type;
  temp.image_channel_order = fmt->image_channel_order;

  return (this->m_ContextCollection->GetImageFormat(m_CurrentPlatformNum, m_CurrentDeviceNum)->GetNearestSupported(&temp, fmt));
}

void OclResourceServiceImpl::PrintContextInfo()
{
  if (!m_ContextInitialized)
    InitContext();
  
  // context and devices available
  if( m_ContextCollection->CanProvideContext() )
  {
    this->m_ContextCollection->PrintContextInfo(m_CurrentPlatformNum, m_CurrentDeviceNum);
  }
}

void OclResourceServiceImpl::InsertProgram(cl_program _program_in, std::string name, bool forceOverride)
{
  typedef std::pair < std::string, ProgramData > MapElemPair;
  std::pair< ProgramMapType::iterator, bool> retValue;

  ProgramData data;
  data.counter = 1;
  data.program = _program_in;
  data.mutex = itk::FastMutexLock::New();

  // program is not stored, insert first instance (count = 1)
  m_ProgramStorageMutex->Lock();
  retValue = m_ProgramStorage.insert( MapElemPair(name, data) );
  m_ProgramStorageMutex->Unlock();

  // insertion failed, i.e. a program with same name exists
  if( !retValue.second )
  {
    std::string overrideMsg("");
    if( forceOverride )
    {
      // overwrite old instance
      m_ProgramStorage[name].program = _program_in;
      overrideMsg +=" The old program was overwritten!";
    }

    MITK_WARN("OpenCL.ResourceService") << "The program " << name << " already exists." << overrideMsg;
  }
}

cl_program OclResourceServiceImpl::GetProgram(const std::string &name)
{
  ProgramMapType::iterator it = m_ProgramStorage.find(name);

  if( it != m_ProgramStorage.end() )
  {
    it->second.mutex->Lock();
    // first check if the program was deleted
    // while waiting on the mutex
    if ( it->second.counter == 0 )
      mitkThrow() << "Requested OpenCL Program (" << name <<") not found."
                  << "(deleted while waiting on the mutex)";
    // increase the reference counter
    // by one if the program is requestet
    it->second.counter += 1;
    it->second.mutex->Unlock();

    // return the cl_program
    return it->second.program;
  }

  mitkThrow() << "Requested OpenCL Program (" << name <<") not found.";
}

void OclResourceServiceImpl::RemoveProgram(const std::string& name)
{
  ProgramMapType::iterator it = m_ProgramStorage.find(name);
  cl_int status = 0;
  cl_program program = NULL;

  if( it != m_ProgramStorage.end() )
  {
    it->second.mutex->Lock();
    // decrease reference by one
    it->second.counter -= 1;
    it->second.mutex->Unlock();

    // remove from the storage
    if( it->second.counter == 0 )
    {
      program = it->second.program;

      m_ProgramStorageMutex->Lock();
      m_ProgramStorage.erase(it);
      m_ProgramStorageMutex->Unlock();
    }

    // delete the program
    if( program )
    {
      status = clReleaseProgram(program);
      CHECK_OCL_ERR(status);
    }
  }
  else
  {
    MITK_WARN("OpenCL.ResourceService") << "Program name [" <<name <<"] passed for deletion not found.";
  }
}

void OclResourceServiceImpl::InvalidateStorage()
{
  // do nothing if no context present, there is also no storage
  if( !m_ContextCollection->CanProvideContext() )
    return;

  // query the map
  ProgramMapType::iterator it = m_ProgramStorage.begin();

  while(it != m_ProgramStorage.end() )
  {
    // query the program build status
    cl_build_status status;
    unsigned int query = clGetProgramBuildInfo( it->second.program, m_ContextCollection->GetDeviceID(m_CurrentPlatformNum, m_CurrentDeviceNum), CL_PROGRAM_BUILD_STATUS, sizeof(cl_int), &status, NULL );
    CHECK_OCL_ERR( query )

    MITK_DEBUG << "Quering status for " << it->first << std::endl;

    // remove program if no succesfull build
    // we need to pay attention to the increment of the iterator when erasing current element
    if( status != CL_BUILD_SUCCESS )
    {
      MITK_DEBUG << " +-- Build failed " << std::endl;
      m_ProgramStorage.erase( it++ );
    }
    else
    {
      ++it;
    }
  }
}

unsigned int OclResourceServiceImpl::GetMaximumImageSize(unsigned int dimension, cl_mem_object_type _imagetype)
{
  if( ! m_ContextCollection->CanProvideContext() )
    return 0;

  size_t retValue = 0;

  switch(dimension)
  {
  case 0:
    if ( _imagetype == CL_MEM_OBJECT_IMAGE2D)
      clGetDeviceInfo( m_ContextCollection->GetDeviceID(m_CurrentPlatformNum, m_CurrentDeviceNum), CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof( cl_uint ), &retValue, NULL );
    else
      clGetDeviceInfo( m_ContextCollection->GetDeviceID(m_CurrentPlatformNum, m_CurrentDeviceNum), CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof( cl_uint ), &retValue, NULL );

    break;
  case 1:
    if ( _imagetype == CL_MEM_OBJECT_IMAGE2D)
      clGetDeviceInfo( m_ContextCollection->GetDeviceID(m_CurrentPlatformNum, m_CurrentDeviceNum), CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof( cl_uint ), &retValue, NULL );
    else
      clGetDeviceInfo( m_ContextCollection->GetDeviceID(m_CurrentPlatformNum, m_CurrentDeviceNum), CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof( cl_uint ), &retValue, NULL );
    break;
  case 2:
    if ( _imagetype == CL_MEM_OBJECT_IMAGE3D)
      clGetDeviceInfo( m_ContextCollection->GetDeviceID(m_CurrentPlatformNum, m_CurrentDeviceNum), CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof( cl_uint ), &retValue, NULL);
    break;
  default:
    MITK_WARN << "Could not recieve info. Desired dimension or object type does not exist. ";
    break;
  }

  return retValue;
}
