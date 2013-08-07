# message("In package CTK config")
find_package(CTK REQUIRED)
list(APPEND ALL_INCLUDE_DIRECTORIES ${CTK_INCLUDE_DIRS})
list(APPEND ALL_LIBRARIES ${CTK_LIBRARIES})
link_directories(${CTK_LIBRARY_DIRS})

# Temporary workaround for CTK bug of not exposing external project library dirs.
# Should be removed as soon as this is fixed in CTK. (espakm)
if(EXISTS "${CTK_DIR}/qRestAPI-build/qRestAPIConfig.cmake")
  include(${CTK_DIR}/qRestAPI-build/qRestAPIConfig.cmake)
  link_directories(${qRestAPI_LIBRARY_DIRS})
endif()
