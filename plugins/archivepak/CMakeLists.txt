add_library(archivepak MODULE
            archive.cpp
            archive.h
            pak.cpp
            pak.h
            plugin.cpp
            plugin.h
            archivepak.def
           )

#target_include_directories(shaders PRIVATE ${GLIB_INCLUDE_DIRS})
#target_link_libraries(shaders PRIVATE ${GLIB_LIBRARIES})