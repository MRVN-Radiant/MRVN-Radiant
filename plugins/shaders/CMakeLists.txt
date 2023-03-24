add_library(shaders MODULE
            plugin.cpp
            plugin.h
            shaders.cpp
            shaders.h
            shadersq3.def
           )

target_include_directories(shaders PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(shaders PRIVATE
                      ${GLIB_LIBRARIES}
                      commandlib
                     )