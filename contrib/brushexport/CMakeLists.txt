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
                           ${QT6_INCLUDE_DIRS}
                          )

target_link_libraries(brushexport
                      Qt6::Gui
                      Qt6::Widgets
                      Qt6::Core
                      ${GLIB_LIBRARIES}
                     )