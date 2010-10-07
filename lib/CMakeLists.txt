include_directories (
        ${netcfgfx_BINARY_DIR}
        ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        ${QT_QTCORE_INCLUDE_DIR} 
        )
set(libqnetcfg_MOCS
	profile.h
        profiles.h
        util.h
    )
set(libqnetcfg_SRCS
	profile.cpp
        profiles.cpp
        util.cpp
    )
QT4_WRAP_CPP(lib_MOCS
    ${libqnetcfg_MOCS}
    )
add_library(qnetcfg SHARED ${libqnetcfg_SRCS} ${lib_MOCS})
target_link_libraries(qnetcfg ${QT_QTCORE_LIBRARY})
