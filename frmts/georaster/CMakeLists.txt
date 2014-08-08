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
set(LIB_NAME "frmts_georaster")
project ("lib${LIB_NAME}")

find_package(Oracle REQUIRED)
if(ORACLE_FOUND)
    MESSAGE(STATUS "Found Oracle: ${ORACLE_LIBRARY}")
	include_directories(${ORACLE_INCLUDE_DIR})
    
    include_directories(${GDAL_ROOT_SOURCE_DIR}/alg)
    include_directories(${GDAL_ROOT_SOURCE_DIR}/frmts/zlib)
    include_directories(${GDAL_ROOT_SOURCE_DIR}/frmts/jpeg/libjpeg)
    
	add_definitions(-DHAVE_GEORASTER)
	set(GDAL_HAVE_GEORASTER ON CACHE INTERNAL "GDAL_HAVE_GEORASTER")
endif(ORACLE_FOUND)

set(LIB_HHEADERS ${LIB_INST_HEADERS}
    oci_wrapper.h
    georaster_priv.h
)

set(LIB_CSOURCES
	georaster_dataset.cpp
	georaster_rasterband.cpp
	georaster_wrapper.cpp
	oci_wrapper.cpp
)

set(GDAL_TARGET_OBJECTS ${GDAL_TARGET_OBJECTS} $<TARGET_OBJECTS:${LIB_NAME}> PARENT_SCOPE)
add_library(${LIB_NAME} OBJECT ${LIB_HHEADERS} ${LIB_CSOURCES})

if(ORACLE_FOUND)
    if(WIN32)
		set(GDAL_TARGET_LINK_LIB ${GDAL_TARGET_LINK_LIB} ${ORACLE_LIBRARY} PARENT_SCOPE)
	else(WIN32)	
		set(GDAL_TARGET_LINK_LIB ${GDAL_TARGET_LINK_LIB} ${ORACLE_LIBRARY} PARENT_SCOPE)
    endif(WIN32)
endif(ORACLE_FOUND)
