add_library(shaderplug MODULE
            shaderplug.cpp
            shaderplug.h
            shaderplug.def
           )


target_include_directories(shaderplug PRIVATE .
                           ${GLIB_INCLUDE_DIRS}
                           ${QT6_INCLUDE_DIRS}
                           ${LIBXML2_INCLUDE_DIRS}
                          )

target_link_libraries(shaderplug
                      xml
                      Qt6::Gui
                      Qt6::Widgets
                      Qt6::Core
                      ${GLIB_LIBRARIES}
                      ${LIBXML2_LIBRARIES}
                     )
