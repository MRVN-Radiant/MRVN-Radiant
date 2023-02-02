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
#include "../bspfile_abstract.h"
#include <ctime>

/*
    BeginModel
*/
void ApexLegends::BeginModel(entity_t &entity) {
    Sys_FPrintf( SYS_VRB, "   BeginModel: \"%s\"\n", entity.classname() );
    ApexLegends::Model_t &model = ApexLegends::Bsp::models.emplace_back();
    model.meshIndex = ApexLegends::Bsp::meshes.size();
    model.bvhNodeIndex = 0;
    model.bvhLeafIndex = 0;
    model.vertexIndex = Titanfall::Bsp::vertices.size();
    model.vertexFlags = 0;
}

/*
    EndModel
*/
void ApexLegends::EndModel() {
    ApexLegends::Model_t &model = ApexLegends::Bsp::models.back();
    model.meshCount = ApexLegends::Bsp::meshes.size() - model.meshIndex;

    for( Titanfall::MeshBounds_t &meshBounds : Titanfall::Bsp::meshBounds ) {
        model.minmax.extend(meshBounds.origin - meshBounds.extents);
        model.minmax.extend(meshBounds.origin + meshBounds.extents);
    }

    Sys_FPrintf( SYS_VRB, "       numMeshes: %i\n", model.meshCount );
    Sys_FPrintf( SYS_VRB, "   EndModel\n" );
}