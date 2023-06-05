add_library(assmodel MODULE
            mdlimage.cpp
            mdlimage.h
            model.cpp
            model.h
            plugin.cpp
            plugin.h
           )

target_include_directories(assmodel PRIVATE
                           assimp
                           ${QT5_INCLUDE_DIRS}
                          )
target_link_libraries(assmodel PRIVATE
                      assimp
                      Qt5::Gui
                     )

if(NOT ${MSVC})
    set_target_properties(assmodel PROPERTIES LINK_FLAGS "-Wl,-rpath,../")
endif()
