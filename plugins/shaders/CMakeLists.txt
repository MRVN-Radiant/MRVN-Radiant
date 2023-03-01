add_library(shaders
            plugin.cpp
            plugin.h
            shaders.cpp
            shaders.h
           )

target_include_directories(shaders PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(shaders PRIVATE
                      ${GLIB_LIBRARIES}
                      commandlib
                     )