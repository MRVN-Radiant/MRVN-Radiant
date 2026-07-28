add_library(assmodel MODULE
            mdlimage.cpp
            mdlimage.h
            model.cpp
            model.h
            plugin.cpp
            plugin.h
            assmodel.def
           )

target_include_directories(assmodel PRIVATE
                           assimp
                           ${QT6_INCLUDE_DIRS}
                          )

target_link_libraries(assmodel PRIVATE
                      assimp
                      Qt6::Gui
                      Qt6::OpenGL
                      Qt6::OpenGLWidgets
                     )

if(NOT ${MSVC})
    set_target_properties(assmodel PROPERTIES LINK_FLAGS "-Wl,-rpath,../")
endif()
