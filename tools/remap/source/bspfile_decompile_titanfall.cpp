#include "bspfile_decompile_titanfall.h"


void Titanfall::LoadLumpsAndEntities(const char* filename) {
	Sys_FPrintf( SYS_VRB, "Loading bsp file: \"%s\"\n", filename );

    // Load file into memory
    MemBuffer file = LoadFile( filename );

    rbspHeader_t* header = file.data();

    // Make sure magic matches the format we're trying to load
    if (!force && memcmp( header->ident, g_game->bspIdent, 4)) {
        Error("%s is not a %s file", filename, g_game->bspIdent);
    }
    // TODO: Make this game agnostic so we can for example import tfo maps while apex legends is our game
    if (!force && header->version != g_game->bspVersion) {
        Error("%s is version %d, not %d", filename, header->version, g_game->bspVersion);
    }

    // Load bsp lumps
    CopyLump((rbspHeader_t*)header, R1_LUMP_ENTITIES, Titanfall::Bsp::entities);
    CopyLump((rbspHeader_t*)header, R1_LUMP_PLANES, Titanfall::Bsp::planes);
    CopyLump((rbspHeader_t*)header, R1_LUMP_VERTICES, Titanfall::Bsp::vertices);
    CopyLump((rbspHeader_t*)header, R1_LUMP_TEXTURE_DATA, Titanfall::Bsp::textureData);
    CopyLump((rbspHeader_t*)header, R1_LUMP_ENTITY_PARTITIONS, Titanfall::Bsp::entityPartitions);
    CopyLump((rbspHeader_t*)header, R1_LUMP_TEXTURE_DATA_STRING_DATA, Titanfall::Bsp::textureDataData);
    CopyLump((rbspHeader_t*)header, R1_LUMP_TEXTURE_DATA_STRING_TABLE, Titanfall::Bsp::textureDataTable);
    CopyLump((rbspHeader_t*)header, R1_LUMP_ENTITY_PARTITIONS, Titanfall::Bsp::entityPartitions);
    CopyLump((rbspHeader_t*)header, R1_LUMP_TRICOLL_TRIS, Titanfall::Bsp::tricollTriangles);
    CopyLump((rbspHeader_t*)header, R1_LUMP_TRICOLL_HEADERS, Titanfall::Bsp::tricollHeaders);
    CopyLump((rbspHeader_t*)header, R1_LUMP_CM_GRID, Titanfall::Bsp::cmGrid);
    CopyLump((rbspHeader_t*)header, R1_LUMP_CM_GRID_CELLS, Titanfall::Bsp::cmGridCells);
    CopyLump((rbspHeader_t*)header, R1_LUMP_CM_GEO_SETS, Titanfall::Bsp::cmGeoSets);
    CopyLump((rbspHeader_t*)header, R1_LUMP_CM_PRIMITIVES, Titanfall::Bsp::cmPrimitives);
    CopyLump((rbspHeader_t*)header, R1_LUMP_CM_BRUSHES, Titanfall::Bsp::cmBrushes);
    CopyLump((rbspHeader_t*)header, R1_LUMP_CM_BRUSH_SIDE_PLANE_OFFSETS, Titanfall::Bsp::cmBrushSidePlaneOffsets);
    CopyLump((rbspHeader_t*)header, R1_LUMP_CM_BRUSH_SIDE_PROPS, Titanfall::Bsp::cmBrushSideProperties);

    // Load ent files
    {
        std::vector<std::string> partitions;

        {
            std::string partition;
            for( const char &c : Titanfall::Bsp::entityPartitions ) {
                if( c == ' ' || c == '\0' ) {
                    partition += ".ent\0";
                    partitions.emplace_back( partition );
                    partition.clear();
                    partition += "_";
                    continue;
                }

                partition += c;
            }
        }

        // Remove header
        partitions.erase(partitions.begin());
        
        for( std::string &partition : partitions ) {
            auto name = StringOutputStream(256)( PathExtensionless(filename), partition.c_str() );
            LoadEntFile( name.c_str(), Titanfall::Bsp::entities );
        }
    }
}


void Titanfall::ParseLoadedBSP() {
    ParseEntities();

    // Loop through entities
    for ( std::size_t i = 0; i < entities.size(); i++ ) {
        entity_t &e = entities.at(i);
        // worldspawn gets all the gridcells and the first model ( index 0 )
        if ( striEqual( e.classname(), "worldspawn" ) ) {
            Titanfall::ParseWorldspawn( e );
        }
    }


    entities.insert(entities.end(), Titanfall::Ent::entities.begin(), Titanfall::Ent::entities.end());
}


void Titanfall::ParseWorldspawn( entity_t &entity ) {
    std::list<int> brushIndicies;
    std::list<int> tricollIndicies;

    // Create a list of all brush indicies belonging to worldspawn
    std::size_t cells = Titanfall::Bsp::cmGrid.at(0).xCount * Titanfall::Bsp::cmGrid.at(0).yCount + 1;
    for( std::size_t i = 0; i < cells; i++ ) {
        Titanfall::CMGridCell_t &cell = Titanfall::Bsp::cmGridCells.at(i);

        for( uint16_t j = cell.start; j < cell.start + cell.count; j++ ) {
            Titanfall::CMGeoSet_t &set = Titanfall::Bsp::cmGeoSets.at(j);


            // Tricoll ( patches )
            #if 1
            if( set.collisionShapeType == 64 ) {
                Titanfall::TricollHeader_t &header = Titanfall::Bsp::tricollHeaders.at(set.collisionShapeIndex);

                int width, height;

                // Guess dimensions
                // We need to know the dimensions of the patch, but tricoll doesn't save them so we have to guess
                // This algorithm uses the surface area of the sides of the entire tricoll mesh to guess its
                // orientation ( patches are usually flat with bumps ). Once we know the orientation we try to match
                // vertices to corners. We can then use the indicies of these vertices to guess the dimensions.
                // If our dimensions dont match the totaly num of vertices we skip.
                #define X 0
                #define Y 1
                #define Z 2
                MinMax minmax;
                for( int i = 0; i < header.numVerts; i++ ) {
                    minmax.extend( Titanfall::Bsp::vertices.at( header.firstVert + i ) );
                }

                // Check surface area
                float surfaceAreaX, surfaceAreaY, surfaceAreaZ;
                surfaceAreaX = ( minmax.maxs[Y] - minmax.mins[Y] ) * ( minmax.maxs[Z] - minmax.mins[Z] );
                surfaceAreaY = ( minmax.maxs[X] - minmax.mins[X] ) * ( minmax.maxs[Z] - minmax.mins[Z] );
                surfaceAreaZ = ( minmax.maxs[Y] - minmax.mins[Y] ) * ( minmax.maxs[X] - minmax.mins[X] );

                //Sys_Printf("%f, %f, %f\n", surfaceAreaX, surfaceAreaY, surfaceAreaZ);
                // Since we want to only do math in 2D we need to ignore an axis
                int ignoreAxis = X;
                if( surfaceAreaY > surfaceAreaX && surfaceAreaY > surfaceAreaZ )
                    ignoreAxis = Y;
                if( surfaceAreaZ > surfaceAreaX && surfaceAreaZ > surfaceAreaY )
                    ignoreAxis = Z;
                
                Vector2 cornerLeft, cornerRight;
                if( ignoreAxis == X ) {
                    cornerLeft =  Vector2( minmax.mins[Y], minmax.maxs[Z] );
                    cornerRight = Vector2( minmax.maxs[Y], minmax.maxs[Z] );
                } else if( ignoreAxis == Y ) {
                    cornerLeft =  Vector2( minmax.mins[X], minmax.maxs[Z] );
                    cornerRight = Vector2( minmax.maxs[X], minmax.maxs[Z] );
                } else {
                    cornerLeft =  Vector2( minmax.mins[Y], minmax.maxs[X] );
                    cornerRight = Vector2( minmax.maxs[Y], minmax.maxs[X] );
                }

                int indexLeft, indexRight;
                for( int i = 0; i < header.numVerts; i++ ) {
                    Vector3 vertex3 = Titanfall::Bsp::vertices.at( header.firstVert + i );
                    Vector2 vertex2;
                    if( ignoreAxis == X ) {
                        vertex2 = Vector2( vertex3[Y], vertex3[Z] );
                    } else if( ignoreAxis == Y ) {
                        vertex2 = Vector2( vertex3[X], vertex3[Z] );
                    } else {
                        vertex2 = Vector2( vertex3[Y], vertex3[X] );
                    }

                    if( vector2_equal_epsilon( vertex2, cornerLeft, EQUAL_EPSILON) )
                        indexLeft = i;
                    if( vector2_equal_epsilon( vertex2, cornerRight, EQUAL_EPSILON) )
                        indexRight = i;
                }

                //Sys_Printf("%i: %i\n", width, height);

                height = indexRight - indexLeft;
                height = height < 0 ? height * -1 : height;
                height += 1;
                width = header.numVerts / height;

                #undef X
                #undef Y
                #undef Z

                if( width * height != header.numVerts || width < 2 || height < 2 ) {
                    Sys_FPrintf( SYS_WRN, "Couldn't get patch dimensions from tricoll header( %i * %i != %i )!\n\n", width, height, header.numVerts );
                    continue;
                }
                
                // Entity pointcloud ( dont run this on large maps )
                /*for (int v = 0; v < header.numVerts; v++) {
                    entity_t& e = Titanfall::Ent::entities.emplace_back();
                    StringOutputStream cs;
                    cs << "tricoll_header_" << ignoreAxis << "__" << indexLeft << "_" << indexRight;
                    e.setKeyValue("classname", cs.c_str());
                    StringOutputStream ss;
                    Vector3 &vec = Titanfall::Bsp::vertices.at(header.firstVert + v);
                    ss << vec.x() << " " << vec.y() << " " << vec.z();
                    e.setKeyValue("origin", ss.c_str());
                    //drawVerts[v].xyz = Titanfall::Bsp::vertices.at( header.firstVert + v );
                }*/
                

                /*
                int width, height;

                // Guess dimensions
                for( int d = header.numVerts - 1; d > 0; d-- ) {
                    if( header.numVerts % d == 0 ) {
                        width = d;
                        height = header.numVerts / d;

                        if( width * height == header.numVerts )
                            break;
                    }
                }
                
                // Check if valid
                // Editor no likey patches slimmer than 3 verts rn
                if( width * height != header.numVerts || width < 3 || height < 3 || width > 29 || height > 29 ) {
                    Sys_FPrintf( SYS_WRN, "Couldn't get patch dimensions from tricoll header( %i )!\n", set.collisionShapeIndex );
                    continue;
                }
                
                */
                // Build mesh
                
                mesh_t mesh;
                mesh.width = width;
                mesh.height = height;
                bspDrawVert_t *drawVerts = new bspDrawVert_t[header.numVerts];
                for( int v = 0; v < header.numVerts; v++ ) {
                    Vector3 vec = Titanfall::Bsp::vertices.at(header.firstVert + v);
                    //Sys_Printf("%f: %f: %f\n", vec[0], vec[1], vec[2]);
                    drawVerts[v].xyz = vec;
                }
                mesh.verts = drawVerts;

                // Create patch
                parseMesh_t* pm = safe_calloc(sizeof(*pm));
                //pm->entityNum = 0;
                //pm->brushNum = entity.patches->brushNum++;

                pm->shaderInfo = ShaderInfoForShader("NULL");

                //pm->longestCurve = 0.0f;
                //pm->maxIterations = 0;

                pm->mesh = mesh;

                pm->next = entity.patches;
                entity.patches = pm;
            }
            #endif

            if( set.collisionShapeType != 0 )
                continue;
            
            // Brushes
            if( set.primitiveCount > 1 ) {
                for( int k = 0; k < set.primitiveCount; k++ ) {
                    Titanfall::CMPrimitive_t &primitive = Titanfall::Bsp::cmPrimitives.at( set.collisionShapeIndex + k );
                    if( primitive.collisionShapeType != 0 )
                        continue;
                    
                    brushIndicies.push_back( primitive.collisionShapeIndex );
                }
            } else {
                brushIndicies.push_back( set.collisionShapeIndex );
            }
        }
    }

    brushIndicies.sort();
    brushIndicies.unique();
    
    for( int i : brushIndicies ) {
        Titanfall::CMBrush_t &brush = Titanfall::Bsp::cmBrushes.at( i );
        MinMax minmax;
        minmax.maxs = brush.origin + brush.extents;
        minmax.mins = brush.origin - brush.extents;

        std::vector<Plane3> planes = Titanfall::BuildPlanesFromMinMax( minmax );

        // Cutting planes
        for( int p = 0; p < brush.planeCount; p++ ) {
            Plane3f &plane = Titanfall::Bsp::planes.at( Titanfall::Bsp::cmGrid.at(0).brushSidePlaneOffset + brush.sidePlaneIndex + p - Titanfall::Bsp::cmBrushSidePlaneOffsets.at( brush.sidePlaneIndex + p ) );
            planes.emplace_back( plane );
        }

        // Create compiler brush, add planes to it and assign shaderInfo if possible
        brush_t &b = entity.brushes.emplace_back();
        for (std::size_t p = 0; p < planes.size(); p++ ) {
            Plane3 &plane = planes.at(p);

            side_t &side = b.sides.emplace_back();
            side.plane = plane;
            
            // Shader
            uint16_t &property = Titanfall::Bsp::cmBrushSideProperties.at( p + brush.index * 6 + brush.sidePlaneIndex );
            
            Titanfall::TextureData_t &textureData = Titanfall::Bsp::textureData.at( property & MASK_TEXTURE_DATA );
            uint32_t nameStart = Titanfall::Bsp::textureDataTable.at( textureData.name_index );

            std::string table = std::string( Titanfall::Bsp::textureDataData.begin(), Titanfall::Bsp::textureDataData.end() );

            // p0358 <3, my first favourite polish femboy
            std::string name = std::string( table.begin() + nameStart, table.begin() + nameStart + strlen( table.c_str() + nameStart ) );


            std::replace(name.begin(), name.end(), '\\', '/');
            
            side.shaderInfo = ShaderInfoForShader(name.c_str());
            
        }
    }
}


std::vector<Plane3> Titanfall::BuildPlanesFromMinMax( MinMax &minmax ) {
    std::vector<Plane3> planes;
    
    Vector3 vertices[8];
    vertices[0] = minmax.maxs;
    vertices[1] = Vector3(minmax.maxs.x(), minmax.maxs.y(), minmax.mins.z());
    vertices[2] = Vector3(minmax.maxs.x(), minmax.mins.y(), minmax.maxs.z());
    vertices[3] = Vector3(minmax.mins.x(), minmax.maxs.y(), minmax.maxs.z());
    vertices[4] = minmax.mins;
    vertices[5] = Vector3(minmax.mins.x(), minmax.mins.y(), minmax.maxs.z());
    vertices[6] = Vector3(minmax.mins.x(), minmax.maxs.y(), minmax.mins.z());
    vertices[7] = Vector3(minmax.maxs.x(), minmax.mins.y(), minmax.mins.z());

    // Create planes from AABB
    Plane3& plane3 = planes.emplace_back();
    PlaneFromPoints(plane3, vertices[6], vertices[5], vertices[4]);
    Plane3& plane0 = planes.emplace_back();
    PlaneFromPoints(plane0, vertices[0], vertices[1], vertices[2]);
    Plane3& plane5 = planes.emplace_back();
    PlaneFromPoints(plane5, vertices[5], vertices[7], vertices[4]);
    Plane3& plane2 = planes.emplace_back();
    PlaneFromPoints(plane2, vertices[0], vertices[3], vertices[1]);
    Plane3& plane4 = planes.emplace_back();
    PlaneFromPoints(plane4, vertices[7], vertices[6], vertices[4]);
    Plane3& plane1 = planes.emplace_back();
    PlaneFromPoints(plane1, vertices[0], vertices[2], vertices[3]);

    
    return planes;
}
