# Find FreeXL
# ~~~~~~~~~
# Copyright (c) 2014, Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# CMake module to search for GEOS library
#
# If it's found it sets GEOS_FOUND to TRUE
# and following variables are set:
#    FREEXL_INCLUDE_DIR
#    FREEXL_LIBRARY
#
 
if (UNIX)
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(_FreeXL freexl)
  endif (PKG_CONFIG_FOUND)
endif (UNIX)

SET(_FREEXL_ROOT_HINTS
  $ENV{FREEXL}
  ${FREEXL_ROOT_DIR}
  )
SET(_FREEXL_ROOT_PATHS
  $ENV{FREEXL}/src
  /usr
  /usr/local
  )
SET(_FREEXL_ROOT_HINTS_AND_PATHS
  HINTS ${_FREEXL_ROOT_HINTS}
  PATHS ${_FREEXL_ROOT_PATHS}
  )

FIND_PATH(FREEXL_INCLUDE_DIR
  NAMES
    freexl.h
  HINTS
    ${_FREEXL_INCLUDEDIR}
  ${_FREEXL_ROOT_HINTS_AND_PATHS}
  PATH_SUFFIXES
    include
)

IF(WIN32 AND NOT CYGWIN)
  # MINGW should go here too
  IF(MSVC)
    # Implementation details:
    # We are using the libraries located in the VC subdir instead of the parent directory eventhough :
    FIND_LIBRARY(FREEXL_DEBUG
      NAMES
        freexld
      ${_FREEXL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        "lib"
        "VC"
        "lib/VC"
    )

    FIND_LIBRARY(FREEXL_RELEASE
      NAMES
        freexl
      ${_FREEXL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        "lib"
        "VC"
        "lib/VC"
    )

    if( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
        if(NOT FREEXL_DEBUG)
            set(FREEXL_DEBUG ${FREEXL_RELEASE})
        endif(NOT FREEXL_DEBUG)
      set( FREEXL_LIBRARIES
        optimized ${FREEXL_RELEASE} debug ${FREEXL_DEBUG}
        )
    else()
      set( FREEXL_LIBRARIES ${FREEXL_RELEASE})
    endif()
    MARK_AS_ADVANCED(FREEXL_DEBUG FREEXL_RELEASE)
  ELSEIF(MINGW)
    # same player, for MingW
    FIND_LIBRARY(FREEXL
      NAMES        
        freexl
      ${_FREEXL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        "lib"
        "lib/MinGW"
    )

    MARK_AS_ADVANCED(FREEXL)
    set( FREEXL_LIBRARIES ${FREEXL})
  ELSE(MSVC)
    # Not sure what to pick for -say- intel, let's use the toplevel ones and hope someone report issues:
    FIND_LIBRARY(FREEXL
      NAMES
        freexl
      HINTS
        ${_FREEXL_LIBDIR}
      ${_FREEXL_ROOT_HINTS_AND_PATHS}
      PATH_SUFFIXES
        lib
    ) 

    MARK_AS_ADVANCED(FREEXL)
    set( FREEXL_LIBRARIES ${FREEXL} )
  ENDIF(MSVC)
ELSE(WIN32 AND NOT CYGWIN)

  FIND_LIBRARY(FREEXL_LIBRARY
    NAMES
        freexl
    HINTS
      ${_FREEXL_LIBDIR}
    ${_FREEXL_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
      "lib"
      "local/lib"
  ) 

  MARK_AS_ADVANCED(FREEXL_LIBRARY)

  # compat defines
  SET(FREEXL_LIBRARIES ${FREEXL_LIBRARY})

ENDIF(WIN32 AND NOT CYGWIN)

include(FindPackageHandleStandardArgs)

  find_package_handle_standard_args(FREEXL "Could NOT find FREEXL, try to set the path to FREEXL root folder in the system variable FREEXL"
    FREEXL_LIBRARIES
    FREEXL_INCLUDE_DIR
  )
# endif (FREEXL_VERSION)

MARK_AS_ADVANCED(FREEXL_INCLUDE_DIR FREEXL_LIBRARIES)
