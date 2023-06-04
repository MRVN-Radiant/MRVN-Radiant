add_library(imagepng MODULE
            plugin.cpp
            plugin.h
            imagepng.def
           )

target_link_libraries(imagepng PRIVATE ${PNG_LIBRARIES})

target_include_directories(imagepng PRIVATE ${PNG_INCLUDE_DIRS})