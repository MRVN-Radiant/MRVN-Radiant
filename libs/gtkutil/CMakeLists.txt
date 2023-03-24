
add_library(gtkutil STATIC
            accelerator_translate.h
            accelerator.cpp
            accelerator.h
#            button.cpp
#            button.h
            clipboard.cpp
            clipboard.h
#            closure.cpp
#            closure.h
            combobox.h
#            container.cpp
#            container.h
#            cursor.cpp
#            cursor.h
            dialog.cpp
            dialog.h
            entry.cpp
            entry.h
            fbo.h
            filechooser.cpp
            filechooser.h
#            frame.cpp
#            frame.h
            glfont.cpp
            glfont.h
            glwidget.cpp
            glwidget.h
            guisettings.cpp
            guisettings.h
            idledraw.cpp
            idledraw.h
            image.cpp
            image.h
            lineedit.h
            menu.cpp
            menu.h
            messagebox.cpp
            messagebox.h
            mousepresses.h
            nonmodal.cpp
            nonmodal.h
#            paned.cpp
#            paned.h
            pointer.cpp
            pointer.h
            spinbox.h
            toolbar.cpp
            toolbar.h
            widget.cpp
            widget.h
#            window.cpp
#            window.h
            xorrectangle.cpp
            xorrectangle.h
        )

#target_include_directories(libs PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(gtkutil PRIVATE Qt5::Gui
                                      Qt5::Widgets
                                      Qt5::Core
                                      ${GLIB_LIBRARIES}
                     )
