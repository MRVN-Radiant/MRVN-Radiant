add_library(imagepng MODULE
            plugin.cpp
            plugin.h
            imagepng.def
           )

#target_include_directories(shaders PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(imagepng PRIVATE ${PNG_LIBRARIES})