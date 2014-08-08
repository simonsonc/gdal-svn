# ******************************************************************************
# * Project:  CMake4GDAL
# * Purpose:  CMake build scripts
# * Author: 
# ******************************************************************************
# * Copyright (C) 2014
# * 
# * Permission is hereby granted, free of charge, to any person obtaining a
# * copy of this software and associated documentation files (the "Software"),
# * to deal in the Software without restriction, including without limitation
# * the rights to use, copy, modify, merge, publish, distribute, sublicense,
# * and/or sell copies of the Software, and to permit persons to whom the
# * Software is furnished to do so, subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included
# * in all copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# * DEALINGS IN THE SOFTWARE.
# ******************************************************************************

cmake_minimum_required (VERSION 2.8.10)
set(LIB_NAME "frmts_ecw")
project ("lib${LIB_NAME}")

#include_directories(${GDAL_ROOT_SOURCE_DIR}/frmts/vrt)  
find_package(ECW REQUIRED)
if(ECW_FOUND)
    MESSAGE(STATUS "Found ECW: ${ECW_LIBRARIES}")
	include_directories(${ECW_INCLUDE_DIR})
	add_definitions(-DHAVE_ECW)
	add_definitions(-DHAVE_ECW_BUILDNUMBER_H)
	set(GDAL_HAVE_ECW ON CACHE INTERNAL "GDAL_HAVE_ECW")
endif(ECW_FOUND)

set(LIB_HHEADERS ${LIB_INST_HEADERS}
    gdal_ecw.h
)

set(LIB_CSOURCES
	ecwasyncreader.cpp
	ecwcreatecopy.cpp
	ecwdataset.cpp
	jp2userbox.cpp
)

set(GDAL_TARGET_OBJECTS ${GDAL_TARGET_OBJECTS} $<TARGET_OBJECTS:${LIB_NAME}> PARENT_SCOPE)
add_library(${LIB_NAME} OBJECT ${LIB_HHEADERS} ${LIB_CSOURCES})

if(ECW_FOUND)
    if(WIN32)
		set(GDAL_TARGET_LINK_LIB ${GDAL_TARGET_LINK_LIB} ${ECW_LIBRARIES} PARENT_SCOPE)
	else(WIN32)	
		set(GDAL_TARGET_LINK_LIB ${GDAL_TARGET_LINK_LIB} ${ECW_LIBRARIES} PARENT_SCOPE)
    endif(WIN32)
endif(ECW_FOUND)

#install(FILES ${LIB_INST_HEADERS} DESTINATION include/gdal)
