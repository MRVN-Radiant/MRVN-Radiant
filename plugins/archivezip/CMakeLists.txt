add_library(archivezip
            archive.cpp
            archive.h
            pkzip.cpp
            pkzip.h
            plugin.cpp
            plugin.h
            zlibstream.cpp
            zlibstream.h
           )

#target_include_directories(shaders PRIVATE ${GLIB_INCLUDE_DIRS})
#target_link_libraries(shaders PRIVATE ${GLIB_LIBRARIES})