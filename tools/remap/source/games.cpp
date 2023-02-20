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


struct game_titanfallonline : game_t {
    /* most of this is copied over, just want to get vis tree injected first */
    game_titanfallonline() : game_t {
        "titanfallonline",      /* -game x */
        "assets",               /* default base game data dir */
        ".assets",              /* unix home sub-dir */
        "soldier",              /* magic path word */
        "shaders",              /* shader directory */
        64,                     /* max lightmapped surface verts */
        999,                    /* max surface verts */
        6000,                   /* max surface indexes */
        true,                   /* flares */
        "gfx/misc/lens_flare",  /* default flare shader */
        false,                  /* wolf lighting model? */
        128,                    /* lightmap width/height */
        1.0f,                   /* lightmap gamma */
        false,                  /* lightmap sRGB */
        false,                  /* texture sRGB */
        false,                  /* color sRGB */
        0.0f,                   /* lightmap exposure */
        1.0f,                   /* lightmap compensate */
        1.0f,                   /* lightgrid scale */
        1.0f,                   /* lightgrid ambient scale */
        false,                  /* light angle attenuation uses half-lambert curve */
        false,                  /* disable shader lightstyles hack */
        false,                  /* keep light entities on bsp */
        8,                      /* default patchMeta subdivisions tolerance */
        false,                  /* patch casting enabled */
        "rBSP",                 /* bsp file prefix */
        29,                     /* bsp file version */
        LoadR1BSPFile,          /* bsp load function */
        WriteR1BSPFile,         /* bsp write function */
        CompileR1BSPFile,
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


/* titanfall2 */
struct game_titanfall2 : game_t {
    /* most of this is copied over, just want to get vis tree injected first */
    game_titanfall2() : game_t {
        "titanfall2",           /* -game x */
        "assets",               /* default base game data dir */
        ".assets",              /* unix home sub-dir */
        "soldier",              /* magic path word */
        "shaders",              /* shader directory */
        64,                     /* max lightmapped surface verts */
        999,                    /* max surface verts */
        6000,                   /* max surface indexes */
        true,                   /* flares */
        "gfx/misc/lens_flare",  /* default flare shader */
        false,                  /* wolf lighting model? */
        128,                    /* lightmap width/height */
        1.0f,                   /* lightmap gamma */
        false,                  /* lightmap sRGB */
        false,                  /* texture sRGB */
        false,                  /* color sRGB */
        0.0f,                   /* lightmap exposure */
        1.0f,                   /* lightmap compensate */
        1.0f,                   /* lightgrid scale */
        1.0f,                   /* lightgrid ambient scale */
        false,                  /* light angle attenuation uses half-lambert curve */
        false,                  /* disable shader lightstyles hack */
        false,                  /* keep light entities on bsp */
        8,                      /* default patchMeta subdivisions tolerance */
        false,                  /* patch casting enabled */
        "rBSP",                 /* bsp file prefix */
        37,                     /* bsp file version */
        LoadR2BSPFile,          /* bsp load function */
        WriteR2BSPFile,         /* bsp write function */
        CompileR2BSPFile,
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


/* apex legends */
struct game_apexlegends : game_t {
    /* most of this is copied over, just want to get vis tree injected first */
    game_apexlegends() : game_t {
        "apexlegends",          /* -game x */
        "assets",               /* default base game data dir */
        ".assets",              /* unix home sub-dir */
        "soldier",              /* magic path word */
        "shaders",              /* shader directory */
        64,                     /* max lightmapped surface verts */
        999,                    /* max surface verts */
        6000,                   /* max surface indexes */
        true,                   /* flares */
        "gfx/misc/lens_flare",  /* default flare shader */
        false,                  /* wolf lighting model? */
        128,                    /* lightmap width/height */
        1.0f,                   /* lightmap gamma */
        false,                  /* lightmap sRGB */
        false,                  /* texture sRGB */
        false,                  /* color sRGB */
        0.0f,                   /* lightmap exposure */
        1.0f,                   /* lightmap compensate */
        1.0f,                   /* lightgrid scale */
        1.0f,                   /* lightgrid ambient scale */
        false,                  /* light angle attenuation uses half-lambert curve */
        false,                  /* disable shader lightstyles hack */
        false,                  /* keep light entities on bsp */
        8,                      /* default patchMeta subdivisions tolerance */
        false,                  /* patch casting enabled */
        "rBSP",                 /* bsp file prefix */
        47,                     /* bsp file version */
        LoadR5BSPFile,          /* bsp load function */
        WriteR5BSPFile,         /* bsp write function */
        CompileR5BSPFile,
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


const std::vector<game_t> g_games = { game_titanfallonline(),
                                      game_titanfall2(),
                                      game_apexlegends()
                                    };
const game_t *g_game = &g_games[0];
