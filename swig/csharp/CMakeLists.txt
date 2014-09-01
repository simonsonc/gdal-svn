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

find_package(CSharp REQUIRED)

include_directories(${SWIG}/include)
include_directories(${SWIG}/include/${BINDING})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${SWIG})

include_directories(${GDAL_ROOT_SOURCE_DIR}/gcore)
include_directories(${GDAL_ROOT_SOURCE_DIR}/frmts)
include_directories(${GDAL_ROOT_SOURCE_DIR}/ogr)
include_directories(${GDAL_ROOT_SOURCE_DIR}/ogr/ogrsf_frmts)
include_directories(${GDAL_ROOT_SOURCE_DIR}/alg)

function(add_csharp_module name namespace moddep main_interface)
    set(SWIG_MODULE_${name}csharp_EXTRA_DEPS ${ARGN})

    set_source_files_properties(${main_interface} PROPERTIES CPLUSPLUS ON)
    set(args -namespace "${namespace}" -dllimport "${name}_wrap")
    set_source_files_properties(${main_interface} PROPERTIES SWIG_FLAGS "${args}")

    set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/${name})

    swig_add_module(${name}csharp ${BINDING} ${main_interface})
    swig_link_libraries(${name}csharp ${PYTHON_LIBRARIES} ${GDAL_LIB_NAME})

    if(moddep)
        set(moddep_arg "/r:${moddep}.dll")
    else()
        set(moddep_arg "")
    endif()

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}_csharp.dll
        COMMAND ${CSHARP_COMPILER} ARGS "/debug:full" "/target:library"
            "/out:${CMAKE_CURRENT_BINARY_DIR}/${name}_csharp.dll"
            "${name}/*.cs"
            "/keyfile:${CMAKE_CURRENT_SOURCE_DIR}/gdal.snk"
            ${moddep_arg}
            "${CMAKE_CURRENT_SOURCE_DIR}/AssemblyInfo.cs"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS "${swig_generated_file_fullname}"
            ${CMAKE_CURRENT_SOURCE_DIR}/AssemblyInfo.cs
            ${CMAKE_CURRENT_SOURCE_DIR}/gdal.snk
            ${moddep}
        COMMENT "csc *.cs")

    add_custom_target(${name}_csharp ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${name}_csharp.dll)

    unset(CMAKE_SWIG_OUTDIR)
endfunction()

add_csharp_module(gdal OSGeo.GDAL ogr_csharp
    ../include/gdal.i

    ../include/Band.i
    ../include/ColorTable.i
    ../include/Dataset.i
    ../include/Driver.i
    ../include/MajorObject.i
    ../include/Operations.i
    ../include/RasterAttributeTable.i
    ../include/XMLNode.i
    ../include/cpl.i
    ../include/cpl_exceptions.i
    ../include/cplvirtualmem.i
    ../include/${BINDING}/gdal_${BINDING}.i
    ../include/${BINDING}/gdal_${BINDING}_extend.i
    ../include/${BINDING}/swig_csharp_extensions.i
    ../include/${BINDING}/typemaps_${BINDING}.i
)

add_csharp_module(gdalconst OSGeo.GDAL "" ../include/gdalconst.i)

add_csharp_module(ogr OSGeo.OGR osr_csharp
    ../include/ogr.i

    ../include/MajorObject.i
    ../include/cpl_exceptions.i
    ../include/osr.i
    ../include/${BINDING}/ogr_${BINDING}.i
    ../include/${BINDING}/ogr_${BINDING}_extend.i
    ../include/${BINDING}/osr_${BINDING}.i
    ../include/${BINDING}/swig_csharp_extensions.i
    ../include/${BINDING}/typemaps_${BINDING}.i
)

add_csharp_module(osr OSGeo.OSR ""
    ../include/osr.i

    ../include/cpl_exceptions.i
    ../include/${BINDING}/osr_${BINDING}.i
    ../include/${BINDING}/swig_csharp_extensions.i
    ../include/${BINDING}/typemaps_${BINDING}.i
)
