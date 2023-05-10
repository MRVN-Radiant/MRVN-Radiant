/* -------------------------------------------------------------------------------

   Copyright (C) 2022-2023 MRVN-Radiant and contributors.
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
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ------------------------------------------------------------------------------- */

#pragma once

#include "../titanfall/titanfall.h"
#include "qmath.h"
#include <cstdint>
#include "../remap.h"
#include "../lump_names.h"


void WriteR2EntFiles(const char* filename);

void LoadR2BSPFile( rbspHeader_t *header, const char *filename );
void WriteR2BSPFile(const char* filename);
void CompileR2BSPFile();


/* -------------------------------------------------------------------------------

   abstracted bsp globals

   ------------------------------------------------------------------------------- */


namespace Titanfall2 {
    void SetUpGameLump();
    void EmitStaticProp(entity_t &e);
    void EmitLevelInfo();

    void EmitEntity(const entity_t &e);
    void EmitStubs();

    // 0x23
    // GameLump is the only lump which isn't just a vector
    // The order in which these structs were defined is the order in which both our and the respawn compiler saves them
    struct GameLumpHeader_t {
        uint32_t  version;   // Always 1
        char      ident[4];  // "prps"
        uint32_t  gameConst; // r1: 786432; r2: 851968
        uint32_t  offset;
        uint32_t  length;
    };

    struct GameLumpPathHeader_t {
        uint32_t numPaths;
    };

    struct GameLumpPropHeader_t {
        uint32_t numProps;
        uint32_t unk0;
        uint32_t unk1;
    };

    struct GameLumpProp_t {
        Vector3 origin;
        Vector3 angles;
        float   scale;
        int16_t model_name;
        int8_t  unk0;
        int8_t  unk1;
        int8_t  unk2;
        int8_t  unk3;
        int8_t  unk4;
        int8_t  unk5;
        float   fade_scale;
        Vector3 unk; // lighting_origin ?
        int8_t  cpu_level[2]; // -1, -1
        int8_t  gpu_level[2]; // -1, -1
        int8_t  diffuse_modulation[4]; // RGBA, ?
        int16_t collision_flags[2]; // Always 0, 0??
    };

    struct GameLumpUnknownHeader_t {
        uint32_t unknown;
    };

    namespace Bsp {
        inline GameLumpHeader_t                   gameLumpHeader;
        inline GameLumpPathHeader_t               gameLumpPathHeader;
        inline std::vector<Titanfall::GameLumpPath_t>        gameLumpPaths;
        inline GameLumpPropHeader_t               gameLumpPropHeader;
        inline std::vector<GameLumpProp_t>        gameLumpProps;
        inline GameLumpUnknownHeader_t            gameLumpUnknownHeader;
        /* Stubs */
        inline std::vector<uint8_t>  worldLights_stub;
        inline std::vector<uint8_t>  lightMapHeaders_stub;
        inline std::vector<uint8_t>  lightMapDataSky_stub;
    }
}
