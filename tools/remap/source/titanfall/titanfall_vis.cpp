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
    EmitTextureData()
    Emits texture data and returns its index
*/
uint32_t Titanfall::EmitTextureData(shaderInfo_t shader) {
    std::string  tex = shader.shader.c_str();
    std::size_t  index;

    // Strip 'textures/'
    tex.erase(tex.begin(), tex.begin() + strlen("textures/"));
    std::replace(tex.begin(), tex.end(), '/', '\\');  // Do we even need to do this ?

    // Check if it's already saved
    std::string table = std::string(Titanfall::Bsp::textureDataData.begin(), Titanfall::Bsp::textureDataData.end());
    index = table.find(tex);
    if (index != std::string::npos) {
        // Is already saved, find the index of its textureData

        for (std::size_t i = 0; i < Titanfall::Bsp::textureData.size(); i++) {
            Titanfall::TextureData_t &td = Titanfall::Bsp::textureData.at(i);
            uint32_t &tdt = Titanfall::Bsp::textureDataTable.at(td.name_index);

            if (tdt == index) {
                return i;
            }
        }
    }

    // Wasn't already saved, save it
    index = Titanfall::Bsp::textureData.size();

    // Add to Table
    StringOutputStream  data;
    data << tex.c_str();
    std::vector<char>  str = { data.begin(), data.end() + 1 };

    Titanfall::Bsp::textureDataTable.emplace_back(Titanfall::Bsp::textureDataData.size());
    Titanfall::Bsp::textureDataData.insert(Titanfall::Bsp::textureDataData.end(), str.begin(), str.end());

    Titanfall::TextureData_t &td = Titanfall::Bsp::textureData.emplace_back();
    td.name_index = Titanfall::Bsp::textureDataTable.size() - 1;
    td.sizeX = shader.shaderImage->width;
    td.sizeY = shader.shaderImage->height;
    td.visibleX = shader.shaderImage->width;
    td.visibleY = shader.shaderImage->height;
    td.flags = shader.surfaceFlags;

    return index;
}


/*
    EmitVertex
    Saves a vertex into Titanfall::vertices and returns its index
*/
uint32_t Titanfall::EmitVertex(Vector3 &vertex) {
    for (uint32_t i = 0; i < (uint32_t)Titanfall::Bsp::vertices.size(); i++) {
        if (VectorCompare(vertex, Titanfall::Bsp::vertices.at(i))) {
            return i;
        }
    }

    Titanfall::Bsp::vertices.emplace_back(vertex);
    return (uint32_t)Titanfall::Bsp::vertices.size() - 1;
}


/*
    EmitVertexNormal
    Saves a vertex normal into Titanfall::vertexNormals and returns its index
*/
uint32_t Titanfall::EmitVertexNormal(Vector3 &normal) {
    for (uint32_t i = 0; i < (uint32_t)Titanfall::Bsp::vertexNormals.size(); i++) {
        if (VectorCompare(normal, Titanfall::Bsp::vertexNormals.at(i)))
            return i;
    }

    Titanfall::Bsp::vertexNormals.emplace_back(normal);
    return (uint32_t)Titanfall::Bsp::vertexNormals.size() - 1;
}


void Titanfall::SetupGameLump() {
    Titanfall::Bsp::gameLumpHeader.version = 1;
    memcpy( &Titanfall::Bsp::gameLumpHeader.ident, "prps", 4 );
    Titanfall::Bsp::gameLumpHeader.gameConst = 786432;
}

/*
    EmitOcclusionMeshVertex
    Emits a vertex in occlusion vertices, checks for duplicates
*/
uint16_t Titanfall::EmitOcclusionMeshVertex( Vector3 vertex ) {
    for( std::size_t i = 0; i < Titanfall::Bsp::occlusionMeshVertices.size(); i++ ) {
        if( VectorCompare(vertex, Titanfall::Bsp::occlusionMeshVertices.at(i)) ) {
            return i;
        }
    }

    Titanfall::Bsp::occlusionMeshVertices.emplace_back(vertex);
    return Titanfall::Bsp::occlusionMeshVertices.size() - 1;
}

/*
    EmitOcclusionMeshes
    Emits mesh information for func_occluder
*/
void Titanfall::EmitOcclusionMeshes( const entity_t &entity ) {
    for( const brush_t &brush : entity.brushes ) {
        for( const side_t &side : brush.sides ) {
            // Skip bevels as they have no windings
            if( side.bevel )
                continue;
            
            // Skip degenerate windings as they contribute no faces
            if( side.winding.size() < 3 )
                continue;
            
            for (std::size_t i = 0; i < side.winding.size() - 2; i++) {
                for (int j = 0; j < 3; j++) {
                    int vertIndex = (j == 0) ? 0 : i + j;
                    uint16_t index = Titanfall::EmitOcclusionMeshVertex( side.winding.at(vertIndex) );
                    Titanfall::Bsp::occlusionMeshIndices.emplace_back( index );
                }
            }
        }
    }

    for( parseMesh_t *patch = entity.patches; patch != NULL; patch = patch->next ) {
        mesh_t patchMesh = patch->mesh;

        for( int x = 0; x < (patchMesh.width - 1); x++ ) {
            for( int y = 0; y < (patchMesh.height - 1); y++ ) {
                int vertIndex = x + y * patchMesh.width;

                uint16_t index;

                index = Titanfall::EmitOcclusionMeshVertex( patchMesh.verts[vertIndex].xyz );
                Titanfall::Bsp::occlusionMeshIndices.emplace_back( index );
                index = Titanfall::EmitOcclusionMeshVertex( patchMesh.verts[vertIndex + patchMesh.width].xyz );
                Titanfall::Bsp::occlusionMeshIndices.emplace_back( index );
                index = Titanfall::EmitOcclusionMeshVertex( patchMesh.verts[vertIndex + patchMesh.width + 1].xyz );
                Titanfall::Bsp::occlusionMeshIndices.emplace_back( index );

                index = Titanfall::EmitOcclusionMeshVertex( patchMesh.verts[vertIndex].xyz );
                Titanfall::Bsp::occlusionMeshIndices.emplace_back( index );
                index = Titanfall::EmitOcclusionMeshVertex( patchMesh.verts[vertIndex + patchMesh.width + 1].xyz );
                Titanfall::Bsp::occlusionMeshIndices.emplace_back( index );
                index = Titanfall::EmitOcclusionMeshVertex( patchMesh.verts[vertIndex + 1].xyz );
                Titanfall::Bsp::occlusionMeshIndices.emplace_back( index );
            }
        }
    }
}

/*
    EmitObjReferences
    Emits obj references and returns an index to the first one
*/
std::size_t Titanfall::EmitObjReferences(Shared::visNode_t &node) {
    // Stores which indicies were found
    std::list<std::size_t>  indicies;
    // Try to check for duplicates
    for (Shared::visRef_t &ref : node.refs) {
        for (std::size_t i = 0; i < Titanfall::Bsp::objReferences.size(); i++) {
            uint16_t &tf = Titanfall::Bsp::objReferences.at(i);
            if (tf == ref.index) {
                indicies.push_back(i);
            }
        }
    }
    indicies.sort();

    // We found all, now check if they're next to each other
    if (indicies.size() == node.refs.size()) {
        std::size_t  lastIndex = -1;
        for (std::size_t &i : indicies) {
            if (lastIndex == -1) {
                lastIndex = i;
                continue;
            }

            if (i != lastIndex + 1) {
                lastIndex = -1;
                break;
            }
        }

        if (lastIndex != -1) {
            return indicies.front();
        }
    }

    for (Shared::visRef_t &ref : node.refs) {
        Titanfall::ObjReferenceBounds_t &rb = Titanfall::Bsp::objReferenceBounds.emplace_back();
        rb.maxs = ref.minmax.maxs;
        rb.mins = ref.minmax.mins;

        Titanfall::Bsp::objReferences.emplace_back(ref.index);
    }

    return Titanfall::Bsp::objReferences.size() - node.refs.size();
}


/*
    GetTotalVisRefCount
    Calculates the total number of obj refs under a vistree node
*/
uint16_t GetTotalVisRefCount(Shared::visNode_t node) {
    uint16_t  count = node.refs.size();

    for (Shared::visNode_t &n : node.children) {
        count += GetTotalVisRefCount(n);
    }

    return count;
}


/*
    GetTotalVisNodeChildCount
    Calculates the total child count of node
*/
uint16_t GetTotalVisNodeChildCount(Shared::visNode_t node) {
    uint16_t  count = node.children.size();

    for (Shared::visNode_t &n : node.children) {
        count += GetTotalVisNodeChildCount(n);
    }

    return count;
}


/*
    EmitVisChildrenOfTreeNode
    Emits children of a vis tree node
*/
int Titanfall::EmitVisChildrenOfTreeNode(Shared::visNode_t node) {
    int  index = Titanfall::Bsp::cellAABBNodes.size();  // Index of first child of node

    for (std::size_t i = 0; i < node.children.size(); i++) {
        Shared::visNode_t &n = node.children.at(i);

        Titanfall::CellAABBNode_t &bn = Titanfall::Bsp::cellAABBNodes.emplace_back();
        bn.maxs = n.minmax.maxs;
        bn.mins = n.minmax.mins;
        bn.childCount = n.children.size();
        bn.totalRefCount = GetTotalVisRefCount(n);

        if (n.refs.size()) {
            bn.objRef = Titanfall::EmitObjReferences(n);
            bn.objRefCount = n.refs.size();
        }
    }

    for (std::size_t i = 0; i < node.children.size(); i++) {
        int  firstChild = EmitVisChildrenOfTreeNode(node.children.at(i));

        Titanfall::CellAABBNode_t &bn = Titanfall::Bsp::cellAABBNodes.at(index + i);
        bn.firstChild = firstChild;
    }

    return index;
}


/*
    EmitVisTree
    Emits vistree
*/
void Titanfall::EmitVisTree() {
    Shared::visNode_t &root = Shared::visRoot;

    Titanfall::CellAABBNode_t &bn = Titanfall::Bsp::cellAABBNodes.emplace_back();
    bn.maxs = root.minmax.maxs;
    bn.mins = root.minmax.mins;
    bn.firstChild = Titanfall::Bsp::cellAABBNodes.size();
    bn.childCount = root.children.size();
    bn.totalRefCount = GetTotalVisRefCount(root);
    if (root.refs.size()) {
        bn.objRef = EmitObjReferences(root);
        bn.objRefCount = root.refs.size();
    }

    EmitVisChildrenOfTreeNode(Shared::visRoot);
}


/*
    EmitVertexUnlit
    Saves a vertex into Titanfall::Bsp::vertexUnlitVertices
*/
void Titanfall::EmitVertexUnlit(Shared::Vertex_t &vertex) {
    Titanfall::VertexUnlit_t &ul = Titanfall::Bsp::vertexUnlitVertices.emplace_back();
    ul.vertexIndex = Titanfall::EmitVertex(vertex.xyz);
    ul.normalIndex = Titanfall::EmitVertexNormal(vertex.normal);
    ul.uv0 = vertex.st;
}


/*
    EmitVertexLitFlat
    Saves a vertex into Titanfall::Bsp::vertexLitFlatVertices
*/
void Titanfall::EmitVertexLitFlat(Shared::Vertex_t &vertex) {
    Titanfall::VertexLitFlat_t &lf = Titanfall::Bsp::vertexLitFlatVertices.emplace_back();
    lf.vertexIndex = Titanfall::EmitVertex(vertex.xyz);
    lf.normalIndex = Titanfall::EmitVertexNormal(vertex.normal);
    lf.uv0 = vertex.st;
}


/*
    EmitVertexLitBump
    Saves a vertex into Titanfall::Bsp::vertexLitBumpVertices
*/
void Titanfall::EmitVertexLitBump(Shared::Vertex_t &vertex) {
    Titanfall::VertexLitBump_t &lv = Titanfall::Bsp::vertexLitBumpVertices.emplace_back();
    lv.vertexIndex = Titanfall::EmitVertex(vertex.xyz);
    lv.normalIndex = Titanfall::EmitVertexNormal(vertex.normal);
    lv.uv0 = vertex.st;
    lv.negativeOne = -1;
}


/*
    EmitVertexUnlitTS
    Saves a vertex into Titanfall::Bsp::vertexUnlitTSVertices
*/
void Titanfall::EmitVertexUnlitTS(Shared::Vertex_t &vertex) {
    Titanfall::VertexUnlitTS_t &ul = Titanfall::Bsp::vertexUnlitTSVertices.emplace_back();
    ul.vertexIndex = Titanfall::EmitVertex(vertex.xyz);
    ul.normalIndex = Titanfall::EmitVertexNormal(vertex.normal);
    ul.uv0 = vertex.st;
}


/*
    EmitVertexBlinnPhong
    Saves a vertex into Titanfall::Bsp::vertexBlinnPhongVertices
*/
void Titanfall::EmitVertexBlinnPhong(Shared::Vertex_t &vertex) {
    Titanfall::VertexBlinnPhong_t &bp = Titanfall::Bsp::vertexBlinnPhongVertices.emplace_back();
    bp.vertexIndex = Titanfall::EmitVertex(vertex.xyz);
    bp.normalIndex = Titanfall::EmitVertexNormal(vertex.normal);
}


/*
    EmitMeshes()
    writes the mesh list to the bsp
*/
void Titanfall::EmitMeshes(const entity_t &e) {
    // ALSO: TextureData, TextureDataStringData, TextureDataStringTable, MaterialSort,
    // -- VertexReservedX, Vertices, VertexNormals & MeshBounds
    // NOTE: we don't actually build meshes from const entity_t &e
    // -- Shared::meshes has every mesh from brushes & patches built & merged before EmitMeshes get called

    // Emit Bsp::meshes
    for (const Shared::Mesh_t &mesh : Shared::meshes) {
        Titanfall::Mesh_t &m = Titanfall::Bsp::meshes.emplace_back();
        m.const0 = 4294967040;  // :)
        m.flags = mesh.shaderInfo->surfaceFlags;
        m.vertexCount = mesh.vertices.size();
        m.triOffset = Titanfall::Bsp::meshIndices.size();
        m.triCount = mesh.triangles.size() / 3;
        
        int vertexOffset = 0;
        
        if (CHECK_FLAG(mesh.shaderInfo->surfaceFlags, S_VERTEX_UNLIT_TS)) {
            m.vertexType = 3;
            vertexOffset = Titanfall::Bsp::vertexUnlitTSVertices.size();
        } else if (CHECK_FLAG(mesh.shaderInfo->surfaceFlags,S_VERTEX_LIT_BUMP)) {
            m.vertexType = 2;
            vertexOffset = Titanfall::Bsp::vertexLitBumpVertices.size();
        } else if (CHECK_FLAG(mesh.shaderInfo->surfaceFlags, S_VERTEX_UNLIT)) {
            m.vertexType = 1;
            vertexOffset = Titanfall::Bsp::vertexUnlitVertices.size();
        } else {
            m.vertexType = 0;
            vertexOffset = Titanfall::Bsp::vertexLitFlatVertices.size();
        }

        m.vertexOffset = vertexOffset;

        // Emit texture related structs
        uint32_t  textureIndex = Titanfall::EmitTextureData(*mesh.shaderInfo);
        m.materialOffset = Titanfall::EmitMaterialSort(textureIndex, vertexOffset, m.vertexCount);
        int materialSortOffset = Titanfall::Bsp::materialSorts.at(m.materialOffset).vertexOffset;
        MinMax  aabb;

        // Save vertices and vertexnormals
        for (std::size_t i = 0; i < mesh.vertices.size(); i++) {
            Shared::Vertex_t  vertex = mesh.vertices.at(i);

            aabb.extend(vertex.xyz);

            if (CHECK_FLAG(mesh.shaderInfo->surfaceFlags, S_VERTEX_UNLIT_TS)) {
                Titanfall::EmitVertexUnlitTS(vertex);
            } else if (CHECK_FLAG(mesh.shaderInfo->surfaceFlags, S_VERTEX_LIT_BUMP)) {
                Titanfall::EmitVertexLitBump(vertex);
            } else if (CHECK_FLAG(mesh.shaderInfo->surfaceFlags, S_VERTEX_UNLIT)) {
                Titanfall::EmitVertexUnlit(vertex);
            } else {
                Titanfall::EmitVertexLitFlat(vertex);
            }
        }

        // Save triangles
        for (uint16_t triangle : mesh.triangles) {
            Titanfall::Bsp::meshIndices.emplace_back(triangle + m.vertexOffset - materialSortOffset);
        }

        // Save MeshBounds
        Titanfall::MeshBounds_t &mb = Titanfall::Bsp::meshBounds.emplace_back();
        mb.origin = (aabb.maxs + aabb.mins) / 2;
        mb.extents = (aabb.maxs - aabb.mins) / 2;
    }
}


/*
    EmitMaterialSort()
    Tries to create a material sort of the last texture
*/
uint16_t Titanfall::EmitMaterialSort(uint32_t index, int offset, int count) {
    /* Check if the material sort we need already exists */
    std::size_t pos = 0;
    for (Titanfall::MaterialSort_t &ms : Titanfall::Bsp::materialSorts) {
        if (ms.textureData == index && offset - ms.vertexOffset + count < 65535) {
            return pos;
        }

        pos++;
    }

    Titanfall::MaterialSort_t &ms = Titanfall::Bsp::materialSorts.emplace_back();
    ms.textureData = index;
    ms.lightmapHeader = -1;
    ms.cubemap = -1;
    ms.vertexOffset = offset;

    return pos;
}

/*
    EmitLevelInfoWorldspawn()
*/
void Titanfall::EmitLevelInfoWorldspawn() {
    Titanfall::LevelInfo_t &li = Titanfall::Bsp::levelInfo.emplace_back();

    // Mest indicies
    li.firstDecalMeshIndex = 0;
    for (Shared::Mesh_t &mesh : Shared::meshes) {
        if (CHECK_FLAG(mesh.shaderInfo->compileFlags, C_DECAL)) { break; }
            li.firstDecalMeshIndex++;
    }

    // TODO: start from firstDecalMeshIndex
    li.firstTransMeshIndex = 0;
    for (Shared::Mesh_t &mesh : Shared::meshes) {
        if (CHECK_FLAG(mesh.shaderInfo->compileFlags, C_TRANSLUCENT)) { break; }
            li.firstTransMeshIndex++;
    }

    // TODO: start from firstTransMeshIndex
    li.firstSkyMeshIndex = 0;
    for (Shared::Mesh_t &mesh : Shared::meshes) {
        if (CHECK_FLAG(mesh.shaderInfo->compileFlags, C_SKY)) { break; }
            li.firstSkyMeshIndex++;
    }

    if( li.firstTransMeshIndex > li.firstSkyMeshIndex )
        li.firstTransMeshIndex = li.firstSkyMeshIndex;
    
    if( li.firstDecalMeshIndex > li.firstTransMeshIndex )
        li.firstDecalMeshIndex = li.firstTransMeshIndex;
}

/*
    EmitLevelInfo()
*/
void Titanfall::EmitLevelInfo() {
    Titanfall::LevelInfo_t &li = Titanfall::Bsp::levelInfo.back();

    li.propCount = Titanfall::Bsp::gameLumpPropHeader.numProps;
    // TODO: unk2 sun vector from last light_environment
    li.unk2 = { 0.1, 0.8, -0.6 };  // placeholder
}