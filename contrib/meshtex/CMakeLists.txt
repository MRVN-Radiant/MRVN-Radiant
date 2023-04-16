add_library(meshtex MODULE
            GeneralFunctionDialog.cpp
            GeneralFunctionDialog.h
            GenericDialog.cpp
            GenericDialog.h
            GenericMainMenu.cpp
            GenericMainMenu.h
            GenericPluginUI.cpp
            GenericPluginUI.h
            GetInfoDialog.cpp
            GetInfoDialog.h
            MainMenu.cpp
            MainMenu.h
            MeshEntity.cpp
            MeshEntity.h
            MeshVisitor.cpp
            MeshVisitor.h
            PluginModule.cpp
            PluginModule.h
            PluginRegistration.cpp
            PluginUI.cpp
            PluginUI.h
            RefCounted.cpp
            RefCounted.h
            SetScaleDialog.cpp
            SetScaleDialog.h
            meshtex.def
           )

target_include_directories(meshtex PRIVATE .
                                            ${GLIB_INCLUDE_DIRS}
                                            ${QT5_INCLUDE_DIRS}
                          )

target_link_libraries(meshtex
                      Qt5::Gui
                      Qt5::Widgets
                      Qt5::Core
                      ${GLIB_LIBRARIES}
                     )