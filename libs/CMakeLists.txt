
set(ASSIMP_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/install)
set(ASSIMP_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/install)
set(ASSIMP_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/install)

add_compile_definitions(ASSIMP_BUILD_DLL_EXPORT)
add_compile_definitions(ASSIMP_BUILD_NO_C4D_IMPORTER)
add_compile_definitions(ASSIMP_BUILD_NO_EXPORT)
add_compile_definitions(ASSIMP_BUILD_NO_IFC_IMPORTER)
add_compile_definitions(ASSIMP_BUILD_NO_OWN_ZLIB)
add_compile_definitions(OPENDDLPARSER_BUILD)
add_compile_definitions(RAPIDJSON_NOMEMBERITERATORCLASS)
add_compile_definitions(WIN32_LEAN_AND_MEAN)
add_compile_definitions(assimp_EXPORTS)
set(ASSIMP_IMPORTER_GLTF_USE_OPEN3DGC true)
set(MINIZ_USE_UNALIGNED_LOADS_AND_STORES false)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_DOCS OFF)
set(ASSIMP_BUILD_TOOLS OFF)
set(ASSIMP_BUILD_DRACO OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_MINIZIP ON)
add_subdirectory(assimp)

# These are static
add_subdirectory(ddslib)
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

add_library(etclib STATIC
            etclib.c
            etclib.h
           )
