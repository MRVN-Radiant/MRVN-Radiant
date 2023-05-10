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

#include "../remap.h"
#include "../model.h"
#include "../bspfile_abstract.h"
#include "titanfall2.h"

/*
    SetUpGameLump()
*/
void Titanfall2::SetUpGameLump() {
    Titanfall2::Bsp::gameLumpHeader.version = 1;
    memcpy( &Titanfall2::Bsp::gameLumpHeader.ident, "prps", 4 );
    Titanfall2::Bsp::gameLumpHeader.gameConst = 851968;
}

/*
    EmitStaticProp()
    Emits a static prop
*/
void Titanfall2::EmitStaticProp(entity_t &e) {
    const char *model = e.valueForKey("model");
    int16_t pathIdx = -1;
    MinMax minmax;

    std::vector<const AssMeshWalker*> meshes = LoadModelWalker( model, 0 );
    if(meshes.empty()) {
        Sys_Warning("Failed to load model: %s\n", model);
        return;
    }

    Sys_Printf("Emitting static prop: %s\n", model);

    for(std::size_t i = 0; i < Titanfall::Bsp::gameLumpPaths.size(); i++) {
        Titanfall::GameLumpPath_t &path = Titanfall::Bsp::gameLumpPaths.at(i);

        if(!string_compare_nocase(path.path, model)) {
            pathIdx = i;
            break;
        }
    }

    if(pathIdx == -1) {
        Titanfall2::Bsp::gameLumpPathHeader.numPaths++;
        Titanfall::GameLumpPath_t &path = Titanfall::Bsp::gameLumpPaths.emplace_back();
        strncpy(path.path, model, 127);
        pathIdx = Titanfall::Bsp::gameLumpPaths.size() - 1;
    }

    Titanfall2::Bsp::gameLumpPropHeader.numProps++;
    Titanfall2::Bsp::gameLumpPropHeader.unk0++;
    Titanfall2::Bsp::gameLumpPropHeader.unk1++;
    Titanfall2::GameLumpProp_t &prop = Titanfall2::Bsp::gameLumpProps.emplace_back();
    Vector3 origin;
    Vector3 angles;
    if(!e.read_keyvalue(origin, "origin")) {
        origin.x() = 0.0f;
        origin.y() = 0.0f;
        origin.z() = 0.0f;
    }
    if(!e.read_keyvalue(angles, "angles")) {
        angles.x() = 0.0f;
        angles.y() = 0.0f;
        angles.z() = 0.0f;
    }

    prop.origin = origin;
    prop.angles = angles;
    prop.scale = 1.0f;
    prop.model_name = pathIdx;
    prop.fade_scale = -1.0f;
    prop.cpu_level[0] = -1.0f;
    prop.cpu_level[1] = -1.0f;
    prop.gpu_level[0] = -1.0f;
    prop.gpu_level[1] = -1.0f;
    prop.unk.x() = 1.0f;
    prop.unk.y() = 1.0f;
    prop.unk.z() = 1.0f;

    for ( const auto mesh : meshes )
    {
        mesh->forEachFace( [&minmax, &origin]( const Vector3 ( &xyz )[3], const Vector2 ( &st )[3] ){
			minmax.extend(xyz[0] + origin);
            minmax.extend(xyz[1] + origin);
            minmax.extend(xyz[2] + origin);
		} );
    }

    Shared::visRef_t &ref = Shared::visRefs.emplace_back();

    ref.minmax = minmax;
    ref.index = Titanfall::Bsp::models.at(0).meshCount + Titanfall2::Bsp::gameLumpProps.size() - 1;
}

/*
    EmitLevelInfo()
*/
void Titanfall2::EmitLevelInfo() {
    Titanfall::LevelInfo_t &li = Titanfall::Bsp::levelInfo.back();

    li.propCount = Titanfall2::Bsp::gameLumpPropHeader.numProps;
    // TODO: unk2 sun vector from last light_environment
    li.unk2 = { 0.1, 0.8, -0.6 };  // placeholder
}