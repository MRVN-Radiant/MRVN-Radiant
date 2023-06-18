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

   ----------------------------------------------------------------------------------

   "Q3Map2" has been significantly modified in the form of "remap" to support
   Respawn Entertainment games.

   ---------------------------------------------------------------------------------- */


#include "games.h"
#include "bspfile_abstract.h"
#include "qstringops.h"
#include "inout.h"

//------------------------------------------------------------
// Titanfall Online
struct game_titanfallonline : Game_t {
    game_titanfallonline() : Game_t {
        "titanfallonline",      // -game x
        "shaders",              // shader directory
        "rBSP",                 // bsp file header magic
        29,                     // bsp file version
        WriteR1BSPFile,         // bsp write function
        CompileR1BSPFile,       // bsp compile function
        // Shader Type
        {
            // name                     surfaceFlags, surfaceFlagsClear,    contentsFlags, contentsFlagsClear, compileFlags, compileFlagsClear
            {"default",               S_MESH_UNKNOWN,                -1,   CONTENTS_SOLID,                 -1,            0,                -1},
            {"UnlitGeneric",      MASK_UNLIT_GENERIC,                 0,                0,                  0,            0,                 0},
            {"LitFlatGeneric", MASK_LIT_FLAT_GENERIC,                 0,                0,                  0,            0,                 0},
            {"LitBumpGeneric", MASK_LIT_BUMP_GENERIC,                 0,                0,                  0,            0,                 0},
            {"UnlitTSGeneric", MASK_UNLIT_TS_GENERIC,                 0,                0,                  0,            0,                 0},
        },
        // Surface Flags
        {
            // name                 flags, flagsClear
            {"unlit",      S_VERTEX_UNLIT,          0},
            {"litflat", S_VERTEX_LIT_FLAT,          0},
            {"litbump", S_VERTEX_LIT_BUMP,          0},
            {"unlitts", S_VERTEX_UNLIT_TS,          0},
            {"sky2d",            S_SKY_2D,          0},
            {"sky",                 S_SKY,          0},
        },
        // Content Flags
        {
            // name                                            flags,         flagsClear
            {"nonsolid",                                           0,     CONTENTS_SOLID},
            {"solid",                                 CONTENTS_SOLID,                  0},
            {"window",                               CONTENTS_WINDOW,                  0},
            {"aux",                                     CONTENTS_AUX,                  0},
            {"grate",                                 CONTENTS_GRATE,                  0},
            {"slime",                                 CONTENTS_SLIME,                  0},
            {"water",                                 CONTENTS_WATER,                  0},
            {"windownocollide",            CONTENTS_WINDOW_NOCOLLIDE,     CONTENTS_SOLID},
            {"opaque",                               CONTENTS_OPAQUE,                  0},
            {"fogvolume",                     CONTENTS_TESTFOGVOLUME,                  0},
            {"physicsclip",                     CONTENTS_PHYSICSCLIP,     CONTENTS_SOLID},
            {"blocklight",                       CONTENTS_BLOCKLIGHT,                  0},
            {"nograpple",                         CONTENTS_NOGRAPPLE,                  0},
            {"ignorenodrawopaque",     CONTENTS_IGNORE_NODRAW_OPAQUE,                  0},
            {"moveable",                           CONTENTS_MOVEABLE,                  0},
            {"solidbody",              CONTENTS_TEST_SOLID_BODY_SHOT,                  0},
            {"playerclip",                       CONTENTS_PLAYERCLIP,     CONTENTS_SOLID},
            {"monsterclip",                     CONTENTS_MONSTERCLIP,     CONTENTS_SOLID},
            {"operatorfloorclip",            CONTENTS_OPERATOR_FLOOR,     CONTENTS_SOLID},
            {"blocklosclip",                       CONTENTS_BLOCKLOS,                  0},
            {"noclimb",                             CONTENTS_NOCLIMB,                  0},
            {"titanclip",                         CONTENTS_TITANCLIP,     CONTENTS_SOLID},
            {"bulletclip",                       CONTENTS_BULLETCLIP,     CONTENTS_SOLID},
            {"operatorclip",                   CONTENTS_OPERATORCLIP,     CONTENTS_SOLID},
            {"monster",                             CONTENTS_MONSTER,                  0},
            {"debris",                               CONTENTS_DEBRIS,                  0},
            {"detail",                               CONTENTS_DETAIL,                  0},
            {"trans",                           CONTENTS_TRANSLUCENT,                  0},
        },
        // Compile Flags
        {
            // name           flags, flagsClear
            {"nodraw",     C_NODRAW,          0},
            {"sky",           C_SKY,          0},
            {"decal",       C_DECAL,          0},
            {"trans", C_TRANSLUCENT,          0},
        }
    }{}
};

//------------------------------------------------------------
// Titanfall 2
struct game_titanfall2 : Game_t {
    game_titanfall2() : Game_t {
        "titanfall2",           // -game x
        "shaders",              // shader directory
        "rBSP",                 // bsp file header magic
        37,                     // bsp file version
        WriteR2BSPFile,         // bsp write function
        CompileR2BSPFile,       // bsp compile function
        // Shader Type
        {
            // name                     surfaceFlags, surfaceFlagsClear,    contentsFlags, contentsFlagsClear, compileFlags, compileFlagsClear
            {"default",               S_MESH_UNKNOWN,                -1,   CONTENTS_SOLID,                 -1,            0,                -1},
            {"UnlitGeneric",      MASK_UNLIT_GENERIC,                 0,                0,                  0,            0,                 0},
            {"LitFlatGeneric", MASK_LIT_FLAT_GENERIC,                 0,                0,                  0,            0,                 0},
            {"LitBumpGeneric", MASK_LIT_BUMP_GENERIC,                 0,                0,                  0,            0,                 0},
            {"UnlitTSGeneric", MASK_UNLIT_TS_GENERIC,                 0,                0,                  0,            0,                 0},
        },
        // Surface Flags
        {
            // name                 flags, flagsClear
            {"unlit",      S_VERTEX_UNLIT,          0},
            {"litflat", S_VERTEX_LIT_FLAT,          0},
            {"litbump", S_VERTEX_LIT_BUMP,          0},
            {"unlitts", S_VERTEX_UNLIT_TS,          0},
            {"sky2d",            S_SKY_2D,          0},
            {"sky",                 S_SKY,          0},
        },
        // Content Flags
        {
            // name                                            flags,         flagsClear
            {"nonsolid",                                           0,     CONTENTS_SOLID},
            {"solid",                                 CONTENTS_SOLID,                  0},
            {"window",                               CONTENTS_WINDOW,                  0},
            {"aux",                                     CONTENTS_AUX,                  0},
            {"grate",                                 CONTENTS_GRATE,                  0},
            {"slime",                                 CONTENTS_SLIME,                  0},
            {"water",                                 CONTENTS_WATER,                  0},
            {"windownocollide",            CONTENTS_WINDOW_NOCOLLIDE,     CONTENTS_SOLID},
            {"opaque",                               CONTENTS_OPAQUE,                  0},
            {"fogvolume",                     CONTENTS_TESTFOGVOLUME,                  0},
            {"physicsclip",                     CONTENTS_PHYSICSCLIP,     CONTENTS_SOLID},
            {"blocklight",                       CONTENTS_BLOCKLIGHT,                  0},
            {"nograpple",                         CONTENTS_NOGRAPPLE,                  0},
            {"ignorenodrawopaque",     CONTENTS_IGNORE_NODRAW_OPAQUE,                  0},
            {"moveable",                           CONTENTS_MOVEABLE,                  0},
            {"solidbody",              CONTENTS_TEST_SOLID_BODY_SHOT,                  0},
            {"playerclip",                       CONTENTS_PLAYERCLIP,     CONTENTS_SOLID},
            {"monsterclip",                     CONTENTS_MONSTERCLIP,     CONTENTS_SOLID},
            {"operatorfloorclip",            CONTENTS_OPERATOR_FLOOR,     CONTENTS_SOLID},
            {"blocklosclip",                       CONTENTS_BLOCKLOS,                  0},
            {"noclimb",                             CONTENTS_NOCLIMB,                  0},
            {"titanclip",                         CONTENTS_TITANCLIP,     CONTENTS_SOLID},
            {"bulletclip",                       CONTENTS_BULLETCLIP,     CONTENTS_SOLID},
            {"operatorclip",                   CONTENTS_OPERATORCLIP,     CONTENTS_SOLID},
            {"monster",                             CONTENTS_MONSTER,                  0},
            {"debris",                               CONTENTS_DEBRIS,                  0},
            {"detail",                               CONTENTS_DETAIL,                  0},
            {"trans",                           CONTENTS_TRANSLUCENT,                  0},
        },
        // Compile Flags
        {
            // name           flags, flagsClear
            {"nodraw",     C_NODRAW,          0},
            {"sky",           C_SKY,          0},
            {"decal",       C_DECAL,          0},
            {"trans", C_TRANSLUCENT,          0},
        }
    }{}
};

//------------------------------------------------------------
// Apex Legends
struct game_apexlegends : Game_t {
    game_apexlegends() : Game_t {
        "apexlegends",          // -game x
        "shaders",              // shader directory
        "rBSP",                 // bsp file header magic
        47,                     // bsp file version
        WriteR5BSPFile,         // bsp write function
        CompileR5BSPFile,       // bsp compile function
        // Shader Type
        {
            // name                     surfaceFlags, surfaceFlagsClear,    contentsFlags, contentsFlagsClear, compileFlags, compileFlagsClear
            {"default",               S_MESH_UNKNOWN,                -1,   CONTENTS_SOLID,                 -1,            0,                -1},
            {"UnlitGeneric",      MASK_UNLIT_GENERIC,                 0,                0,                  0,            0,                 0},
            {"LitFlatGeneric", MASK_LIT_FLAT_GENERIC,                 0,                0,                  0,            0,                 0},
            {"LitBumpGeneric", MASK_LIT_BUMP_GENERIC,                 0,                0,                  0,            0,                 0},
            {"UnlitTSGeneric", MASK_UNLIT_TS_GENERIC,                 0,                0,                  0,            0,                 0},
        },
        // Surface Flags
        {
            // name                 flags, flagsClear
            {"unlit",      S_VERTEX_UNLIT,          0},
            {"litflat", S_VERTEX_LIT_FLAT,          0},
            {"litbump", S_VERTEX_LIT_BUMP,          0},
            {"unlitts", S_VERTEX_UNLIT_TS,          0},
            {"sky2d",            S_SKY_2D,          0},
            {"sky",                 S_SKY,          0},
        },
        // Content Flags
        {
            // name                                            flags,         flagsClear
            {"nonsolid",                                           0,     CONTENTS_SOLID},
            {"solid",                                 CONTENTS_SOLID,                  0},
            {"window",                               CONTENTS_WINDOW,                  0},
            {"aux",                                     CONTENTS_AUX,                  0},
            {"grate",                                 CONTENTS_GRATE,                  0},
            {"slime",                                 CONTENTS_SLIME,                  0},
            {"water",                                 CONTENTS_WATER,                  0},
            {"windownocollide",            CONTENTS_WINDOW_NOCOLLIDE,     CONTENTS_SOLID},
            {"opaque",                               CONTENTS_OPAQUE,                  0},
            {"fogvolume",                     CONTENTS_TESTFOGVOLUME,                  0},
            {"physicsclip",                     CONTENTS_PHYSICSCLIP,     CONTENTS_SOLID},
            {"blocklight",                       CONTENTS_BLOCKLIGHT,                  0},
            {"nograpple",                         CONTENTS_NOGRAPPLE,                  0},
            {"ignorenodrawopaque",     CONTENTS_IGNORE_NODRAW_OPAQUE,                  0},
            {"moveable",                           CONTENTS_MOVEABLE,                  0},
            {"solidbody",              CONTENTS_TEST_SOLID_BODY_SHOT,                  0},
            {"playerclip",                       CONTENTS_PLAYERCLIP,     CONTENTS_SOLID},
            {"monsterclip",                     CONTENTS_MONSTERCLIP,     CONTENTS_SOLID},
            {"operatorfloorclip",            CONTENTS_OPERATOR_FLOOR,     CONTENTS_SOLID},
            {"blocklosclip",                       CONTENTS_BLOCKLOS,                  0},
            {"noclimb",                             CONTENTS_NOCLIMB,                  0},
            {"titanclip",                         CONTENTS_TITANCLIP,     CONTENTS_SOLID},
            {"bulletclip",                       CONTENTS_BULLETCLIP,     CONTENTS_SOLID},
            {"operatorclip",                   CONTENTS_OPERATORCLIP,     CONTENTS_SOLID},
            {"monster",                             CONTENTS_MONSTER,                  0},
            {"debris",                               CONTENTS_DEBRIS,                  0},
            {"detail",                               CONTENTS_DETAIL,                  0},
            {"trans",                           CONTENTS_TRANSLUCENT,                  0},
            {"soundtrigger",                   CONTENTS_SOUNDTRIGGER,                  0},
            {"occludesound",                   CONTENTS_OCCLUDESOUND,                  0},
            {"noairdrop",                         CONTENTS_NOAIRDROP,                  0},
            {"blockping",                        CONTENTS_BLOCK_PING,                  0},
        },
        // Compile Flags
        {
            // name           flags, flagsClear
            {"nodraw",     C_NODRAW,          0},
            {"sky",           C_SKY,          0},
            {"decal",       C_DECAL,          0},
            {"trans", C_TRANSLUCENT,          0},
        }
    }{}
};

//------------------------------------------------------------
// Globals
const vector<Game_t> g_games = { game_titanfallonline(),
                                 game_titanfall2(),
                                 game_apexlegends()
                               };
const Game_t *g_pGame = &g_games[0];

//------------------------------------------------------------
// Purpose: Gets pointer to game
// Input  : &arg - The name fo the game
//------------------------------------------------------------
const Game_t *GetGame( const char *arg ){
	// Dummy check
	if ( strEmptyOrNull( arg ) ) {
		return NULL;
	}

	// Joke
	if ( striEqual( arg, "quake1" ) ||
	     striEqual( arg, "quake2" ) ||
	     striEqual( arg, "unreal" ) ||
	     striEqual( arg, "ut2k3" ) ||
	     striEqual( arg, "dn3d" ) ||
	     striEqual( arg, "dnf" ) ||
	     striEqual( arg, "hl" ) ) {
		Sys_Printf( "April fools, silly rabbit!\n" );
		exit( 0 );
	}

	// Find the game
	for( const Game_t& game : g_games )
	{
		if ( striEqual( arg, game.arg ) )
			return &game;
	}

	/* no matching game */
	Sys_Warning( "Game \"%s\" is unknown.\n", arg );
	return NULL;
}