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
set(LIB_NAME "frmts_hdf4")
project ("lib${LIB_NAME}")

find_package(HDF4 REQUIRED)
if(HDF4_FOUND)
	include_directories(${HDF4_INCLUDE_DIRS})
    include_directories(hdf-eos)
    add_subdirectory(hdf-eos)
    include_directories(${GDAL_ROOT_SOURCE_DIR}/frmts/pds)
    
	add_definitions(-DHAVE_HDF4)
	set(GDAL_HAVE_HDF4 ON CACHE INTERNAL "GDAL_HAVE_HDF4")
endif(HDF4_FOUND)

set(LIB_HHEADERS ${LIB_INST_HEADERS}
    hdf4compat.h
    hdf4dataset.h
)

set(LIB_CSOURCES
	hdf4dataset.cpp
	hdf4imagedataset.cpp
)

set(GDAL_TARGET_OBJECTS ${GDAL_TARGET_OBJECTS} $<TARGET_OBJECTS:${LIB_NAME}> PARENT_SCOPE)
add_library(${LIB_NAME} OBJECT ${LIB_HHEADERS} ${LIB_CSOURCES})

if(HDF4_FOUND)
    if(WIN32)
		set(GDAL_TARGET_LINK_LIB ${GDAL_TARGET_LINK_LIB} ${HDF4_LIBRARIES} PARENT_SCOPE)
	else(WIN32)	
		set(GDAL_TARGET_LINK_LIB ${GDAL_TARGET_LINK_LIB} ${HDF4_LIBRARIES} PARENT_SCOPE)
    endif(WIN32)
endif(HDF4_FOUND)
