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

#ifndef __mitkOclContextCollection_h
#define __mitkOclContextCollection_h

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

#include <itkFastMutexLock.h>

US_USE_NAMESPACE

// List of vendors
enum cl_vendor 
{
  VENDOR_ANY,
  VENDOR_NVIDIA,
  VENDOR_AMD,
  VENDOR_INTEL
};

// Select a preferred one
#define prefVendor VENDOR_NVIDIA

// Enable context sharing
#if defined (__APPLE__) || defined(MACOSX)
  #ifndef cl_apple_gl_sharing
    #define cl_apple_gl_sharing 1
  #endif
  #define CL_GL_SHARING_EXT "cl_apple_gl_sharing"
#else
  #ifndef cl_khr_gl_sharing
    #define cl_khr_gl_sharing 1
  #endif

  #define CL_GL_SHARING_EXT "cl_khr_gl_sharing"

  #ifndef cl_khr_gl_event
    #define cl_khr_gl_event 1
  #endif

  #define CL_GL_EVENT "cl_khr_gl_event"
#endif
 
//todo add docu!

/** @struct OclContextCollection
 *  @brief An capsulation of all OpenCL context related variables needed for the OclResourceService implementation
 *
 *  The struct gets created on first call to GetContext in the OclResourceService and attepts to initialize all
 *  relevant parts, i.e. the context itself, the device and the command queue
 */
class OclContextCollection
{

public:
  OclContextCollection();
  ~OclContextCollection();

  bool CanProvideContext() const;
  void PrintContextInfo(cl_uint platformNum = 0, cl_uint deviceNum = 0) const;

  inline cl_uint GetNumOfPlatforms() { return (cl_uint)m_clPlatformsVector.size(); }
  inline cl_uint GetNumOfDevicesOnPlatform(cl_uint platformID) { return (cl_uint)m_clDevicesPerPlatform[m_clPlatformsVector[platformID]].size(); }

  bool CreateContext(cl_uint platformNum = 0, cl_uint deviceNum = 0);
  cl_context GetContext(cl_uint platformNum = 0, cl_uint deviceNum = 0);
  cl_command_queue GetCommandQueue(cl_uint platformNum = 0, cl_uint deviceNum = 0);

  mitk::OclImageFormats::Pointer GetImageFormat(cl_uint platformNum = 0, cl_uint deviceNum = 0);
  cl_platform_id GetPlatformID(cl_uint platformNum = 0);
  cl_device_id   GetDeviceID(cl_uint platformNum = 0, cl_uint deviceNum = 0);

  inline void GetPreferredPlatformAndDeviceNum(cl_uint &pla, cl_uint &dev) { pla = m_PreferredPlatformNum; dev = m_PreferredDeviceNum;}

  inline void EnableCLGLContextSharing(bool state) { m_CLGLSharingEnabled = state; }
  bool IsValidGLCLInteropDevice(cl_platform_id platform, cl_device_id device, cl_context_properties* properties);

private:
  /** Available OpenCL platforms */
  std::vector<cl_platform_id>                                   m_clPlatformsVector;
  
  /** Available OpenCL devices per platform */
  std::map<cl_platform_id, std::vector<cl_device_id> >          m_clDevicesPerPlatform;
  std::map<int, std::map<int, cl_context> >                     m_clContextPerDevicePerPlatform;
  std::map<int, std::map<int, cl_command_queue> >               m_clCmdQueuePerDevicePerPlatform;
  std::map<int, std::map<int, mitk::OclImageFormats::Pointer> > m_clImageFormatsPerDevicePerPlatform;
  
  bool           m_CreateContextFailed;
  bool           m_CLGLSharingEnabled;

  cl_platform_id m_PreferredPlatform;
  cl_device_id   m_PreferredDevice;

  cl_uint        m_PreferredPlatformNum;
  cl_uint        m_PreferredDeviceNum;
};


#endif // __mitkOclContextCollection_h
