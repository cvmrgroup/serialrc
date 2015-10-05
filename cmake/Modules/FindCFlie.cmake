# - Try to find CFlie
# Once done this will define
#  CFlie_FOUND - System has libcflie
#  CFlie_INCLUDE_DIRS - The CFlie include directories

find_path(CFlie_INCLUDE_DIR "CCrazyflie.h"
          HINTS /usr/local/include/cflie)

find_library(CFlie_LIBRARY NAMES libcflie.a
             HINTS /usr/local/lib)

set(CFlie_INCLUDE_DIRS ${CFlie_INCLUDE_DIR})
set(CFlie_LIBRARIES ${CFlie_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CFlie_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CFlie DEFAULT_MSG
                                  CFlie_INCLUDE_DIR CFlie_LIBRARY)

mark_as_advanced(CFlie_INCLUDE_DIR CFlie_LIBRARY)