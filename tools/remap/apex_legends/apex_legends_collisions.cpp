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
    EmitBVHNode
    Placeholder
*/
void ApexLegends::EmitBVHNode() {
    ApexLegends::BVHNode_t &node = ApexLegends::Bsp::bvhNodes.emplace_back();
    node.cmIndex = EmitContentsMask( CONTENTS_SOLID );
    node.childType0 = 8;
    node.childType1 = BVH_CHILD_NONE;
    node.childType2 = BVH_CHILD_NONE;
    node.childType3 = BVH_CHILD_NONE;

    ApexLegends::EmitBVHDataleaf();
}

/*
    EmitBVHDataleaf
    Placeholder
*/
int ApexLegends::EmitBVHDataleaf() {
    int32_t &data = ApexLegends::Bsp::bvhLeafDatas.emplace_back();
    return ApexLegends::Bsp::bvhLeafDatas.size() - 1;
}

/*
    EmitContentsMask
    Emits collision flags and returns an index to them
*/
int ApexLegends::EmitContentsMask( int mask ) {
    for( int32_t i = 0; i < ApexLegends::Bsp::contentsMasks.size(); i++ ) {
        if( ApexLegends::Bsp::contentsMasks.at(i) == mask )
            return i;
    }

    // Didn't find our mask, make new one
    ApexLegends::Bsp::contentsMasks.emplace_back( mask );
    return ApexLegends::Bsp::contentsMasks.size() - 1;
}