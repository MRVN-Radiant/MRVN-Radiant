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

#include "remap.h"
#include "bspfile_abstract.h"


/*
    MakeMeshes()
    Makes the initial shared meshes for a given entity
*/
void Shared::MakeMeshes(const entity_t &e) {
    // Multiple entities can have meshes, make sure we clear before making new meshes
    Shared::meshes.clear();

    std::vector<Shared::Island_t>  lightmap_islands;

    int mesh_index = 0;
    // Loop through brushes
    for (const brush_t &brush : e.brushes) {
        // Loop through sides
        for (const side_t &side : brush.sides) {
            // Skip bevels & check flags
            if (side.bevel
             || side.shaderInfo->compileFlags & C_NODRAW) {
                continue;
            }

            // Check for degenerate windings
            if (side.winding.size() < 3) {
                Sys_FPrintf(SYS_WRN, "        Skipping degenerate winding!\n");
                continue;
            }

            Shared::Mesh_t &mesh = Shared::meshes.emplace_back();
            mesh.shaderInfo = side.shaderInfo;
            Shared::Island_t  island;

            Vector3 normal = side.plane.normal();

            // Loop through vertices
            for (const Vector3 &vertex : side.winding) {
                // Extend AABB
                mesh.minmax.extend(vertex);

                // Texturing
                // This is taken from surface.cpp, It somewhat works
                // TODO: Make this work better
                Vector2  textureUV, lightmapUV;
                Vector3  vTranslated, texX, texY;
                float    x, y;
                ComputeAxisBase(side.plane.normal(), texX, texY);
                vTranslated = vertex + e.originbrush_origin;
                // st vectors -> uv coords
                x = vector3_dot(vTranslated, texX);
                y = vector3_dot(vTranslated, texY);
                // TODO: scale lightmapUV by side.luxelScale
                lightmapUV = {x / 16, y / 16};
                // TODO: valve 220 st vector projection
                textureUV[0] = side.texMat[0][0] * x + side.texMat[0][1] * y + side.texMat[0][2];
                textureUV[1] = side.texMat[1][0] * x + side.texMat[1][1] * y + side.texMat[1][2];

                Shared::Vertex_t &sv = mesh.vertices.emplace_back();
                sv.xyz          = vertex;
                sv.normal       = normal;
                sv.textureUV    = textureUV;
                sv.colour       = {0xFF, 0xFF, 0xFF, 0xFF};
                sv.lightmapUV   = lightmapUV;
                sv.lightmapStep = {0.0f, 0.0f};  // unused

                island.bounds.extend(lightmapUV);
            }

            if (shaderInfo->surfaceFlags & 0x600 < 0x400) {  // mesh is lit
                island.mesh = mesh_index;
                lightmap_islands.push_back(island);
            } else {
                mesh.lightmapPage = -1;
            }

            // Create triangles for side
            for (std::size_t i = 0; i < side.winding.size() - 2; i++) {
                for (int j = 0; j < 3; j++) {
                    int vert_index = (j == 0) ? 0 : i + j;
                    mesh.triangles.emplace_back(vert_index);
                }
            }

            mesh_index++;
        }
    }

    // Loop through patches
    parseMesh_t *patch;
    patch = e.patches;
    while (patch != NULL) {
        mesh_t patchMesh = patch->mesh;

        Shared::Mesh_t &mesh = Shared::meshes.emplace_back();
        mesh.shaderInfo = patch->shaderInfo;
        Shared::Island_t  island;

        // Get vertices
        for (int index = 0; index < (patchMesh.width * patchMesh.height); index++) {
            Shared::Vertex_t &vertex = mesh.vertices.emplace_back();

            vertex.xyz          = patchMesh.verts[index].xyz;
            vertex.normal       = patchMesh.verts[index].normal;
            vertex.textureUV    = patchMesh.verts[index].st;
            vertex.colour       = {0xFF, 0xFF, 0xFF, 0xFF};  // TODO: patchMesh.verts[index].colour
            Vector2  lightmapUV = {patchMesh.verts[index].st[0] / 16,
                                   patchMesh.verts[index].st[1] / 16};  // TODO: patchMesh.luxelScale
            vertex.lightmapUV   = lightmapUV;
            vertex.lightmapStep = {0.0f, 0.0f};  // unused

            mesh.minmax.extend(vertex.xyz);
            island.bounds.extend(lightmapUV);
        }

        if (shaderInfo->surfaceFlags & 0x600 < 0x400) {  // mesh is lit
            island.mesh = mesh_index;
            lightmap_islands.push_back(island);
        } else {
            mesh.lightmapPage = -1;
        }

        // Make triangles
        for (int x = 0; x < (patchMesh.width - 1); x++) {
            for (int y = 0; y < (patchMesh.height - 1); y++) {
                int index = x + y * patchMesh.width;

                mesh.triangles.emplace_back(index);
                mesh.triangles.emplace_back(index + patchMesh.width);
                mesh.triangles.emplace_back(index + patchMesh.width + 1);

                mesh.triangles.emplace_back(index);
                mesh.triangles.emplace_back(index + patchMesh.width + 1);
                mesh.triangles.emplace_back(index + 1);
            }
        }

        patch = patch->next;
        mesh_index++;
    }

    // // Pack lightmap uv islands
    // int total_texels = 0;
    // for (Shared Island_t &island : lightmap_islands) {
    //     total_texels += island.area();  // also a sort key
    // }
    // int num_lightmap_pages = (total_texels / (1024 * 1024)) + 1;
    // // NOTE: approximate, doesn't account for packing algo effeciency
    // // sort islands
    // // might be more efficient to sort indices based on a list of areas, rather than multiple area calculations
    // std::sort(lightmap_islands.begin(), lightmap_islands.end(),
    //           [](MinMax2D a, MinMax2D b) { return a.area() > b.area(); });
    // // organise islands into pages
    // std::vector<std::vector<Island_t>>  lightmap_pages;
    // // pack lightmap pages
    // MinMax2D row({island.width, 0}, {1024 - island.width, island.height});
    // // islands that don't fit in their page move to a 'leftovers' page
    // // apply island transforms to island.mesh.vertices.lightmapUV (including pixel coords -> uv coords scale)

    // Combine all meshes based on shaderInfo and AABB tests
    std::size_t index = 0;
    std::size_t iterationsSinceCombine = 0;
    while (true) {
        // std::out_of_range can't comprehend this
        if (index >= Shared::meshes.size()) {
            index = 0;
        }

        // No more meshes to combine; break from the loop
        if (iterationsSinceCombine >= Shared::meshes.size()) {
            break;
        }

        // Get mesh which we then compare to the rest, maybe combine, maybe not
        Shared::Mesh_t &mesh1 = Shared::meshes.at(index);
        for (std::size_t i = 0; i < Shared::meshes.size(); i++) {
            if (index == i) { continue; }  // We dont want to compare the same mesh

            Shared::Mesh_t &mesh2 = Shared::meshes.at(i);

            if (mesh1.triangles.size() + mesh2.triangles.size() > 63000
             || mesh1.lightmapPage != mesh2.lightmapPage
             || !striEqual(mesh1.shaderInfo->shader.c_str(), mesh2.shaderInfo->shader.c_str())
             || !mesh1.minmax.test(mesh2.minmax)) {
                continue;
            }

            // All tests passed! We can combine these meshes
            for (uint16_t &triIndex : mesh2.triangles) {
                mesh1.triangles.emplace_back(triIndex + mesh1.vertices.size());
            }
            mesh1.vertices.insert(mesh1.vertices.end(), mesh2.vertices.begin(), mesh2.vertices.end());
            mesh1.minmax.extend(mesh2.minmax);

            // Delete mesh we combined as to not create duplicates
            Shared::meshes.erase(Shared::meshes.begin() + i);
            iterationsSinceCombine = 0;
            break;
        }

        iterationsSinceCombine++;
        index++;
    }

    // sort meshes
    std::vector<Shared::Mesh_t>  opaque_meshes;
    std::vector<Shared::Mesh_t>  decal_meshes;
    std::vector<Shared::Mesh_t>  trans_meshes;
    std::vector<Shared::Mesh_t>  sky_meshes;

    for (Shared::Mesh_t &mesh : Shared::meshes) {
        if (CHECK_FLAG(mesh.shaderInfo->compileFlags, C_SKY)) {
            sky_meshes.push_back(mesh);
            // Respawn does this on their skyboxes, seems to work even when kept
            REMOVE_FLAG(mesh.shaderInfo->surfaceFlags, S_MESH_UNKNOWN);
        } else if (CHECK_FLAG(mesh.shaderInfo->compileFlags, C_DECAL)) {
            decal_meshes.push_back(mesh);
        } else if (CHECK_FLAG(mesh.shaderInfo->compileFlags, C_TRANSLUCENT)) {
            trans_meshes.push_back(mesh);
        } else {
            opaque_meshes.push_back(mesh);
        }
    }

    // rebuild Shared::meshes from the sorted groups
    Shared::meshes.clear();
    #define COPY_MESHES(x)  Shared::meshes.insert(Shared::meshes.end(), x.begin(), x.end())
    COPY_MESHES(opaque_meshes);
    COPY_MESHES(decal_meshes);
    COPY_MESHES(trans_meshes);
    COPY_MESHES(sky_meshes);
    #undef COPY_MESHES

    // TODO: pack lightmaps for lit meshes
}


void Shared::MakeVisReferences() {
    /* Meshes */
    for (std::size_t i = 0; i < Shared::meshes.size(); i++) {
        Shared::Mesh_t &mesh = Shared::meshes.at(i);
        Shared::visRef_t &ref = Shared::visRefs.emplace_back();

        ref.minmax = mesh.minmax;
        ref.index = i;
    }

    /* Props */
    Sys_Printf("%9zu shared vis references\n", Shared::visRefs.size());
}


/*
    CalculateSAH
    calculates the surface-area-heurestic for a BVHx Tree
*/
float CalculateSAH(std::vector<Shared::visRef_t> &refs, MinMax &parent, int &axis, float *pos) {
    // NOTE [Fifty]: I plan on rewriting this soon
    const int type = 2;
    MinMax childMinMaxs[type] = {};
    std::size_t childRefCounts[type] = {};

    // Count and create AABBs from references
    for (Shared::visRef_t &ref : refs) {
        float refPos = ref.minmax.maxs[axis];

        // Since this function supports any number of children we need to
        // First node handling
        if (refPos < pos[0]) {
            childRefCounts[0]++;
            childMinMaxs[0].extend(ref.minmax);
            continue;
        }

        // Middle nodes
        for (int separator = 1; separator < type - 1; separator++) {
            if (refPos > pos[separator - 1] && refPos < pos[separator]) {
                childRefCounts[separator]++;
                childMinMaxs[separator].extend(ref.minmax);
            }
        }

        // Last node handling
        childRefCounts[type - 1]++;
        childMinMaxs[type - 1].extend(ref.minmax);
    }

    float parentArea = parent.area();

    float cost = .5f;
    for (int i = 0; i < type; i++) {
        if (childRefCounts[i] != 0) {
            float childArea = childMinMaxs[i].area();
            cost += childRefCounts[i] * childArea;
        }
    }

    cost /= parentArea;
    // Sys_Printf("%f\n", cost);

    return cost;
}


/*
    MakeVisTree
    Creates a node and either stops or tries to create children
*/
Shared::visNode_t Shared::MakeVisTree(std::vector<Shared::visRef_t> refs, float parentCost) {
    Shared::visNode_t  node;

    // Create MinMax of our node
    MinMax minmax;
    for (Shared::visRef_t &ref : refs) {
        minmax.extend(ref.minmax);
    }

    node.minmax = minmax;

    // Check if a vis ref is large enough to reference directly
    std::vector<Shared::visRef_t>  visRefs = refs;
    refs.clear();
    for (Shared::visRef_t &ref : visRefs) {
        if (minmax.surrounds(ref.minmax) && ref.minmax.area() / minmax.area() > 0.8) {
            node.refs.emplace_back(ref);
        } else {
            refs.emplace_back(ref);
        }
    }

    // Check all possible ways of splitting
    float bestCost = 0;
    float bestPos = 0;
    int bestAxis = 0;

    bestCost = 1e30f;
    float pos[1];
    for (int axis = 0; axis < 3; axis++) {
        for (std::size_t i = 0; i < refs.size(); i++) {
            pos[0] = refs.at(i).minmax.mins[axis];

            float cost = CalculateSAH(refs, minmax, axis, pos);
            // Sys_Printf("%f\n", cost);
            if (cost < bestCost) {
                bestCost = cost;
                bestPos = pos[0];
                bestAxis = axis;
            }
        }
    }

    if (bestCost >= parentCost) {
        if (refs.size() > 255) {
            Sys_FPrintf(SYS_ERR, "CellAABBNode references more than 255 refs\n");
        }

        for (Shared::visRef_t &ref : refs) {
            node.refs.emplace_back(ref);
        }

        return node;
    }

    MinMax  nodes[2];
    nodes[0] = minmax;
    nodes[1] = minmax;
    std::vector<Shared::visRef_t>  nodeRefs[2];

    nodes[0].maxs[bestAxis] = bestPos;
    nodes[1].mins[bestAxis] = bestPos;

    for (Shared::visRef_t &ref : refs) {
        for (int i = 0; i < 2; i++) {
            if (ref.minmax.test(nodes[i])) {
                nodeRefs[i].emplace_back(ref);
            }
        }
    }

    // Sys_Printf("%li; %li; %li; %li\n", nodeRefs[0].size(), nodeRefs[1].size(), nodeRefs[2].size(), nodeRefs[3].size());

    for (int i = 0; i < 2; i++) {
        if (nodeRefs[i].size() != 0) {
            node.children.emplace_back(Shared::MakeVisTree(nodeRefs[i], bestCost));
        }
    }

    return node;
}


/*
    MergeVisTree
    Walks the tree and tries to merge as many nodes as it can
    This gives us a shallower tree which should reduce mesh flickering
*/
void Shared::MergeVisTree(Shared::visNode_t &node) {
    float originalSAH = 0;
    for (Shared::visNode_t &child : node.children) {
        originalSAH = child.minmax.area();
    }
    originalSAH /= node.minmax.area();

    int timeout = 0;

    float newSAH = 1e30f;
    // Try to merge as many children as we can
    // Both the timeout and the "/ 10" are arbitrary
    while (originalSAH / 10 < newSAH) {
        std::vector<visNode_t> children;
        children = node.children;
        node.children.clear();

        for (Shared::visNode_t &child : children) {
            if (child.children.size() == 0 || child.refs.size() != 0) {
                node.children.push_back(child);
                continue;
            }

            for (Shared::visNode_t &c : child.children) {
                node.children.push_back(c);
            }
        }

        newSAH = 0;
        for (Shared::visNode_t &child : node.children) {
            newSAH = child.minmax.area();
        }
        newSAH /= node.minmax.area();

        timeout++;
        if (timeout > 10) {
            break;
        }
    }

    for (Shared::visNode_t &child : node.children) {
        if (child.children.size() != 0) {
            MergeVisTree(child);
        }
    }
}
