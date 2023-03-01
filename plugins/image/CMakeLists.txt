add_library(image
            bmp.cpp
            bmp.h
            dds.cpp
            dds.h
            image.cpp
            image.h
            jpeg.cpp
            jpeg.h
            ktx.cpp
            ktx.h
            pcx.cpp
            pcx.h
            tga.cpp
            tga.h
           )

#target_include_directories(shaders PRIVATE ${GLIB_INCLUDE_DIRS})
#target_link_libraries(shaders PRIVATE ${GLIB_LIBRARIES})