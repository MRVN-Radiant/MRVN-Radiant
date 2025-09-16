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

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "remap.h"



inline void AAS_DData( unsigned char *data, int size ){
	for ( int i = 0; i < size; ++i )
		data[i] ^= (unsigned char) i * 119;
}

/*
   FixAAS()
   resets an aas checksum to match the given BSP
 */

int FixAAS( Args& args ){
	/* arg checking */
	if ( args.empty() ) {
		Sys_Printf( "Usage: q3map2 -fixaas [-v] <mapname>\n" );
		return 0;
	}

	/* do some path mangling */
	strcpy( source, ExpandArg( args.takeBack() ) );
	path_set_extension( source, ".bsp" );

	/* note it */
	Sys_Printf( "--- FixAAS ---\n" );

	/* load the bsp */
	Sys_Printf( "Loading %s\n", source );
	MemBuffer buffer = LoadFile( source );

	/* create bsp checksum */
	Sys_Printf( "Creating checksum...\n" );
	int checksum = LittleLong( (int)Com_BlockChecksum( buffer.data(), buffer.size() ) ); // md4 checksum for a block of data
	AAS_DData( (unsigned char *) &checksum, 4 );

	/* write checksum to aas */
	for( auto&& ext : { ".aas", "_b0.aas", "_b1.aas" } )
	{
		/* mangle name */
		char aas[ 1024 ];
		strcpy( aas, source );
		path_set_extension( aas, ext );
		Sys_Printf( "Trying %s\n", aas );

		/* fix it */
		FILE *file = fopen( aas, "r+b" );
		if ( !file ) {
			continue;
		}
		if ( fseek( file, 8, SEEK_SET ) != 0 || fwrite( &checksum, 4, 1, file ) != 1 ) {
			Error( "Error writing checksum to %s", aas );
		}
		fclose( file );
	}

	/* return to sender */
	return 0;
}



/*
   AnalyzeBSP() - ydnar
   analyzes a Quake engine BSP file
 */

struct abspHeader_t
{
	char ident[ 4 ];
	int version;

	bspLump_t lumps[ 1 ];       /* unknown size */
};

struct abspLumpTest_t
{
	int radix, minCount;
	const char     *name;
};

int AnalyzeBSP( Args& args ){
	
	/* return to caller */
	return 0;
}



/*
   BSPInfo()
   emits statistics about the bsp file
 */

int BSPInfo( Args& args ){
	char source[ 1024 ];


	/* dummy check */
	if ( args.empty() ) {
		Sys_Printf( "No files to dump info for.\n" );
		return -1;
	}

	/* walk file list */
	while ( !args.empty() )
	{
		Sys_Printf( "---------------------------------\n" );

		/* mangle filename and get size */
		const char *fileName = args.takeFront();
		strcpy( source, fileName );
		path_set_extension( source, ".bsp" );
		int size = 0;
		if ( FILE *f = fopen( source, "rb" ); f != nullptr ) {
			size = Q_filelength( f );
			fclose( f );
		}

		/* load the bsp file and print lump sizes */
		Sys_Printf( "%s\n", source );
		LoadBSPFile( source );
		//PrintBSPFileSizes();

		/* print sizes */
		Sys_Printf( "\n" );
		Sys_Printf( "          total         %9d\n", size );
		Sys_Printf( "                        %9d KB\n", size / 1024 );
		Sys_Printf( "                        %9d MB\n", size / ( 1024 * 1024 ) );

		Sys_Printf( "---------------------------------\n" );
	}

	return 0;
}


static void ExtrapolateTexcoords( const bspDrawVert_t& a,
                                  const bspDrawVert_t& b,
								  const bspDrawVert_t& c,
								  bspDrawVert_t& anew,
								  bspDrawVert_t& bnew,
								  bspDrawVert_t& cnew ){

	const Vector3 norm = vector3_cross( b.xyz - a.xyz, c.xyz - a.xyz );

	// assume:
	//   s = f(x, y, z)
	//   s(v + norm) = s(v) when n ortho xyz

	// s(v) = DotProduct(v, scoeffs) + scoeffs[3]

	// solve:
	//   scoeffs * (axyz, 1) == ast[0]
	//   scoeffs * (bxyz, 1) == bst[0]
	//   scoeffs * (cxyz, 1) == cst[0]
	//   scoeffs * (norm, 0) == 0
	// scoeffs * [axyz, 1 | bxyz, 1 | cxyz, 1 | norm, 0] = [ast[0], bst[0], cst[0], 0]
	Matrix4 solvematrix;
	solvematrix.x() = { a.xyz, 1 };
	solvematrix.y() = { b.xyz, 1 };
	solvematrix.z() = { c.xyz, 1 };
	solvematrix.t() = { norm, 0 };
	matrix4_transpose( solvematrix );

	const double md = matrix4_determinant( solvematrix );
	if ( md * md < 1e-10 ) {
		Sys_Printf( "Cannot invert some matrix, some texcoords aren't extrapolated!" );
		return;
	}

	matrix4_full_invert( solvematrix );

	Matrix4 stcoeffs( g_matrix4_identity );
	stcoeffs.x() = { a.st[0], b.st[0], c.st[0], 0 };
	stcoeffs.y() = { a.st[1], b.st[1], c.st[1], 0 };
	matrix4_premultiply_by_matrix4( stcoeffs, solvematrix );
	matrix4_transpose( stcoeffs );
	anew.st = matrix4_transformed_point( stcoeffs, anew.xyz ).vec2();
	bnew.st = matrix4_transformed_point( stcoeffs, bnew.xyz ).vec2();
	cnew.st = matrix4_transformed_point( stcoeffs, cnew.xyz ).vec2();
}

/*
   ScaleBSPMain()
   amaze and confuse your enemies with weird scaled maps!
 */

int ScaleBSPMain( Args& args ){
	
	/* return to sender */
	return 0;
}


/*
   ShiftBSPMain()
   shifts a map: for testing physics with huge coordinates
 */

int ShiftBSPMain( Args& args ){
	/* return to sender */
	return 0;
}


/*
   MergeBSPMain()
   merges two bsps
 */

int MergeBSPMain( Args& args ){
	Sys_Printf("I don't work :) - MergeBSPMain");
	return -1;
}


/*
   PseudoCompileBSP()
   a stripped down ProcessModels
 */
static void PseudoCompileBSP( bool need_tree ){
	
}

/*
   ConvertBSPMain()
   main argument processing function for bsp conversion
 */

int ConvertBSPMain( Args& args ){
	// int ( *convertFunc )( char * );
	// const game_t  *convertGame;
	// bool map_allowed, force_bsp, force_map;


	// /* set default */
	// convertFunc = ConvertBSPToMap;
	// convertGame = NULL;
	// map_allowed = false;
	// force_bsp = false;
	// force_map = false;

	// /* arg checking */
	// if ( args.empty() ) {
	// 	Sys_Printf( "Usage: q3map2 -convert [-format <ase|obj|map_bp|map|game name>] [-shadersasbitmap|-lightmapsastexcoord|-deluxemapsastexcoord] [-readbsp|-readmap [-meta|-patchmeta]] [-v] <mapname>\n" );
	// 	return 0;
	// }

	// /* process arguments */
	// const char *fileName = args.takeBack();
	// {
	// 	/* -format map|ase|... */
	// 	while ( args.takeArg( "-format" ) ) {
	// 		const char *fmt = args.takeNext();
	// 		if ( striEqual( fmt, "obj" ) ) {
	// 			convertFunc = ConvertBSPToOBJ;
	// 			map_allowed = false;
	// 		}
	// 		else if ( striEqual( fmt, "map_bp" ) ) {
	// 			convertFunc = ConvertBSPToMap_BP;
	// 			map_allowed = true;
	// 		}
	// 		else if ( striEqual( fmt, "map" ) ) {
	// 			convertFunc = ConvertBSPToMap;
	// 			map_allowed = true;
	// 		}
	// 		else
	// 		{
	// 			convertGame = GetGame( fmt );
	// 			map_allowed = false;
	// 			if ( convertGame == NULL ) {
	// 				Sys_Printf( "Unknown conversion format \"%s\". Defaulting to MAP.\n", fmt );
	// 			}
	// 		}
	// 	}
	// 	while ( args.takeArg( "-ne" ) ) {
	// 		normalEpsilon = atof( args.takeNext() );
	// 		Sys_Printf( "Normal epsilon set to %lf\n", normalEpsilon );
	// 	}
	// 	while ( args.takeArg( "-de" ) ) {
	// 		distanceEpsilon = atof( args.takeNext() );
	// 		Sys_Printf( "Distance epsilon set to %lf\n", distanceEpsilon );
	// 	}
	// 	while ( args.takeArg( "-shaderasbitmap", "-shadersasbitmap" ) ) {
	// 		shadersAsBitmap = true;
	// 	}
	// 	while ( args.takeArg( "-lightmapastexcoord", "-lightmapsastexcoord" ) ) {
	// 		lightmapsAsTexcoord = true;
	// 	}
	// 	while ( args.takeArg( "-deluxemapastexcoord", "-deluxemapsastexcoord" ) ) {
	// 		lightmapsAsTexcoord = true;
	// 		deluxemap = true;
	// 	}
	// 	while ( args.takeArg( "-readbsp" ) ) {
	// 		force_bsp = true;
	// 	}
	// 	while ( args.takeArg( "-readmap" ) ) {
	// 		force_map = true;
	// 	}
	// 	while ( args.takeArg( "-meta" ) ) {
	// 		meta = true;
	// 	}
	// 	while ( args.takeArg( "-patchmeta" ) ) {
	// 		meta = true;
	// 		patchMeta = true;
	// 	}
	// 	while ( args.takeArg( "-fast" ) ) {
	// 		fast = true;
	// 	}
	// }

	// LoadShaderInfo();

	// /* clean up map name */
	// strcpy( source, ExpandArg( fileName ) );

	// if ( !map_allowed && !force_map ) {
	// 	force_bsp = true;
	// }

	// if ( force_map || ( !force_bsp && path_extension_is( source, "map" ) && map_allowed ) ) {
	// 	if ( !map_allowed ) {
	// 		Sys_Warning( "the requested conversion should not be done from .map files. Compile a .bsp first.\n" );
	// 	}
	// 	path_set_extension( source, ".map" );
	// 	Sys_Printf( "Loading %s\n", source );
	// 	LoadMapFile( source, false, convertGame == NULL );
	// 	PseudoCompileBSP( convertGame != NULL );
	// }
	// else
	// {
	// 	path_set_extension( source, ".bsp" );
	// 	Sys_FPrintf( SYS_VRB, "Loading bsp file: \"%s\"\n", source );

	// 	// Load file into memory
	// 	MemBuffer file = LoadFile( source );

	// 	rbspHeader_t* header = file.data();


	// 	// Make sure magic matches the format we're trying to load
	// 	if (!force && memcmp(header->ident, "rBSP", 4)) {
	// 		Error("%s is not a %s file", source, g_game->bspIdent);
	// 	}

	// 	for( const game_t &game : g_games ) {
	// 		if( header->version == game.bspVersion ) {
	// 			g_game = &game;
	// 			Sys_FPrintf( SYS_VRB, "Detected game: %s\n", g_game->arg );
	// 			break;
	// 		}
	// 	}
	// 	Sys_FPrintf( SYS_VRB, "--- DecompileMap ---\n" );
	// 	g_game->load( header, source );
	// 	Sys_Printf("\n" );
	// }
	
	// /* bsp format convert? */
	// if ( convertGame != NULL ) {
	// 	/* set global game */
	// 	//g_game = convertGame;
	// 	/* write bsp */
	// 	path_set_extension( source, "_c.bsp" );
	// 	WriteBSPFile( source );

	// 	/* return to sender */
	// 	return 0;
	// }

	// /* normal convert */
	// return convertFunc( source );
	return 0;
}
