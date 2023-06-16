/* -------------------------------------------------------------------------------

   Copyright (C) 1999-2007 id Software, Inc. and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ----------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   -------------------------------------------------------------------------------

   "Q3Map2" has been significantly modified in the form of "remap" to support
   Respawn Entertainment games.

   ------------------------------------------------------------------------------- */

/* dependencies */
#include "remap.h"
#include "bspfile_abstract.h"

//------------------------------------------------------------
// Purpose: Writes .bsp and .ent files to disk
// Input  : *szMapFileName
//------------------------------------------------------------
void WriteBspFiles( const char* szMapFileName)
{
	fs::path phFileName(szMapFileName);
	phFileName.replace_extension( fs::path("") );
	
	Sys_FPrintf(SYS_VRB, "--- WriteBspFiles ---\n");

	WriteBSPFile( phFileName.string().c_str() );
	WriteEntFiles( phFileName.string().c_str() );
}

//------------------------------------------------------------
// Purpose: Handles creation of a bsp file
// Input  : &args
//------------------------------------------------------------
int BSPMain( Args& args )
{
	if( g_game == NULL )
	{
		Sys_Warning( "Not running BSPMain as g_game is NULL! This is an error, make sure to specify -game argument!\n" );
		return -1;
	}

	// Note it
	Sys_Printf( "--- BSP ---\n" );

	// Last argument is the map name
	const char *pszFileName = args.takeBack();

	// Process arguments
	{
		while ( args.takeArg( "-externalmodels" ) ) {
			Sys_Printf( "External models enabled\n" );
			g_bExternalModels = true;
		}
	}

	// Check arg game compatibility
	if( g_game->bspVersion == 29 && g_bExternalModels )
	{
		// NOTE [Fifty]: "External models", brush entities defined in .ent files aren't supported in titanfall 1
		Sys_Warning( "Game: \"titanfallonline\" doesn't support the \"-externalmodels\" flag!\n" );
		g_bExternalModels = false;
	}

	// Load shaders
	LoadShaderInfo();

	// Load map file
	char szMapFileName[_MAX_PATH];
	strncpy(szMapFileName, pszFileName, _MAX_PATH);

	LoadMapFile(szMapFileName, false, false );
	
	// Compile map
	Sys_FPrintf( SYS_VRB, "--- CompileMap ---\n" );
	g_game->compile();
	Sys_Printf( "\n" );

	// Write .bsp and .ent files
	WriteBspFiles(pszFileName);

	// Return to sender
	return 0;
}
