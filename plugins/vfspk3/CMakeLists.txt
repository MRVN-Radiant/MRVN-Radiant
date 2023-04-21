add_library(vfspk3 MODULE
            archive.cpp
            archive.h
            vfs.cpp
            vfs.h
            vfspk3.cpp
            vfspk3.h
            vfsq3.def
           )

target_include_directories(vfspk3 PRIVATE
                           ${GLIB_INCLUDE_DIRS}
                          )

target_link_libraries(vfspk3 PRIVATE
                      ${GLIB_LIBRARIES}
                      filematch
                     )