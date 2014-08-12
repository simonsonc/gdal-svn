# Find MrSID
# ~~~~~~~~
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Once run this will define:
#
# MrSID_FOUND        = system has MrSID lib
# MrSID_LIBRARY      = full path to the MrSID libraries
# MrSID_INCLUDE_DIRS = where to find headers
#

FIND_PATH(MrSID_INCLUDE_DIRS PATHS
    )

SET(LIB_SEARCH_PATH 
    )

FIND_LIBRARY(MrSID_LIBRARY NAMES PATHS LIB_SEARCH_PATH)

IF (MrSID_INCLUDE_DIRS AND MrSID_LIBRARY)
  SET(MrSID_FOUND TRUE)
ENDIF (MrSID_INCLUDE_DIRS AND MrSID_LIBRARY)

IF (MrSID_FOUND)
    MESSAGE(STATUS "Found MrSID: ${MrSID_LIBRARY}")
ELSE (MrSID_FOUND)
  IF (MrSID_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find MrSID")
  ENDIF (MrSID_FIND_REQUIRED)
ENDIF (MrSID_FOUND)