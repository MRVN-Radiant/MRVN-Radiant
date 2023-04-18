add_library(imagepng MODULE
            plugin.cpp
            plugin.h
            imagepng.def
           )

target_link_libraries(imagepng PRIVATE ${PNG_LIBRARIES})