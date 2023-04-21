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

target_link_libraries(archivezip PRIVATE z)