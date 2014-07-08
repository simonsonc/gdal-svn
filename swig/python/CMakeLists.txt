# ******************************************************************************
# * Project:  CMake4GDAL
# * Purpose:  CMake build scripts
# * Author: Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
# ******************************************************************************
# * Copyright (C) 2014 Dmitry Baryshnikov
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

find_package(PythonLibs REQUIRED)
if(PYTHONLIBS_FOUND)
    set(PACKAGE_DIR "osgeo")    
    set(SWIGOUTPUTDIR "extensions/")
    
    include_directories(${PYTHON_INCLUDE_DIRS})  
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
    
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${GDAL_ROOT_BINARY_DIR}/swig/include)   
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/${SWIGOUTPUTDIR} )
    configure_file(${CMAKE_MODULE_PATH}/python_setup.cfg.cmake ${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/setup.cfg IMMEDIATE @ONLY)
 
    
    add_definitions(-DSWIGPYTHON)
    
    set(GDAL_WRAP_INTERFACE_FILES
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
    
    set_source_files_properties(${GDAL_WRAP_INTERFACE_FILES}  PROPERTIES CPLUSPLUS ON)
    set_source_files_properties(${GDAL_WRAP_INTERFACE_FILES} PROPERTIES SWIG_FLAGS "-includeall")
    swig_add_module(gdal_wrap ${BINDING}
        ../include/gdal.i 
    )    
    swig_link_libraries(gdal_wrap ${PYTHON_LIBRARIES})
    
    set_source_files_properties(../include/gdalconst.i PROPERTIES CPLUSPLUS OFF)
    swig_add_module(gdalconst_wrap ${BINDING} 
        ../include/gdalconst.i 
    )
    swig_link_libraries(gdalconst_wrap ${PYTHON_LIBRARIES})
    
    
    set(GDAL_WRAP_INTERFACE_FILES
    ../include/ogr.i 
    ../include/${BINDING}/ogr_${BINDING}.i 
    ../include/${BINDING}/typemaps_${BINDING}.i    
    )
    set_source_files_properties(${GDAL_WRAP_INTERFACE_FILES}  PROPERTIES CPLUSPLUS ON)    
    set_source_files_properties(${GDAL_WRAP_INTERFACE_FILES} PROPERTIES SWIG_FLAGS "-includeall")
    swig_add_module(ogr_wrap ${BINDING}
        ../include/ogr.i 
    )
    swig_link_libraries(ogr_wrap ${PYTHON_LIBRARIES})


    set(GDAL_WRAP_INTERFACE_FILES
    ../include/osr.i 
    ../include/${BINDING}/osr_${BINDING}.i 
    ../include/${BINDING}/typemaps_${BINDING}.i   
    )  
    set_source_files_properties(${GDAL_WRAP_INTERFACE_FILES}  PROPERTIES CPLUSPLUS ON) 
    set_source_files_properties(${GDAL_WRAP_INTERFACE_FILES} PROPERTIES SWIG_FLAGS "-includeall")
    swig_add_module(osr_wrap ${BINDING}
        ../include/osr.i 
    )
    swig_link_libraries(osr_wrap ${PYTHON_LIBRARIES})   
    
    
    set(GDAL_WRAP_INTERFACE_FILES
    ../include/gdal_array.i
    ../include/${BINDING}/typemaps_${BINDING}.i  
    )    
    set_source_files_properties(${GDAL_WRAP_INTERFACE_FILES}  PROPERTIES CPLUSPLUS ON)    
    set_source_files_properties(${GDAL_WRAP_INTERFACE_FILES} PROPERTIES SWIG_FLAGS "-includeall")
    swig_add_module(gdal_array_wrap ${BINDING}
        ../include/gdal_array.i
    )
    swig_link_libraries(gdal_array_wrap ${PYTHON_LIBRARIES})   
    
    add_custom_target(swig_build ALL
        COMMAND cp -u "${GDAL_ROOT_BINARY_DIR}/swig/include/gdal_arrayPYTHON_wrap.cxx" "${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/${SWIGOUTPUTDIR}/gdal_array_wrap.cpp"
        COMMAND cp -u "${GDAL_ROOT_BINARY_DIR}/swig/include/gdalconstPYTHON_wrap.c" "${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/${SWIGOUTPUTDIR}/gdalconst_wrap.c"
        COMMAND cp -u "${GDAL_ROOT_BINARY_DIR}/swig/include/gdalPYTHON_wrap.cxx" "${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/${SWIGOUTPUTDIR}/gdal_wrap.cpp"
        COMMAND cp -u "${GDAL_ROOT_BINARY_DIR}/swig/include/ogrPYTHON_wrap.cxx" "${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/${SWIGOUTPUTDIR}/ogr_wrap.cpp"
        COMMAND cp -u "${GDAL_ROOT_BINARY_DIR}/swig/include/osrPYTHON_wrap.cxx" "${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/${SWIGOUTPUTDIR}/osr_wrap.cpp"
        COMMAND cp -u "${GDAL_ROOT_SOURCE_DIR}/swig/${BINDING}/README.txt" "${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/README.txt"
        COMMAND cp -u -r "${GDAL_ROOT_SOURCE_DIR}/swig/${BINDING}/osgeo" "${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/osgeo"
        COMMAND cp -u "${GDAL_ROOT_SOURCE_DIR}/swig/${BINDING}/gdalnumeric.py" "${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/gdalnumeric.py"
        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/setup.py" "build" 
        WORKING_DIRECTORY  ${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}/
        DEPENDS _gdal_wrap _gdalconst_wrap _ogr_wrap _osr_wrap _gdal_array_wrap       
    )
    
    
    install(CODE "execute_process(
        COMMAND python \"${CMAKE_CURRENT_SOURCE_DIR}/setup.py\" install
        WORKING_DIRECTORY  ${GDAL_ROOT_BINARY_DIR}/swig/${BINDING}
    )")

#    add_custom_target(swig_install
#        COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/setup.py" "install"
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
#    )
    
#    install(TARGETS swig_install)
    
#    set(PY_COMMANDS     
#        epsg_tr.py 
#        gdalchksum.py 
#        gdal2xyz.py 
#        gcps2wld.py
#        gdalimport.py 
#        gdal_merge.py 
#        pct2rgb.py 
#        rgb2pct.py
#        gcps2vec.py
#        )

#    set(PY_MODULES 
#        ${PACKAGE_DIR}/gdal.py 
#        ${PACKAGE_DIR}/ogr.py
#        ${PACKAGE_DIR}/osr.py 
#        ${PACKAGE_DIR}/gdalconst.py 
#        ${PACKAGE_DIR}/gdal_array.py
#        )
        
#    execute_process(COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
#    install(TARGETS _gdal_wrap _gdalconst_wrap _ogr_wrap _osr_wrap _gdal_array_wrap DESTINATION ${PYTHON_SITE_PACKAGES}/${PACKAGE_DIR})
#    install(DIRECTORY ${CMAKE_BINARY_DIR}/swig/python DESTINATION ${PYTHON_SITE_PACKAGES}/${PACKAGE_DIR} FILES_MATCHING PATTERN "*.py")
endif()
        
