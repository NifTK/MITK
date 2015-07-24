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

#ifndef __mitkOclUtils_h
#define __mitkOclUtils_h

#include <math.h>
#include "mitkOpenCL.h"

#include <string>
#include <vector>
#include <MitkOpenCLExports.h>

#define CHECK_OCL_ERR(_er) oclCheckError(_er, __FILE__, __LINE__);

/**
 @brief Method to estimate an integer quotient C from given dividend and divisor higher or equal to
        the corresponding floating quotient

  If the divisor is a factor of the dividend, the dividend/divisor is an integer value and is returned. If not,
  the nearest higher integer is returned. So it holds for the return value C that C * divisor is equal or greater then
  the dividend. In OpenCL context useful for estimating the local/global working dimension of a NDRange so that all
  image data is covered by the parallelisation scheme.
 */
MITKOPENCL_EXPORT unsigned int iDivUp(unsigned int dividend, unsigned int divisor);

/**
 @brief Returns the name of an OpenCL Error as a string

 Most of the OpenCL Methods ( cl<NAME> ) return an integer error code. This method translates the
 error value given as parameter to the corresponding error name. For example the value -30 will be translated
 to CL_INVALID_VALUE
 */
MITKOPENCL_EXPORT std::string GetOclErrorAsString( int _clErr );

/**
 @brief Checks whether the given value corresponds to an OpenCL Error value and prints this message out as MITK_ERROR if yes
 */
MITKOPENCL_EXPORT void GetOclError(int _clErr);

/** @brief Returns all platform IDs of any OpenCL-capable GPUs, or throws an exception */
MITKOPENCL_EXPORT cl_int oclGetPlatformIDs(std::vector<cl_platform_id> &platforms);

/*! \brief Prints out the essential support information about current device */
MITKOPENCL_EXPORT void oclPrintDeviceInfo(cl_device_id);

/*! \brief Prints out all information that is available about OpenCL platforms and devices */
MITKOPENCL_EXPORT void oclPrintFullCLInfo();

/*! @brief Prints the available memory info about the given object to std::cout
  */
MITKOPENCL_EXPORT void oclPrintMemObjectInfo( cl_mem memobj);

/*! \brief Checks the given code for errors and produces a std::cout output if
  the _err does not equal CL_SUCCESS. The output includes also the filename and the line
  number of the method call.
  */
MITKOPENCL_EXPORT bool oclCheckError(int _err, const char*, int);

/*! \brief Logs the GPU Program binary code

@param clProg: the OpenCL Program to log
@param clDev: the OpenCL-capable device the program was tried to be compiled for
*/
MITKOPENCL_EXPORT void oclLogBinary(cl_program clProg, cl_device_id clDev);

/*! \brief Shows the OpenCL-Program build info, called if clBuildProgram != CL_SUCCES

@param clProg: the OpenCL Program to log
@param clDev: the OpenCL-capable device the program was tried to be compiled for
*/
MITKOPENCL_EXPORT void oclLogBuildInfo(cl_program clProg, cl_device_id clDev);

/** \brief Print out all supported image formats for given image type

  @param _type the image type ( CL_MEM_OBJECT_2D or CL_MEM_OBJECT_3D )
  @param _context the OpenCL context to be examined
  */
MITKOPENCL_EXPORT void GetSupportedImageFormats(cl_context _context, cl_mem_object_type _type);

/**
 @brief Translates the internal image type identifier to a human readable description string
*/
MITKOPENCL_EXPORT std::string GetImageTypeAsString( const unsigned int _in);

/// \brief Checks if an extension is supported by a device
bool IsCLExtensionSupported(const char* extensionName, cl_device_id device);

/// \brief Checks if the required extension is in the list of supported extensions (space delimited)
bool SearchInExtensionsList(const char* support_str, const char* ext_string, size_t ext_buffer_size);

/// \brief Search for a substring in a sequence of characters. E.g. finds needle in a haystack :)
char * SearchForSubstring(const char *haystack, const char *needle, size_t len);

/// \brief Round up to the nearest power of two value
inline unsigned int GetNextPowerOfTwo(unsigned int v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  ++v;
  return v;
}

/// \brief Round up to the nearest value that is multiple of base
inline size_t ToMultipleOf(size_t N, size_t base)
{
  return (size_t)(ceil((double)N / (double)base) * base);
}


#endif //mitkOclUtils_h
