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
                           ${QT5_INCLUDE_DIRS}
                          )

target_link_libraries(prtview
                      profile
                      Qt5::Gui
                      Qt5::Widgets
                      Qt5::Core
                      ${GLIB_LIBRARIES}
                     )