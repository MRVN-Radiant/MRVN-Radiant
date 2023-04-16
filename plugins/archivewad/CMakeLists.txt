add_library(archivewad MODULE
            archive.cpp
            archive.h
            wad.cpp
            wad.h
            plugin.cpp
            plugin.h
            archivewad.def
           )

#target_include_directories(shaders PRIVATE ${GLIB_INCLUDE_DIRS})
#target_link_libraries(shaders PRIVATE ${GLIB_LIBRARIES})