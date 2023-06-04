add_library(image MODULE
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
            imageq3.def
           )

target_link_libraries(image PRIVATE ddslib etclib ${JPEG_LIBRARIES})

target_include_directories(image PRIVATE ${JPEG_INCLUDE_DIRS})