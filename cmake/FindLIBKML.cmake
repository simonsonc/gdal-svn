# Find Libkml
# ~~~~~~~~~
# Copyright (c) 2012, Dmitry Baryshnikov <polimax at mail.ru>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# CMake module to search for Libkml library
#
# If it's found it sets LIBKML_FOUND to TRUE
# and following variables are set:
#    LIBKML_INCLUDE_DIR
#    LIBKML_LIBRARY

# FIND_PATH and FIND_LIBRARY normally search standard locations
# before the specified paths. To search non-standard paths first,
# FIND_* is invoked first with specified paths and NO_DEFAULT_PATH
# and then again with no specified paths to search the default
# locations. When an earlier FIND_* succeeds, subsequent FIND_*s
# searching for the same item do nothing. 

# try to use framework on mac
# want clean framework path, not unix compatibility path

IF (NOT LIBKML_INCLUDE_DIR_T)
    find_path(LIBKML_INCLUDE_DIR_T dom.h
      "$ENV{LIB_DIR}/include/kml"
      "$ENV{LIB_DIR}/include"
      /usr/include/kml
      /usr/local/include/kml
      #mingw
      c:/msys/local/include/kml
      NO_DEFAULT_PATH
      )
    #FIND_PATH(LIBKML_INCLUDE_DIR_T dom.h)
    mark_as_advanced(LIBKML_INCLUDE_DIR_T)
ENDIF (NOT LIBKML_INCLUDE_DIR_T)

get_filename_component(LIBKML_INCLUDE_DIR ${LIBKML_INCLUDE_DIR_T} PATH)

IF (NOT LIBKML_LIBRARY)

find_library(LIBKML_RELEASE1_T
    NAMES
      kml.lib      
      libkml.lib 
      libkmlbase.so        
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_RELEASE1_T)
    set (LIBKML_RELEASE ${LIBKML_RELEASE} ${LIBKML_RELEASE1_T})
endif()  

find_library(LIBKML_RELEASE2_T
    NAMES
      libkmldom.so           
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_RELEASE2_T)
    set (LIBKML_RELEASE ${LIBKML_RELEASE} ${LIBKML_RELEASE2_T})
endif()    

find_library(LIBKML_RELEASE3_T
    NAMES
      libkmlconvenience.so           
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_RELEASE3_T)
    set (LIBKML_RELEASE ${LIBKML_RELEASE} ${LIBKML_RELEASE3_T})
endif()  

find_library(LIBKML_RELEASE4_T
    NAMES
      libkmlengine.so         
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_RELEASE4_T)
    set (LIBKML_RELEASE ${LIBKML_RELEASE} ${LIBKML_RELEASE4_T})
endif()

find_library(LIBKML_RELEASE5_T
    NAMES
      libkmlregionator.so         
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_RELEASE5_T)
    set (LIBKML_RELEASE ${LIBKML_RELEASE} ${LIBKML_RELEASE5_T})
endif()

find_library(LIBKML_RELEASE6_T
    NAMES
      libkmlxsd.so        
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_RELEASE6_T)
    set (LIBKML_RELEASE ${LIBKML_RELEASE} ${LIBKML_RELEASE6_T})
endif()

find_library(LIBKML_DEBUG1_T
    NAMES
      kmld.lib   
      libkmld.lib  
      libkmlbase.so 
      libkmldom.so           
    PATHS
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)  

if(LIBKML_DEBUG1_T)
    set (LIBKML_DEBUG ${LIBKML_DEBUG} ${LIBKML_DEBUG1_T})
endif()

find_library(LIBKML_DEBUG2_T
    NAMES
      libkmldom.so           
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_DEBUG2_T)
    set (LIBKML_DEBUG ${LIBKML_DEBUG} ${LIBKML_DEBUG2_T})
endif()

find_library(LIBKML_DEBUG3_T
    NAMES
      libkmlconvenience.so           
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_DEBUG3_T)
set (LIBKML_DEBUG ${LIBKML_DEBUG} ${LIBKML_DEBUG3_T})
endif()

find_library(LIBKML_DEBUG4_T
    NAMES
      libkmlengine.so         
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_DEBUG4_T)
set (LIBKML_DEBUG ${LIBKML_DEBUG} ${LIBKML_DEBUG4_T})
endif()

find_library(LIBKML_DEBUG5_T
    NAMES
      libkmlregionator.so         
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_DEBUG5_T)
set (LIBKML_DEBUG ${LIBKML_DEBUG} ${LIBKML_DEBUG5_T})
endif()

find_library(LIBKML_DEBUG6_T
    NAMES
      libkmlxsd.so        
    PATHS 
      "$ENV{LIB_DIR}/lib"
      /usr/lib
      /usr/local/lib
      #mingw
      c:/msys/local/lib
)	

if(LIBKML_DEBUG6_T)
set (LIBKML_DEBUG ${LIBKML_DEBUG} ${LIBKML_DEBUG6_T})
endif()

if(NOT LIBKML_RELEASE AND LIBKML_DEBUG)
    set(LIBKML_RELEASE ${LIBKML_DEBUG})
endif(NOT LIBKML_RELEASE AND LIBKML_DEBUG)

LIST(APPEND LIBKML_LIBRARY
    debug ${LIBKML_DEBUG} optimized ${LIBKML_RELEASE}
)

ENDIF (NOT LIBKML_LIBRARY)
  
IF (LIBKML_INCLUDE_DIR AND LIBKML_LIBRARY)
   SET(LIBKML_FOUND TRUE)
ENDIF (LIBKML_INCLUDE_DIR AND LIBKML_LIBRARY)

IF (LIBKML_FOUND)

   IF (NOT LIBKML_FIND_QUIETLY)
      MESSAGE(STATUS "Found LIBKML: ${LIBKML_LIBRARY}")
      MESSAGE(STATUS "Found LIBKML Headers: ${LIBKML_INCLUDE_DIR}")
   ENDIF (NOT LIBKML_FIND_QUIETLY)

ELSE (LIBKML_FOUND)

   IF (LIBKML_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find LIBKML")
   ELSE (LIBKML_FIND_REQUIRED)
      MESSAGE(STATUS "Could not find LIBKML")
   ENDIF (LIBKML_FIND_REQUIRED)

ENDIF (LIBKML_FOUND)
