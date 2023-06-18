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

// dependencies
#include "remap.h"





//------------------------------------------------------------
// Purpose: Initilazes vfs paths
// Input  : &args
//------------------------------------------------------------
void InitPaths( Args& args )
{
	if( g_pGame == NULL )
	{
		Sys_Warning( "Not running InitPaths as g_game is NULL! This is expected when running with -info.\n" );
		return;
	}

	unordered_set<string> basePaths;
	unordered_set<string> gamePaths;

	Sys_FPrintf( SYS_VRB, "--- InitPaths ---\n" );

	/* parse through the arguments and extract those relevant to paths */
	{
		// -fs_basepath
		while ( args.takeArg( "-fs_basepath" ) ) {
			basePaths.insert( args.takeNext() );
		}

		// -fs_game
		while ( args.takeArg( "-fs_game" ) ) {
			gamePaths.insert( args.takeNext() );
		}
	}

	// Initilaze directories
	for( string gamePath : gamePaths )
	{
		for( string basePath : basePaths )
		{
			vfsInitDirectory( ( basePath + gamePath ).c_str() );
		}
	}

	// Done
	Sys_Printf( "\n" );
}
