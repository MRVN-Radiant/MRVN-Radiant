add_library(ToolsShared STATIC
            "vfs/membuffer.h"
            "vfs/vfs.cpp"
            "vfs/vfs.h"
            "bitflags.h"
            "cmdlib.cpp"
            "cmdlib.h"
            "inout_xml.h"
            "inout.cpp"
            "inout.h"
            "jpeg.cpp"
            "maxworld.h"
            "md4.cpp"
            "md4.h"
            "miniz.cpp"
            "miniz.h"
            "polylib.cpp"
            "polylib.h"
            "qimagelib.cpp"
            "qimagelib.h"
            "qmath.h"
            "qpathops.h"
            "qstringops.h"
            "qthreads.h"
            "scriplib.cpp"
            "scriplib.h"
            "stringfixedsize.h"
            "threads.cpp"
            "toolsdef.h"
            "unzip.cpp"
            "unzip.h")

target_include_directories(ToolsShared PRIVATE .
                           ${LIBXML2_INCLUDE_DIRS}
                           ${GLIB_INCLUDE_DIRS}
                           ${JPEG_INCLUDE_DIRS}
                          )

target_link_libraries(ToolsShared
                      ${GLIB_LIBRARIES}
                      ${JPEG_LIBRARIES}
                      ${LIBXML2_LIBRARIES}
                     )