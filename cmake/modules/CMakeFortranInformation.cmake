
# This file sets the basic flags for the Fortran language in CMake.
# It also loads the available platform file for the system-compiler
# if it exists.

GET_FILENAME_COMPONENT(CMAKE_BASE_NAME ${CMAKE_Fortran_COMPILER} NAME_WE)
# since the gnu compiler has several names force g++
IF(CMAKE_COMPILER_IS_GNUG77)
  SET(CMAKE_BASE_NAME g77)
ENDIF(CMAKE_COMPILER_IS_GNUG77)
IF(CMAKE_Fortran_COMPILER_ID)
  IF(EXISTS ${CMAKE_MODULE_PATH}/Platform/${CMAKE_SYSTEM_NAME}-${CMAKE_Fortran_COMPILER_ID}-Fortran.cmake OR EXISTS ${CMAKE_ROOT}/Modules/Platform/${CMAKE_SYSTEM_NAME}-${CMAKE_Fortran_COMPILER_ID}-Fortran.cmake)
    SET(CMAKE_BASE_NAME ${CMAKE_Fortran_COMPILER_ID}-Fortran)
  ENDIF(EXISTS ${CMAKE_MODULE_PATH}/Platform/${CMAKE_SYSTEM_NAME}-${CMAKE_Fortran_COMPILER_ID}-Fortran.cmake OR EXISTS ${CMAKE_ROOT}/Modules/Platform/${CMAKE_SYSTEM_NAME}-${CMAKE_Fortran_COMPILER_ID}-Fortran.cmake)
ENDIF(CMAKE_Fortran_COMPILER_ID)
IF(EXISTS ${CMAKE_MODULE_PATH}/Platform/${CMAKE_SYSTEM_NAME}-${CMAKE_BASE_NAME}.cmake)
  # Use this file if it exists.
  SET(CMAKE_SYSTEM_AND_Fortran_COMPILER_INFO_FILE
  ${CMAKE_MODULE_PATH}/Platform/${CMAKE_SYSTEM_NAME}-${CMAKE_BASE_NAME}.cmake)
ELSE(EXISTS ${CMAKE_MODULE_PATH}/Platform/${CMAKE_SYSTEM_NAME}-${CMAKE_BASE_NAME}.cmake)
  # This one apparently doesn't have to actually exist, see OPTIONAL below.
  SET(CMAKE_SYSTEM_AND_Fortran_COMPILER_INFO_FILE
  ${CMAKE_ROOT}/Modules/Platform/${CMAKE_SYSTEM_NAME}-${CMAKE_BASE_NAME}.cmake)
ENDIF(EXISTS ${CMAKE_MODULE_PATH}/Platform/${CMAKE_SYSTEM_NAME}-${CMAKE_BASE_NAME}.cmake)
INCLUDE(Platform/${CMAKE_SYSTEM_NAME}-${CMAKE_BASE_NAME} OPTIONAL)

# This should be included before the _INIT variables are
# used to initialize the cache.  Since the rule variables 
# have if blocks on them, users can still define them here.
# But, it should still be after the platform file so changes can
# be made to those values.

IF(CMAKE_USER_MAKE_RULES_OVERRIDE)
   INCLUDE(${CMAKE_USER_MAKE_RULES_OVERRIDE})
ENDIF(CMAKE_USER_MAKE_RULES_OVERRIDE)

IF(CMAKE_USER_MAKE_RULES_OVERRIDE_Fortran)
   INCLUDE(${CMAKE_USER_MAKE_RULES_OVERRIDE_Fortran})
ENDIF(CMAKE_USER_MAKE_RULES_OVERRIDE_Fortran)


# Fortran needs cmake to do a requires step during its build process to 
# catch any modules
SET(CMAKE_NEEDS_REQUIRES_STEP_Fortran_FLAG 1)

# Create a set of shared library variable specific to Fortran
# For 90% of the systems, these are the same flags as the C versions
# so if these are not set just copy the flags from the c version
IF(NOT CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS)
  SET(CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS})
ENDIF(NOT CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS)

IF(NOT CMAKE_SHARED_LIBRARY_Fortran_FLAGS)
  SET(CMAKE_SHARED_LIBRARY_Fortran_FLAGS ${CMAKE_SHARED_LIBRARY_C_FLAGS})
ENDIF(NOT CMAKE_SHARED_LIBRARY_Fortran_FLAGS)

IF(NOT DEFINED CMAKE_SHARED_LIBRARY_LINK_Fortran_FLAGS)
  SET(CMAKE_SHARED_LIBRARY_LINK_Fortran_FLAGS ${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS})
ENDIF(NOT DEFINED CMAKE_SHARED_LIBRARY_LINK_Fortran_FLAGS)

IF(NOT CMAKE_SHARED_LIBRARY_RUNTIME_Fortran_FLAG)
  SET(CMAKE_SHARED_LIBRARY_RUNTIME_Fortran_FLAG ${CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG}) 
ENDIF(NOT CMAKE_SHARED_LIBRARY_RUNTIME_Fortran_FLAG)

IF(NOT CMAKE_SHARED_LIBRARY_RUNTIME_Fortran_FLAG_SEP)
  SET(CMAKE_SHARED_LIBRARY_RUNTIME_Fortran_FLAG_SEP ${CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG_SEP})
ENDIF(NOT CMAKE_SHARED_LIBRARY_RUNTIME_Fortran_FLAG_SEP)

IF(NOT CMAKE_SHARED_LIBRARY_RPATH_LINK_Fortran_FLAG)
  SET(CMAKE_SHARED_LIBRARY_RPATH_LINK_Fortran_FLAG ${CMAKE_SHARED_LIBRARY_RPATH_LINK_C_FLAG})
ENDIF(NOT CMAKE_SHARED_LIBRARY_RPATH_LINK_Fortran_FLAG)

# repeat for modules
IF(NOT CMAKE_SHARED_MODULE_CREATE_Fortran_FLAGS)
  SET(CMAKE_SHARED_MODULE_CREATE_Fortran_FLAGS ${CMAKE_SHARED_MODULE_CREATE_C_FLAGS})
ENDIF(NOT CMAKE_SHARED_MODULE_CREATE_Fortran_FLAGS)

IF(NOT CMAKE_SHARED_MODULE_Fortran_FLAGS)
  SET(CMAKE_SHARED_MODULE_Fortran_FLAGS ${CMAKE_SHARED_MODULE_C_FLAGS})
ENDIF(NOT CMAKE_SHARED_MODULE_Fortran_FLAGS)

IF(NOT CMAKE_SHARED_MODULE_RUNTIME_Fortran_FLAG)
  SET(CMAKE_SHARED_MODULE_RUNTIME_Fortran_FLAG ${CMAKE_SHARED_MODULE_RUNTIME_C_FLAG}) 
ENDIF(NOT CMAKE_SHARED_MODULE_RUNTIME_Fortran_FLAG)

IF(NOT CMAKE_SHARED_MODULE_RUNTIME_Fortran_FLAG_SEP)
  SET(CMAKE_SHARED_MODULE_RUNTIME_Fortran_FLAG_SEP ${CMAKE_SHARED_MODULE_RUNTIME_C_FLAG_SEP})
ENDIF(NOT CMAKE_SHARED_MODULE_RUNTIME_Fortran_FLAG_SEP)

IF(NOT CMAKE_EXECUTABLE_RUNTIME_Fortran_FLAG)
  SET(CMAKE_EXECUTABLE_RUNTIME_Fortran_FLAG ${CMAKE_SHARED_LIBRARY_RUNTIME_Fortran_FLAG})
ENDIF(NOT CMAKE_EXECUTABLE_RUNTIME_Fortran_FLAG)

IF(NOT CMAKE_EXECUTABLE_RUNTIME_Fortran_FLAG_SEP)
  SET(CMAKE_EXECUTABLE_RUNTIME_Fortran_FLAG_SEP ${CMAKE_SHARED_LIBRARY_RUNTIME_Fortran_FLAG_SEP})
ENDIF(NOT CMAKE_EXECUTABLE_RUNTIME_Fortran_FLAG_SEP)

IF(NOT CMAKE_EXECUTABLE_RPATH_LINK_Fortran_FLAG)
  SET(CMAKE_EXECUTABLE_RPATH_LINK_Fortran_FLAG ${CMAKE_SHARED_LIBRARY_RPATH_LINK_Fortran_FLAG})
ENDIF(NOT CMAKE_EXECUTABLE_RPATH_LINK_Fortran_FLAG)

IF(NOT DEFINED CMAKE_SHARED_LIBRARY_LINK_Fortran_WITH_RUNTIME_PATH)
  SET(CMAKE_SHARED_LIBRARY_LINK_Fortran_WITH_RUNTIME_PATH ${CMAKE_SHARED_LIBRARY_LINK_C_WITH_RUNTIME_PATH})
ENDIF(NOT DEFINED CMAKE_SHARED_LIBRARY_LINK_Fortran_WITH_RUNTIME_PATH)

IF(NOT CMAKE_INCLUDE_FLAG_Fortran)
  SET(CMAKE_INCLUDE_FLAG_Fortran ${CMAKE_INCLUDE_FLAG_C})
ENDIF(NOT CMAKE_INCLUDE_FLAG_Fortran)

IF(NOT CMAKE_INCLUDE_FLAG_SEP_Fortran)
  SET(CMAKE_INCLUDE_FLAG_SEP_Fortran ${CMAKE_INCLUDE_FLAG_SEP_C})
ENDIF(NOT CMAKE_INCLUDE_FLAG_SEP_Fortran)

SET(CMAKE_VERBOSE_MAKEFILE FALSE CACHE BOOL "If this value is on, makefiles will be generated without the .SILENT directive, and all commands will be echoed to the console during the make.  This is useful for debugging only. With Visual Studio IDE projects all commands are done without /nologo.")

SET(CMAKE_Fortran_FLAGS_INIT "$ENV{FFLAGS} ${CMAKE_Fortran_FLAGS_INIT}")
# avoid just having a space as the initial value for the cache 
IF(CMAKE_Fortran_FLAGS_INIT STREQUAL " ")
  SET(CMAKE_Fortran_FLAGS_INIT)
ENDIF(CMAKE_Fortran_FLAGS_INIT STREQUAL " ")
SET (CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS_INIT}" CACHE STRING
     "Flags for Fortran compiler.")

INCLUDE(CMakeCommonLanguageInclude)

# now define the following rule variables
# CMAKE_Fortran_CREATE_SHARED_LIBRARY
# CMAKE_Fortran_CREATE_SHARED_MODULE
# CMAKE_Fortran_CREATE_STATIC_LIBRARY
# CMAKE_Fortran_COMPILE_OBJECT
# CMAKE_Fortran_LINK_EXECUTABLE

# create a Fortran shared library
IF(NOT CMAKE_Fortran_CREATE_SHARED_LIBRARY)
  SET(CMAKE_Fortran_CREATE_SHARED_LIBRARY
      "<CMAKE_Fortran_COMPILER> <CMAKE_SHARED_LIBRARY_Fortran_FLAGS> <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <CMAKE_SHARED_LIBRARY_CREATE_Fortran_FLAGS> <CMAKE_SHARED_LIBRARY_SONAME_Fortran_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
ENDIF(NOT CMAKE_Fortran_CREATE_SHARED_LIBRARY)

# create a Fortran shared module just copy the shared library rule
IF(NOT CMAKE_Fortran_CREATE_SHARED_MODULE)
  SET(CMAKE_Fortran_CREATE_SHARED_MODULE ${CMAKE_Fortran_CREATE_SHARED_LIBRARY})
ENDIF(NOT CMAKE_Fortran_CREATE_SHARED_MODULE)

# create a Fortran static library
IF(NOT CMAKE_Fortran_CREATE_STATIC_LIBRARY)
  SET(CMAKE_Fortran_CREATE_STATIC_LIBRARY
      "<CMAKE_AR> cr <TARGET> <LINK_FLAGS> <OBJECTS> "
      "<CMAKE_RANLIB> <TARGET> ")
ENDIF(NOT CMAKE_Fortran_CREATE_STATIC_LIBRARY)

# compile a Fortran file into an object file
IF(NOT CMAKE_Fortran_COMPILE_OBJECT)
  IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)
    SET(CMAKE_Fortran_COMPILE_OBJECT
      "<CMAKE_Fortran_COMPILER> -o <OBJECT> <DEFINES> <FLAGS> -c <SOURCE>")
  ELSE(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)
    SET(CMAKE_Fortran_COMPILE_OBJECT
      "<CMAKE_Fortran_COMPILER> -o <OBJECT> <FLAGS> -c <SOURCE>")
  ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)
ENDIF(NOT CMAKE_Fortran_COMPILE_OBJECT)

# link a fortran program
IF(NOT CMAKE_Fortran_LINK_EXECUTABLE)
  SET(CMAKE_Fortran_LINK_EXECUTABLE
    "<CMAKE_Fortran_COMPILER> <CMAKE_Fortran_LINK_FLAGS> <LINK_FLAGS> <FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")
ENDIF(NOT CMAKE_Fortran_LINK_EXECUTABLE)

IF(CMAKE_Fortran_STANDARD_LIBRARIES_INIT)
  SET(CMAKE_Fortran_STANDARD_LIBRARIES "${CMAKE_Fortran_STANDARD_LIBRARIES_INIT}"
    CACHE STRING "Libraries linked by defalut with all Fortran applications.")
  MARK_AS_ADVANCED(CMAKE_Fortran_STANDARD_LIBRARIES)
ENDIF(CMAKE_Fortran_STANDARD_LIBRARIES_INIT)

IF(NOT CMAKE_NOT_USING_CONFIG_FLAGS)
  SET (CMAKE_Fortran_FLAGS_DEBUG "${CMAKE_Fortran_FLAGS_DEBUG_INIT}" CACHE STRING
     "Flags used by the compiler during debug builds.")
  SET (CMAKE_Fortran_FLAGS_MINSIZEREL "${CMAKE_Fortran_FLAGS_MINSIZEREL_INIT}" CACHE STRING
      "Flags used by the compiler during release minsize builds.")
  SET (CMAKE_Fortran_FLAGS_RELEASE "${CMAKE_Fortran_FLAGS_RELEASE_INIT}" CACHE STRING
     "Flags used by the compiler during release builds (/MD /Ob1 /Oi /Ot /Oy /Gs will produce slightly less optimized but smaller files).")
  SET (CMAKE_Fortran_FLAGS_RELWITHDEBINFO "${CMAKE_Fortran_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING
     "Flags used by the compiler during Release with Debug Info builds.")

ENDIF(NOT CMAKE_NOT_USING_CONFIG_FLAGS)

MARK_AS_ADVANCED(
CMAKE_Fortran_FLAGS
CMAKE_Fortran_FLAGS_DEBUG
CMAKE_Fortran_FLAGS_MINSIZEREL
CMAKE_Fortran_FLAGS_RELEASE
CMAKE_Fortran_FLAGS_RELWITHDEBINFO)

# set this variable so we can avoid loading this more than once.
SET(CMAKE_Fortran_INFORMATION_LOADED 1)
