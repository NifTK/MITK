if(MITK_USE_OpenCL)
  find_package(OpenCL REQUIRED)
  include_directories(${OPENCL_INCLUDE_DIRS})
  set(dependencies  ITK|ITKThresholding OpenCL Qt4|QtCore)

  # on OS X the linker needs a flag pointing to the OpenCL.framework
  if(APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework OpenCL")
  endif(APPLE)

  # create the module
  MITK_CREATE_MODULE(
    INCLUDE_DIRS ${OPENCL_INCLUDE_DIRS}
    DEPENDS MitkCore
    PACKAGE_DEPENDS ${dependencies}
    ADDITIONAL_LIBS ${OPENCL_LIBRARIES}
    EXPORT_DEFINE MitkOpenCL_EXPORT
  )
  if(BUILD_TESTING)
    add_subdirectory(Testing)
  endif(BUILD_TESTING)
  
endif(MITK_USE_OpenCL)
