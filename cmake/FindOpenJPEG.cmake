###
# -*- cmake -*-
#
# File:  FindOpenJPEG.cmake
#
# Original script was copied from:
# http://code.google.com/p/emeraldviewer/source/browse/indra/cmake
#
# $Id$
###

# - Find OpenJPEG
# Find the OpenJPEG includes and library
# This module defines
#  OPENJPEG_INCLUDE_DIR, where to find openjpeg.h, etc.
#  OPENJPEG_LIBRARIES, the libraries needed to use OpenJPEG.
#  OPENJPEG_FOUND, If false, do not try to use OpenJPEG.
# also defined, but not for general use are
#  OPENJPEG_LIBRARY, where to find the OpenJPEG library.

FIND_PATH(OPENJPEG_INCLUDE_DIR openjpeg.h
  PATHS
    /usr/local/include/openjpeg
    /usr/local/include
    /usr/include/openjpeg
    /usr/include
  PATH_SUFFIXES
    openjpeg-2.0
  DOC "Location of OpenJPEG Headers"
)

SET(OPENJPEG_NAMES ${OPENJPEG_NAMES} openjpeg)
FIND_LIBRARY(OPENJPEG_LIBRARY
  NAMES ${OPENJPEG_NAMES}
  PATHS /usr/lib /usr/local/lib
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenJPEG DEFAULT_MSG OPENJPEG_LIBRARY OPENJPEG_INCLUDE_DIR)

IF (OPENJPEG_FOUND)
   SET(OPENJPEG_LIBRARIES ${OPENJPEG_LIBRARY})
ENDIF (OPENJPEG_FOUND)

MARK_AS_ADVANCED(OPENJPEG_LIBRARY OPENJPEG_INCLUDE_DIR)
