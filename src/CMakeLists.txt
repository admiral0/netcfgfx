set(netcfgfx_SRCS
    main.cpp
    )
add_executable(netcfgfx ${netcfgfx_SRCS})
target_link_libraries(netcfgfx ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY})
