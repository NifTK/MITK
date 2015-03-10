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

#ifndef MITKOPENCL_H_HEADER_INCLUDED
#define MITKOPENCL_H_HEADER_INCLUDED

#if defined (__APPLE__) || defined(MACOSX)
  #include <OpenGL/OpenGL.h>
  #include <OpenCL/OpenCL.h>
  #include <OpenCL/cl_ext.h>
  #include <OpenCL/cl_gl.h>
  #include <OpenCL/cl_gl_ext.h>

#else
  #include <CL/cl.h>
  #include <CL/cl_gl.h>
#endif

// These includes are required for the GL-CL context sharing
#ifdef _WIN32
  #include <windows.h>
#else
  #include <GL/glx.h>
  #include <X11/Xlib.h>
#endif

#include <GL/gl.h>


#endif /* MITKOPENCL_H_HEADER_INCLUDED */
