include_directories (
        ${netcfgfx_BINARY_DIR}
        ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
        )
set(netcfgfx_MOCS
    notifier.h
    application.h
    profiledialog.h
    )
set(netcfgfx_SRCS
    main.cpp
    notifier.cpp
    application.cpp
    profiledialog.cpp
    )
set(netcfgfx_UIS
    profiles.ui
    )
QT4_ADD_DBUS_INTERFACE(netcfgfx_SRCS
        ../xml/org.freedesktop.Notifications.xml
        NotifyIF
        )
QT4_WRAP_UI(UIS ${netcfgfx_UIS})
QT4_WRAP_CPP(MOCS
    ${netcfgfx_MOCS}
    )
set(netcfgfx_RCS
   ../resources/netcfgfx.qrc
   )
QT4_ADD_RESOURCES(RCS ${netcfgfx_RCS})
add_executable(netcfgfx ${netcfgfx_SRCS} ${MOCS} ${RCS} ${UIS})
target_link_libraries(netcfgfx qnetcfg ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY})
