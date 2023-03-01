add_library(mapxml
            plugin.cpp
            plugin.h
            xmlparse.cpp
            xmlparse.h
            xmlwrite.cpp
            xmlwrite.h
           )

target_include_directories(mapxml PRIVATE
                           ${XML_INCLUDE_DIRS}
                           ${GLIB_INCLUDE_DIRS}
                          )

target_link_libraries(mapxml PRIVATE
                      xml
                      ${GLIB_LIBRARIES}
                     )