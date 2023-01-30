/* -------------------------------------------------------------------------------

   My feeble attempt at generating a r1 ( Titanfall1 ) .bsp file.

   A lot of this is temporary and will be altered heavily with new information.
   If you know how to c++ better than me feel free to contribute or call me words
   in discord. :)

   - Fifty#8113, also known as Fifteen, Sixteen, Seventeen, Eigtheen and FORTY

   ------------------------------------------------------------------------------- */


/* dependencies */
#include "bspfile_abstract.h"
#include <algorithm>
#include <ctime>


/*
   LoadR2BSPFile()
   loads a r1 bsp file
*/
void LoadR1BSPFile(rbspHeader_t *header, const char *filename) {
    Titanfall::LoadLumpsAndEntities(header, filename);
    Titanfall::ParseLoadedBSP();
}


/*
   WriteR2BSPFile()
   writes a r1 bsp file
*/
void WriteR1BSPFile(const char *filename) {
    rbspHeader_t  header {};

    // Set up header
    memcpy(header.ident, g_game->bspIdent, 4);
    header.version = LittleLong(g_game->bspVersion);
    header.mapVersion = 6;
    header.maxLump = 127;

    // This should be automated maybe
    header.lumps[0x02].lumpVer = 1;
    header.lumps[0x36].lumpVer = 1;
    header.lumps[0x49].lumpVer = 1;
    header.lumps[0x53].lumpVer = 1;

    // write initial header
    FILE *file = SafeOpenWrite(filename);
    SafeWrite(file, &header, sizeof(header));

    {
        char message[64] = "Built with love using MRVN-radiant :)";
        SafeWrite(file, &message, sizeof(message));
    }
    {
        char message[64];
        strncpy(message,StringOutputStream(64)("Version:        ", Q3MAP_VERSION).c_str(), 64);
        SafeWrite(file, &message, sizeof(message));
    }
    {
        time_t t;
        time(&t);
        char message[64];
        strncpy(message,StringOutputStream(64)("Time:           ", asctime(localtime(&t))).c_str(), 64);
        SafeWrite(file, &message, sizeof(message));
    }

    /* Write lumps */
    AddLump(file, header.lumps[R1_LUMP_ENTITIES],          Titanfall::Bsp::entities);
    AddLump(file, header.lumps[R1_LUMP_PLANES],            Titanfall::Bsp::planes);
    AddLump(file, header.lumps[R1_LUMP_TEXTURE_DATA],      Titanfall::Bsp::textureData);
    AddLump(file, header.lumps[R1_LUMP_VERTICES],          Titanfall::Bsp::vertices);
    AddLump(file, header.lumps[R1_LUMP_MODELS],            Titanfall::Bsp::models);
    AddLump(file, header.lumps[R1_LUMP_ENTITY_PARTITIONS], Titanfall::Bsp::entityPartitions);
    AddLump(file, header.lumps[R1_LUMP_PHYSICS_COLLIDE],   Titanfall::Bsp::physicsCollide_stub);  // stub
    AddLump(file, header.lumps[R1_LUMP_VERTEX_NORMALS],    Titanfall::Bsp::vertexNormals);
    // GameLump Stub
    {
        header.lumps[R1_LUMP_GAME_LUMP].offset = ftell(file);
        header.lumps[R1_LUMP_GAME_LUMP].length = sizeof(Titanfall::GameLumpHeader_t)
                                               + sizeof(Titanfall::GameLumpPathHeader_t)
                                               + sizeof(Titanfall::GameLumpPath_t) * Titanfall::Bsp::gameLumpPathHeader.numPaths
                                               + sizeof(Titanfall::GameLumpPropHeader_t)
                                               + sizeof(Titanfall::GameLumpProp_t) * Titanfall::Bsp::gameLumpPropHeader.numProps
                                               + sizeof(Titanfall::GameLumpUnknownHeader_t);

        Titanfall::Bsp::gameLumpHeader.offset = ftell(file) + sizeof(Titanfall::GameLumpHeader_t);
        Titanfall::Bsp::gameLumpHeader.length = sizeof(Titanfall::GameLumpPathHeader_t)
                                              + sizeof(Titanfall::GameLumpPath_t) * Titanfall::Bsp::gameLumpPathHeader.numPaths
                                              + sizeof(Titanfall::GameLumpPropHeader_t)
                                              + sizeof(Titanfall::GameLumpProp_t) * Titanfall::Bsp::gameLumpPropHeader.numProps
                                              + sizeof(Titanfall::GameLumpUnknownHeader_t);

        SafeWrite(file, &Titanfall::Bsp::gameLumpHeader, sizeof(Titanfall::GameLumpHeader_t));
        SafeWrite(file, &Titanfall::Bsp::gameLumpPathHeader, sizeof(Titanfall::GameLumpPathHeader_t));
        SafeWrite(file, &Titanfall::Bsp::gameLumpPropHeader, sizeof(Titanfall::GameLumpPropHeader_t));
        SafeWrite(file, &Titanfall::Bsp::gameLumpUnknownHeader, sizeof(Titanfall::GameLumpUnknownHeader_t));
    }
    AddLump(file, header.lumps[R1_LUMP_TEXTURE_DATA_STRING_DATA],    Titanfall::Bsp::textureDataData);
    AddLump(file, header.lumps[R1_LUMP_TEXTURE_DATA_STRING_TABLE],   Titanfall::Bsp::textureDataTable);
    AddLump(file, header.lumps[R1_LUMP_WORLD_LIGHTS],                Titanfall::Bsp::worldLights_stub);  // stub
    AddLump(file, header.lumps[R1_LUMP_VERTEX_UNLIT],                Titanfall::Bsp::vertexUnlitVertices);
    AddLump(file, header.lumps[R1_LUMP_VERTEX_LIT_FLAT],             Titanfall::Bsp::vertexLitFlatVertices);
    AddLump(file, header.lumps[R1_LUMP_VERTEX_LIT_BUMP],             Titanfall::Bsp::vertexLitBumpVertices);
    AddLump(file, header.lumps[R1_LUMP_VERTEX_UNLIT_TS],             Titanfall::Bsp::vertexUnlitTSVertices);
    AddLump(file, header.lumps[R1_LUMP_VERTEX_BLINN_PHONG],          Titanfall::Bsp::vertexBlinnPhongVertices);
    AddLump(file, header.lumps[R1_LUMP_MESH_INDICES],                Titanfall::Bsp::meshIndices);
    AddLump(file, header.lumps[R1_LUMP_MESHES],                      Titanfall::Bsp::meshes);
    AddLump(file, header.lumps[R1_LUMP_MESH_BOUNDS],                 Titanfall::Bsp::meshBounds);
    AddLump(file, header.lumps[R1_LUMP_MATERIAL_SORT],               Titanfall::Bsp::materialSorts);
    AddLump(file, header.lumps[R1_LUMP_LIGHTMAP_HEADERS],            Titanfall::Bsp::lightmapHeaders_stub);  // stub
    AddLump(file, header.lumps[R1_LUMP_CM_GRID],                     Titanfall::Bsp::cmGrid);
    AddLump(file, header.lumps[R1_LUMP_CM_GRID_CELLS],               Titanfall::Bsp::cmGridCells);
    AddLump(file, header.lumps[R1_LUMP_CM_GEO_SETS],                 Titanfall::Bsp::cmGeoSets);
    AddLump(file, header.lumps[R1_LUMP_CM_GEO_SET_BOUNDS],           Titanfall::Bsp::cmGeoSetBounds);
    AddLump(file, header.lumps[R1_LUMP_CM_UNIQUE_CONTENTS],          Titanfall::Bsp::cmUniqueContents);
    AddLump(file, header.lumps[R1_LUMP_CM_BRUSHES],                  Titanfall::Bsp::cmBrushes);
    AddLump(file, header.lumps[R1_LUMP_CM_BRUSH_SIDE_PROPS],         Titanfall::Bsp::cmBrushSideProperties);
    AddLump(file, header.lumps[R1_LUMP_CM_BRUSH_SIDE_PLANE_OFFSETS], Titanfall::Bsp::cmBrushSidePlaneOffsets);
    AddLump(file, header.lumps[R1_LUMP_LIGHTMAP_DATA_SKY],           Titanfall::Bsp::lightMapDataSky_stub);  // stub
    AddLump(file, header.lumps[R1_LUMP_CSM_AABB_NODES],              Titanfall::Bsp::csmAABBNodes_stub);  // stub
    AddLump(file, header.lumps[R1_LUMP_CELL_BSP_NODES],              Titanfall::Bsp::cellBSPNodes_stub);  // stub
    AddLump(file, header.lumps[R1_LUMP_CELLS],                       Titanfall::Bsp::cells_stub);  // stub
    AddLump(file, header.lumps[R1_LUMP_OCCLUSION_MESH_VERTICES],     Titanfall::Bsp::occlusionMeshVertices);
    AddLump(file, header.lumps[R1_LUMP_OCCLUSION_MESH_INDICES],      Titanfall::Bsp::occlusionMeshIndices);
    AddLump(file, header.lumps[R1_LUMP_CELL_AABB_NODES],             Titanfall::Bsp::cellAABBNodes);
    AddLump(file, header.lumps[R1_LUMP_OBJ_REFERENCES],              Titanfall::Bsp::objReferences);
    AddLump(file, header.lumps[R1_LUMP_OBJ_REFERENCE_BOUNDS],        Titanfall::Bsp::objReferenceBounds);
    AddLump(file, header.lumps[R1_LUMP_LEVEL_INFO],                  Titanfall::Bsp::levelInfo);

    /* emit bsp size */
    const int size = ftell(file);
    Sys_Printf("Wrote %.1f MB (%d bytes)\n", (float)size / (1024 * 1024), size);

    /* write the completed header */
    fseek(file, 0, SEEK_SET);
    SafeWrite(file, &header, sizeof(header));

    /* close the file */
    fclose(file);
}


/*
    CompileR1BSPFile
    Compiles a Titanfall 1 bsp file and sorts entition into ent files
*/
void CompileR1BSPFile() {
    Titanfall::SetupGameLump();

    for (entity_t &entity : entities) {
        const char *pszClassname = entity.classname();

        #define ENT_IS(classname) striEqual(pszClassname, classname)

        // Visible geo
        if (ENT_IS("worldspawn")) { // "worldspawn" is most of the map, should always be the 1st entity
            Titanfall::BeginModel(entity);

            Shared::MakeMeshes(entity);
            Shared::MakeVisReferences();

            Titanfall::EmitMeshes(entity);

            Titanfall::EmitCollisionGrid(entity);

            Titanfall::EmitLevelInfoWorldspawn();

            Titanfall::EndModel();
        } else if (ENT_IS("misc_model")) { // Compile as static props into gamelump
            // TODO: use prop_static instead
            // EmitProp(entity);
            continue; // Don't emit as entity
        } else if (ENT_IS("func_occluder")) {
            Titanfall::EmitOcclusionMeshes( entity );
            continue; // Don't emit as entity
        } else {
            if( entity.brushes.size() ) {
                Titanfall::BeginModel(entity);
                Shared::MakeMeshes(entity);
                Titanfall::EmitMeshes(entity);
                Titanfall::EmitModelGridCell(entity);
                Titanfall::EndModel();
            }
        }

        Titanfall::EmitEntity(entity);

        #undef ENT_IS
    }

    // Write entity partitions we used
    Titanfall::EmitEntityPartitions();

    // Generate vis tree for worldspawn, we do this here as we'll need portals once we reverse further
    Shared::visRoot = Shared::MakeVisTree(Shared::visRefs, 1e30f);
    Shared::MergeVisTree(Shared::visRoot);
    Titanfall::EmitVisTree();

    // Emit level info
    Titanfall::EmitLevelInfo();

    // Emit lumps we dont generate yet, but need for the map to load
    Titanfall::EmitStubs();
}


/*
   EmitEntity()
   Saves an entity into it's corresponding .ent file or the lump in the .bsp
*/
void Titanfall::EmitEntity(const entity_t &e) {
    // temporary enums
    #define ENT_BSP     0
    #define ENT_ENV     1
    #define ENT_FX      2
    #define ENT_SCRIPT  3
    #define ENT_SND     4
    #define ENT_SPAWN   5

    int dest = ENT_BSP;  // deferred write
    const char *classname = e.classname();
    entity_t e2 = (entity_t)e;  // stripping const so we can make changes

    // NOTE: key string compares also exist as methods but we really need to unroll all those function calls
    // -- resist the urge to optimise all these string compares, there are more important jobs to do.
    #define ENT_IS(x)          striEqual(classname, x)
    #define ENT_STARTS(x)      striEqualPrefix(classname, x)
    #define EDITORCLASS(x, y)  if (ENT_IS(y)) { e2.setKeyValue("classname", x); e2.setKeyValue("editorclass", y); }
    // env
    if (ENT_IS("env_fog_controler")
     || ENT_IS("sky_camera")) {
        dest = ENT_ENV;
    // fx
    } else if (ENT_IS("beam_spotlight")
            || ENT_IS("env_sprite_clientside")
            || ENT_STARTS("info_target_fx")
            || ENT_IS("info_particle_system")) {
        dest = ENT_FX;
        /* editorclass conversion */
        EDITORCLASS("info_target", "info_target_fx")
        else EDITORCLASS("info_target_clientside", "info_target_fx_clientside")
    // script
    } else if (ENT_IS("assault_assaultpoint")
            || ENT_IS("info_hardpoint")
            || ENT_IS("info_hint")
            || ENT_STARTS("info_node")
            || ENT_IS("info_target")
            || ENT_IS("info_target_clientside")
            || ENT_IS("path_track")
            || ENT_IS("prop_control_panel")
            || ENT_IS("prop_dynamic")
            || ENT_IS("prop_refuel_pump")
            || ENT_IS("script_ref")
            || ENT_IS("traverse")) {
        dest = ENT_SCRIPT;
        /* editorclass conversion */
        EDITORCLASS("info_node", "info_node_spectre")
    // snd
    } else if (ENT_IS("ambient_generic")) {
        dest = ENT_SND;
    // spawn
    } else if (ENT_IS("info_frontline")
            || ENT_STARTS("info_spawnpoint_")) {
        dest = ENT_SPAWN;
    // TODO: filter out editor only entities *just* before this else
    // e.g. prop_static, env_cubemap, info_lightprobe, light (static worldlights), etc.
    // !!! and make sure those entities still get used for the appropriate lumps !!!
    // bsp
    } else {
        dest = ENT_BSP;
    }
    #undef ENT_IS
    #undef ENT_STARTS
    #undef EDITORCLASS

    // TODO: general entity edits (e.g. float angle (Yaw) -> float[3] angles (Pitch Yaw Roll))

    // write
    StringOutputStream  data;
    data << "{\n";
    for (const epair_t &pair : e2.epairs) {  // e2 to get editorclass changes
        data << "\"" << pair.key.c_str() << "\" \"" << pair.value.c_str() << "\"\n";
    }
    data << "}\n";

    std::vector<char> str = { data.begin(), data.end() };

    #define ENT_APPEND(x)  Titanfall::Ent::x.insert(Titanfall::Ent::x.end(), str.begin(), str.end()); break
    switch (dest) {
        case ENT_BSP:
            Titanfall::Bsp::entities.insert(Titanfall::Bsp::entities.end(), str.begin(), str.end()); break;
        case ENT_ENV:
            ENT_APPEND(env);
        case ENT_FX:
            ENT_APPEND(fx);
        case ENT_SCRIPT:
            ENT_APPEND(script);
        case ENT_SND:
            ENT_APPEND(snd);
        case ENT_SPAWN:
            ENT_APPEND(spawn);
        // NOTE: default should never be hit
    }
    #undef ENT_APPEND
    #undef ENT_BSP
    #undef ENT_ENV
    #undef ENT_FX
    #undef ENT_SCRIPT
    #undef ENT_SND
    #undef ENT_SPAWN
}


/*
    EmitTriggerBrushPlaneKeyValues()
    Emits brush planes as keyvalues because respawn
*/
void Titanfall::EmitTriggerBrushPlaneKeyValues(entity_t &e) {
    MinMax  minmax;
    std::size_t  b = 0;
    for (const brush_t &brush : e.brushes) {
        minmax.extend(brush.minmax);

        for (std::size_t s = 0; s < brush.sides.size(); s++) {
            const side_t &side = brush.sides.at(s);
            StringOutputStream  key;
            key << "*trigger_brush_" << b << "_plane_" << s;

            StringOutputStream  value;
            value << side.plane.a << " " << side.plane.b << " " << side.plane.c << " " << side.plane.d;

            e.setKeyValue(key.c_str(), value.c_str());
        }
        b++;
    }

    {
        StringOutputStream  key;
        key << "*trigger_bounds_mins";

        StringOutputStream  value;
        value << minmax.mins.x() << " " << minmax.mins.y() << " " << minmax.mins.z();

        e.setKeyValue(key.c_str(), value.c_str());
    }
    {
        StringOutputStream  key;
        key << "*trigger_bounds_maxs";

        StringOutputStream  value;
        value << minmax.maxs.x() << " " << minmax.maxs.y() << " " << minmax.maxs.z();

        e.setKeyValue(key.c_str(), value.c_str());
    }
}


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
    BeginModel()
    Creates a new model entry
*/
void Titanfall::BeginModel(entity_t &entity) {
    Sys_FPrintf( SYS_VRB, "   BeginModel: \"%s\"\n", entity.classname() );

    Titanfall::Model_t &model = Titanfall::Bsp::models.emplace_back();
    model.firstMesh = (uint32_t)Titanfall::Bsp::meshes.size();

    StringOutputStream ss;
    ss << "*" << Titanfall::Bsp::models.size() - 1;

    if( entity.mapEntityNum != 0 ) {
        entity.setKeyValue("model", ss.c_str());
    }
}


/*
    EndModel()
    Fills the last model entry
*/
void Titanfall::EndModel() {
    Titanfall::Model_t &model = Titanfall::Bsp::models.back();
    model.meshCount = (uint32_t)Titanfall::Bsp::meshes.size() - model.firstMesh;

    for (Titanfall::MeshBounds_t &meshBounds : Titanfall::Bsp::meshBounds) {
        model.minmax.extend(meshBounds.origin - meshBounds.extents);
        model.minmax.extend(meshBounds.origin + meshBounds.extents);
    }

    Sys_FPrintf( SYS_VRB, "       numMeshes: %i\n", model.meshCount );
    Sys_FPrintf( SYS_VRB, "   EndModel\n" );
}


/*
    EmitEntityPartitions()
    Writes entitiy partitions respawn uses
*/
void Titanfall::EmitEntityPartitions() {
    std::string  partitions = "01*";

    if (Titanfall::Ent::env.size()) {
        partitions += " env";
    }

    if (Titanfall::Ent::fx.size()) {
        partitions += " fx";
    }

    if (Titanfall::Ent::script.size()) {
        partitions += " script";
    }

    if (Titanfall::Ent::snd.size()) {
        partitions += " snd";
    }

    if (Titanfall::Ent::spawn.size()) {
        partitions += " spawn";
    }

    Titanfall::Bsp::entityPartitions = { partitions.begin(), partitions.end() + 1 };
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
        
        
        if (CHECK_FLAG(mesh.shaderInfo->surfaceFlags, S_VERTEX_UNLIT_TS)) {
            m.vertexType = 3;
            m.vertexOffset = Titanfall::Bsp::vertexUnlitTSVertices.size();
        } else if (CHECK_FLAG(mesh.shaderInfo->surfaceFlags,S_VERTEX_LIT_BUMP)) {
            m.vertexType = 2;
            m.vertexOffset = Titanfall::Bsp::vertexLitBumpVertices.size();
        } else if (CHECK_FLAG(mesh.shaderInfo->surfaceFlags, S_VERTEX_UNLIT)) {
            m.vertexType = 1;
            m.vertexOffset = Titanfall::Bsp::vertexUnlitVertices.size();
        } else {
            m.vertexType = 0;
            m.vertexOffset = Titanfall::Bsp::vertexLitFlatVertices.size();
        }

        // Emit texture related structs
        uint32_t  textureIndex = Titanfall::EmitTextureData(*mesh.shaderInfo);
        m.materialOffset = Titanfall::EmitMaterialSort(textureIndex);
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
            Titanfall::Bsp::meshIndices.emplace_back(triangle + m.vertexOffset);
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
uint16_t Titanfall::EmitMaterialSort(uint32_t index) {
    /* Check if the material sort we need already exists */
    std::size_t pos = 0;
    for (Titanfall::MaterialSort_t &ms : Titanfall::Bsp::materialSorts) {
        if (ms.textureData == index) {
            return pos;
        }

        pos++;
    }

    Titanfall::MaterialSort_t &ms = Titanfall::Bsp::materialSorts.emplace_back();
    ms.textureData = index;

    return pos;
}


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
    float scale = 256;

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
    bool axials[6];
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
            axials[0] = true;  // +X
        } else if (normal[0] == -1.0f) {
            axials[1] = true;  // -X
        }

        if (normal[1] == 1.0f) {
            axials[2] = true;  // +Y
        } else if (normal[1] == -1.0f) {
            axials[3] = true;  // -Y
        }

        if (normal[2] == 1.0f) {
            axials[4] = true;  // +Z
        } else if (normal[2] == -1.0f) {
            axials[5] = true;  // -Z
        }
    }

    // TODO: Add correct material offset
    int test = 0;
    for (int i = 0; i < 6; i++) {
        if (axials[i]) {
            Titanfall::Bsp::cmBrushSideProperties.emplace_back(1);
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
            || normal[2] == -1.0f || normal[2] == 1.0f || (normal[2] == 0.0f && normal[0] == 0.0f)) {
            continue;  // axial, only test non-axial edges
        }

        Titanfall::EmitPlane(side.plane);
        b.planeCount++;
        Titanfall::Bsp::cmBrushSideProperties.emplace_back(1);
        uint16_t &so = Titanfall::Bsp::cmBrushSidePlaneOffsets.emplace_back();
        so = 0;
    }

    if (b.planeCount) {
        b.unknown = b.planeCount / 2 + 1;
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

/*
    EmitPlane
*/
void Titanfall::EmitPlane(const Plane3 &plane) {
    Titanfall::Bsp::planes.emplace_back(Plane3f(plane));
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


/*
    EmitStubs
    Emits the lumps we cant generate yet, but need for the map to boot
*/
void Titanfall::EmitStubs() {
    {  // Physics Collide
        constexpr std::array<uint8_t, 2315> data = {
            0x00, 0x00, 0x00, 0x00, 0x64, 0x08, 0x00, 0x00, 0x87, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
            0x60, 0x08, 0x00, 0x00, 0x56, 0x50, 0x48, 0x59, 0x00, 0x01, 0x00, 0x00, 0x44, 0x08, 0x00, 0x00,
            0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00,
            0x4D, 0x58, 0x3C, 0xB4, 0x23, 0x51, 0xF1, 0xB4, 0x94, 0x00, 0xCE, 0xB3, 0x8E, 0x46, 0x99, 0x41,
            0x8E, 0x46, 0x99, 0x41, 0x8E, 0x46, 0x99, 0x41, 0x7A, 0x0D, 0x09, 0x41, 0xCE, 0x44, 0x08, 0x00,
            0x10, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x56, 0x50, 0x53,
            0xE0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x15, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,
            0x00, 0xB0, 0x00, 0x01, 0x00, 0x00, 0x06, 0x00, 0x01, 0x00, 0x0F, 0x00, 0x02, 0x00, 0x08, 0x00,
            0x01, 0xA0, 0x00, 0x01, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 0x1B, 0x00, 0x01, 0x00, 0xFA, 0x7F,
            0x02, 0x90, 0x00, 0x02, 0x02, 0x00, 0x06, 0x00, 0x04, 0x00, 0x13, 0x00, 0x00, 0x00, 0xF8, 0x7F,
            0x03, 0x80, 0x00, 0x02, 0x02, 0x00, 0x06, 0x00, 0x05, 0x00, 0x20, 0x00, 0x04, 0x00, 0xFA, 0x7F,
            0x04, 0x70, 0x00, 0x01, 0x02, 0x00, 0xF1, 0x7F, 0x01, 0x00, 0x03, 0x00, 0x05, 0x00, 0xFA, 0x7F,
            0x05, 0x60, 0x00, 0x01, 0x05, 0x00, 0xFD, 0x7F, 0x01, 0x00, 0x0D, 0x00, 0x06, 0x00, 0x18, 0x00,
            0x06, 0x50, 0x00, 0x01, 0x00, 0x00, 0x06, 0x00, 0x07, 0x00, 0x0C, 0x00, 0x03, 0x00, 0xEA, 0x7F,
            0x07, 0x40, 0x00, 0x01, 0x00, 0x00, 0xED, 0x7F, 0x04, 0x00, 0x0B, 0x00, 0x07, 0x00, 0xFA, 0x7F,
            0x08, 0x30, 0x00, 0x01, 0x01, 0x00, 0xE5, 0x7F, 0x03, 0x00, 0x03, 0x00, 0x06, 0x00, 0xF3, 0x7F,
            0x09, 0x20, 0x00, 0x01, 0x06, 0x00, 0xFD, 0x7F, 0x03, 0x00, 0xF4, 0x7F, 0x07, 0x00, 0x04, 0x00,
            0x0A, 0x10, 0x00, 0x01, 0x07, 0x00, 0xF5, 0x7F, 0x04, 0x00, 0x03, 0x00, 0x06, 0x00, 0xFC, 0x7F,
            0x0B, 0x00, 0x00, 0x01, 0x06, 0x00, 0xFD, 0x7F, 0x04, 0x00, 0xE0, 0x7F, 0x05, 0x00, 0xE8, 0x7F,
            0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x15, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,
            0x00, 0xB0, 0x00, 0x02, 0x08, 0x00, 0x06, 0x00, 0x09, 0x00, 0x0F, 0x00, 0x0A, 0x00, 0x08, 0x00,
            0x01, 0xA0, 0x00, 0x02, 0x08, 0x00, 0x16, 0x00, 0x0B, 0x00, 0x1B, 0x00, 0x09, 0x00, 0xFA, 0x7F,
            0x02, 0x90, 0x00, 0x01, 0x0A, 0x00, 0x06, 0x00, 0x03, 0x00, 0x13, 0x00, 0x08, 0x00, 0xF8, 0x7F,
            0x03, 0x80, 0x00, 0x01, 0x0A, 0x00, 0x06, 0x00, 0x01, 0x00, 0x20, 0x00, 0x03, 0x00, 0xFA, 0x7F,
            0x04, 0x70, 0x00, 0x01, 0x0A, 0x00, 0xF1, 0x7F, 0x09, 0x00, 0x03, 0x00, 0x01, 0x00, 0xFA, 0x7F,
            0x05, 0x60, 0x00, 0x01, 0x01, 0x00, 0xFD, 0x7F, 0x09, 0x00, 0x0D, 0x00, 0x0C, 0x00, 0x18, 0x00,
            0x06, 0x50, 0x00, 0x01, 0x08, 0x00, 0x06, 0x00, 0x0D, 0x00, 0x0C, 0x00, 0x0B, 0x00, 0xEA, 0x7F,
            0x07, 0x40, 0x00, 0x01, 0x08, 0x00, 0xED, 0x7F, 0x03, 0x00, 0x0B, 0x00, 0x0D, 0x00, 0xFA, 0x7F,
            0x08, 0x30, 0x00, 0x01, 0x09, 0x00, 0xE5, 0x7F, 0x0B, 0x00, 0x03, 0x00, 0x0C, 0x00, 0xF3, 0x7F,
            0x09, 0x20, 0x00, 0x01, 0x0C, 0x00, 0xFD, 0x7F, 0x0B, 0x00, 0xF4, 0x7F, 0x0D, 0x00, 0x04, 0x00,
            0x0A, 0x10, 0x00, 0x01, 0x0D, 0x00, 0xF5, 0x7F, 0x03, 0x00, 0x03, 0x00, 0x0C, 0x00, 0xFC, 0x7F,
            0x0B, 0x00, 0x00, 0x01, 0x0C, 0x00, 0xFD, 0x7F, 0x03, 0x00, 0xE0, 0x7F, 0x01, 0x00, 0xE8, 0x7F,
            0x40, 0x03, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x15, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,
            0x00, 0xB0, 0x00, 0x01, 0x0E, 0x00, 0x06, 0x00, 0x0D, 0x00, 0x0F, 0x00, 0x03, 0x00, 0x08, 0x00,
            0x01, 0xA0, 0x00, 0x01, 0x0E, 0x00, 0x16, 0x00, 0x0F, 0x00, 0x1B, 0x00, 0x0D, 0x00, 0xFA, 0x7F,
            0x02, 0x90, 0x00, 0x01, 0x03, 0x00, 0x06, 0x00, 0x10, 0x00, 0x13, 0x00, 0x0E, 0x00, 0xF8, 0x7F,
            0x03, 0x80, 0x00, 0x01, 0x03, 0x00, 0x06, 0x00, 0x07, 0x00, 0x20, 0x00, 0x10, 0x00, 0xFA, 0x7F,
            0x04, 0x70, 0x00, 0x01, 0x03, 0x00, 0xF1, 0x7F, 0x0D, 0x00, 0x03, 0x00, 0x07, 0x00, 0xFA, 0x7F,
            0x05, 0x60, 0x00, 0x01, 0x07, 0x00, 0xFD, 0x7F, 0x0D, 0x00, 0x0D, 0x00, 0x11, 0x00, 0x18, 0x00,
            0x06, 0x50, 0x00, 0x02, 0x0E, 0x00, 0x06, 0x00, 0x12, 0x00, 0x0C, 0x00, 0x0F, 0x00, 0xEA, 0x7F,
            0x07, 0x40, 0x00, 0x02, 0x0E, 0x00, 0xED, 0x7F, 0x10, 0x00, 0x0B, 0x00, 0x12, 0x00, 0xFA, 0x7F,
            0x08, 0x30, 0x00, 0x01, 0x0D, 0x00, 0xE5, 0x7F, 0x0F, 0x00, 0x03, 0x00, 0x11, 0x00, 0xF3, 0x7F,
            0x09, 0x20, 0x00, 0x01, 0x11, 0x00, 0xFD, 0x7F, 0x0F, 0x00, 0xF4, 0x7F, 0x12, 0x00, 0x04, 0x00,
            0x0A, 0x10, 0x00, 0x01, 0x12, 0x00, 0xF5, 0x7F, 0x10, 0x00, 0x03, 0x00, 0x11, 0x00, 0xFC, 0x7F,
            0x0B, 0x00, 0x00, 0x01, 0x11, 0x00, 0xFD, 0x7F, 0x10, 0x00, 0xE0, 0x7F, 0x07, 0x00, 0xE8, 0x7F,
            0x70, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x15, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,
            0x00, 0xB0, 0x00, 0x01, 0x0D, 0x00, 0x06, 0x00, 0x13, 0x00, 0x0F, 0x00, 0x0C, 0x00, 0x08, 0x00,
            0x01, 0xA0, 0x00, 0x01, 0x0D, 0x00, 0x16, 0x00, 0x14, 0x00, 0x1B, 0x00, 0x13, 0x00, 0xFA, 0x7F,
            0x02, 0x90, 0x00, 0x01, 0x0C, 0x00, 0x06, 0x00, 0x11, 0x00, 0x13, 0x00, 0x0D, 0x00, 0xF8, 0x7F,
            0x03, 0x80, 0x00, 0x01, 0x0C, 0x00, 0x06, 0x00, 0x15, 0x00, 0x20, 0x00, 0x11, 0x00, 0xFA, 0x7F,
            0x04, 0x70, 0x00, 0x01, 0x0C, 0x00, 0xF1, 0x7F, 0x13, 0x00, 0x03, 0x00, 0x15, 0x00, 0xFA, 0x7F,
            0x05, 0x60, 0x00, 0x01, 0x15, 0x00, 0xFD, 0x7F, 0x13, 0x00, 0x0D, 0x00, 0x16, 0x00, 0x18, 0x00,
            0x06, 0x50, 0x00, 0x01, 0x0D, 0x00, 0x06, 0x00, 0x17, 0x00, 0x0C, 0x00, 0x14, 0x00, 0xEA, 0x7F,
            0x07, 0x40, 0x00, 0x01, 0x0D, 0x00, 0xED, 0x7F, 0x11, 0x00, 0x0B, 0x00, 0x17, 0x00, 0xFA, 0x7F,
            0x08, 0x30, 0x00, 0x02, 0x13, 0x00, 0xE5, 0x7F, 0x14, 0x00, 0x03, 0x00, 0x16, 0x00, 0xF3, 0x7F,
            0x09, 0x20, 0x00, 0x02, 0x16, 0x00, 0xFD, 0x7F, 0x14, 0x00, 0xF4, 0x7F, 0x17, 0x00, 0x04, 0x00,
            0x0A, 0x10, 0x00, 0x01, 0x17, 0x00, 0xF5, 0x7F, 0x11, 0x00, 0x03, 0x00, 0x16, 0x00, 0xFC, 0x7F,
            0x0B, 0x00, 0x00, 0x01, 0x16, 0x00, 0xFD, 0x7F, 0x11, 0x00, 0xE0, 0x7F, 0x15, 0x00, 0xE8, 0x7F,
            0xA0, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x15, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,
            0x00, 0xB0, 0x00, 0x01, 0x07, 0x00, 0x06, 0x00, 0x15, 0x00, 0x0F, 0x00, 0x06, 0x00, 0x08, 0x00,
            0x01, 0xA0, 0x00, 0x01, 0x07, 0x00, 0x16, 0x00, 0x11, 0x00, 0x1B, 0x00, 0x15, 0x00, 0xFA, 0x7F,
            0x02, 0x90, 0x00, 0x01, 0x06, 0x00, 0x06, 0x00, 0x18, 0x00, 0x13, 0x00, 0x07, 0x00, 0xF8, 0x7F,
            0x03, 0x80, 0x00, 0x01, 0x06, 0x00, 0x06, 0x00, 0x19, 0x00, 0x20, 0x00, 0x18, 0x00, 0xFA, 0x7F,
            0x04, 0x70, 0x00, 0x01, 0x06, 0x00, 0xF1, 0x7F, 0x15, 0x00, 0x03, 0x00, 0x19, 0x00, 0xFA, 0x7F,
            0x05, 0x60, 0x00, 0x01, 0x19, 0x00, 0xFD, 0x7F, 0x15, 0x00, 0x0D, 0x00, 0x1A, 0x00, 0x18, 0x00,
            0x06, 0x50, 0x00, 0x01, 0x07, 0x00, 0x06, 0x00, 0x1B, 0x00, 0x0C, 0x00, 0x11, 0x00, 0xEA, 0x7F,
            0x07, 0x40, 0x00, 0x01, 0x07, 0x00, 0xED, 0x7F, 0x18, 0x00, 0x0B, 0x00, 0x1B, 0x00, 0xFA, 0x7F,
            0x08, 0x30, 0x00, 0x01, 0x15, 0x00, 0xE5, 0x7F, 0x11, 0x00, 0x03, 0x00, 0x1A, 0x00, 0xF3, 0x7F,
            0x09, 0x20, 0x00, 0x01, 0x1A, 0x00, 0xFD, 0x7F, 0x11, 0x00, 0xF4, 0x7F, 0x1B, 0x00, 0x04, 0x00,
            0x0A, 0x10, 0x00, 0x02, 0x1B, 0x00, 0xF5, 0x7F, 0x18, 0x00, 0x03, 0x00, 0x1A, 0x00, 0xFC, 0x7F,
            0x0B, 0x00, 0x00, 0x02, 0x1A, 0x00, 0xFD, 0x7F, 0x18, 0x00, 0xE0, 0x7F, 0x19, 0x00, 0xE8, 0x7F,
            0xD0, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x04, 0x15, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,
            0x00, 0xB0, 0x00, 0x01, 0x01, 0x00, 0x06, 0x00, 0x1C, 0x00, 0x0F, 0x00, 0x1D, 0x00, 0x08, 0x00,
            0x01, 0xA0, 0x00, 0x01, 0x01, 0x00, 0x16, 0x00, 0x0C, 0x00, 0x1B, 0x00, 0x1C, 0x00, 0xFA, 0x7F,
            0x02, 0x90, 0x00, 0x01, 0x1D, 0x00, 0x06, 0x00, 0x06, 0x00, 0x13, 0x00, 0x01, 0x00, 0xF8, 0x7F,
            0x03, 0x80, 0x00, 0x01, 0x1D, 0x00, 0x06, 0x00, 0x1E, 0x00, 0x20, 0x00, 0x06, 0x00, 0xFA, 0x7F,
            0x04, 0x70, 0x00, 0x02, 0x1D, 0x00, 0xF1, 0x7F, 0x1C, 0x00, 0x03, 0x00, 0x1E, 0x00, 0xFA, 0x7F,
            0x05, 0x60, 0x00, 0x02, 0x1E, 0x00, 0xFD, 0x7F, 0x1C, 0x00, 0x0D, 0x00, 0x1F, 0x00, 0x18, 0x00,
            0x06, 0x50, 0x00, 0x01, 0x01, 0x00, 0x06, 0x00, 0x15, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0xEA, 0x7F,
            0x07, 0x40, 0x00, 0x01, 0x01, 0x00, 0xED, 0x7F, 0x06, 0x00, 0x0B, 0x00, 0x15, 0x00, 0xFA, 0x7F,
            0x08, 0x30, 0x00, 0x01, 0x1C, 0x00, 0xE5, 0x7F, 0x0C, 0x00, 0x03, 0x00, 0x1F, 0x00, 0xF3, 0x7F,
            0x09, 0x20, 0x00, 0x01, 0x1F, 0x00, 0xFD, 0x7F, 0x0C, 0x00, 0xF4, 0x7F, 0x15, 0x00, 0x04, 0x00,
            0x0A, 0x10, 0x00, 0x01, 0x15, 0x00, 0xF5, 0x7F, 0x06, 0x00, 0x03, 0x00, 0x1F, 0x00, 0xFC, 0x7F,
            0x0B, 0x00, 0x00, 0x01, 0x1F, 0x00, 0xFD, 0x7F, 0x06, 0x00, 0xE0, 0x7F, 0x1E, 0x00, 0xE8, 0x7F,
            0xBF, 0x0E, 0x9C, 0x40, 0x5C, 0x8F, 0xA2, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0x5C, 0x8F, 0xA2, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0x5C, 0x8F, 0xA2, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0x5C, 0x8F, 0xA2, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0x5C, 0x8F, 0xA2, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0x5C, 0x8F, 0xA2, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0x5C, 0x8F, 0xA2, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0x5C, 0x8F, 0xA2, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0x5C, 0x8F, 0xA2, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0x5C, 0x8F, 0xA2, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0x5C, 0x8F, 0xA2, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0x5C, 0x8F, 0xA2, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0x5C, 0x8F, 0xA2, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0x5C, 0x8F, 0xA2, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0x5C, 0x8F, 0xA2, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0x5C, 0x8F, 0xA2, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0x5C, 0x8F, 0xA2, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0x5C, 0x8F, 0xA2, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0x5C, 0x8F, 0xA2, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0x5C, 0x8F, 0xA2, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0x5C, 0x8F, 0xA2, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0x5C, 0x8F, 0xA2, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0x00, 0x00, 0x00, 0x00,
            0x5C, 0x8F, 0xA2, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0x5C, 0x8F, 0xA2, 0xC0, 0xBF, 0x0E, 0x9C, 0x40, 0xBF, 0x0E, 0x9C, 0xC0, 0x00, 0x00, 0x00, 0x00,
            0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x1F, 0x06, 0x0D, 0x41, 0x91, 0x91, 0x91, 0x00, 0x70, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xA0, 0x32, 0xD2, 0x3D, 0xA0, 0x32, 0xD2, 0xBD, 0xA0, 0x32, 0xD2, 0xBD,
            0xF9, 0x2D, 0x0A, 0x41, 0x91, 0x91, 0x91, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xA0, 0x32, 0xD2, 0xBD, 0xA0, 0x32, 0xD2, 0xBD, 0xEF, 0x3C, 0x09, 0x41,
            0x8F, 0x92, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xF8, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x0E, 0x4F, 0x9F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x04, 0xB9, 0xDC, 0x40, 0xB1, 0x04, 0xB1, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xF0, 0xFB, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x0E, 0x4F, 0x9F, 0xC0, 0x04, 0xB9, 0xDC, 0x40, 0xB1, 0xB1, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00,
            0x34, 0xFA, 0xFF, 0xFF, 0x0E, 0x4F, 0x9F, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x04, 0xB9, 0xDC, 0x40, 0x04, 0xB1, 0xB1, 0x02, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xA0, 0x32, 0xD2, 0xBD, 0xA0, 0x32, 0xD2, 0x3D, 0xA0, 0x32, 0xD2, 0x3D, 0xF9, 0x2D, 0x0A, 0x41,
            0x91, 0x91, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6C, 0xFC, 0xFF, 0xFF, 0x0E, 0x4F, 0x9F, 0xC0,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xB9, 0xDC, 0x40, 0x04, 0xB1, 0xB1, 0x03,
            0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x32, 0xD2, 0x3D,
            0xA0, 0x32, 0xD2, 0x3D, 0xEF, 0x3C, 0x09, 0x41, 0x8F, 0x92, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xF4, 0xF8, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x4F, 0x9F, 0x40,
            0x04, 0xB9, 0xDC, 0x40, 0xB1, 0xB1, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x78, 0xFA, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0x0E, 0x4F, 0x9F, 0x40, 0x00, 0x00, 0x00, 0x00, 0x04, 0xB9, 0xDC, 0x40,
            0xB1, 0x04, 0xB1, 0x05, 0x73, 0x74, 0x61, 0x74, 0x69, 0x63, 0x73, 0x6F, 0x6C, 0x69, 0x64, 0x20,
            0x7B, 0x0A, 0x22, 0x69, 0x6E, 0x64, 0x65, 0x78, 0x22, 0x20, 0x22, 0x30, 0x22, 0x0A, 0x22, 0x63,
            0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x73, 0x22, 0x20, 0x22, 0x33, 0x33, 0x35, 0x37, 0x30, 0x38,
            0x31, 0x39, 0x22, 0x0A, 0x7D, 0x0A, 0x76, 0x69, 0x72, 0x74, 0x75, 0x61, 0x6C, 0x74, 0x65, 0x72,
            0x72, 0x61, 0x69, 0x6E, 0x20, 0x7B, 0x7D, 0x0A, 0x76, 0x69, 0x72, 0x74, 0x75, 0x61, 0x6C, 0x74,
            0x72, 0x69, 0x6D, 0x65, 0x73, 0x68, 0x65, 0x73, 0x20, 0x7B, 0x7D, 0x0A, 0x6D, 0x61, 0x74, 0x65,
            0x72, 0x69, 0x61, 0x6C, 0x74, 0x61, 0x62, 0x6C, 0x65, 0x20, 0x7B, 0x0A, 0x22, 0x64, 0x65, 0x66,
            0x61, 0x75, 0x6C, 0x74, 0x22, 0x20, 0x22, 0x31, 0x22, 0x0A, 0x22, 0x64, 0x65, 0x66, 0x61, 0x75,
            0x6C, 0x74, 0x22, 0x20, 0x22, 0x32, 0x22, 0x0A, 0x7D, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        Titanfall::Bsp::physicsCollide_stub = { data.begin(), data.end() };
    }
    {  // GameLump
        constexpr std::array<uint8_t, 40> data = {
            0x01, 0x00, 0x00, 0x00, 0x70, 0x72, 0x70, 0x73, 0x00, 0x00, 0x0C, 0x00, 0x0C, 0x26, 0x0C, 0x00,
            0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        Titanfall::Bsp::gameLump_stub = { data.begin(), data.end() };
    }
    {  // World lights
        constexpr std::array<uint8_t, 200> data = {
            0x00, 0x00, 0x20, 0xC2, 0x00, 0x00, 0x20, 0x42, 0x00, 0x00, 0xD0, 0x42, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x52, 0x15, 0x3F, 0x66, 0x0E, 0xFE, 0xBE,
            0x5B, 0xA1, 0x24, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x42, 0x00, 0x00, 0x40, 0x42, 0x00, 0x00, 0xD0, 0x42,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x43,
            0x00, 0x00, 0x80, 0x3F, 0x87, 0x40, 0xD5, 0xBB, 0x63, 0x89, 0x31, 0x37, 0x06, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        Titanfall::Bsp::worldLights_stub = { data.begin(), data.end() };
    }
    {  // Lightmap headers
        constexpr std::array<uint8_t, 8> data = {
            0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x00
        };
        Titanfall::Bsp::lightmapHeaders_stub = { data.begin(), data.end() };
    }
    {  // LightMap Data Sky
        for (std::size_t i = 0; i < 524288; i++) {
            Titanfall::Bsp::lightMapDataSky_stub.emplace_back(0);
        }
    }
    {  // CSM AABB Nodes
        constexpr std::array<uint8_t, 32> data = {
            0x71, 0xFD, 0x3F, 0xC3, 0x71, 0xFD, 0x3F, 0xC3, 0x71, 0xFD, 0x3F, 0xC3, 0x00, 0x00, 0x00, 0x00,
            0x71, 0xFD, 0x3F, 0x43, 0x71, 0xFD, 0x3F, 0x43, 0x71, 0xFD, 0x3F, 0x43, 0x00, 0x00, 0x00, 0x00
        };
        Titanfall::Bsp::csmAABBNodes_stub = { data.begin(), data.end() };
    }
    {  // Cell BSP Nodes
        constexpr std::array<uint8_t, 8> data = {
            0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00
        };
        Titanfall::Bsp::cellBSPNodes_stub = { data.begin(), data.end() };
    }
    {  // Cells
        constexpr std::array<uint8_t, 8> data = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF
        };
        Titanfall::Bsp::cells_stub = { data.begin(), data.end() };
    }
}

/*
    EmitExtraEntity
    Emits an entity into Titanfall::Ent::extra, only used when -onlyentities argument is used
*/
void Titanfall::EmitExtraEntity( entity_t &e ) {
    if( striEqual( e.classname(), "worldspawn" ) )
        return;

    Titanfall::EmitTriggerBrushPlaneKeyValues( e );

    StringOutputStream  data;
    data << "{\n";
    for( const epair_t& pair : e.epairs ) {
        data << "\"" << pair.key.c_str() << "\" \"" << pair.value.c_str() << "\"\n";
    }
    data << "}\n";

    std::vector<char> str = { data.begin(), data.end() };

    Titanfall::Ent::extra.insert( Titanfall::Ent::extra.end(), str.begin(), str.end() );
}
