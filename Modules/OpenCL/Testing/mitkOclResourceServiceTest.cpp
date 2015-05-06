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

#include <mitkTestingMacros.h>
#include <mitkLogMacros.h>
#include "mitkOclUtils.h"

#include <usModuleContext.h>
#include <usGetModuleContext.h>

#include "mitkOclResourceService.h"
#include "mitkException.h"

#include <itkLightObject.h>

#include <string>

using namespace mitk;

void TestKernelCompilation()
{
  us::ServiceReference<OclResourceService> ref = us::GetModuleContext()->GetServiceReference<OclResourceService>();
  MITK_TEST_CONDITION_REQUIRED( ref != NULL, "Resource service available." );

  OclResourceService* resources = us::GetModuleContext()->GetService<OclResourceService>(ref);
  cl_context gpuContext = resources->GetContext();
  MITK_TEST_CONDITION_REQUIRED(gpuContext != NULL, "Got not-null OpenCL context.");

  // further tests requires for valid context
  if (gpuContext)
  {
    cl_image_format testFmt;
    testFmt.image_channel_data_type = CL_FLOAT;
    testFmt.image_channel_order = CL_RGBA;

    MITK_TEST_CONDITION( resources->GetIsFormatSupported( &testFmt ), "Checking if format CL_FLOAT / CL_RGBA supported." );
  }

  // create test program
  const std::string testProgramSource =
    "__kernel void testKernel( __global uchar* buffer ){ \
    const unsigned int globalPosX = get_global_id(0); \
    buffer[globalPosX] = buffer[globalPosX] + 1;}";

  cl_int err = 0;
  size_t progSize = testProgramSource.length();
  const char* progSource = testProgramSource.c_str();
  cl_program testProgram = clCreateProgramWithSource(gpuContext, 1, &progSource, &progSize, &err );

  MITK_TEST_CONDITION_REQUIRED( err == CL_SUCCESS, "Test program loaded succesfully.");

  err = clBuildProgram(testProgram, 0, NULL, NULL, NULL, NULL);
  MITK_TEST_CONDITION_REQUIRED( err == CL_SUCCESS, "Test program built succesfully.");

  resources->InsertProgram( testProgram, "test_program", true);
  MITK_TEST_CONDITION( resources->GetProgram("test_program") == testProgram, "Program correctly stored by ResourceService");

  // the manger throws exception when accessing non-existant programs
  MITK_TEST_FOR_EXCEPTION( mitk::Exception, resources->GetProgram("blah"); );

  // another test source, this one does not compile
  const std::string testProgramSource_notCompiling =
    "__kernel void testKernel( __global uchar* buffer ){ \
    const unsigned intt globalPosX = get_global_id(0); }";

  progSize = testProgramSource_notCompiling.length();
  const char* progSource2 = testProgramSource_notCompiling.c_str();
  cl_program notComp_testProgram = clCreateProgramWithSource(gpuContext, 1, &progSource2, &progSize, &err );

  // the error in the source code has no influence on loading the program
  MITK_TEST_CONDITION_REQUIRED( err == CL_SUCCESS, "Test program 2 loaded succesfully.");

  err = clBuildProgram(notComp_testProgram, 0, NULL, NULL, NULL, NULL);
  MITK_TEST_CONDITION_REQUIRED( err == CL_BUILD_PROGRAM_FAILURE, "Test program 2 failed to build.");
  std::cout << " --> The (expected) OpenCL Build Error occured : ";// << GetOclErrorString(err);

  resources->InsertProgram( notComp_testProgram, "test_program_failed", true);
  MITK_TEST_CONDITION( resources->GetProgram("test_program_failed") == notComp_testProgram, "Program correctly stored by ResourceService");

  // calling the InvalidateStorage() will result in removing the _failed test program inserted above
  resources->InvalidateStorage();

  // the second test program should no more exist in the storage, hence we await an exception
  MITK_TEST_FOR_EXCEPTION( mitk::Exception, resources->GetProgram("test_program_failed"); );
}

void TestKernelExecution()
{
  us::ServiceReference<OclResourceService> ref = us::GetModuleContext()->GetServiceReference<OclResourceService>();
  MITK_TEST_CONDITION_REQUIRED( ref != NULL, "Resource service available." );

  OclResourceService* resources = us::GetModuleContext()->GetService<OclResourceService>(ref);
  cl_context gpuContext = resources->GetContext();
  MITK_TEST_CONDITION_REQUIRED(gpuContext != NULL, "Got not-null OpenCL context.");

  cl_command_queue cmdQueue = resources->GetCommandQueue();
  MITK_TEST_CONDITION_REQUIRED(cmdQueue != NULL, "Got not-null OpenCL command queue." );

  // further tests requires for valid context
  if (gpuContext)
  {
    cl_image_format testFmt;
    testFmt.image_channel_data_type = CL_FLOAT;
    testFmt.image_channel_order = CL_RGBA;
  }

  // create test program
  const std::string testProgramSource =
    "__kernel void testKernel( __global uchar* buffer ){ \
    const unsigned int globalPosX = get_global_id(0); \
    buffer[globalPosX] = buffer[globalPosX] + 1;}";

  cl_int err = 0;
  size_t progSize = testProgramSource.length();
  const char* progSource = testProgramSource.c_str();
  cl_program testProgram = clCreateProgramWithSource(gpuContext, 1, &progSource, &progSize, &err );
  err = clBuildProgram(testProgram, 0, NULL, NULL, NULL, NULL);
  MITK_TEST_CONDITION_REQUIRED( err == CL_SUCCESS, "Test program built succesfully.");
  resources->InsertProgram( testProgram, "test_program", true);

   // Create the kernels
  cl_kernel testKernel = clCreateKernel(testProgram, "testKernel", &err);
  MITK_TEST_CONDITION_REQUIRED( err == CL_SUCCESS, "Test kernel created succesfully.");

  // Try to run the kernel
  unsigned int sNum = 256;
  unsigned char * testBuf = new unsigned char[sNum];
  memset(testBuf, 0, sNum);
  cl_mem testMem = clCreateBuffer(gpuContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sNum * sizeof(cl_uchar), &testBuf, &err);
  CHECK_OCL_ERR(err);

  size_t gWorkDims[1];
  gWorkDims[0] = 256;//ToMultipleOf(((entryPointNum > 8) ? entryPointNum : 8), 8);

  // Launch kernel
  err = clSetKernelArg(testKernel, 0, sizeof(cl_mem), &testMem);
  MITK_TEST_CONDITION_REQUIRED( err == CL_SUCCESS, "Test kernel argument was assigned successfully.");

  err = clEnqueueNDRangeKernel(cmdQueue, testKernel, 1, NULL, gWorkDims, 0, 0, 0, 0);
  err |= clFinish(cmdQueue);

  MITK_TEST_CONDITION_REQUIRED( err == CL_SUCCESS, "Test kernel executed without errors.");

  unsigned char * outBuf = new unsigned char[sNum];
  err = clEnqueueReadBuffer(cmdQueue, testMem, CL_TRUE, 0, sNum * sizeof(cl_uchar), outBuf, 0, 0, 0);
  MITK_TEST_CONDITION_REQUIRED( err == CL_SUCCESS, "Results were read without errors.");

  bool error = false;

  for (int i = 0; i < sNum; i++)
  {
    error = (outBuf[i] == i+1);
    if (!error)
      break;
  }
  
  MITK_TEST_CONDITION_REQUIRED(err == false, "Results are as expected.");

  clReleaseMemObject(testMem);

  delete testBuf;
  delete outBuf;

  // calling the InvalidateStorage() will result in removing the _failed test program inserted above
  resources->InvalidateStorage();

  // the second test program should no more exist in the storage, hence we await an exception
  MITK_TEST_FOR_EXCEPTION( mitk::Exception, resources->GetProgram("test_program_failed"); );
}


/**
  This function is testing the class mitk::OclContextManager.
  */
int mitkOclResourceServiceTest( int argc, char* argv[] )
{
  MITK_TEST_BEGIN("mitkOclResourceServiceTest");

  // Print detailed OpenCL info
  oclPrintFullCLInfo();

  us::ServiceReference<OclResourceService> ref = us::GetModuleContext()->GetServiceReference<OclResourceService>();
  MITK_TEST_CONDITION_REQUIRED( ref != NULL, "Resource service available." );

  OclResourceService* resources = us::GetModuleContext()->GetService<OclResourceService>(ref);
  MITK_TEST_CONDITION_REQUIRED( resources != NULL, "Resource service available." );

  cl_context first = resources->GetContext();
  MITK_TEST_CONDITION_REQUIRED(first != NULL, "Got not-null OpenCL context.");

  OclResourceService* resources_2 = us::GetModuleContext()->GetService<OclResourceService>(ref);
  MITK_TEST_CONDITION_REQUIRED( resources == resources_2, "Same resource reference the second time." );

  cl_context second = resources_2->GetContext();
  MITK_TEST_CONDITION_REQUIRED( first == second, "Both return same context");

  int totalNumOfDevices = 0;
  MITK_INFO <<"Performing a platform and device query.. ";
  for (int i = 0; i < resources->GetNumOfPlatforms(); i++)
  {
    MITK_INFO <<"Number of CL devices on platform " <<i <<": " <<resources->GetNumOfDevicesOnPlatform(i) <<"\n";
    totalNumOfDevices += resources->GetNumOfDevicesOnPlatform(i);
  }

  TestKernelCompilation();
  TestKernelExecution();

  // In case we have a multi GPU environment
  if (resources->GetNumOfPlatforms() > 1 && totalNumOfDevices > 1)
  {
    // Query the default platform
    resources->SpecifyPlatformAndDevice(0, 0);

    cl_platform_id platf0 = resources->GetCurrentPlatform();
    cl_device_id   dev00  = resources->GetCurrentDevice();
    MITK_TEST_CONDITION_REQUIRED((platf0 != NULL) && (dev00 != NULL), "Platform - device pair is valid.");

    cl_context context00 = resources->GetContext();
    MITK_TEST_CONDITION_REQUIRED(context00 != NULL, "Got not-null OpenCL context.");

    cl_command_queue cmdQueue00 = resources->GetCommandQueue();
    MITK_TEST_CONDITION_REQUIRED(cmdQueue00 != NULL, "Got not-null OpenCL command queue." );

    // Switch to the second platform / device
    resources->SpecifyPlatformAndDevice(1, 0);

    cl_platform_id platf1 = resources->GetCurrentPlatform();
    cl_device_id   dev10  = resources->GetCurrentDevice();
    MITK_TEST_CONDITION_REQUIRED((platf1 != NULL) && (dev10 != NULL), "Platform - device pair is valid.");

    cl_context context10 = resources->GetContext();
    MITK_TEST_CONDITION_REQUIRED(context10 != NULL, "Got not-null OpenCL context.");

    cl_command_queue cmdQueue10 = resources->GetCommandQueue();
    MITK_TEST_CONDITION_REQUIRED(cmdQueue10 != NULL, "Got not-null OpenCL command queue." );

    // Test if the acquired IDs are different for platform - device pairs
    MITK_TEST_CONDITION_REQUIRED((platf0 != platf1), "Platform IDs are different - they were resolved correctly.");
    MITK_TEST_CONDITION_REQUIRED((dev00 != dev10), "Device IDs are different - they were resolved correctly.");
    MITK_TEST_CONDITION_REQUIRED((context10 != context00), "Context IDs are different - they were resolved correctly.");
    MITK_TEST_CONDITION_REQUIRED((cmdQueue10 != cmdQueue00), "Command queues are different - they were resolved correctly.");

    // Switch platform again
    resources->SpecifyPlatformAndDevice(0, 0);
    MITK_TEST_CONDITION_REQUIRED((platf0 == resources->GetCurrentPlatform()), "Platform switched successfuly");
    MITK_TEST_CONDITION_REQUIRED((dev00 == resources->GetCurrentDevice()), "Device switched successfuly");
    MITK_TEST_CONDITION_REQUIRED((context00 == resources->GetContext()), "Context switched successfuly");
    MITK_TEST_CONDITION_REQUIRED((cmdQueue00 == resources->GetCommandQueue()), "Command queue switched successfuly");
    TestKernelCompilation();
    TestKernelExecution();

    // Switch platform again
    resources->SpecifyPlatformAndDevice(1, 0);
    MITK_TEST_CONDITION_REQUIRED((platf1 == resources->GetCurrentPlatform()), "Platform switched successfuly");
    MITK_TEST_CONDITION_REQUIRED((dev10 == resources->GetCurrentDevice()), "Device switched successfuly");
    MITK_TEST_CONDITION_REQUIRED((context10 == resources->GetContext()), "Context switched successfuly");
    MITK_TEST_CONDITION_REQUIRED((cmdQueue10 == resources->GetCommandQueue()), "Command queue switched successfuly");
    TestKernelCompilation();
    TestKernelExecution();
  }

  MITK_TEST_END();
}

US_INITIALIZE_MODULE
