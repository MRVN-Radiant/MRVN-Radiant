

add_library(xml STATIC
            ixml.cpp
            ixml.h
            xmlelement.cpp
            xmlelement.h
            xmlparser.cpp
            xmlparser.h
            xmltextags.cpp
            xmltextags.h
            xmlwriter.cpp
            xmlwriter.h
           )

target_include_directories(xml PRIVATE
                           ${LIBXML2_INCLUDE_DIRS}
                           ${GLIB_INCLUDE_DIRS}
                          )
target_link_libraries(xml PRIVATE ${LIBXML2_LIBRARIES} ${GLIB_LIBRARIES})
