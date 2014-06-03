# ******************************************************************************
# * Project:  CMake4GDAL
# * Purpose:  CMake build scripts
# * Author: Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
# ******************************************************************************
# * Copyright (C) 2012,2013 Dmitry Baryshnikov
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

include (cmake_utils)

#diagnosing info
message(STATUS "c++ compiler ... " ${CMAKE_CXX_COMPILER})

# For windows, do not allow the compiler to use default target (Vista).
if(WIN32)
  add_definitions(-D_WIN32_WINNT=0x0501 -D_USRDLL)
endif()

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  #add_definitions(-D_MBCS)
  set(GDAL_CSOURCES ${GDAL_CSOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/gcore/Version.rc)
  source_group("Resource Files" FILES ${CMAKE_CURRENT_SOURCE_DIR}/gcore/Version.rc)  

    if(CMAKE_CL_64)
        set_target_properties(${GDAL_LIB_NAME} PROPERTIES STATIC_LIBRARY_FLAGS "/machine:x64")
        add_definitions(-D_WIN64)
    endif()
    configure_file(${CMAKE_MODULE_PATH}/cpl_config.h.vc.cmake ${GDAL_ROOT_SOURCE_DIR}/port/cpl_config.h @ONLY)
endif()

if(UNIX)
    # Include all the necessary files for macros
    include (CheckFunctionExists)
    include (CheckIncludeFile)
    include (CheckIncludeFiles)
    include (CheckLibraryExists)
    include (CheckSymbolExists)
    include (CheckTypeSize)
    include (TestBigEndian)
#    include (CheckCXXSourceCompiles)
#    include (CompilerFlags)
    
    # check libs    
    check_library_exists(c printf "" HAVE_LIBC)
    check_library_exists(m ccos "" HAVE_LIBM)
    check_library_exists(dl    dlopen  "" HAVE_LIBDL)
    check_library_exists(rt    clock_gettime   "" HAVE_LIBRT)
    check_library_exists(pq PQconnectdb "" HAVE_LIBPQ)
    check_library_exists(spatialite spatialite_init "" HAS_SPATIALITE)

    option(CPL_MULTIPROC_PTHREAD "Set to ON if you want to use pthreads based multiprocessing support." ON)
    mark_as_advanced(CPL_MULTIPROC_PTHREAD)

    check_symbol_exists(pthread_mutex_recursive pthread.h HAVE_PTHREAD_MUTEX_RECURSIVE)

    check_include_files("assert.h" HAVE_ASSERT_H)
    gdal_add_definitions(HAVE_ASSERT_H)

    check_function_exists("atoll" HAVE_ATOLL)

    check_include_files("csf.h" HAVE_CSF_H)  
    gdal_add_definitions(HAVE_CSF_H)

    check_include_files("dbmalloc.h" HAVE_DBMALLOC_H) 
    gdal_add_definitions(HAVE_DBMALLOC_H)

    check_function_exists("strtof" HAVE_DECL_STRTOF)    

    check_include_files( direct.h HAVE_DIRECT_H)    
    gdal_add_definitions(HAVE_DIRECT_H)
		
    check_include_files("dlfcn.h" HAVE_DLFCN_H) 
    gdal_add_definitions(HAVE_DLFCN_H)
	
    check_include_files("errno.h" HAVE_ERRNO_H)
    gdal_add_definitions(HAVE_ERRNO_H)

    check_include_files("fcntl.h" HAVE_FCNTL_H) 
    gdal_add_definitions(HAVE_FCNTL_H)

    check_include_files("float.h" HAVE_FLOAT_H) 
    gdal_add_definitions(HAVE_FLOAT_H)

    check_include_files("ieeefp.h" HAVE_IEEEFP_H)
    if(HAVE_IEEEFP_H)
        set(HAVE_IEEEFP TRUE)
	endif()
	
    check_include_files("inttypes.h" HAVE_INTTYPES_H)
    gdal_add_definitions(HAVE_INTTYPES_H)

    check_include_files("jpeglib.h" HAVE_JPEGLIB_H)
    check_include_files("png.h" HAVE_PNG_H)
    
    check_include_files("limits.h" HAVE_LIMITS_H)
    gdal_add_definitions(HAVE_LIMITS_H)

    check_include_files("locale.h" HAVE_LOCALE_H)
    gdal_add_definitions(DHAVE_LOCALE_H)

    check_include_files("memory.h" HAVE_MEMORY_H)
    gdal_add_definitions(HAVE_MEMORY_H)
	
    check_include_files("stdint.h" HAVE_STDINT_H)
    gdal_add_definitions(HAVE_STDINT_H)

    check_include_files("stddef.h" HAVE_STDDEF_H)
    gdal_add_definitions(HAVE_STDDEF_H)

    check_include_files("stdlib.h" HAVE_STDLIB_H)
    gdal_add_definitions(HAVE_STDLIB_H)

    check_include_files("strings.h" HAVE_STRINGS_H)
    gdal_add_definitions(HAVE_STRINGS_H)

    check_include_files("string.h" HAVE_STRING_H)
    gdal_add_definitions(HAVE_STRING_H)

    check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
    gdal_add_definitions(HAVE_SYS_STAT_H)

    check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
    gdal_add_definitions(HAVE_SYS_TYPES_H)

    check_include_file("unistd.h" HAVE_UNISTD_H)
    gdal_add_definitions(HAVE_UNISTD_H)

    check_include_file("values.h" HAVE_VALUES_H)
    gdal_add_definitions(HAVE_VALUES_H)
	    
    check_type_size ("int16"        SIZEOF_INT16)
    check_type_size ("int32"        SIZEOF_INT32)
    check_type_size ("int8"         SIZEOF_INT8)
    check_type_size ("int"          SIZEOF_INT)
    check_type_size ("uint32_t"     SIZEOF_UINT32_T)
    check_type_size ("int64_t"      SIZEOF_INT64_T)
    check_type_size ("float"        SIZEOF_FLOAT)
    check_type_size ("double"       SIZEOF_DOUBLE)
    check_type_size ("long"         SIZEOF_LONG)
    check_type_size ("unsigned long"    SIZEOF_UNSIGNED_LONG)
    check_type_size ("long long"    SIZEOF_LONG_LONG)
    check_type_size ("short"        SIZEOF_SHORT)
    check_type_size ("off_t"        SIZEOF_OFF_T)
    check_type_size ("pid_t"        SIZEOF_PID_T)
    check_type_size ("size_t"       SIZEOF_SIZE_T)
    check_type_size ("socklen_t"    SIZEOF_SOCKLEN_T)
    check_type_size ("sig_atomic_t" SIZEOF_SIG_ATOMIC_T)
    check_type_size ("void *"       SIZEOF_VOID_P)
    check_type_size ("uintptr_t"    SIZEOF_UINTPTR_T)
    check_type_size ("_Bool"        SIZEOF__BOOL)
    check_type_size ("intptr_t"     SIZEOF_INTPTR_T)
    
    
    check_function_exists(fopen64 HAVE_FOPEN64)
    check_function_exists(stat64 HAVE_STAT64)    
    check_function_exists(getcwd HAVE_GETCWD)
    check_function_exists(snprintf HAVE_SNPRINTF)
    check_function_exists(strtof HAVE_STRTOF)
    check_function_exists(vprintf HAVE_VPRINTF)
    check_function_exists(vsnprintf HAVE_VSNPRINTF)
    check_function_exists(readlink HAVE_READLINK)
    
    if(HAVE_FOPEN64 AND HAVE_STAT64)
	set(UNIX_STDIO_64 TRUE)
        set(VSI_LARGE_API_SUPPORTED TRUE)  
        set(VSI_FSEEK64 "fseeko64")
        set(VSI_FTELL64 "ftello64")
        set(VSI_FOPEN64 "fopen64")
        set(VSI_STAT64 "stat64")
        set(VSI_TRANCATE64 ftruncate64)
    else()
	set(UNIX_STDIO_64 FALSE)
        set(VSI_LARGE_API_SUPPORTED FALSE)  
        set(VSI_FSEEK64 "fseek")
        set(VSI_FTELL64 "ftell")
        set(VSI_FOPEN64 "fopen")
        set(VSI_STAT64 "stat")
        set(VSI_TRANCATE64 "ftruncate")
    endif()

    set(VSI_STAT64_T ${VSI_STAT64})    

    check_function_exists("vprintf" HAVE_VPRINTF)
    if(NOT HAVE_VPRINTF)
        check_function_exists("_doprnt" HAVE_DOPRNT)
    endif()


    test_big_endian(BIGENDIAN)    
    if (BIGENDIAN)
        set (HOST_FILLORDER FILLORDER_MSB2LSB)
    else ()
        set (HOST_FILLORDER FILLORDER_LSB2MSB)
    endif ()
 
    if(MACOSX_BUNDLE)
        set(CPL_CONFIG_EXTRAS "#include \"cpl_config_extras.h\"" INTERNAL)
    endif(MACOSX_BUNDLE)

    if (HAVE_STDDEF_H AND HAVE_STDINT_H)
      set(STDC_HEADERS TRUE)
    endif ()
    
    #sqlite/spatialite checks
    check_library_exists(sqlite3 sqlite3_column_table_name "" HAS_COLUMN_METADATA)
    check_library_exists(pcre pcre_compile "" HAS_PCRE)
    check_library_exists(spatialite spatialite_target_cpu "" HAS_SPATIALITE_412_OR_LATER)
	
    configure_file(${CMAKE_MODULE_PATH}/cpl_config.h.cmake ${GDAL_ROOT_SOURCE_DIR}/port/cpl_config.h @ONLY)

	message(STATUS "cpl_config.h is configured")
	
    add_definitions(-DCPL_LSB)
     if(CMAKE_COMPILER_IS_GNUCXX OR CV_ICC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fno-strict-aliasing")#-Wextra -Wall -W -pthread -O2 -fno-strict-aliasing -pthrea
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-strict-aliasing")
     endif()	
endif()

if(WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -W4 -wd4127 -wd4251 -wd4275 -wd4786 -wd4100 -wd4245 -wd4206 -wd4018 -wd4389")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4 -wd4127 -wd4251 -wd4275 -wd4786 -wd4100 -wd4245 -wd4206 -wd4018 -wd4389")
endif()

add_definitions(-DSTRICT -DHAVE_SSE_AT_COMPILE_TIME)

configure_file(${CMAKE_MODULE_PATH}/gdal_def.cmake ${GDAL_ROOT_SOURCE_DIR}/gcore/gdal_def.h @ONLY)

