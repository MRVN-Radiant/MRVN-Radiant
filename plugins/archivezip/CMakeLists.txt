add_library(archivezip MODULE
            archive.cpp
            archive.h
            pkzip.cpp
            pkzip.h
            plugin.cpp
            plugin.h
            zlibstream.cpp
            zlibstream.h
            archivezip.def
           )

#target_include_directories(shaders PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(archivezip PRIVATE z)