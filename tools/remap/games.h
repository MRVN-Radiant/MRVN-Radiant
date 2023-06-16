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
*/


/* marker */
#pragma once

#include <vector>


struct bspLump_t {
        int offset, length, lumpVer, padding;
};

struct rbspHeader_t {
    char  ident[4];    /* rBSP */
    int   version;     /* 37 for r2 */
    int   mapVersion;  /* 30 */
    int   maxLump;     /* 127 */

    bspLump_t  lumps[128];
};


/* ydnar: compiler flags, because games have widely varying content/surface flags */
const int C_SOLID                = 0x00000001;
const int C_TRANSLUCENT          = 0x00000002;
const int C_STRUCTURAL           = 0x00000004;
const int C_HINT                 = 0x00000008;
const int C_NODRAW               = 0x00000010;
const int C_LIGHTGRID            = 0x00000020;
const int C_ALPHASHADOW          = 0x00000040;
const int C_LIGHTFILTER          = 0x00000080;
const int C_VERTEXLIT            = 0x00000100;
const int C_LIQUID               = 0x00000200;
const int C_FOG                  = 0x00000400;
const int C_SKY                  = 0x00000800;
const int C_ORIGIN               = 0x00001000;
const int C_AREAPORTAL           = 0x00002000;
const int C_ANTIPORTAL           = 0x00004000;  /* like hint, but doesn't generate portals */
const int C_SKIP                 = 0x00008000;  /* like hint, but skips this face (doesn't split bsp) */
const int C_NOMARKS              = 0x00010000;  /* no decals */
const int C_OB                   = 0x00020000;  /* skip -noob for this */
const int C_DECAL                = 0x00040000;  /* Titanfall decal mesh */
const int C_DETAIL               = 0x08000000;  /* THIS MUST BE THE SAME AS IN RADIANT! */

// Surface flags
const int S_SKY_2D               = 0x00000002;
const int S_SKY                  = 0x00000004;
const int S_TRANSLUCENT          = 0x00000010;
// NOTE: VERTEX_RESERVED_X might be selected w/ an unknown in Mesh_t
const int S_VERTEX_LIT_FLAT      = 0x00000000;
const int S_VERTEX_LIT_BUMP      = 0x00000200;
const int S_VERTEX_UNLIT         = 0x00000400;
const int S_VERTEX_UNLIT_TS      = 0x00000600;
// const int S_VERTEX_BLINN_PHONG   = ?

const int S_MESH_UNKNOWN         = 0x00100000;

// Collision flags
const int CONTENTS_SOLID                = 0x00000001;  // Everything collides with me
const int CONTENTS_WINDOW               = 0x00000002;
const int CONTENTS_AUX                  = 0x00000004;
const int CONTENTS_GRATE                = 0x00000008;
const int CONTENTS_SLIME                = 0x00000010;
const int CONTENTS_WATER                = 0x00000020;
const int CONTENTS_WINDOW_NOCOLLIDE     = 0x00000040;
const int CONTENTS_OPAQUE               = 0x00000080;
const int CONTENTS_TESTFOGVOLUME        = 0x00000100;
const int CONTENTS_PHYSICSCLIP          = 0x00000200;	
const int CONTENTS_BLOCKLIGHT           = 0x00000400;
const int CONTENTS_NOGRAPPLE            = 0x00000800;
const int CONTENTS_UNUSED_03            = 0x00001000;
const int CONTENTS_IGNORE_NODRAW_OPAQUE = 0x00002000;
const int CONTENTS_MOVEABLE             = 0x00004000;
const int CONTENTS_TEST_SOLID_BODY_SHOT = 0x00008000;
const int CONTENTS_PLAYERCLIP           = 0x00010000;
const int CONTENTS_MONSTERCLIP          = 0x00020000;
const int CONTENTS_OPERATOR_FLOOR       = 0x00040000;
const int CONTENTS_BLOCKLOS             = 0x00080000;
const int CONTENTS_NOCLIMB              = 0x00100000;
const int CONTENTS_TITANCLIP            = 0x00200000;  // Titans collide with me ( TODO: Test reapers )
const int CONTENTS_BULLETCLIP           = 0x00400000;
const int CONTENTS_OPERATORCLIP         = 0x00800000;
const int CONTENTS_MONSTER              = 0x02000000;
const int CONTENTS_DEBRIS               = 0x04000000;
const int CONTENTS_DETAIL               = 0x08000000;
const int CONTENTS_TRANSLUCENT          = 0x10000000;
const int CONTENTS_HITBOX               = 0x40000000;  // Non-map entities have this

// New in apex
const int CONTENTS_SOUNDTRIGGER         = 0x00000400;
const int CONTENTS_OCCLUDESOUND         = 0x00001000;
const int CONTENTS_NOAIRDROP            = 0x01000000;
const int CONTENTS_BLOCK_PING           = 0x20000000;

const int MASK_UNLIT_GENERIC            = S_VERTEX_UNLIT;
const int MASK_LIT_FLAT_GENERIC         = S_VERTEX_LIT_FLAT;
const int MASK_LIT_BUMP_GENERIC         = S_VERTEX_LIT_BUMP;
const int MASK_UNLIT_TS_GENERIC         = S_VERTEX_UNLIT_TS;


struct ShaderType_t {
    const char *name;
    int surfaceFlags, surfaceFlagsClear;
    int contentFlags, contentFlagsClear;
    int compileFlags, compileFlagsClear;
};

struct ShaderFlag_t {
    const char* name;
    int flags, flagsClear;
};

/* ydnar: for multiple game support */
struct surfaceParm_t {
    const char *name;
    int  contentFlags, contentFlagsClear;
    int  surfaceFlags, surfaceFlagsClear;
    int  compileFlags, compileFlagsClear;
};

enum class EMiniMapMode {
    Gray,
    Black,
    White
};


struct game_t {
    const char     *arg;                           /* -game matches this */
    const char     *gamePath;                      /* main game data dir */
    const char     *homeBasePath;                  /* home sub-dir on unix */
    const char     *magic;                         /* magic word for figuring out base path */
    const char     *shaderPath;                    /* shader directory */
    int             maxLMSurfaceVerts;             /* default maximum lightmapped surface verts */
    int             maxSurfaceVerts;               /* default maximum surface verts */
    int             maxSurfaceIndexes;             /* default maximum surface indexes (tris * 3) */
    bool            emitFlares;                    /* when true, emit flare surfaces */
    const char     *flareShader;                   /* default flare shader (MUST BE SET) */
    bool            wolfLight;                     /* when true, lights work like wolf q3map  */
    int             lightmapSize;                  /* bsp lightmap width/height */
    float           lightmapGamma;                 /* default lightmap gamma */
    bool            lightmapsRGB;                  /* default lightmap sRGB mode */
    bool            texturesRGB;                   /* default texture sRGB mode */
    bool            colorsRGB;                     /* default color sRGB mode */
    float           lightmapExposure;              /* default lightmap exposure */
    float           lightmapCompensate;            /* default lightmap compensate value */
    float           gridScale;                     /* vortex: default lightgrid scale (affects both directional and ambient spectres) */
    float           gridAmbientScale;              /* vortex: default lightgrid ambient spectre scale */
    bool            lightAngleHL;                  /* jal: use half-lambert curve for light angle attenuation */
    bool            noStyles;                      /* use lightstyles hack or not */
    bool            keepLights;                    /* keep light entities on bsp */
    int             patchSubdivisions;             /* default patchMeta subdivisions tolerance */
    bool            patchShadows;                  /* patch casting enabled */
    const char     *bspIdent;                      /* 4-letter bsp file prefix */
    int             bspVersion;                    /* bsp version to use */
    typedef void    (*bspWriteFunc)(const char*);
    bspWriteFunc    write;
    typedef void    (*bspCompileFunc)();
    bspCompileFunc  compile;
    std::vector<ShaderType_t> shaderTypes;
    std::vector<ShaderFlag_t> surfaceFlags;
    std::vector<ShaderFlag_t> contentFlags;
    std::vector<ShaderFlag_t> compileFlags;
};


extern const std::vector<game_t>  g_games;
extern const game_t              *g_game;
