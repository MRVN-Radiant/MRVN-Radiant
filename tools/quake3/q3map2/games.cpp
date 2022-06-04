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

   ------------------------------------------------------------------------------- */

#include "games.h"
#include "bspfile_ibsp.h"
#include "bspfile_rbsp.h"
#include "bspfile_r2bsp.h"
#include "bspfile_r5bsp.h"
#include "qstringops.h"
#include "inout.h"


/* titanfall2 */
struct game_titanfall2 : game_t
{
	/* thanks to the gracious fellows at raven */
	static const int S_CONT_SOLID = 0x00000001;  /* Default setting. An eye is never valid in a solid */
	static const int S_CONT_LAVA = 0x00000002;
	static const int S_CONT_WATER = 0x00000004;
	static const int S_CONT_FOG = 0x00000008;
	static const int S_CONT_PLAYERCLIP = 0x00000010;
	static const int S_CONT_MONSTERCLIP = 0x00000020;
	static const int S_CONT_BOTCLIP = 0x00000040;
	static const int S_CONT_SHOTCLIP = 0x00000080;
	static const int S_CONT_BODY = 0x00000100;  /* should never be on a brush, only in game */
	static const int S_CONT_CORPSE = 0x00000200;  /* should never be on a brush, only in game */
	static const int S_CONT_TRIGGER = 0x00000400;
	static const int S_CONT_NODROP = 0x00000800;  /* don't leave bodies or items (death fog, lava) */
	static const int S_CONT_TERRAIN = 0x00001000;  /* volume contains terrain data */
	static const int S_CONT_LADDER = 0x00002000;
	static const int S_CONT_ABSEIL = 0x00004000;  /* used like ladder to define where an NPC can abseil */
	static const int S_CONT_OPAQUE = 0x00008000;  /* defaults to on, when off, solid can be seen through */
	static const int S_CONT_OUTSIDE = 0x00010000;  /* volume is considered to be in the outside (i.e. not indoors) */
	static const int S_CONT_SLIME = 0x00020000;  /* don't be fooled. it may SAY "slime" but it really means "projectileclip" */
	static const int S_CONT_LIGHTSABER = 0x00040000;
	static const int S_CONT_TELEPORTER = 0x00080000;
	static const int S_CONT_ITEM = 0x00100000;
	static const int S_CONT_DETAIL = 0x08000000;  /* brushes not used for the bsp */
	static const int S_CONT_TRANSLUCENT = 0x80000000;  /* don't consume surface fragments inside */

	static const int S_SURF_SKY = 0x00002000;  /* lighting from environment map */
	static const int S_SURF_SLICK = 0x00004000;  /* affects game physics */
	static const int S_SURF_METALSTEPS = 0x00008000;  /* chc needs this since we use same tools */
	static const int S_SURF_FORCEFIELD = 0x00010000;  /* chc */
	static const int S_SURF_NODAMAGE = 0x00040000;  /* never give falling damage */
	static const int S_SURF_NOIMPACT = 0x00080000;  /* don't make missile explosions */
	static const int S_SURF_NOMARKS = 0x00100000;  /* don't leave missile marks */
	static const int S_SURF_NODRAW = 0x00200000;  /* don't generate a drawsurface at all */
	static const int S_SURF_NOSTEPS = 0x00400000;  /* no footstep sounds */
	static const int S_SURF_NODLIGHT = 0x00800000;  /* don't dlight even if solid (solid lava, skies) */
	static const int S_SURF_NOMISCENTS = 0x01000000;  /* no client models allowed on this surface */

	static const int S_SURF_PATCH = 0x80000000;  /* mark this face as a patch(editor only) */

	static const int S_SURF_BEVELSMASK = (S_SURF_SLICK | S_SURF_METALSTEPS | S_SURF_NODAMAGE | S_SURF_NOSTEPS | S_SURF_NOMISCENTS); /* compiler utility */

	/* materials */
	static const int S_MAT_BITS = 5;
	static const int S_MAT_MASK = 0x1f;        /* mask to get the material type */

	static const int S_MAT_NONE = 0;           /* for when the artist hasn't set anything up =) */
	static const int S_MAT_SOLIDWOOD = 1;           /* freshly cut timber */
	static const int S_MAT_HOLLOWWOOD = 2;           /* termite infested creaky wood */
	static const int S_MAT_SOLIDMETAL = 3;           /* solid girders */
	static const int S_MAT_HOLLOWMETAL = 4;           /* hollow metal machines */
	static const int S_MAT_SHORTGRASS = 5;           /* manicured lawn */
	static const int S_MAT_LONGGRASS = 6;           /* long jungle grass */
	static const int S_MAT_DIRT = 7;           /* hard mud */
	static const int S_MAT_SAND = 8;           /* sandy beach */
	static const int S_MAT_GRAVEL = 9;           /* lots of small stones */
	static const int S_MAT_GLASS = 10;
	static const int S_MAT_CONCRETE = 11;          /* hardened concrete pavement */
	static const int S_MAT_MARBLE = 12;          /* marble floors */
	static const int S_MAT_WATER = 13;          /* light covering of water on a surface */
	static const int S_MAT_SNOW = 14;          /* freshly laid snow */
	static const int S_MAT_ICE = 15;          /* packed snow/solid ice */
	static const int S_MAT_FLESH = 16;          /* hung meat, corpses in the world */
	static const int S_MAT_MUD = 17;          /* wet soil */
	static const int S_MAT_BPGLASS = 18;          /* bulletproof glass */
	static const int S_MAT_DRYLEAVES = 19;          /* dried up leaves on the floor */
	static const int S_MAT_GREENLEAVES = 20;          /* fresh leaves still on a tree */
	static const int S_MAT_FABRIC = 21;          /* Cotton sheets */
	static const int S_MAT_CANVAS = 22;          /* tent material */
	static const int S_MAT_ROCK = 23;
	static const int S_MAT_RUBBER = 24;          /* hard tire like rubber */
	static const int S_MAT_PLASTIC = 25;
	static const int S_MAT_TILES = 26;          /* tiled floor */
	static const int S_MAT_CARPET = 27;          /* lush carpet */
	static const int S_MAT_PLASTER = 28;          /* drywall style plaster */
	static const int S_MAT_SHATTERGLASS = 29;          /* glass with the Crisis Zone style shattering */
	static const int S_MAT_ARMOR = 30;          /* body armor */
	static const int S_MAT_COMPUTER = 31;          /* computers/electronic equipment */
	static const int S_MAT_LAST = 32;          /* number of materials */

	/* most of this is copied ower, just want to et vis tree injected first */
	game_titanfall2() : game_t
	{
		"titanfall2",                 /* -game x */
		"assets",                 /* default base game data dir */
		".assets",                /* unix home sub-dir */
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
		false,                  /* compile deluxemaps */
		0,                      /* deluxemaps default mode */
		512,                    /* minimap size */
		1.0f,                   /* minimap sharpener */
		0.0f,                   /* minimap border */
		true,                   /* minimap keep aspect */
		EMiniMapMode::Gray,     /* minimap mode */
		"%s.tga",               /* minimap name format */
		"rBSP",                 /* bsp file prefix */
		37,                      /* bsp file version */
		false,                  /* cod-style lump len/ofs order */
		LoadR2BSPFile,           /* bsp load function */
		WriteR2BSPFile,          /* bsp write function */
		CompileR2BSPFile,
		{
			/* name             contentFlags                contentFlagsClear           surfaceFlags                surfaceFlagsClear           compileFlags                compileFlagsClear */

			/* default */
			{ "default",        S_CONT_SOLID | S_CONT_OPAQUE,   -1,                     0,                          -1,                         C_SOLID,                    -1 },


			/* ydnar */
			{ "lightgrid",      0,                          0,                          0,                          0,                          C_LIGHTGRID,                0 },
			{ "antiportal",     0,                          0,                          0,                          0,                          C_ANTIPORTAL,               0 },
			{ "skip",           0,                          0,                          0,                          0,                          C_SKIP,                     0 },


			/* compiler */
			{ "origin",         0,                          S_CONT_SOLID,               0,                          0,                          C_ORIGIN | C_TRANSLUCENT,   C_SOLID },
			{ "areaportal",     S_CONT_TRANSLUCENT,         S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_AREAPORTAL | C_TRANSLUCENT,   C_SOLID },
			{ "trans",          S_CONT_TRANSLUCENT,         0,                          0,                          0,                          C_TRANSLUCENT,              0 },
			{ "detail",         S_CONT_DETAIL,              0,                          0,                          0,                          C_DETAIL,                   0 },
			{ "structural",     0,                          0,                          0,                          0,                          C_STRUCTURAL,               0 },
			{ "hint",           0,                          0,                          0,                          0,                          C_HINT,                     0 },
			{ "nodraw",         0,                          0,                          S_SURF_NODRAW,              0,                          C_NODRAW,                   0 },

			{ "alphashadow",    0,                          0,                          0,                          0,                          C_ALPHASHADOW | C_TRANSLUCENT,  0 },
			{ "lightfilter",    0,                          0,                          0,                          0,                          C_LIGHTFILTER | C_TRANSLUCENT,  0 },
			{ "nolightmap",     0,                          0,                          0,                          0,                          C_VERTEXLIT,                0 },
			{ "pointlight",     0,                          0,                          0,                          0,                          C_VERTEXLIT,                0 },


			/* game */
			{ "nonsolid",       0,                          S_CONT_SOLID,               0,                          0,                          0,                          C_SOLID },
			{ "nonopaque",      0,                          S_CONT_OPAQUE,              0,                          0,                          C_TRANSLUCENT,              0 },        /* setting trans ok? */

			{ "trigger",        S_CONT_TRIGGER,             S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_TRANSLUCENT,              C_SOLID },

			{ "water",          S_CONT_WATER,               S_CONT_SOLID,               0,                          0,                          C_LIQUID | C_TRANSLUCENT,   C_SOLID },
			{ "slime",          S_CONT_SLIME,               S_CONT_SOLID,               0,                          0,                          C_LIQUID | C_TRANSLUCENT,   C_SOLID },
			{ "lava",           S_CONT_LAVA,                S_CONT_SOLID,               0,                          0,                          C_LIQUID | C_TRANSLUCENT,   C_SOLID },

			{ "shotclip",       S_CONT_SHOTCLIP,            S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },  /* setting trans/detail ok? */
			{ "playerclip",     S_CONT_PLAYERCLIP,          S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
			{ "monsterclip",    S_CONT_MONSTERCLIP,         S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
			{ "nodrop",         S_CONT_NODROP,              S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },

			{ "terrain",        S_CONT_TERRAIN,             S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
			{ "ladder",         S_CONT_LADDER,              S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
			{ "abseil",         S_CONT_ABSEIL,              S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
			{ "outside",        S_CONT_OUTSIDE,             S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },

			{ "botclip",        S_CONT_BOTCLIP,             S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },

			{ "fog",            S_CONT_FOG,                 S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_FOG | C_DETAIL | C_TRANSLUCENT,   C_SOLID },  /* nonopaque? */
			{ "sky",            0,                          0,                          S_SURF_SKY,                 0,                          C_SKY,                      0 },

			{ "slick",          0,                          0,                          S_SURF_SLICK,               0,                          0,                          0 },

			{ "noimpact",       0,                          0,                          S_SURF_NOIMPACT,            0,                          0,                          0 },
			{ "nomarks",        0,                          0,                          S_SURF_NOMARKS,             0,                          C_NOMARKS,                  0 },
			{ "nodamage",       0,                          0,                          S_SURF_NODAMAGE,            0,                          0,                          0 },
			{ "metalsteps",     0,                          0,                          S_SURF_METALSTEPS,          0,                          0,                          0 },
			{ "nosteps",        0,                          0,                          S_SURF_NOSTEPS,             0,                          0,                          0 },
			{ "nodlight",       0,                          0,                          S_SURF_NODLIGHT,            0,                          0,                          0 },
			{ "nomiscents",     0,                          0,                          S_SURF_NOMISCENTS,          0,                          0,                          0 },
			{ "forcefield",     0,                          0,                          S_SURF_FORCEFIELD,          0,                          0,                          0 },


			/* materials */
			{ "*mat_none",      0,                          0,                          S_MAT_NONE,                 S_MAT_MASK,                 0,                          0 },
			{ "*mat_solidwood", 0,                          0,                          S_MAT_SOLIDWOOD,            S_MAT_MASK,                 0,                          0 },
			{ "*mat_hollowwood",    0,                      0,                          S_MAT_HOLLOWWOOD,           S_MAT_MASK,                 0,                          0 },
			{ "*mat_solidmetal",    0,                      0,                          S_MAT_SOLIDMETAL,           S_MAT_MASK,                 0,                          0 },
			{ "*mat_hollowmetal",   0,                      0,                          S_MAT_HOLLOWMETAL,          S_MAT_MASK,                 0,                          0 },
			{ "*mat_shortgrass",    0,                      0,                          S_MAT_SHORTGRASS,           S_MAT_MASK,                 0,                          0 },
			{ "*mat_longgrass",     0,                      0,                          S_MAT_LONGGRASS,            S_MAT_MASK,                 0,                          0 },
			{ "*mat_dirt",      0,                          0,                          S_MAT_DIRT,                 S_MAT_MASK,                 0,                          0 },
			{ "*mat_sand",      0,                          0,                          S_MAT_SAND,                 S_MAT_MASK,                 0,                          0 },
			{ "*mat_gravel",    0,                          0,                          S_MAT_GRAVEL,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_glass",     0,                          0,                          S_MAT_GLASS,                S_MAT_MASK,                 0,                          0 },
			{ "*mat_concrete",  0,                          0,                          S_MAT_CONCRETE,             S_MAT_MASK,                 0,                          0 },
			{ "*mat_marble",    0,                          0,                          S_MAT_MARBLE,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_water",     0,                          0,                          S_MAT_WATER,                S_MAT_MASK,                 0,                          0 },
			{ "*mat_snow",      0,                          0,                          S_MAT_SNOW,                 S_MAT_MASK,                 0,                          0 },
			{ "*mat_ice",       0,                          0,                          S_MAT_ICE,                  S_MAT_MASK,                 0,                          0 },
			{ "*mat_flesh",     0,                          0,                          S_MAT_FLESH,                S_MAT_MASK,                 0,                          0 },
			{ "*mat_mud",       0,                          0,                          S_MAT_MUD,                  S_MAT_MASK,                 0,                          0 },
			{ "*mat_bpglass",   0,                          0,                          S_MAT_BPGLASS,              S_MAT_MASK,                 0,                          0 },
			{ "*mat_dryleaves", 0,                          0,                          S_MAT_DRYLEAVES,            S_MAT_MASK,                 0,                          0 },
			{ "*mat_greenleaves",   0,                      0,                          S_MAT_GREENLEAVES,          S_MAT_MASK,                 0,                          0 },
			{ "*mat_fabric",    0,                          0,                          S_MAT_FABRIC,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_canvas",    0,                          0,                          S_MAT_CANVAS,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_rock",      0,                          0,                          S_MAT_ROCK,                 S_MAT_MASK,                 0,                          0 },
			{ "*mat_rubber",    0,                          0,                          S_MAT_RUBBER,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_plastic",   0,                          0,                          S_MAT_PLASTIC,              S_MAT_MASK,                 0,                          0 },
			{ "*mat_tiles",     0,                          0,                          S_MAT_TILES,                S_MAT_MASK,                 0,                          0 },
			{ "*mat_carpet",    0,                          0,                          S_MAT_CARPET,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_plaster",   0,                          0,                          S_MAT_PLASTER,              S_MAT_MASK,                 0,                          0 },
			{ "*mat_shatterglass",  0,                      0,                          S_MAT_SHATTERGLASS,         S_MAT_MASK,                 0,                          0 },
			{ "*mat_armor",     0,                          0,                          S_MAT_ARMOR,                S_MAT_MASK,                 0,                          0 },
			{ "*mat_computer",  0,                          0,                          S_MAT_COMPUTER,             S_MAT_MASK,                 0,                          0 },
		},
		S_SURF_BEVELSMASK
	}{}
};

/* apex legends */
struct game_apexlegends : game_t
{
	/* thanks to the gracious fellows at raven */
	static const int S_CONT_SOLID = 0x00000001;  /* Default setting. An eye is never valid in a solid */
	static const int S_CONT_LAVA = 0x00000002;
	static const int S_CONT_WATER = 0x00000004;
	static const int S_CONT_FOG = 0x00000008;
	static const int S_CONT_PLAYERCLIP = 0x00000010;
	static const int S_CONT_MONSTERCLIP = 0x00000020;
	static const int S_CONT_BOTCLIP = 0x00000040;
	static const int S_CONT_SHOTCLIP = 0x00000080;
	static const int S_CONT_BODY = 0x00000100;  /* should never be on a brush, only in game */
	static const int S_CONT_CORPSE = 0x00000200;  /* should never be on a brush, only in game */
	static const int S_CONT_TRIGGER = 0x00000400;
	static const int S_CONT_NODROP = 0x00000800;  /* don't leave bodies or items (death fog, lava) */
	static const int S_CONT_TERRAIN = 0x00001000;  /* volume contains terrain data */
	static const int S_CONT_LADDER = 0x00002000;
	static const int S_CONT_ABSEIL = 0x00004000;  /* used like ladder to define where an NPC can abseil */
	static const int S_CONT_OPAQUE = 0x00008000;  /* defaults to on, when off, solid can be seen through */
	static const int S_CONT_OUTSIDE = 0x00010000;  /* volume is considered to be in the outside (i.e. not indoors) */
	static const int S_CONT_SLIME = 0x00020000;  /* don't be fooled. it may SAY "slime" but it really means "projectileclip" */
	static const int S_CONT_LIGHTSABER = 0x00040000;
	static const int S_CONT_TELEPORTER = 0x00080000;
	static const int S_CONT_ITEM = 0x00100000;
	static const int S_CONT_DETAIL = 0x08000000;  /* brushes not used for the bsp */
	static const int S_CONT_TRANSLUCENT = 0x80000000;  /* don't consume surface fragments inside */

	static const int S_SURF_SKY = 0x00002000;  /* lighting from environment map */
	static const int S_SURF_SLICK = 0x00004000;  /* affects game physics */
	static const int S_SURF_METALSTEPS = 0x00008000;  /* chc needs this since we use same tools */
	static const int S_SURF_FORCEFIELD = 0x00010000;  /* chc */
	static const int S_SURF_NODAMAGE = 0x00040000;  /* never give falling damage */
	static const int S_SURF_NOIMPACT = 0x00080000;  /* don't make missile explosions */
	static const int S_SURF_NOMARKS = 0x00100000;  /* don't leave missile marks */
	static const int S_SURF_NODRAW = 0x00200000;  /* don't generate a drawsurface at all */
	static const int S_SURF_NOSTEPS = 0x00400000;  /* no footstep sounds */
	static const int S_SURF_NODLIGHT = 0x00800000;  /* don't dlight even if solid (solid lava, skies) */
	static const int S_SURF_NOMISCENTS = 0x01000000;  /* no client models allowed on this surface */

	static const int S_SURF_PATCH = 0x80000000;  /* mark this face as a patch(editor only) */

	static const int S_SURF_BEVELSMASK = (S_SURF_SLICK | S_SURF_METALSTEPS | S_SURF_NODAMAGE | S_SURF_NOSTEPS | S_SURF_NOMISCENTS); /* compiler utility */

	/* materials */
	static const int S_MAT_BITS = 5;
	static const int S_MAT_MASK = 0x1f;        /* mask to get the material type */

	static const int S_MAT_NONE = 0;           /* for when the artist hasn't set anything up =) */
	static const int S_MAT_SOLIDWOOD = 1;           /* freshly cut timber */
	static const int S_MAT_HOLLOWWOOD = 2;           /* termite infested creaky wood */
	static const int S_MAT_SOLIDMETAL = 3;           /* solid girders */
	static const int S_MAT_HOLLOWMETAL = 4;           /* hollow metal machines */
	static const int S_MAT_SHORTGRASS = 5;           /* manicured lawn */
	static const int S_MAT_LONGGRASS = 6;           /* long jungle grass */
	static const int S_MAT_DIRT = 7;           /* hard mud */
	static const int S_MAT_SAND = 8;           /* sandy beach */
	static const int S_MAT_GRAVEL = 9;           /* lots of small stones */
	static const int S_MAT_GLASS = 10;
	static const int S_MAT_CONCRETE = 11;          /* hardened concrete pavement */
	static const int S_MAT_MARBLE = 12;          /* marble floors */
	static const int S_MAT_WATER = 13;          /* light covering of water on a surface */
	static const int S_MAT_SNOW = 14;          /* freshly laid snow */
	static const int S_MAT_ICE = 15;          /* packed snow/solid ice */
	static const int S_MAT_FLESH = 16;          /* hung meat, corpses in the world */
	static const int S_MAT_MUD = 17;          /* wet soil */
	static const int S_MAT_BPGLASS = 18;          /* bulletproof glass */
	static const int S_MAT_DRYLEAVES = 19;          /* dried up leaves on the floor */
	static const int S_MAT_GREENLEAVES = 20;          /* fresh leaves still on a tree */
	static const int S_MAT_FABRIC = 21;          /* Cotton sheets */
	static const int S_MAT_CANVAS = 22;          /* tent material */
	static const int S_MAT_ROCK = 23;
	static const int S_MAT_RUBBER = 24;          /* hard tire like rubber */
	static const int S_MAT_PLASTIC = 25;
	static const int S_MAT_TILES = 26;          /* tiled floor */
	static const int S_MAT_CARPET = 27;          /* lush carpet */
	static const int S_MAT_PLASTER = 28;          /* drywall style plaster */
	static const int S_MAT_SHATTERGLASS = 29;          /* glass with the Crisis Zone style shattering */
	static const int S_MAT_ARMOR = 30;          /* body armor */
	static const int S_MAT_COMPUTER = 31;          /* computers/electronic equipment */
	static const int S_MAT_LAST = 32;          /* number of materials */

	/* most of this is copied ower, just want to et vis tree injected first */
	game_apexlegends() : game_t
	{
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
		false,                  /* compile deluxemaps */
		0,                      /* deluxemaps default mode */
		512,                    /* minimap size */
		1.0f,                   /* minimap sharpener */
		0.0f,                   /* minimap border */
		true,                   /* minimap keep aspect */
		EMiniMapMode::Gray,     /* minimap mode */
		"%s.tga",               /* minimap name format */
		"rBSP",                 /* bsp file prefix */
		37,                      /* bsp file version */
		false,                  /* cod-style lump len/ofs order */
		LoadR5BSPFile,           /* bsp load function */
		WriteR5BSPFile,          /* bsp write function */
		CompileR5BSPFile,
		{
			/* name             contentFlags                contentFlagsClear           surfaceFlags                surfaceFlagsClear           compileFlags                compileFlagsClear */

			/* default */
			{ "default",        S_CONT_SOLID | S_CONT_OPAQUE,   -1,                     0,                          -1,                         C_SOLID,                    -1 },


			/* ydnar */
			{ "lightgrid",      0,                          0,                          0,                          0,                          C_LIGHTGRID,                0 },
			{ "antiportal",     0,                          0,                          0,                          0,                          C_ANTIPORTAL,               0 },
			{ "skip",           0,                          0,                          0,                          0,                          C_SKIP,                     0 },


			/* compiler */
			{ "origin",         0,                          S_CONT_SOLID,               0,                          0,                          C_ORIGIN | C_TRANSLUCENT,   C_SOLID },
			{ "areaportal",     S_CONT_TRANSLUCENT,         S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_AREAPORTAL | C_TRANSLUCENT,   C_SOLID },
			{ "trans",          S_CONT_TRANSLUCENT,         0,                          0,                          0,                          C_TRANSLUCENT,              0 },
			{ "detail",         S_CONT_DETAIL,              0,                          0,                          0,                          C_DETAIL,                   0 },
			{ "structural",     0,                          0,                          0,                          0,                          C_STRUCTURAL,               0 },
			{ "hint",           0,                          0,                          0,                          0,                          C_HINT,                     0 },
			{ "nodraw",         0,                          0,                          S_SURF_NODRAW,              0,                          C_NODRAW,                   0 },

			{ "alphashadow",    0,                          0,                          0,                          0,                          C_ALPHASHADOW | C_TRANSLUCENT,  0 },
			{ "lightfilter",    0,                          0,                          0,                          0,                          C_LIGHTFILTER | C_TRANSLUCENT,  0 },
			{ "nolightmap",     0,                          0,                          0,                          0,                          C_VERTEXLIT,                0 },
			{ "pointlight",     0,                          0,                          0,                          0,                          C_VERTEXLIT,                0 },


			/* game */
			{ "nonsolid",       0,                          S_CONT_SOLID,               0,                          0,                          0,                          C_SOLID },
			{ "nonopaque",      0,                          S_CONT_OPAQUE,              0,                          0,                          C_TRANSLUCENT,              0 },        /* setting trans ok? */

			{ "trigger",        S_CONT_TRIGGER,             S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_TRANSLUCENT,              C_SOLID },

			{ "water",          S_CONT_WATER,               S_CONT_SOLID,               0,                          0,                          C_LIQUID | C_TRANSLUCENT,   C_SOLID },
			{ "slime",          S_CONT_SLIME,               S_CONT_SOLID,               0,                          0,                          C_LIQUID | C_TRANSLUCENT,   C_SOLID },
			{ "lava",           S_CONT_LAVA,                S_CONT_SOLID,               0,                          0,                          C_LIQUID | C_TRANSLUCENT,   C_SOLID },

			{ "shotclip",       S_CONT_SHOTCLIP,            S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },  /* setting trans/detail ok? */
			{ "playerclip",     S_CONT_PLAYERCLIP,          S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
			{ "monsterclip",    S_CONT_MONSTERCLIP,         S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
			{ "nodrop",         S_CONT_NODROP,              S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },

			{ "terrain",        S_CONT_TERRAIN,             S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
			{ "ladder",         S_CONT_LADDER,              S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
			{ "abseil",         S_CONT_ABSEIL,              S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
			{ "outside",        S_CONT_OUTSIDE,             S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },

			{ "botclip",        S_CONT_BOTCLIP,             S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },

			{ "fog",            S_CONT_FOG,                 S_CONT_SOLID | S_CONT_OPAQUE,   0,                      0,                          C_FOG | C_DETAIL | C_TRANSLUCENT,   C_SOLID },  /* nonopaque? */
			{ "sky",            0,                          0,                          S_SURF_SKY,                 0,                          C_SKY,                      0 },

			{ "slick",          0,                          0,                          S_SURF_SLICK,               0,                          0,                          0 },

			{ "noimpact",       0,                          0,                          S_SURF_NOIMPACT,            0,                          0,                          0 },
			{ "nomarks",        0,                          0,                          S_SURF_NOMARKS,             0,                          C_NOMARKS,                  0 },
			{ "nodamage",       0,                          0,                          S_SURF_NODAMAGE,            0,                          0,                          0 },
			{ "metalsteps",     0,                          0,                          S_SURF_METALSTEPS,          0,                          0,                          0 },
			{ "nosteps",        0,                          0,                          S_SURF_NOSTEPS,             0,                          0,                          0 },
			{ "nodlight",       0,                          0,                          S_SURF_NODLIGHT,            0,                          0,                          0 },
			{ "nomiscents",     0,                          0,                          S_SURF_NOMISCENTS,          0,                          0,                          0 },
			{ "forcefield",     0,                          0,                          S_SURF_FORCEFIELD,          0,                          0,                          0 },


			/* materials */
			{ "*mat_none",      0,                          0,                          S_MAT_NONE,                 S_MAT_MASK,                 0,                          0 },
			{ "*mat_solidwood", 0,                          0,                          S_MAT_SOLIDWOOD,            S_MAT_MASK,                 0,                          0 },
			{ "*mat_hollowwood",    0,                      0,                          S_MAT_HOLLOWWOOD,           S_MAT_MASK,                 0,                          0 },
			{ "*mat_solidmetal",    0,                      0,                          S_MAT_SOLIDMETAL,           S_MAT_MASK,                 0,                          0 },
			{ "*mat_hollowmetal",   0,                      0,                          S_MAT_HOLLOWMETAL,          S_MAT_MASK,                 0,                          0 },
			{ "*mat_shortgrass",    0,                      0,                          S_MAT_SHORTGRASS,           S_MAT_MASK,                 0,                          0 },
			{ "*mat_longgrass",     0,                      0,                          S_MAT_LONGGRASS,            S_MAT_MASK,                 0,                          0 },
			{ "*mat_dirt",      0,                          0,                          S_MAT_DIRT,                 S_MAT_MASK,                 0,                          0 },
			{ "*mat_sand",      0,                          0,                          S_MAT_SAND,                 S_MAT_MASK,                 0,                          0 },
			{ "*mat_gravel",    0,                          0,                          S_MAT_GRAVEL,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_glass",     0,                          0,                          S_MAT_GLASS,                S_MAT_MASK,                 0,                          0 },
			{ "*mat_concrete",  0,                          0,                          S_MAT_CONCRETE,             S_MAT_MASK,                 0,                          0 },
			{ "*mat_marble",    0,                          0,                          S_MAT_MARBLE,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_water",     0,                          0,                          S_MAT_WATER,                S_MAT_MASK,                 0,                          0 },
			{ "*mat_snow",      0,                          0,                          S_MAT_SNOW,                 S_MAT_MASK,                 0,                          0 },
			{ "*mat_ice",       0,                          0,                          S_MAT_ICE,                  S_MAT_MASK,                 0,                          0 },
			{ "*mat_flesh",     0,                          0,                          S_MAT_FLESH,                S_MAT_MASK,                 0,                          0 },
			{ "*mat_mud",       0,                          0,                          S_MAT_MUD,                  S_MAT_MASK,                 0,                          0 },
			{ "*mat_bpglass",   0,                          0,                          S_MAT_BPGLASS,              S_MAT_MASK,                 0,                          0 },
			{ "*mat_dryleaves", 0,                          0,                          S_MAT_DRYLEAVES,            S_MAT_MASK,                 0,                          0 },
			{ "*mat_greenleaves",   0,                      0,                          S_MAT_GREENLEAVES,          S_MAT_MASK,                 0,                          0 },
			{ "*mat_fabric",    0,                          0,                          S_MAT_FABRIC,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_canvas",    0,                          0,                          S_MAT_CANVAS,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_rock",      0,                          0,                          S_MAT_ROCK,                 S_MAT_MASK,                 0,                          0 },
			{ "*mat_rubber",    0,                          0,                          S_MAT_RUBBER,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_plastic",   0,                          0,                          S_MAT_PLASTIC,              S_MAT_MASK,                 0,                          0 },
			{ "*mat_tiles",     0,                          0,                          S_MAT_TILES,                S_MAT_MASK,                 0,                          0 },
			{ "*mat_carpet",    0,                          0,                          S_MAT_CARPET,               S_MAT_MASK,                 0,                          0 },
			{ "*mat_plaster",   0,                          0,                          S_MAT_PLASTER,              S_MAT_MASK,                 0,                          0 },
			{ "*mat_shatterglass",  0,                      0,                          S_MAT_SHATTERGLASS,         S_MAT_MASK,                 0,                          0 },
			{ "*mat_armor",     0,                          0,                          S_MAT_ARMOR,                S_MAT_MASK,                 0,                          0 },
			{ "*mat_computer",  0,                          0,                          S_MAT_COMPUTER,             S_MAT_MASK,                 0,                          0 },
		},
		S_SURF_BEVELSMASK
	}{}
};

const std::vector<game_t> g_games = { game_titanfall2(),
									  game_apexlegends()
									};
const game_t *g_game = &g_games[0];
