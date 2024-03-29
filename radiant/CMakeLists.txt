add_executable(radiant
               autosave.cpp
               brushmanip.cpp
               brushmodule.cpp
               brushnode.cpp
               brush.cpp
               brush_primit.cpp
               brushtokens.cpp
               brushxml.cpp
               build.cpp
               camwindow.cpp
               clippertool.cpp
               colors.cpp
               commands.cpp
               console.cpp
               csg.cpp
               dialog.cpp
               eclass_def.cpp
               eclass_doom3.cpp
               eclass_fgd.cpp
               eclass.cpp
               eclass_xml.cpp
               entityinspector.cpp
               entitylist.cpp
               entity.cpp
               environment.cpp
               error.cpp
               feedback.cpp
               filetypes.cpp
               filterbar.cpp
               filters.cpp
               findtexturedialog.cpp
               glwidget.cpp
               grid.cpp
               groupdialog.cpp
               gtkdlgs.cpp
               gtkmisc.cpp
               help.cpp
               image.cpp
               mainframe.cpp
               main.cpp
               map.cpp
               modelwindow.cpp
               mru.cpp
               nullmodel.cpp
               parse.cpp
               patchdialog.cpp
               patchmanip.cpp
               patchmodule.cpp
               patch.cpp
               pluginapi.cpp
               pluginmanager.cpp
               pluginmenu.cpp
               plugin.cpp
               plugintoolbar.cpp
               points.cpp
               preferencedictionary.cpp
               preferences.cpp
               qe3.cpp
               qgl.cpp
               referencecache.cpp
               renderer.cpp
               renderstate.cpp
               scenegraph.cpp
               selection.cpp
               select.cpp
               server.cpp
               sockets.cpp
               stacktrace.cpp
               surfacedialog.cpp
               texmanip.cpp
               textures.cpp
               texwindow.cpp
               theme.cpp
               tools.cpp
               treemodel.cpp
               undo.cpp
               url.cpp
               view.cpp
               watchbsp.cpp
               winding.cpp
               windowobservers.cpp
               xmlstuff.cpp
               xywindow.cpp
               ${CMAKE_SOURCE_DIR}/icons/radiant.rc
              )

target_include_directories(radiant PRIVATE .
                           ${LIBXML2_INCLUDE_DIRS}
                           ${GLIB_INCLUDE_DIRS}
                           ${QT5_INCLUDE_DIRS}
                          )

target_link_libraries(radiant
                      commandlib
                      gtkutil
                      l_net
                      quickhull
                      profile
                      xml
                      Qt5::Gui
                      Qt5::Widgets
                      Qt5::Core
                      ${GLIB_LIBRARIES}
                      ${LIBXML2_LIBRARIES}
                      ${CMAKE_DL_LIBS}
                     )

if(WIN32)
    if(${MSVC})
        target_link_options(radiant PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
    else()
        target_link_options(radiant PRIVATE "-mwindows")
    endif()
endif()

if(WIN32)
    target_link_libraries(radiant dbghelp)
endif()
