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

#include "mitkOclContextCollection.h"

OclContextCollection::OclContextCollection()
: m_CreateContextFailed(false)
, m_CLGLSharingEnabled(false)
, m_PreferredPlatform(0)
, m_PreferredDevice(0)
, m_PreferredPlatformNum(0)
, m_PreferredDeviceNum(0)
{
  cl_int clErr = 0;
  char device_string[1024];
  
  try
  {
    clErr = oclGetPlatformIDs(m_clPlatformsVector);
    CHECK_OCL_ERR(clErr);

    if (m_clPlatformsVector.size() == 0)
    {
      MITK_ERROR("OpenCL.ResourceService") << "No OpenCL compatible platforms were found on the system \n";
      return;
    }
    else
    {
      for (unsigned int i = 0; i < m_clPlatformsVector.size(); i++)
      {
        cl_device_id devices[100];
        cl_uint devices_n = 0;

        cl_platform_id pid = m_clPlatformsVector[i];

        clErr = clGetDeviceIDs(pid, CL_DEVICE_TYPE_GPU, 100, devices, &devices_n);
        CHECK_OCL_ERR(clErr);

        std::vector<cl_device_id> deviceVector;

        for (unsigned int j = 0; j < devices_n; j++)
        {
          deviceVector.push_back(devices[j]);
          
          clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(device_string), &device_string, NULL);
          std::string str(device_string);
          std::string::size_type pos = std::string::npos;
          
          switch (prefVendor)
          {
            case VENDOR_AMD:
              pos = str.find("AMD");
              break;
            case VENDOR_NVIDIA:
              pos = str.find("NVIDIA");
              break;
            case VENDOR_INTEL:
              pos = str.find("INTEL");
              break;
            default:
              pos = std::string::npos;
          }

          if (pos != std::string::npos) 
          {
            m_PreferredPlatform = pid;
            m_PreferredDevice   = devices[j];
            m_PreferredPlatformNum = i;
            m_PreferredDeviceNum   = j;
          }
        }

        m_clDevicesPerPlatform[pid] = deviceVector;
      }
    }
  }
  catch( std::exception& e)
  {
    MITK_ERROR("OpenCL.ResourceService") << "Exception while creating context: \n" << e.what();
  }
}

OclContextCollection::~OclContextCollection()
{
  std::map<int, std::map<int, cl_command_queue> >::iterator it0;
  for (it0 = m_clCmdQueuePerDevicePerPlatform.begin(); it0 != m_clCmdQueuePerDevicePerPlatform.end(); it0++)
  {
    std::map<int, cl_command_queue>::iterator it1;
    for (it1 = it0->second.begin(); it1 != it0->second.end(); it1++)
    {
      clReleaseCommandQueue(it1->second);
    }
  }

  std::map<int, std::map<int, cl_context> >::iterator it2;
  for (it2 = m_clContextPerDevicePerPlatform.begin(); it2 != m_clContextPerDevicePerPlatform.end(); it2++)
  {
    std::map<int, cl_context>::iterator it3;
    for (it3 = it2->second.begin(); it3 != it2->second.end(); it3++)
    {
      clReleaseContext(it3->second);
    }
  }
}

bool OclContextCollection::CanProvideContext() const
{
  return (m_clContextPerDevicePerPlatform.size() != 0 && !m_CreateContextFailed);
}

void OclContextCollection::PrintContextInfo(cl_uint platformNum, cl_uint deviceNum) const
{
  if (m_clDevicesPerPlatform.size() != 0)
  {
    cl_platform_id pid = m_clPlatformsVector[platformNum];
    oclPrintDeviceInfo(m_clDevicesPerPlatform.find(pid)->second[deviceNum]);
  }
}

bool OclContextCollection::CreateContext(cl_uint platformNum, cl_uint deviceNum)
{
  cl_int clErr = 0;

  // Get the requested platform
  if (platformNum < 0 || platformNum > m_clPlatformsVector.size())
    return false;

  cl_platform_id plId = m_clPlatformsVector[platformNum];
  
  std::map<cl_platform_id, std::vector<cl_device_id> >::iterator plIt;
  plIt = m_clDevicesPerPlatform.find(plId);

  if (plIt == m_clDevicesPerPlatform.end())
    return false;

  if (deviceNum < 0 || deviceNum > plIt->second.size())
    return false;

  // Do some platform specific magic to get the current OpenGL context
  #if defined(__APPLE__) || defined(__MACOSX)
    // Apple (untested)
    cl_context_properties contextProperties[] = 
    {
      //CL_CGL_SHAREGROUP_KHR,
      CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
      (cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext()),
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)(plId),
      0
    };
  #else
  #ifdef _WIN32
    // Windows
    cl_context_properties contextProperties[] = 
    {
      CL_GL_CONTEXT_KHR,
      (cl_context_properties)wglGetCurrentContext(),
      CL_WGL_HDC_KHR,
      (cl_context_properties)wglGetCurrentDC(),
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)(plId),
      0
    };
  #else
    // Linux
    cl_context_properties contextProperties[] = 
    {
      CL_GL_CONTEXT_KHR,
      (cl_context_properties)glXGetCurrentContext(),
      CL_GLX_DISPLAY_KHR,
      (cl_context_properties)glXGetCurrentDisplay(),
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)(plId),
      0
    };
  #endif
  #endif

  // Get device ID
  cl_device_id devId = plIt->second[deviceNum];
 // MITK_INFO <<"Requested device num: " <<deviceNum <<" device id: " <<devId <<"\n";

  bool interopPossible =  false;
  
  // Check if the selected device is capable of sharing the context
  if (m_CLGLSharingEnabled)
  {
    interopPossible = IsValidGLCLInteropDevice(plId, devId, contextProperties);
  }

  std::string what;
  cl_context context = 0;
  if (!interopPossible || !m_CLGLSharingEnabled)
  {
    // If context sharing is disbled or not possbile we revert to creating a normal non-shared CL context
    cl_context_properties contextProperties2[] =
    {
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)plId,
      0
    };

    what.append("Attempting to create a non-shared CL-GL context...");
    context = clCreateContext(contextProperties2, 1, &devId, NULL, NULL, &clErr);
  }
  else
  {
    what.append("Attempting to create a shared CL-GL context...");
    context = clCreateContext(contextProperties, 1, &devId, NULL, NULL, &clErr);
  }
  
  if (clErr == 0)
  {
    MITK_INFO <<what <<" SUCCESS!";
  }
  else if (clErr == -1000)
  {
    MITK_INFO <<what <<" FAILED!";
    MITK_INFO <<"Invalid OpenGL context, cannot create shared CL-GL context! " <<clErr;
  }
  else
  {
    MITK_INFO <<what <<" FAILED! Error: " <<clErr;
    CHECK_OCL_ERR(clErr);
  }

  m_CreateContextFailed = (clErr != CL_SUCCESS);

  std::map<int, std::map<int, cl_context> >::iterator it;
  it = m_clContextPerDevicePerPlatform.find(platformNum);

  if (it == m_clContextPerDevicePerPlatform.end())
  {
    std::map<int, cl_context> contextPerDevice;
    contextPerDevice[deviceNum] = context;
    m_clContextPerDevicePerPlatform[platformNum] = contextPerDevice;
  }
  else
  {
    it->second[deviceNum] = context;
  }

  cl_device_id deviceInContext = 0;

  // get device info
  clErr = clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &deviceInContext, NULL);
  CHECK_OCL_ERR(clErr);

  // create command queue
  cl_command_queue cmdQueue = clCreateCommandQueue(context, deviceInContext, 0, &clErr);
  CHECK_OCL_ERR(clErr);

  std::map<int, std::map<int, cl_command_queue> >::iterator it2;
  it2 = m_clCmdQueuePerDevicePerPlatform.find(platformNum);

  if (it2 == m_clCmdQueuePerDevicePerPlatform.end())
  {
    std::map<int, cl_command_queue> cmdQueuePerDevice;
    cmdQueuePerDevice[deviceNum] = cmdQueue;
    m_clCmdQueuePerDevicePerPlatform[platformNum] = cmdQueuePerDevice;
  }
  else
  {
    it2->second[deviceNum] = cmdQueue;
  }

  this->PrintContextInfo(platformNum, deviceNum);

  // collect available image formats for current context
  mitk::OclImageFormats::Pointer imageFormats = mitk::OclImageFormats::New();
  imageFormats->SetGPUContext(context);

  std::map<int, std::map<int, mitk::OclImageFormats::Pointer> >::iterator it3;
  it3 = m_clImageFormatsPerDevicePerPlatform.find(platformNum);

  if (it3 == m_clImageFormatsPerDevicePerPlatform.end())
  {
    std::map<int, mitk::OclImageFormats::Pointer> imageFormatPerDevice;
    imageFormatPerDevice[deviceNum] = imageFormats;
    m_clImageFormatsPerDevicePerPlatform[platformNum] = imageFormatPerDevice;
  }
  else
  {
    it3->second[deviceNum] = imageFormats;
  }
}

cl_context OclContextCollection::GetContext(cl_uint platformNum, cl_uint deviceNum)
{
  std::map<int, std::map<int, cl_context> >::iterator it;
  it = m_clContextPerDevicePerPlatform.find(platformNum);

  //std::cout <<"Using command queue of platform " <<platformNum <<" / device " <<deviceNum <<"\n";

  if (it == m_clContextPerDevicePerPlatform.end())
  {
    CreateContext(platformNum, deviceNum);
    return m_clContextPerDevicePerPlatform[platformNum][deviceNum];
  }

  std::map<int, cl_context>::iterator it2;
  it2 = it->second.find(deviceNum);

  if (it2 == it->second.end())
    CreateContext(platformNum, deviceNum);

  return m_clContextPerDevicePerPlatform[platformNum][deviceNum];
}

cl_command_queue OclContextCollection::GetCommandQueue(cl_uint platformNum, cl_uint deviceNum)
{
  std::map<int, std::map<int, cl_command_queue> >::iterator it;
  it = m_clCmdQueuePerDevicePerPlatform.find(platformNum);

  //std::cout <<"Using command queue of platform " <<platformNum <<" / device " <<deviceNum <<"\n";

  if (it == m_clCmdQueuePerDevicePerPlatform.end())
  {
    CreateContext(platformNum, deviceNum);
    return m_clCmdQueuePerDevicePerPlatform[platformNum][deviceNum];
  }

  std::map<int, cl_command_queue>::iterator it2;
  it2 = it->second.find(deviceNum);

  if (it2 == it->second.end())
    CreateContext(platformNum, deviceNum);

  return m_clCmdQueuePerDevicePerPlatform[platformNum][deviceNum];
}

mitk::OclImageFormats::Pointer OclContextCollection::GetImageFormat(cl_uint platformNum, cl_uint deviceNum)
{
  std::map<int, std::map<int, mitk::OclImageFormats::Pointer> >::iterator it;
  it = m_clImageFormatsPerDevicePerPlatform.find(platformNum);

  if (it == m_clImageFormatsPerDevicePerPlatform.end())
  {
    CreateContext(platformNum, deviceNum);
    return m_clImageFormatsPerDevicePerPlatform[platformNum][deviceNum];
  }

  std::map<int, mitk::OclImageFormats::Pointer>::iterator it2;
  it2 = it->second.find(deviceNum);

  if (it2 == it->second.end())
    CreateContext(platformNum, deviceNum);

  return m_clImageFormatsPerDevicePerPlatform[platformNum][deviceNum];
}

cl_platform_id OclContextCollection::GetPlatformID(cl_uint platformNum)
{
  return m_clPlatformsVector[platformNum];
}

cl_device_id OclContextCollection::GetDeviceID(cl_uint platformNum, cl_uint deviceNum)
{
  cl_platform_id pid = m_clPlatformsVector[platformNum];
  return m_clDevicesPerPlatform.find(pid)->second[deviceNum];
}

bool OclContextCollection::IsValidGLCLInteropDevice(cl_platform_id platform, cl_device_id device, cl_context_properties* properties) 
{
#if !defined(__APPLE__) && !defined(__MACOSX)
  cl_int status = 0;

  // Check if we find the requested device as sharing-capable in the current GL context
  cl_device_id devices[32];
  cl_device_id currentDevice;
  size_t deviceSize = 0;

  // Get the list of devices
  clGetGLContextInfoKHR_fn glGetGLContextInfo_func = (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
  status = glGetGLContextInfo_func(properties, CL_DEVICES_FOR_GL_CONTEXT_KHR, 32 * sizeof(cl_device_id), devices, &deviceSize);
  status = glGetGLContextInfo_func(properties, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, sizeof(cl_device_id), &currentDevice, &deviceSize);

  if(status != CL_SUCCESS) 
  {
    MITK_ERROR <<"CL-GL interop device query (clGetGLContextInfoKHR) has failed. OpenGL context is probably invalid.";
    MITK_ERROR <<"Status: " << status;
    return false;
  }

  if(deviceSize == 0) 
  {
    MITK_ERROR << "No CL-GL interoperable devices found for current platform.";
    return false;
  }

  // Compute how many of them we found
  int numOfDevices = deviceSize / sizeof(cl_device_id);

  // Check if the requested device is listed or not
  for (int i = 0; i < numOfDevices; i++)
    if (devices[i] == device)
      return true;

  return false;
#endif
  return true;

}


