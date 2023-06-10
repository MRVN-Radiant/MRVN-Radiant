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

#include "remap.h"


/*
    Stores things supported bsp versions share

    If it starts with "bsp" it gets saved in a .bsp
    If it starts with "ent" it gets saved in a .ent
    The rest are just helpers and dont get saved
*/
namespace Shared {
    /* Structs */
    struct Vertex_t {
        Vector3  xyz;
        Vector3  normal;
        Vector2  st;
    };

    struct Mesh_t {
        MinMax                 minmax;
        shaderInfo_t          *shaderInfo;
        std::vector<Vertex_t>  vertices;
        std::vector<uint16_t>  triangles;
    };

    struct visRef_t {
        MinMax    minmax;
        uint16_t  index;  // index to mesh / model
    };

    struct visNode_t {
        MinMax                  minmax;
        std::vector<visNode_t>  children;
        std::vector<visRef_t>   refs;
    };

    /* Vectors */
    inline std::vector<Mesh_t>    meshes;
    inline std::vector<visRef_t>  visRefs;
    inline visNode_t              visRoot;
    /* Functions */
    void MakeMeshes(const entity_t &e);
    void MakeVisReferences();
    visNode_t MakeVisTree(std::vector<Shared::visRef_t> refs, float parentCost);
    void MergeVisTree(Shared::visNode_t &node);
}
