project( netcfgfx )
cmake_minimum_required( VERSION 2.6 )

add_definitions ( -Wall )
SET (QT_USE_QTDBUS true)
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include ( CheckIncludeFile )

check_include_file(iwlib.h IWLIB_H -D_GNU_SOURCE)
if(NOT IWLIB_H)
	message(FATAL_ERROR "Unable to find iwlib.h")
endif(NOT IWLIB_H)

find_library(IWLIB_LIB NAMES iw)
if(NOT IWLIB_LIB)
	message(FATAL_ERROR "Unable to find libiw.so")
endif(NOT IWLIB_LIB)

add_subdirectory(src)
