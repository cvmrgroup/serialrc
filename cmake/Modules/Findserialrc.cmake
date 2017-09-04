# try to find serialrc, once done this will define
#  serialrc_FOUND       - System has serialrc
#  serialrc_INCLUDE_DIR - The serialrc include directory
#  serialrc_LIBRARY     - The serialrc library

# check the environment variable
set(_serialrc_ENV_ROOT_DIR "$ENV{serialrc_ROOT_DIR}")

if(NOT serialrc_ROOT_DIR AND _serialrc_ENV_ROOT_DIR)
    set(serialrc_ROOT_DIR "${_serialrc_ENV_ROOT_DIR}")
endif(NOT serialrc_ROOT_DIR AND _serialrc_ENV_ROOT_DIR)

find_path(serialrc_INCLUDE_DIR "arduino/arxx.h"
          HINTS "${serialrc_ROOT_DIR}/include" /usr/local/include/serialrc/)

find_library(serialrc_LIBRARY NAMES serialrc
             HINTS "${serialrc_ROOT_DIR}/lib" /usr/local/lib)

set(serialrc_INCLUDE_DIRS ${serialrc_INCLUDE_DIR})
set(serialrc_LIBRARIES ${serialrc_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(serialrc DEFAULT_MSG
                                  serialrc_INCLUDE_DIR serialrc_LIBRARY)

mark_as_advanced(serialrc_INCLUDE_DIR serialrc_LIBRARY)

message(STATUS "serialrc_FOUND: " ${serialrc_FOUND})
message(STATUS "serialrc_INCLUDE_DIR: " ${serialrc_INCLUDE_DIR})
message(STATUS "serialrc_LIBRARY: " ${serialrc_LIBRARY})