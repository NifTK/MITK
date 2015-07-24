if(MITK_USE_OpenCL)

  # create the module
  MITK_CREATE_MODULE(
    DEPENDS MitkCore
    PACKAGE_DEPENDS
      PUBLIC OpenCL
      PRIVATE ITK|ITKThresholding Qt4|QtCore Qt5|Core
  )

  add_subdirectory(Testing)
endif(MITK_USE_OpenCL)
