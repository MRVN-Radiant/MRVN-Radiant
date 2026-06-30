add_library(prtview MODULE
            AboutDialog.cpp
            AboutDialog.h
            ConfigDialog.cpp
            ConfigDialog.h
            LoadPortalFileDialog.cpp
            LoadPortalFileDialog.h
            portals.cpp
            portals.h
            prtview.cpp
            prtview.h
            PrtView.def
           )

target_include_directories(prtview PRIVATE .
                           ${GLIB_INCLUDE_DIRS}
                           ${QT6_INCLUDE_DIRS}
                          )

target_link_libraries(prtview
                      profile
                      Qt6::Gui
                      Qt6::Widgets
                      Qt6::Core
                      ${GLIB_LIBRARIES}
                     )