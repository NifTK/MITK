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

#include "mitkOclUtils.h"
#include "mitkLogMacros.h"
#include <cstdio>
#include <cstring>
#include <sstream>

unsigned int iDivUp(unsigned int dividend, unsigned int divisor){
  return (dividend % divisor == 0) ? (dividend / divisor) : (dividend / divisor + 1);
}

cl_int oclGetPlatformIDs(std::vector<cl_platform_id> &platforms)
{
  cl_uint num_platforms = 0;
  cl_int ciErrNum = 0;

  ciErrNum = clGetPlatformIDs(0, NULL, &num_platforms);

  if (ciErrNum != CL_SUCCESS)
  {
    MITK_ERROR<<" Error " << ciErrNum << " in clGetPlatformIDs() \n";
    throw std::bad_exception();
    return ciErrNum;
  }
  else
  {
    cl_platform_id * clPlatformIDs = new cl_platform_id[num_platforms];
    ciErrNum = clGetPlatformIDs(num_platforms, clPlatformIDs, NULL);

    for (int i = 0; i < num_platforms; i++)
      platforms.push_back(clPlatformIDs[i]);

    delete clPlatformIDs;
    return ciErrNum;
  }

  return CL_SUCCESS;
}

void oclPrintMemObjectInfo(cl_mem memobj)
{
  cl_int clErr = 0;

  MITK_INFO << "Examining cl_mem object: " << memobj
            << "\n------------------\n";

  // CL_MEM_TYPE
  cl_mem_object_type objtype;
  clErr = clGetMemObjectInfo( memobj, CL_MEM_TYPE, sizeof(cl_mem_object_type),&objtype, NULL);
  CHECK_OCL_ERR( clErr );

  switch(objtype)
  {
  case CL_MEM_OBJECT_BUFFER:
    MITK_INFO << "CL_MEM_TYPE \t" << "BUFFER_OBJ" << "\n";
    break;
  case CL_MEM_OBJECT_IMAGE2D:
    MITK_INFO << "CL_MEM_TYPE \t" << "2D IMAGE" << "\n";
    break;
  case CL_MEM_OBJECT_IMAGE3D:
    MITK_INFO << "CL_MEM_TYPE \t" << "3D IMAGE" << "\n";
    break;
  default:
    MITK_INFO << "CL_MEM_TYPE \t" << "[could not resolve]" << "\n";
    break;
  }

  // CL_MEM_FLAGS
  cl_mem_flags flags;
  clErr = clGetMemObjectInfo( memobj, CL_MEM_FLAGS, sizeof(cl_mem_flags),&flags, NULL);
  CHECK_OCL_ERR( clErr );

  switch(flags)
  {
  case CL_MEM_READ_ONLY:
    MITK_INFO << "CL_MEM_FLAGS \t" << "CL_MEM_READ_ONLY" << "\n";
    break;
  case CL_MEM_WRITE_ONLY:
    MITK_INFO << "CL_MEM_FLAGS \t" << "CL_MEM_WRITE_ONLY" << "\n";
    break;
  case CL_MEM_READ_WRITE:
    MITK_INFO << "CL_MEM_FLAGS \t" << "CL_MEM_READ_WRITE" << "\n";
    break;
  default:
    MITK_INFO << "CL_MEM_FLAGS \t" << "not resolved, " << flags << "\n";
    break;
  }

  // get CL_MEM_SIZE
  size_t memsize;
  clErr = clGetMemObjectInfo( memobj, CL_MEM_SIZE, sizeof(memsize),&memsize, NULL);
  CHECK_OCL_ERR( clErr );

  MITK_INFO << "CL_MEM_SIZE \t" << memsize << "\n";

  // get CL_MEM_HOST_PTR
  float *hostptr;
  clErr = clGetMemObjectInfo( memobj, CL_MEM_HOST_PTR, sizeof(void*), (void*) &hostptr, NULL);
  CHECK_OCL_ERR( clErr );

  MITK_INFO << "CL_MEM_HOST_PTR \t" << hostptr << "\n";

  // get CL_CONTEXT
  cl_context gpuctxt;
  clErr = clGetMemObjectInfo( memobj, CL_MEM_CONTEXT, sizeof(cl_context), &gpuctxt, NULL);
  CHECK_OCL_ERR( clErr );

  MITK_INFO << "CL_CONTEXT \t\t" << gpuctxt << "\n";

  // get CL_MEM_REFERENCE_COUNT
  cl_uint refs;
  clErr = clGetMemObjectInfo( memobj, CL_MEM_REFERENCE_COUNT, sizeof(cl_uint), &refs, NULL);
  CHECK_OCL_ERR(clErr);

  MITK_INFO << "CL_REF_COUNT \t" << refs << "\n";

  MITK_INFO << "================== \n" << std::endl;
}

void oclPrintDeviceInfo(cl_device_id device)
{
  char device_string[1024];

  clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
  MITK_INFO("ocl.log")<< " Device : " << device_string;

  // CL_DEVICE_INFO
  cl_device_type type;
  clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, NULL);
  if( type & CL_DEVICE_TYPE_CPU )
    MITK_INFO("ocl.log")<<"  CL_DEVICE_TYPE:\t\tCL_DEVICE_TYPE_CPU";
  if( type & CL_DEVICE_TYPE_GPU )
    MITK_INFO("ocl.log")<<"  CL_DEVICE_TYPE:\t\tCL_DEVICE_TYPE_GPU";
  if( type & CL_DEVICE_TYPE_ACCELERATOR )
    MITK_INFO("ocl.log")<<"  CL_DEVICE_TYPE:\t\tCL_DEVICE_TYPE_ACCELERATOR";
  if( type & CL_DEVICE_TYPE_DEFAULT )
    MITK_INFO("ocl.log")<<"  CL_DEVICE_TYPE:\t\tCL_DEVICE_TYPE_DEFAULT";

  // CL_DEVICE_MAX_COMPUTE_UNITS
  cl_uint compute_units;
  clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_MAX_COMPUTE_UNITS:\t" << compute_units;

  // CL_DEVICE_MAX_WORK_GROUP_SIZE
  size_t workitem_size[3];
  clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_MAX_WORK_ITEM_SIZES:\t"<< workitem_size[0] <<" " << workitem_size[1] <<" " << workitem_size[2];

  // CL_DEVICE_MAX_WORK_GROUP_SIZE
  size_t workgroup_size;
  clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_MAX_WORK_GROUP_SIZE:\t" << workgroup_size;

  // CL_DEVICE_MAX_CLOCK_FREQUENCY
  cl_uint clock_frequency;
  clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_MAX_CLOCK_FREQUENCY:\t"<< clock_frequency / 1000;

  // CL_DEVICE_IMAGE_SUPPORT
  cl_bool image_support;
  clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(image_support), &image_support, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_IMAGE_SUPPORT:\t" << image_support;

  // CL_DEVICE_GLOBAL_MEM_SIZE
  cl_ulong mem_size;
  clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_GLOBAL_MEM_SIZE:\t"<<(unsigned int)(mem_size / (1024 * 1024))<<"Mbytes";

  // CL_DEVICE_MAX_MEM_ALLOC_SIZE
  clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(mem_size), &mem_size, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_MAX_MEM_ALLOC_SIZE:\t"<<(unsigned int)(mem_size / (1024 * 1024))<<"Mbytes";

  // CL_DEVICE_LOCAL_MEM_SIZE
  clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_LOCAL_MEM_SIZE:\t"<<  (unsigned int)(mem_size / (1024)) <<"KByte\n";

  //check for image support properties
  clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(workgroup_size), &workgroup_size, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_IMAGE2D_MAX_WIDTH:\t" << workgroup_size;

  clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(workgroup_size), &workgroup_size, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_IMAGE2D_MAX_HEIGHT:\t" << workgroup_size;

  clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(workgroup_size), &workgroup_size, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_IMAGE3D_MAX_WIDTH:\t" << workgroup_size;

  clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(workgroup_size), &workgroup_size, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_IMAGE3D_MAX_HEIGHT:\t" << workgroup_size;

  clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(workgroup_size), &workgroup_size, NULL);
  MITK_INFO("ocl.log")<<"  CL_DEVICE_IMAGE3D_MAX_DEPTH:\t" << workgroup_size;


  // CL_DEVICE_QUEUE_PROPERTIES
  cl_command_queue_properties queue_properties;
  clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(queue_properties), &queue_properties, NULL);
  if( queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE )
    MITK_INFO("ocl.log")<<"  CL_DEVICE_QUEUE_PROPERTIES:\t"<< "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE";
  if( queue_properties & CL_QUEUE_PROFILING_ENABLE )
    MITK_INFO("ocl.log")<<"  CL_DEVICE_QUEUE_PROPERTIES:\t"<< "CL_QUEUE_PROFILING_ENABLE";

}

void oclPrintFullCLInfo()
{
  cl_int status = 0;

  //Platform Information
  cl_uint numPlatforms;
  cl_platform_id* clSelectedPlatformID = NULL;

  //get the number of available platforms
  clGetPlatformIDs(0, NULL, &numPlatforms);

  //alloc memory so we can get the whole list
  clSelectedPlatformID = (cl_platform_id*)malloc(sizeof(cl_platform_id)*numPlatforms);

  //get the list of available platforms
  status = clGetPlatformIDs(numPlatforms, clSelectedPlatformID, NULL);

  if(status != CL_SUCCESS) //we aren't going to bother with any error checking just now, just quit out
    return;

  //print out some debug info
  char * tmpStr = new char[1024];
  sprintf(tmpStr, "\n\n------------------------------------\n"); std::cout << tmpStr <<"\n";
  sprintf(tmpStr, "\n\nPrinting CLINFO "); std::cout << tmpStr <<"\n\n";

  sprintf(tmpStr, "Available platforms: \t\t\t%d", numPlatforms);  std::cout << tmpStr <<"\n";
  sprintf(tmpStr, "------------------------------------"); std::cout << tmpStr <<"\n\n";
  char platform_info[1024];
  
  for(unsigned int i=0; i<numPlatforms; i++)
  {

    std::cout <<"PLATFORM "<<i <<"\n";

    if(clGetPlatformInfo(clSelectedPlatformID[i], CL_PLATFORM_NAME, sizeof(char)*128, platform_info, NULL)==CL_SUCCESS)
      sprintf(tmpStr, "#%d CL_PLATFORM_NAME: \t\t\t%s", i, platform_info); std::cout << tmpStr <<"\n";

    if(clGetPlatformInfo(clSelectedPlatformID[i], CL_PLATFORM_PROFILE, sizeof(char)*128, platform_info, NULL)==CL_SUCCESS)
      sprintf(tmpStr, "#%d CL_PLATFORM_PROFILE: \t\t%s", i, platform_info); std::cout << tmpStr <<"\n";       

    if(clGetPlatformInfo(clSelectedPlatformID[i], CL_PLATFORM_VERSION, sizeof(char)*128, platform_info, NULL)==CL_SUCCESS)
      sprintf(tmpStr, "#%d CL_PLATFORM_VERSION: \t\t%s", i, platform_info); std::cout << tmpStr <<"\n";       

    if(clGetPlatformInfo(clSelectedPlatformID[i], CL_PLATFORM_VENDOR, sizeof(char)*128, platform_info, NULL)==CL_SUCCESS)
      sprintf(tmpStr, "#%d CL_PLATFORM_VENDOR: \t\t\t%s", i, platform_info); std::cout << tmpStr <<"\n";    

    //the last one is probably quite long so this is just to giev an idea of how to do it
    if(clGetPlatformInfo(clSelectedPlatformID[i], CL_PLATFORM_EXTENSIONS, sizeof(char)*128, platform_info, NULL)==CL_SUCCESS)
      sprintf(tmpStr, "#%d CL_PLATFORM_EXTENSIONS: \t\t%s", i, platform_info); std::cout << tmpStr <<"\n";

    sprintf(tmpStr, "------------------------------------"); std::cout << tmpStr <<"\n\n";
  }

  for (unsigned int j = 0; j < numPlatforms; j++)
  {
    //Device info
    cl_uint ciDeviceCount;
    cl_device_id* clDevices =  NULL;
    status = clGetDeviceIDs(clSelectedPlatformID[j], CL_DEVICE_TYPE_ALL, 0, NULL, &ciDeviceCount);

    if(status != CL_SUCCESS)
      return;

    clDevices = (cl_device_id*) malloc(sizeof(cl_device_id) * ciDeviceCount);
    status = clGetDeviceIDs(clSelectedPlatformID[j], CL_DEVICE_TYPE_ALL, ciDeviceCount, clDevices, &ciDeviceCount);

    if(status != CL_SUCCESS)
      return;

    //debug info
    sprintf(tmpStr, "Platform %d - Available Devices: \t%d",j, ciDeviceCount); std::cout << tmpStr <<"\n\n";
    char device_string[1024];

    for(unsigned int i = 0; i < ciDeviceCount; i++)
    {
      std::cout <<"PLATFORM "<<j <<" - DEVICE " <<i <<"\n";

      // CL_DEVICE_NAME
      clGetDeviceInfo(clDevices[i], CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
      sprintf(tmpStr, "  CL_DEVICE_NAME:\t\t\t%s", device_string); std::cout << tmpStr <<"\n";

      // CL_DEVICE_VENDOR
      clGetDeviceInfo(clDevices[i], CL_DEVICE_VENDOR, sizeof(device_string), &device_string, NULL);
      sprintf(tmpStr, "  CL_DEVICE_VENDOR: \t\t\t%s", device_string); std::cout << tmpStr <<"\n";

      // CL_DRIVER_VERSION
      clGetDeviceInfo(clDevices[i], CL_DRIVER_VERSION, sizeof(device_string), &device_string, NULL);
      sprintf(tmpStr, "  CL_DRIVER_VERSION: \t\t\t%s", device_string); std::cout << tmpStr <<"\n";

      // CL_DEVICE_INFO
      cl_device_type type = 0;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_TYPE, sizeof(type), &type, NULL);

      if (type == CL_DEVICE_TYPE_CPU)
      {
        sprintf(tmpStr, "  CL_DEVICE_TYPE:\t\t\t%s", "CL_DEVICE_TYPE_CPU"); 
        std::cout << tmpStr <<"\n";
      }
      else if (type == CL_DEVICE_TYPE_GPU)
      {
        sprintf(tmpStr, "  CL_DEVICE_TYPE:\t\t\t%s", "CL_DEVICE_TYPE_GPU"); 
        std::cout << tmpStr <<"\n";
      }
      else if (type == CL_DEVICE_TYPE_ACCELERATOR)
      {
        sprintf(tmpStr, "  CL_DEVICE_TYPE:\t\t\t%s", "CL_DEVICE_TYPE_ACCELERATOR"); 
        std::cout << tmpStr <<"\n";
      }
      else if (type == CL_DEVICE_TYPE_DEFAULT)
      {
        sprintf(tmpStr, "  CL_DEVICE_TYPE:\t\t\t%s", "CL_DEVICE_TYPE_DEFAULT"); 
        std::cout << tmpStr <<"\n";
      }

      // CL_DEVICE_MAX_COMPUTE_UNITS
      cl_uint compute_units;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
      sprintf(tmpStr, "  CL_DEVICE_MAX_COMPUTE_UNITS:\t\t%u", compute_units); std::cout << tmpStr <<"\n";

      // CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
      size_t workitem_dims;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(workitem_dims), &workitem_dims, NULL);
      sprintf(tmpStr, "  CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:\t%u", workitem_dims); std::cout << tmpStr <<"\n";

      // CL_DEVICE_MAX_WORK_ITEM_SIZES
      size_t workitem_size[3];
      clGetDeviceInfo(clDevices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
      sprintf(tmpStr, "  CL_DEVICE_MAX_WORK_ITEM_SIZES:\t%u / %u / %u ", workitem_size[0], workitem_size[1], workitem_size[2]); std::cout << tmpStr <<"\n";

      // CL_DEVICE_MAX_WORK_GROUP_SIZE
      size_t workgroup_size;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
      sprintf(tmpStr, "  CL_DEVICE_MAX_WORK_GROUP_SIZE:\t%u", workgroup_size); std::cout << tmpStr <<"\n";

      // CL_DEVICE_MAX_CLOCK_FREQUENCY
      cl_uint clock_frequency;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
      sprintf(tmpStr, "  CL_DEVICE_MAX_CLOCK_FREQUENCY:\t%u MHz", clock_frequency); std::cout << tmpStr <<"\n";

      // CL_DEVICE_ADDRESS_BITS
      cl_uint addr_bits;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_ADDRESS_BITS, sizeof(addr_bits), &addr_bits, NULL);
      sprintf(tmpStr, "  CL_DEVICE_ADDRESS_BITS:\t\t%u", addr_bits); std::cout << tmpStr <<"\n";

      // CL_DEVICE_MAX_MEM_ALLOC_SIZE
      cl_ulong max_mem_alloc_size;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
      sprintf(tmpStr, "  CL_DEVICE_MAX_MEM_ALLOC_SIZE:\t\t%u MByte", (unsigned int)(max_mem_alloc_size / (1024 * 1024))); std::cout << tmpStr <<"\n";

      // CL_DEVICE_GLOBAL_MEM_SIZE
      cl_ulong mem_size;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
      sprintf(tmpStr, "  CL_DEVICE_GLOBAL_MEM_SIZE:\t\t%u MByte", (unsigned int)(mem_size / (1024 * 1024))); std::cout << tmpStr <<"\n";

      // CL_DEVICE_ERROR_CORRECTION_SUPPORT
      cl_bool error_correction_support;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(error_correction_support), &error_correction_support, NULL);
      sprintf(tmpStr, "  CL_DEVICE_ERROR_CORRECTION_SUPPORT:\t%s", error_correction_support == CL_TRUE ? "yes" : "no"); std::cout << tmpStr <<"\n";

      // CL_DEVICE_LOCAL_MEM_TYPE
      cl_device_local_mem_type local_mem_type;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_LOCAL_MEM_TYPE, sizeof(local_mem_type), &local_mem_type, NULL);
      sprintf(tmpStr, "  CL_DEVICE_LOCAL_MEM_TYPE:\t\t%s", local_mem_type == 1 ? "local" : "global"); std::cout << tmpStr <<"\n";

      // CL_DEVICE_LOCAL_MEM_SIZE
      clGetDeviceInfo(clDevices[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
      sprintf(tmpStr, "  CL_DEVICE_LOCAL_MEM_SIZE:\t\t%u KByte", (unsigned int)(mem_size / 1024)); std::cout << tmpStr <<"\n";

      // CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
      clGetDeviceInfo(clDevices[i], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(mem_size), &mem_size, NULL);
      sprintf(tmpStr, "  CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:\t%u KByte", (unsigned int)(mem_size / 1024)); std::cout << tmpStr <<"\n";

      // CL_DEVICE_QUEUE_PROPERTIES
      cl_command_queue_properties queue_properties;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_QUEUE_PROPERTIES, sizeof(queue_properties), &queue_properties, NULL);
      if( queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE )
        sprintf(tmpStr, "  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s", "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE"); std::cout << tmpStr <<"\n";  
      if( queue_properties & CL_QUEUE_PROFILING_ENABLE )
        sprintf(tmpStr, "  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s", "CL_QUEUE_PROFILING_ENABLE"); std::cout << tmpStr <<"\n";

      // CL_DEVICE_IMAGE_SUPPORT
      cl_bool image_support;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_IMAGE_SUPPORT, sizeof(image_support), &image_support, NULL);
      sprintf(tmpStr, "  CL_DEVICE_IMAGE_SUPPORT:\t\t%s", image_support ?  "Yes" : "No"); std::cout << tmpStr <<"\n";

      // CL_DEVICE_MAX_READ_IMAGE_ARGS
      cl_uint max_read_image_args;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(max_read_image_args), &max_read_image_args, NULL);
      sprintf(tmpStr, "  CL_DEVICE_MAX_READ_IMAGE_ARGS:\t%u", max_read_image_args); std::cout << tmpStr <<"\n";

      // CL_DEVICE_MAX_WRITE_IMAGE_ARGS
      cl_uint max_write_image_args;
      clGetDeviceInfo(clDevices[i], CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(max_write_image_args), &max_write_image_args, NULL);
      sprintf(tmpStr, "  CL_DEVICE_MAX_WRITE_IMAGE_ARGS:\t%u", max_write_image_args); std::cout << tmpStr <<"\n";

      // CL_DEVICE_IMAGE2D_MAX_WIDTH, CL_DEVICE_IMAGE2D_MAX_HEIGHT, CL_DEVICE_IMAGE3D_MAX_WIDTH, CL_DEVICE_IMAGE3D_MAX_HEIGHT, CL_DEVICE_IMAGE3D_MAX_DEPTH
      size_t szMaxDims[5];
      sprintf(tmpStr, "  CL_DEVICE_IMAGE <dim>:");  std::cout << tmpStr <<"\n";
      clGetDeviceInfo(clDevices[i], CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &szMaxDims[0], NULL);
      sprintf(tmpStr, "\t\t2D_MAX_WIDTH:\t\t%u", szMaxDims[0]); std::cout << tmpStr <<"\n";
      clGetDeviceInfo(clDevices[i], CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &szMaxDims[1], NULL);
      sprintf(tmpStr, "\t\t2D_MAX_HEIGHT:\t\t%u", szMaxDims[1]); std::cout << tmpStr <<"\n";
      clGetDeviceInfo(clDevices[i], CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &szMaxDims[2], NULL);
      sprintf(tmpStr, "\t\t3D_MAX_WIDTH:\t\t%u", szMaxDims[2]); std::cout << tmpStr <<"\n";
      clGetDeviceInfo(clDevices[i], CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &szMaxDims[3], NULL);
      sprintf(tmpStr, "\t\t3D_MAX_HEIGHT:\t\t%u", szMaxDims[3]); std::cout << tmpStr <<"\n";
      clGetDeviceInfo(clDevices[i], CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &szMaxDims[4], NULL);
      sprintf(tmpStr, "\t\t3D_MAX_DEPTH:\t\t%u", szMaxDims[4]); std::cout << tmpStr <<"\n\n";

      // CL_DEVICE_PREFERRED_VECTOR_WIDTH_<type>
      cl_uint vec_width [6];
      clGetDeviceInfo(clDevices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(cl_uint), &vec_width[0], NULL);
      clGetDeviceInfo(clDevices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(cl_uint), &vec_width[1], NULL);
      clGetDeviceInfo(clDevices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &vec_width[2], NULL);
      clGetDeviceInfo(clDevices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_uint), &vec_width[3], NULL);
      clGetDeviceInfo(clDevices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &vec_width[4], NULL);
      clGetDeviceInfo(clDevices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &vec_width[5], NULL);
      sprintf(tmpStr, "  CL_DEVICE_PREFERRED_VECTOR_WIDTH_<t>:\tCHAR %u, SHORT %u, INT %u, FLOAT %u, DOUBLE %u\n\n\n", vec_width[0], vec_width[1], vec_width[2], vec_width[3], vec_width[4]); std::cout << tmpStr <<"\n"; 

      // CL_DEVICE_EXTENSIONS: get device extensions, and if any then parse & log the string onto separate lines
      clGetDeviceInfo(clDevices[i], CL_DEVICE_EXTENSIONS, sizeof(device_string), &device_string, NULL);
      if (device_string != 0) 
      {
        sprintf(tmpStr, "  CL_DEVICE_EXTENSIONS:\t\t\t%s",device_string); std::cout << tmpStr <<"\n";
      }
      else 
      {
        sprintf(tmpStr, "  CL_DEVICE_EXTENSIONS:\t\t\tNone"); std::cout << tmpStr <<"\n";
      }

      

      sprintf(tmpStr, "------------------------------------\n"); std::cout << tmpStr <<"\n";
    }
  }

  sprintf(tmpStr, "------------------------------------\n"); std::cout << tmpStr <<"\n";
}



std::string GetOclErrorAsString( int _clErr )
{
  std::string returnString("UNKNOWN\n");

  switch(_clErr)
  {
  case CL_SUCCESS:
    returnString =  "CL_SUCCESS\n";
    break;
  case CL_DEVICE_NOT_FOUND:
    returnString =  "CL_DEVICE_NOT_FOUND\n";
    break;
  case CL_DEVICE_NOT_AVAILABLE:
    returnString =  "CL_DEVICE_NOT_AVAILABLE\n";
    break;
    /*case CL_DEVICE_COMPILER_NOT_AVAILABLE:
    returnString =  "CL_DEVICE_COMPILER_NOT_AVAILABLE\n";
    break;  */
  case CL_MEM_OBJECT_ALLOCATION_FAILURE :
    returnString =  "CL_MEM_OBJECT_ALLOCATION_FAILURE\n";
    break;
  case CL_OUT_OF_RESOURCES:
    returnString =  "CL_OUT_OF_RESOURCES\n";
    break;
  case CL_OUT_OF_HOST_MEMORY:
    returnString =  "CL_OUT_OF_HOST_MEMORY\n";
    break;
  case CL_PROFILING_INFO_NOT_AVAILABLE:
    returnString =  "CL_PROFILING_INFO_NOT_AVAILABLE\n";
    break;
  case CL_MEM_COPY_OVERLAP:
    returnString =  "CL_MEM_COPY_OVERLAP\n";
    break;
  case CL_IMAGE_FORMAT_MISMATCH:
    returnString =  "CL_IMAGE_FORMAT_MISMATCH\n";
    break;
  case CL_IMAGE_FORMAT_NOT_SUPPORTED:
    returnString =  "CL_IMAGE_FORMAT_NOT_SUPPORTED\n";
    break;
  case CL_BUILD_PROGRAM_FAILURE:
    returnString =  "CL_BUILD_PROGRAM_FAILURE\n";
    break;
  case CL_MAP_FAILURE:
    returnString =  "CL_MAP_FAILURE\n";
    break;
  case CL_INVALID_VALUE:
    returnString =  "CL_INVALID_VALUE\n";
    break;
  case CL_INVALID_DEVICE_TYPE:
    returnString =  "CL_INVALID_DEVICE_TYPE\n";
    break;
  case CL_INVALID_PLATFORM:
    returnString =  "CL_INVALID_PLATFORM\n";
    break;
  case CL_INVALID_DEVICE:
    returnString =  "CL_INVALID_DEVICE\n";
    break;
  case CL_INVALID_CONTEXT :
    returnString =  "CL_INVALID_CONTEXT\n";
    break;
  case CL_INVALID_QUEUE_PROPERTIES:
    returnString =  "CL_INVALID_QUEUE_PROPERTIES\n";
    break;
  case CL_INVALID_COMMAND_QUEUE:
    returnString =  "CL_INVALID_COMMAND_QUEUE\n";
    break;
  case CL_INVALID_HOST_PTR:
    returnString =  "CL_INVALID_HOST_PTR\n";
    break;
  case CL_INVALID_MEM_OBJECT:
    returnString =  "CL_INVALID_MEM_OBJECT\n";
    break;
  case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
    returnString =  "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n";
    break;
  case CL_INVALID_IMAGE_SIZE:
    returnString =  "CL_INVALID_IMAGE_SIZE\n";
    break;
  case CL_INVALID_SAMPLER :
    returnString =  "CL_INVALID_SAMPLER\n";
    break;
  case CL_INVALID_BINARY:
    returnString =  "CL_INVALID_BINARY\n";
    break;
  case CL_INVALID_BUILD_OPTIONS:
    returnString =  "CL_INVALID_BUILD_OPTIONS\n";
    break;
  case CL_INVALID_PROGRAM:
    returnString =  "CL_INVALID_PROGRAM\n";
    break;
  case CL_INVALID_PROGRAM_EXECUTABLE:
    returnString =  "CL_INVALID_PROGRAM_EXECUTABLE\n";
    break;
  case CL_INVALID_KERNEL_NAME:
    returnString =  "CL_INVALID_KERNEL_NAME\n";
    break;
  case CL_INVALID_KERNEL_DEFINITION:
    returnString =  "CL_INVALID_KERNEL_DEFINITION\n";
    break;
  case CL_INVALID_KERNEL :
    returnString =  "CL_INVALID_KERNEL\n";
    break;
  case CL_INVALID_ARG_INDEX :
    returnString =  "CL_INVALID_ARG_INDEX\n";
    break;
  case CL_INVALID_ARG_VALUE :
    returnString =  "CL_INVALID_ARG_VALUE\n";
    break;
  case CL_INVALID_ARG_SIZE :
    returnString =  "CL_INVALID_ARG_SIZE\n";
    break;
  case CL_INVALID_KERNEL_ARGS :
    returnString =  "CL_INVALID_KERNEL_ARGS\n";
    break;
  case CL_INVALID_WORK_DIMENSION:
    returnString =  "CL_INVALID_WORK_DIMENSION\n";
    break;
  case CL_INVALID_WORK_GROUP_SIZE:
    returnString =  "CL_INVALID_WORK_GROUP_SIZE\n";
    break;
  case CL_INVALID_WORK_ITEM_SIZE:
    returnString =  "CL_INVALID_WORK_ITEM_SIZE\n";
    break;
  case CL_INVALID_GLOBAL_OFFSET:
    returnString =  "CL_INVALID_GLOBAL_OFFSET\n";
    break;
  case CL_INVALID_EVENT_WAIT_LIST:
    returnString =  "CL_INVALID_EVENT_WAIT_LIST\n";
    break;
  case CL_INVALID_EVENT:
    returnString =  "CL_INVALID_EVENT\n";
    break;
  case CL_INVALID_OPERATION:
    returnString =  "CL_INVALID_OPERATION\n";
    break;
  case CL_INVALID_GL_OBJECT:
    returnString =  "CL_INVALID_GL_OBJECT\n";
    break;
  case CL_INVALID_BUFFER_SIZE :
    returnString =  "CL_INVALID_BUFFER_SIZE\n";
    break;
  case CL_INVALID_MIP_LEVEL :
    returnString =  "CL_INVALID_MIP_LEVEL\n";
    break;
  default:
  {
    // if we dont have a symbolic name then at least show the numeric error id.
    std::ostringstream    idstr;
    idstr << "UNKNOWN(0x" << std::hex << _clErr << ")";
    returnString = idstr.str();
    break;
  }
  }

  return returnString;
}

void GetOclError(int _clErr)
{
  if(_clErr == CL_SUCCESS )
    MITK_WARN << "Called GetOclErr() with no error value: [CL_SUCCESS]";
  else
    MITK_ERROR << GetOclErrorAsString(_clErr);
}

bool oclCheckError(int _err, const char* filepath, int lineno)
{
  if (_err)
  {
    MITK_ERROR<< "OpenCL Error at " << filepath <<":"<< lineno;
    GetOclError(_err);

    return 0;
  }

  return 1;
}

void GetSupportedImageFormats(cl_context _context, cl_mem_object_type _type)
{
  const unsigned int entries = 500;
  cl_image_format* formats = new cl_image_format[entries];

  cl_uint _written = 0;

  // OpenCL constant to catch error IDs
  cl_int ciErr1;
  // Get list of supported image formats for READ_ONLY access
  ciErr1 = clGetSupportedImageFormats( _context, CL_MEM_READ_ONLY, _type, entries, formats, &_written);
  CHECK_OCL_ERR(ciErr1);

  MITK_INFO << "Supported Image Formats, Image: CL_MEM_READ_ONLY \n";

  for (unsigned int i=0; i<_written; i++)
  {
    MITK_INFO<< "ChannelType: " << GetImageTypeAsString(formats[i].image_channel_data_type) << "| ChannelOrder: "<< GetImageTypeAsString(formats[i].image_channel_order) <<"\n";
  }

  _written = 0;

  // Get list of supported image formats for READ_WRITE access
  ciErr1 = clGetSupportedImageFormats( _context, CL_MEM_READ_WRITE, _type, entries, formats, &_written);
  CHECK_OCL_ERR(ciErr1);

  MITK_INFO << "Supported Image Formats, Image: CL_MEM_READ_WRITE (found: " << _written <<") \n";

  for (unsigned int i=0; i<_written; i++)
  {
    MITK_INFO<< "ChannelType: " << GetImageTypeAsString(formats[i].image_channel_data_type) << "| ChannelOrder: "<< GetImageTypeAsString(formats[i].image_channel_order) <<"\n";
  }

  _written = 0;

  // Get list of supported image formats for WRITE_ONLY access
  ciErr1 = clGetSupportedImageFormats( _context, CL_MEM_WRITE_ONLY, _type, entries, formats, &_written);
  CHECK_OCL_ERR(ciErr1);

  MITK_INFO << "Supported Image Formats, Image: CL_MEM_WRITE_ONLY (found: " << _written <<") \n";

  for (unsigned int i=0; i<_written; i++)
  {
    MITK_INFO<< "ChannelType: " << GetImageTypeAsString(formats[i].image_channel_data_type) << "| ChannelOrder: "<< GetImageTypeAsString(formats[i].image_channel_order) <<"\n";
  }
}

std::string GetImageTypeAsString( const unsigned int _in)
{
  switch(_in)
  {
  case CL_R:
    return "CL_R ";
    break;
  case CL_A:
    return "CL_A ";
    break;
  case CL_RG:
    return "CL_RG ";
    break;
  case CL_RA:
    return "CL_RA ";
    break;
  case CL_RGB:
    return "CL_RGB ";
    break;
  case CL_RGBA:
    return "CL_RGBA ";
    break;
  case CL_BGRA:
    return "CL_BGRA ";
    break;
  case CL_ARGB:
    return "CL_ARGB ";
    break;
  case CL_INTENSITY:
    return "CL_INTENSITY ";
    break;
  case CL_LUMINANCE:
    return "CL_LUMINANCE ";
    break;
  case CL_SNORM_INT8:
    return "CL_SNORM_INT8 ";
    break;
  case CL_SNORM_INT16:
    return "CL_SNORM_INT16 ";
    break;
  case CL_UNORM_INT8:
    return "CL_UNORM_INT8 ";
    break;
  case CL_UNORM_INT16:
    return "CL_UNORM_INT16 ";
    break;
  case CL_UNORM_SHORT_565:
    return "CL_UNORM_SHORT_565 ";
    break;
  case CL_UNORM_SHORT_555:
    return "CL_UNORM_SHORT_555 ";
    break;
  case CL_UNORM_INT_101010:
    return "CL_UNORM_INT_101010 ";
    break;
  case CL_SIGNED_INT8:
    return "CL_SIGNED_INT8 ";
    break;
  case CL_SIGNED_INT16:
    return "CL_SIGNED_INT16 ";
    break;
  case CL_SIGNED_INT32:
    return "CL_SIGNED_INT32 ";
    break;
  case CL_UNSIGNED_INT8:
    return "CL_UNSIGNED_INT8 ";
    break;
  case CL_UNSIGNED_INT16:
    return "CL_UNSIGNED_INT16 ";
    break;
  case CL_UNSIGNED_INT32:
    return "CL_UNSIGNED_INT32 ";
    break;
  case CL_HALF_FLOAT:
    return "CL_HALF_FLOAT ";
    break;
  case CL_FLOAT:
    return "CL_FLOAT ";
    break;
  default:
    return "--";
    break;
  }
}



void oclLogBinary(cl_program clProg, cl_device_id clDev)
{
  // Grab the number of devices associated with the program
  cl_uint num_devices;
  clGetProgramInfo(clProg, CL_PROGRAM_NUM_DEVICES, sizeof(cl_uint), &num_devices, NULL);

  // Grab the device ids
  cl_device_id* devices = (cl_device_id*) malloc(num_devices * sizeof(cl_device_id));
  clGetProgramInfo(clProg, CL_PROGRAM_DEVICES, num_devices * sizeof(cl_device_id), devices, 0);

  // Grab the sizes of the binaries
  size_t* binary_sizes = (size_t*)malloc(num_devices * sizeof(size_t));
  clGetProgramInfo(clProg, CL_PROGRAM_BINARY_SIZES, num_devices * sizeof(size_t), binary_sizes, NULL);

  // Now get the binaries
  char** ptx_code = (char**)malloc(num_devices * sizeof(char*));
  for( unsigned int i=0; i<num_devices; ++i)
  {
    ptx_code[i] = (char*)malloc(binary_sizes[i]);
  }
  clGetProgramInfo(clProg, CL_PROGRAM_BINARIES, 0, ptx_code, NULL);

  // Find the index of the device of interest
  unsigned int idx = 0;
  while((idx < num_devices) && (devices[idx] != clDev))
  {
    ++idx;
  }

  // If the index is associated, log the result
  if( idx < num_devices )
  {
    MITK_INFO<< "\n ---------------- \n Program Binary: \n -----------------------\n";
    MITK_INFO<< ptx_code[idx];
  }

  free( devices );
  free( binary_sizes );
  for(unsigned int i=0; i<num_devices; ++i)
  {
    free(ptx_code[i]);
  }
  free( ptx_code );
}

void oclLogBuildInfo(cl_program clProg, cl_device_id clDev)
{
  char cBuildLog[10240];

  clGetProgramBuildInfo(clProg, clDev, CL_PROGRAM_BUILD_LOG, sizeof(cBuildLog), cBuildLog, NULL);
  MITK_INFO<< "\n Program Build Log: \n -----------------------\n";
  MITK_INFO<< cBuildLog;
}

bool IsCLExtensionSupported(const char* extensionName, cl_device_id device)
{
  cl_int err;
  // Get string containing supported device extensions
  size_t ext_size = 1024;
  char* ext_string = (char*)malloc(ext_size);

  err = clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, ext_size, ext_string, &ext_size);
  if (err != CL_SUCCESS)
    return false;

  // Search for CL-GL sharing support in extension string (space delimited)
  return SearchInExtensionsList(extensionName, ext_string, ext_size);
}

bool SearchInExtensionsList(const char* support_str, const char* ext_string, size_t ext_buffer_size)
{
  size_t offset = 0;
  const char* space_substr = SearchForSubstring(ext_string + offset, " ", ext_buffer_size - offset);
  size_t space_pos = space_substr ? space_substr - ext_string : 0;
  while (space_pos < ext_buffer_size)
  {
    if( strncmp(support_str, ext_string + offset, space_pos) == 0 ) 
    {
      // Device supports requested extension!
      MITK_INFO <<"Found extension support " <<support_str <<"!";
      return true;
    }
    // Keep searching -- skip to next token string
    offset = space_pos + 1;
    space_substr = SearchForSubstring(ext_string + offset, " ", ext_buffer_size - offset);
    space_pos = space_substr ? space_substr - ext_string : 0;
  }
  
  MITK_INFO <<"Warning: Extension not supported " <<support_str <<"!";
  return false;
}

char * SearchForSubstring(const char *haystack, const char *needle, size_t len)
{
  int i;
  size_t needle_len;

  /* segfault here if needle is not NULL terminated */
  if (0 == (needle_len = strlen(needle)))
    return (char *)haystack;

  /* Limit the search if haystack is shorter than 'len' */
  len = strnlen(haystack, len);

  for (i=0; i<(int)(len-needle_len); i++)
  {
    if ((haystack[0] == needle[0]) &&
      (0 == strncmp(haystack, needle, needle_len)))
      return (char *)haystack;

    haystack++;
  }
  return NULL;
}

