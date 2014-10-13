# Find GRASS
# ~~~~~~~~
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Once run this will define:
#
# GRASS_BASE         = full path to the GRASS home
#
# GRASS_FOUND        = system has GRASS lib
# GRASS_LIBRARY      = full path to the GRASS libraries
# GRASS_INCLUDE_DIRS = where to find headers
#
FIND_PATH(GRASS_BASE PATHS
    )
    

SET(LIB_SEARCH_PATH 
    )
FIND_LIBRARY(GRASS_LIBRARY NAMES PATHS LIB_SEARCH_PATH)

SET(GRASS_INCLUDE_DIRS "${GRASS_BASE}/include")
SET(GRASS_GISBASE "${GRASS_BASE}/include")

IF (GRASS_INCLUDE_DIRS AND GRASS_LIBRARY)
  SET(GRASS_FOUND TRUE)
ENDIF (GRASS_INCLUDE_DIRS AND GRASS_LIBRARY)

IF (GRASS_FOUND)
    MESSAGE(STATUS "Found GRASS: ${GRASS_LIBRARY}")
ELSE (GRASS_FOUND)
  IF (GRASS_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find GRASS")
  ENDIF (GRASS_FIND_REQUIRED)
ENDIF (GRASS_FOUND)