add_library(shaderplug MODULE
            shaderplug.cpp
            shaderplug.h
            shaderplug.def
           )


target_include_directories(shaderplug PRIVATE .
                           ${GLIB_INCLUDE_DIRS}
                           ${QT5_INCLUDE_DIRS}
                           ${LIBXML2_INCLUDE_DIRS}
                          )

target_link_libraries(shaderplug
                      xml
                      Qt5::Gui
                      Qt5::Widgets
                      Qt5::Core
                      ${GLIB_LIBRARIES}
                      ${LIBXML2_LIBRARIES}
                     )
