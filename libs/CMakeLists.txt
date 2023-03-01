
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_DOCS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_NO_EXPORT OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
add_subdirectory(assimp)

# ...
add_subdirectory(mathlib)
add_subdirectory(gtkutil)
add_subdirectory(l_net)
add_subdirectory(profile)
add_subdirectory(quickhull)
add_subdirectory(xml)


add_library(commandlib STATIC
            commandlib.cpp
            commandlib.h
           )

add_library(filematch STATIC
            filematch.c
            filematch.h
           )

add_library(ddslib STATIC
            ddslib.h
            ddslib/ddslib.c
           )

add_library(etclib STATIC
            etclib.c
            etclib.h
           )

# add_library(libs STATIC
#             archivelib.h
#             bytebool.h
#             bytestreamutils.h
#             character.h
#             commandlib.cpp
#             commandlib.h
#             convert.h
#             ddslib.h
#             dragplanes.h
#             eclasslib.h
#             entitylib.h
#             entityxml.h
#             etclib.c
#             etclib.h
#             filematch.c
#             filematch.h
#             fs_filesystem.h
#             fs_path.h
#             imagelib.h
#             instancelib.h
#             maplib.h
#             mathlib.h
#             md5lib.h
#             moduleobservers.h
#             picomodel.h
#             pivot.h
#             rect_t.h
#             render.h
#             scenelib.h
#             scenelib.h
#             selectionlib.h
#             shaderlib.h
#             str.h
#             stringio.h
#             texturelib.h
#             timer.h
#             transformlib.h
#             traverselib.h
#             typesystem.h
#             undolib.h
#             uniquenames.h
#             versionlib.h
#         )

# target_include_directories(libs PRIVATE ${GLIB_INCLUDE_DIRS} )
# target_link_libraries(libs PRIVATE ${GLIB_LIBRARIES})
