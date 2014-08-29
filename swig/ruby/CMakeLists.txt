# ******************************************************************************
# * Project:  CMake4GDAL
# * Purpose:  CMake build scripts
# * Author: Carl Simonson <simonsonc@gmail.com>
# ******************************************************************************
# * Copyright (C) 2014 Carl Simonson
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

find_package(Ruby REQUIRED)

include_directories(${RUBY_INCLUDE_DIRS})
include_directories(${SWIG}/include)
include_directories(${SWIG}/include/${BINDING})
include_directories(${SWIG}/include/${BINDING}/docs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${SWIG})

include_directories(${GDAL_ROOT_SOURCE_DIR}/gcore)
include_directories(${GDAL_ROOT_SOURCE_DIR}/frmts)
include_directories(${GDAL_ROOT_SOURCE_DIR}/ogr)
include_directories(${GDAL_ROOT_SOURCE_DIR}/ogr/ogrsf_frmts)
include_directories(${GDAL_ROOT_SOURCE_DIR}/alg)

function(add_ruby_module name main_interface)
    set(SWIG_MODULE_${name}_ruby_EXTRA_DEPS ${ARGN})

    set_source_files_properties(${main_interface} PROPERTIES CPLUSPLUS ON)
    set_source_files_properties(${main_interface} PROPERTIES SWIG_FLAGS "-includeall")

    swig_add_module(${name}_ruby ${BINDING} ${main_interface})
    swig_link_libraries(${name}_ruby ${RUBY_LIBRARIES} ${GDAL_LIB_NAME})
    set(target ${SWIG_MODULE_${name}_ruby_REAL_NAME})

    # Ruby wants its targets to not have a "lib" prefix
    set_target_properties(${target} PROPERTIES PREFIX "" OUTPUT_NAME "${name}")

    # install the module to the sitearch dir
    install(TARGETS ${target} LIBRARY DESTINATION ${RUBY_SITEARCH_DIR})
endfunction()

add_ruby_module(gdal
    ../include/gdal.i
    ../include/MajorObject.i
    ../include/Driver.i
    ../include/Dataset.i
    ../include/Band.i
    ../include/ColorTable.i
    ../include/cpl.i
    ../include/${BINDING}/gdal_${BINDING}.i
    ../include/${BINDING}/typemaps_${BINDING}.i
)

add_ruby_module(gdalconst ../include/gdalconst.i)

add_ruby_module(ogr
    ../include/ogr.i
    ../include/${BINDING}/ogr_${BINDING}.i
    ../include/${BINDING}/typemaps_${BINDING}.i
)

add_ruby_module(osr
    ../include/osr.i
    ../include/${BINDING}/osr_${BINDING}.i
    ../include/${BINDING}/typemaps_${BINDING}.i
)
