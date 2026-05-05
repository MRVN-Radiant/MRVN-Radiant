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

#include "plugin.h"

#include "iarchive.h"

#include "debugging/debugging.h"
#include "modulesystem/singletonmodule.h"

#include "archive.h"


class ArchiveVPKAPI
{
	_QERArchiveTable m_archivevpk;
public:
	typedef _QERArchiveTable Type;
	STRING_CONSTANT( Name, "vpk" );

	ArchiveVPKAPI(){
		m_archivevpk.m_pfnOpenArchive = &OpenArchive;
	}
	_QERArchiveTable* getTable(){
		return &m_archivevpk;
	}
};

typedef SingletonModule<ArchiveVPKAPI> ArchiveVPKModule;

ArchiveVPKModule g_ArchiveVPKModule;

extern "C" void RADIANT_DLLEXPORT Radiant_RegisterModules( ModuleServer& server )
{
	initialiseModule( server );

	g_ArchiveVPKModule.selfRegister();
}
