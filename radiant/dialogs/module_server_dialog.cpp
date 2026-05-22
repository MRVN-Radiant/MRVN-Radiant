/*
   Copyright (C) 2026 MRVN-Radiant contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of MRVN-Radiant.

   MRVN-Radiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   MRVN-Radiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with MRVN-Radiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "module_server_dialog.h"
#include "modulesystem.h"

#include <QBoxLayout>
#include <QLabel>
#include <QScrollArea>


static ModuleServerDialog g_ModuleServerDialog;

void ModuleServerDialog::BuildDialog()
{
	GetWidget()->setWindowTitle( "Module server view" );
	QBoxLayout* const dialog_layout = new QBoxLayout(QBoxLayout::TopToBottom, GetWidget());
	dialog_layout->setMargin(0);

	QScrollArea* const scroll_area = new QScrollArea();
	dialog_layout->addWidget(scroll_area);

	QWidget* const widget = new QWidget();
	scroll_area->setWidget(widget);
	scroll_area->setWidgetResizable(true);

	QBoxLayout* const main_layout = new QBoxLayout(QBoxLayout::TopToBottom, widget);

	globalModuleServer().foreachModule([&](const char* type, int version, Module* module) {
		QWidget* const panel = new QWidget();
		panel->setAutoFillBackground(true);
		panel->setStyleSheet("background-color: palette(base);");
		main_layout->addWidget(panel);

		QBoxLayout* const module_layout = new QBoxLayout(QBoxLayout::LeftToRight, panel);

		QLabel* const text_name = new QLabel(type);
		module_layout->addWidget(text_name);

		QLabel* const text_version = new QLabel(QString("Version: %1").arg(version));
		text_version->setAlignment(Qt::AlignRight);
		module_layout->addWidget(text_version);
	});
}

void ModuleServerDialog_constructWindow(QWidget* const main_window)
{
	g_ModuleServerDialog.Create(main_window);
}

void ModuleServerDialog_destructWindow()
{
	g_ModuleServerDialog.Destroy();
}

void ModuleServerDialog_showDialog()
{
	g_ModuleServerDialog.ShowDlg();
}