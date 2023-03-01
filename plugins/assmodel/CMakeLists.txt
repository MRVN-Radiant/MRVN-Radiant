add_library(assmodel
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
                      Qt5::Widgets
                      Qt5::Core
                     )