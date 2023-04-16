add_library(brushexport MODULE
            callbacks.cpp
            callbacks.h
            export.cpp
            export.h
            interface.cpp
            plugin.cpp
            plugin.h
            brushexport.def
           )

target_include_directories(brushexport PRIVATE .
                                            ${GLIB_INCLUDE_DIRS}
                                            ${QT5_INCLUDE_DIRS}
                          )

target_link_libraries(brushexport
                      Qt5::Gui
                      Qt5::Widgets
                      Qt5::Core
                      ${GLIB_LIBRARIES}
                     )