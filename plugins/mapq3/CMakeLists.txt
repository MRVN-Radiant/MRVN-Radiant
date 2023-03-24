add_library(mapq3 MODULE
            parse.cpp
            parse.h
            plugin.cpp
            plugin.h
            write.cpp
            write.h
            mapq3.def
           )

#target_include_directories(shaders PRIVATE ${GLIB_INCLUDE_DIRS})
#target_link_libraries(shaders PRIVATE ${GLIB_LIBRARIES})