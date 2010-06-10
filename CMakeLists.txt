project( netcfgfx )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
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

include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
	)
set ( netcfgfx_HDRS
	version.h
	)

set ( netcfgfx_SRCS
	src/main.cpp
	src/netcfgfx_tray.cpp
	src/netcfgfx_options.cpp
	src/netcfgfx_profile_editor.cpp
	src/netcfgfx_network_scanner.cpp
        src/netcfgfx_notifier.cpp
	)
QT4_ADD_DBUS_INTERFACE(netcfgfx_SRCS
        xml/org.freedesktop.Notifications.xml
        netcfgfx_notifyiface

        )
set ( netcfgfx_UIS
	src/netcfgfx_profile.ui
	src/netcfgfx_options.ui
	src/netcfgfx_profile_editor.ui
	src/netcfgfx_network_scanner.ui
	)
QT4_WRAP_UI(UIS ${netcfgfx_UIS})

set ( netcfgfx_RSCS
	resources/images.qrc
	)
QT4_ADD_RESOURCES(RSCS ${netcfgfx_RSCS})

set ( netcfgfx_TRS
	translations/netcfgfx_de.ts
	)
QT4_ADD_TRANSLATION(TRS ${netcfgfx_TRS})

set ( netcfgfx_MOCS
	src/netcfgfx_tray.h
	src/netcfgfx_options.h
	src/netcfgfx_profile_editor.h
	src/netcfgfx_network_scanner.h
        src/netcfgfx_notifier.h
    )
QT4_WRAP_CPP(MOCS ${netcfgfx_MOCS})

add_executable ( netcfgfx ${netcfgfx_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( netcfgfx  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY}
 ${IWLIB_LIB} )
