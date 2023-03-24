add_library(imageq2 MODULE
            imageq2.cpp
            imageq2.h
            wal.cpp
            wal.h
            wal32.cpp
            wal.h
            imageq2.def
           )

#target_include_directories(shaders PRIVATE ${GLIB_INCLUDE_DIRS})
#target_link_libraries(shaders PRIVATE ${GLIB_LIBRARIES})