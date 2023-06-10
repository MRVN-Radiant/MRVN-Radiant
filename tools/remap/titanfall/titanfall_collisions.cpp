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
#include "titanfall.h"

/*
    EmitCollisionGrid()
    Emits brushes of entity into bsp
*/
void Titanfall::EmitCollisionGrid( entity_t &e ) {
    std::vector<brush_t*> gridBrushes;
    std::vector<brush_t*> modelBrushes;

    MinMax gridSize;

    // Sort brushes
    for( brush_t &brush : e.brushes ) {
        int skySides = 0;

        for( const side_t &side : brush.sides ) {
            if( side.bevel )
                continue;

            if( CHECK_FLAG(side.shaderInfo->compileFlags,C_SKY) )
                skySides++;
        }

        if( skySides ) {
            modelBrushes.emplace_back( &brush );
            continue;
        }

        gridBrushes.emplace_back( &brush );
        gridSize.extend( brush.minmax );
    }

    //Sys_Warning( "Grid brushes: %li\n", gridBrushes.size() );
    //Sys_Warning( "Model brushes: %li\n", modelBrushes.size() );

    // Emit grid brushes
    for( brush_t *brush : gridBrushes )
        Titanfall::EmitBrush( *brush );


    // Worldspawn size
    Vector3 size = gridSize.maxs - gridSize.mins;

     

    // Choose scale
    // The limit seems to be 128x128, try to use size of 256 or higher
    float scale = 256;
    int x, y;
    x = ceil(size.x() / scale) + 2;
    y = ceil(size.y() / scale) + 2;
    while( x > 128 || y > 128 ) {
        scale += 16;
        x = ceil(size.x() / scale) + 2;
        y = ceil(size.y() / scale) + 2;
    }
    Sys_FPrintf( SYS_VRB, "       Grid scale is: %f\n", scale );

    Titanfall::CMGrid_t &grid = Titanfall::Bsp::cmGrid.emplace_back();
    grid.scale = scale;
    grid.xOffset = floor(gridSize.mins.x() / grid.scale) - 1;
    grid.yOffset = floor(gridSize.mins.y() / grid.scale) - 1;
    grid.xCount = ceil(size.x() / grid.scale) + 2;
    grid.yCount = ceil(size.y() / grid.scale) + 2;
    grid.straddleGroupCount = 0;
    grid.brushSidePlaneOffset = 0;

    // Make GridCells
    for (int32_t y = 0; y < grid.yCount; y++) {
        for (int32_t x = 0; x < grid.xCount; x++) {
            MinMax cellMinmax;
            cellMinmax.mins = Vector3((x + grid.xOffset) * grid.scale,
                                      (y + grid.yOffset) * grid.scale,
                                       gridSize.mins.z());
            cellMinmax.maxs = Vector3((x + grid.xOffset + 1) * grid.scale,
                                      (y + grid.yOffset + 1) * grid.scale,
                                       gridSize.maxs.z());

            // Sys_Printf("Worldspawn: %f %f %f : %f %f %f\n",
            //            Titanfall::Bsp::models[0].minmax.mins.x(),
            //            Titanfall::Bsp::models[0].minmax.mins.y(),
            //            Titanfall::Bsp::models[0].minmax.mins.z(),
            //            Titanfall::Bsp::models[0].minmax.maxs.x(),
            //            Titanfall::Bsp::models[0].minmax.maxs.y(),
            //            Titanfall::Bsp::models[0].minmax.maxs.z());
            // Sys_Printf("Cell      : %f %f %f : %f %f %f\n",
            //            cellMinmax.mins.x(), cellMinmax.mins.y(), cellMinmax.mins.z(),
            //            cellMinmax.maxs.x(), cellMinmax.maxs.y(), cellMinmax.maxs.z());

            Titanfall::CMGridCell_t &cell = Titanfall::Bsp::cmGridCells.emplace_back();
            cell.start = Titanfall::Bsp::cmGeoSets.size();

            // Check brushes
            for (uint32_t index = 0; index < Titanfall::Bsp::cmBrushes.size(); index++) {
                Titanfall::CMBrush_t &brush = Titanfall::Bsp::cmBrushes.at(index);

                MinMax brushMinmax;
                brushMinmax.mins = brush.origin - brush.extents;
                brushMinmax.maxs = brush.origin + brush.extents;

                // NOTE: Still doesnt work
                if (!cellMinmax.test(brushMinmax)) {
                    continue;
                }

                Titanfall::EmitGeoSet(brushMinmax, brush.index, gridBrushes.at(index)->contentFlags);
            }

            cell.count = Titanfall::Bsp::cmGeoSets.size() - cell.start;
        }
    }

    std::size_t numGeoSets, numPrimitives;
    numGeoSets = Titanfall::Bsp::cmGeoSets.size();
    numPrimitives = Titanfall::Bsp::cmPrimitives.size();
    Sys_FPrintf( SYS_VRB, "       Grid ( %i : %i ) has %li GeoSets and %li Primitives\n", grid.xCount, grid.yCount, numGeoSets, numPrimitives );

    // Emit model brushes
    for( brush_t *brush : modelBrushes )
        Titanfall::EmitBrush( *brush );

    Titanfall::CMGridCell_t &cell = Titanfall::Bsp::cmGridCells.emplace_back();
    cell.start = Titanfall::Bsp::cmGeoSets.size();

    for( std::size_t i = 0; i < modelBrushes.size(); i++ ) {
        Titanfall::CMBrush_t &brush = Titanfall::Bsp::cmBrushes.at(gridBrushes.size() + i);

        MinMax brushMinmax;
        brushMinmax.mins = brush.origin - brush.extents;
        brushMinmax.maxs = brush.origin + brush.extents;

        Titanfall::EmitGeoSet(brushMinmax, brush.index, modelBrushes.at(i)->contentFlags);
    }

    numGeoSets = Titanfall::Bsp::cmGeoSets.size() - numGeoSets;
    numPrimitives = Titanfall::Bsp::cmPrimitives.size() - numPrimitives;
    Sys_FPrintf( SYS_VRB, "       Worldspawn model has %li GeoSets and %li Primitives\n", numGeoSets, numPrimitives );

    cell.count = Titanfall::Bsp::cmGeoSets.size() - cell.start;
}

/*
    EmitModelGridCell()
    Emits brushes of entity into bsp
*/
void Titanfall::EmitModelGridCell( entity_t &e ) {
    std::size_t offset = Titanfall::Bsp::cmBrushes.size();

    for( brush_t &brush : e.brushes )
        Titanfall::EmitBrush( brush );

    Titanfall::CMGridCell_t &cell = Titanfall::Bsp::cmGridCells.emplace_back();
    cell.start = Titanfall::Bsp::cmGeoSets.size();

    std::size_t i = 0;
    for( brush_t &b : e.brushes ) {
        Titanfall::CMBrush_t &brush = Titanfall::Bsp::cmBrushes.at(offset + i);

        MinMax brushMinmax;
        brushMinmax.mins = brush.origin - brush.extents;
        brushMinmax.maxs = brush.origin + brush.extents;

        Titanfall::EmitGeoSet(brushMinmax, brush.index, b.contentFlags);

        i++;
    }

    cell.count = Titanfall::Bsp::cmGeoSets.size() - cell.start;
}

/*
    EmitGeoSet()
    Emits a geo set into bsp
*/
void Titanfall::EmitGeoSet(MinMax minmax, int index, int flags) {
    Titanfall::CMGeoSet_t &set = Titanfall::Bsp::cmGeoSets.emplace_back();
    set.straddleGroup = 0;
    set.primitiveCount = 1;
    set.uniqueContentsIndex = Titanfall::EmitUniqueContents(flags);
    set.collisionShapeIndex = index;

    Titanfall::CMBound_t &bound = Titanfall::Bsp::cmGeoSetBounds.emplace_back();
    bound.unknown1 = 128;
    bound.origin = minmax.origin();
    // The + 1.0f fixes the infinitely falling in one place while touching a floor bug
    bound.extents = Vector3(minmax.extents().x() + 2.0f, minmax.extents().y() + 2.0f, minmax.extents().z() + 2.0f);
}

/*
    EmitBrush()
    Emits a brush into bsp
*/
void Titanfall::EmitBrush(brush_t &brush) {
    // TODO: The core structs ( entity_t, brush_t parseMest_t, shaderInfo_t, etc... ) need to be rewritten
    // to avoid this
    for( side_t &side : brush.sides ) {
        if( !side.bevel )
            brush.contentFlags |= side.shaderInfo->contentFlags;
    }


    Titanfall::CMBrush_t &b = Titanfall::Bsp::cmBrushes.emplace_back();

    b.extents = brush.minmax.extents();
    b.origin = brush.minmax.origin();
    b.index = Titanfall::Bsp::cmBrushes.size() - 1;
    b.planeCount = 0;
    b.unknown = 0;

    std::vector<side_t>  axialSides;
    std::vector<side_t>  cuttingSides;
    // +X -X +Y -Y +Z -Z
    shaderInfo_t *axials[6];
    // The bsp brushes are AABBs + cutting planes
    // Surface flags are indexed first for AABB ( first 6 planes ) then for the rest
    // Radiant brushes are made purely of planes so we dont have a guarantee that we'll get the
    // Axial ones which define the AABB, that's why we first sort them
    for (const side_t &side : brush.sides) {
        Vector3 normal = side.plane.normal();
        SnapNormal(normal);
        if ((normal[0] == -1.0f || normal[0] == 1.0f || (normal[0] == 0.0f && normal[1] == 0.0f)
            || normal[1] == -1.0f || normal[1] == 1.0f || (normal[1] == 0.0f && normal[2] == 0.0f)
            || normal[2] == -1.0f || normal[2] == 1.0f || (normal[2] == 0.0f && normal[0] == 0.0f)) && !side.bevel) {
            // Axial
            axialSides.emplace_back(side);
        }
        // Not Axial
        cuttingSides.emplace_back(side);
    }

    for (const side_t &side : axialSides) {
        Vector3 normal = side.plane.normal();
        SnapNormal(normal);

        if (normal[0] == 1.0f) {
            axials[1] = side.shaderInfo;
        } else if (normal[0] == -1.0f) {
            axials[0] = side.shaderInfo;
        }

        if (normal[1] == 1.0f) {
            axials[3] = side.shaderInfo;
        } else if (normal[1] == -1.0f) {
            axials[2] = side.shaderInfo;
        }

        if (normal[2] == 1.0f) {
            axials[5] = side.shaderInfo;
        } else if (normal[2] == -1.0f) {
            axials[4] = side.shaderInfo;
        }
    }

    int test = 0;
    for (int i = 0; i < 6; i++) {
        if (axials[i] != nullptr) {
            //Titanfall::Bsp::cmBrushSideProperties.emplace_back(Titanfall::EmitTextureData(*axials[i]));
            Titanfall::Bsp::cmBrushSideProperties.emplace_back(0);
        } else {
            test++;
            Titanfall::Bsp::cmBrushSideProperties.emplace_back(MASK_DISCARD);
        }
    }

#if 1
    for (const side_t &side : cuttingSides) {
        Vector3 normal = side.plane.normal();
        SnapNormal(normal);
        if (normal[0] == -1.0f || normal[0] == 1.0f || (normal[0] == 0.0f && normal[1] == 0.0f)
            || normal[1] == -1.0f || normal[1] == 1.0f || (normal[1] == 0.0f && normal[2] == 0.0f)
            || normal[2] == -1.0f || normal[2] == 1.0f || (normal[2] == 0.0f && normal[0] == 0.0f) || side.bevel) {
            continue;  // axial, only test non-axial edges
        }

        Titanfall::EmitPlane(side.plane);
        b.planeCount++;
        Titanfall::Bsp::cmBrushSideProperties.emplace_back(Titanfall::EmitTextureData(*side.shaderInfo));
        uint16_t &so = Titanfall::Bsp::cmBrushSidePlaneOffsets.emplace_back();
        so = 0;
    }

    if (b.planeCount) {
        b.unknown = b.planeCount;
        b.sidePlaneIndex = Titanfall::Bsp::cmBrushSidePlaneOffsets.size() - b.planeCount;
    }
#endif
}

/*
    EmitUniqueContents()
    Emits collision flags and returns index to them
*/
int Titanfall::EmitUniqueContents(int flags) {
    for( int i = 0; i < Titanfall::Bsp::cmUniqueContents.size(); i++ ) {
        if( Titanfall::Bsp::cmUniqueContents.at(i) == flags )
            return i;
    }

    Titanfall::Bsp::cmUniqueContents.emplace_back( flags );

    return Titanfall::Bsp::cmUniqueContents.size() - 1;
}