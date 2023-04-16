add_library(mapxml MODULE
            plugin.cpp
            plugin.h
            xmlparse.cpp
            xmlparse.h
            xmlwrite.cpp
            xmlwrite.h
            mapxml.def
           )

target_include_directories(mapxml PRIVATE
                           ${LIBXML2_INCLUDE_DIRS}
                           ${GLIB_INCLUDE_DIRS}
                          )

target_link_libraries(mapxml PRIVATE
                      ${LIBXML2_LIBRARIES}
                      ${GLIB_LIBRARIES}
                     )