include_directories (
        ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
        )
set(netcfgfx_MOCS
    notifier.h
    application.h
    )
set(netcfgfx_SRCS
    main.cpp
    notifier.cpp
    application.cpp
    )
QT4_ADD_DBUS_INTERFACE(netcfgfx_SRCS
        ../xml/org.freedesktop.Notifications.xml
        NotifyIF
        )
QT4_WRAP_CPP(MOCS
    ${netcfgfx_MOCS}
    )
add_executable(netcfgfx ${netcfgfx_SRCS} ${MOCS})
target_link_libraries(netcfgfx ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY})
