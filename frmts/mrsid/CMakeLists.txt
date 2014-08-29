# ******************************************************************************
# * Project:  CMake4GDAL
# * Purpose:  CMake build scripts
# * Author:   Alexander Lisovenko, alexander.lisovenko@gmail.com
# ******************************************************************************
# * Copyright (C) 2014 Alexander Lisovenko
# * Copyright (C) 2014 NextGIS
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
set(LIB_NAME "frmts_mrsid")
project ("lib${LIB_NAME}")

find_package(MrSID REQUIRED)
if(MrSID_FOUND)
	include_directories(${MrSID_INCLUDE_DIRS})
    include_directories(${GDAL_ROOT_SOURCE_DIR}/frmts/gtiff/libgeotiff)
    #add_definitions(-DMRSID_ESDK)
    add_definitions(-DMRSID_J2K)

	set(GDAL_HAVE_MrSID ON CACHE INTERNAL "GDAL_HAVE_MrSID")
endif(MrSID_FOUND)

set(LIB_HHEADERS ${LIB_INST_HEADERS}
    mrsidstream.h
)

set(LIB_CSOURCES
	mrsiddataset.cpp
    mrsidstream.cpp
)

set(GDAL_TARGET_OBJECTS ${GDAL_TARGET_OBJECTS} $<TARGET_OBJECTS:${LIB_NAME}> PARENT_SCOPE)
add_library(${LIB_NAME} OBJECT ${LIB_HHEADERS} ${LIB_CSOURCES})

if(MrSID_FOUND)
    if(WIN32)
		set(GDAL_TARGET_LINK_LIB ${GDAL_TARGET_LINK_LIB} ${MrSID_LIBRARY} PARENT_SCOPE)
	else(WIN32)	
		set(GDAL_TARGET_LINK_LIB ${GDAL_TARGET_LINK_LIB} ${MrSID_LIBRARY} PARENT_SCOPE)
    endif(WIN32)
endif(MrSID_FOUND)
