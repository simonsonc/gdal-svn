# Find PoDoFo
# ~~~~~~~~
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Once run this will define:
#
# PoDoFo_FOUND        = system has PoDoFo lib
# PoDoFo_LIBRARY      = full path to the PoDoFo libraries
# PoDoFo_INCLUDE_DIRS = where to find headers
#

FIND_PATH(PoDoFo_INCLUDE_DIRS PATHS
    )

SET(LIB_SEARCH_PATH 
    )

FIND_LIBRARY(PoDoFo_LIBRARY NAMES PATHS LIB_SEARCH_PATH)

IF (PoDoFo_INCLUDE_DIRS AND PoDoFo_LIBRARY)
  SET(PoDoFo_FOUND TRUE)
ENDIF (PoDoFo_INCLUDE_DIRS AND PoDoFo_LIBRARY)

IF (PoDoFo_FOUND)
    MESSAGE(STATUS "Found PoDoFo: ${PoDoFo_LIBRARY}")
ELSE (PoDoFo_FOUND)
  IF (PoDoFo_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find PoDoFo")
  ENDIF (PoDoFo_FIND_REQUIRED)
ENDIF (PoDoFo_FOUND)