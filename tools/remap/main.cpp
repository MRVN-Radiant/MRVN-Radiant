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

   -------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   -------------------------------------------------------------------------------

   "Q3Map2" has been significantly modified in the form of "remap" to support
   Respawn Entertainment games.

   ------------------------------------------------------------------------------- */

#include "remap.h"
#include "timer.h"

//------------------------------------------------------------
// Purpose: Allocation error callback
//------------------------------------------------------------
static void new_handler()
{
	Error( "Memory allocation failed, terminating" );
}

//------------------------------------------------------------
// Purpose: Cleanups on exit
//------------------------------------------------------------
static void ExitReMap()
{
	/* flush xml send buffer, shut down connection */
	Broadcast_Shutdown();
}



//------------------------------------------------------------
// Purpose: Entry point
// Input  :
// Output :
//------------------------------------------------------------
int main( int argc, char *argv[] )
{
	int ret = 0;

#ifdef WIN32
	// Set max number of open io streams
	_setmaxstdio(2048);
#endif

	// We want consistent 'randomness'
	srand( 0 );

	// Start compilation time timer
	CTimer timer;

	// Set exit call
	atexit( ExitReMap );

	// Set allocation error callback
	std::set_new_handler( new_handler );

	Args args( argc, argv );

	// Read general options first
	{
		// -connect
		if ( args.takeArg( "-connect" ) )
		{
			Broadcast_Setup( args.takeNext() );
		}

		// -verbose
		if ( args.takeArg( "-verbose" ) )
		{
			verbose = true;
		}

		// Threads
		while ( args.takeArg( "-threads" ) )
		{
			numthreads = atoi( args.takeNext() );
		}
	}

	// Init assimp
	assimp_init();

	// Set number of threads 
	ThreadSetDefault();

	// Print emblem and generic version information
	for( size_t i = 0; i < ARRAYSIZE(REMAP_EMBLEM); i++ )
	{
		Sys_Printf( "%s\n", REMAP_EMBLEM[i].c_str() );
	}

	Sys_Printf( "Remap version: " Q3MAP_VERSION "\n" );
	Sys_Printf( "Radiant version: " RADIANT_VERSION "\n" );
	Sys_Printf( "%s\n", args.getArg0() );
	Sys_Printf( "\n" );

	// Get game
	if( args.takeArg( "-game" ) )
	{
		g_game = GetGame( args.takeNext() );
	}

	// Print info about passed bsp
	if( args.takeArg( "-info" ) )
	{
		// TODO [Fifty]: Finish this
		Sys_Printf("INFO\n");
	}

	// Init paths
	InitPaths( args );

	// Create bsp
	if( args.takeArg( "-bsp" ) )
	{
		ret = BSPMain( args );
		Sys_Printf( "BspMain returned: %i\n", ret );
	}

	Sys_Printf( "%9.0f seconds elapsed\n", timer.elapsed_sec() );

	// Pause
	if( args.takeArg( "-pause" ) )
	{
		std::system( "pause" );
	}

	// List unknown options
	while( !args.empty() )
	{
		Sys_Warning( "Unknown option \"%s\"\n", args.takeFront() );
	}

	// Return
	return ret;
}
