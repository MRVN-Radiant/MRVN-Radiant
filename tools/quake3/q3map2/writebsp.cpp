/* -------------------------------------------------------------------------------

   Copyright (C) 1999-2007 id Software, Inc. and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ----------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "q3map2.h"


/*
   EmitShader()
   emits a bsp shader entry
 */

int EmitShader( const char *shader, const int *contentFlags, const int *surfaceFlags ){
	shaderInfo_t    *si;


	/* handle special cases */
	if ( shader == NULL ) {
		shader = "noshader";
	}

	/* try to find an existing shader */
	for ( size_t i = 0; i < bspShaders.size(); ++i )
	{
		/* ydnar: handle custom surface/content flags */
		if ( surfaceFlags != NULL && bspShaders[ i ].surfaceFlags != *surfaceFlags ) {
			continue;
		}
		if ( contentFlags != NULL && bspShaders[ i ].contentFlags != *contentFlags ) {
			continue;
		}
		if ( !doingBSP ){
			si = ShaderInfoForShader( shader );
			if ( !strEmptyOrNull( si->remapShader ) ) {
				shader = si->remapShader;
			}
		}
		/* compare name */
		if ( striEqual( shader, bspShaders[ i ].shader ) ) {
			return i;
		}
	}

	/* get shaderinfo */
	si = ShaderInfoForShader( shader );

	/* emit a new shader */
	const int i = bspShaders.size(); // store index
	bspShader_t& bspShader = bspShaders.emplace_back();

	strcpy( bspShader.shader, si->shader );
	/* handle custom content/surface flags */
	bspShader.surfaceFlags = ( surfaceFlags != NULL )? *surfaceFlags : si->surfaceFlags;
	bspShader.contentFlags = ( contentFlags != NULL )? *contentFlags : si->contentFlags;

	/* recursively emit any damage shaders */
	if ( !strEmptyOrNull( si->damageShader ) ) {
		Sys_FPrintf( SYS_VRB, "Shader %s has damage shader %s\n", si->shader.c_str(), si->damageShader );
		EmitShader( si->damageShader, NULL, NULL );
	}

	/* return index */
	return i;
}



/*
   EmitPlanes()
   there is no opportunity to discard planes, because all of the original
   brushes will be saved in the map
 */

static void EmitPlanes(){
	bspPlanes.reserve( mapplanes.size() );
	/* walk plane list */
	for ( const plane_t& plane : mapplanes )
	{
		bspPlanes.push_back( plane.plane );
	}

	/* emit some statistics */
	Sys_FPrintf( SYS_VRB, "%9zu BSP planes\n", bspPlanes.size() );
}



/*
   EmitLeaf()
   emits a leafnode to the bsp file
 */

static void EmitLeaf( node_t *node ){
	bspLeaf_t& leaf = bspLeafs.emplace_back();

	leaf.cluster = node->cluster;
	leaf.area = node->area;

	/* emit bounding box */
	leaf.minmax.maxs = node->minmax.maxs;
	leaf.minmax.mins = node->minmax.mins;

	/* emit leaf brushes */
	leaf.firstBSPLeafBrush = bspLeafBrushes.size();
	for ( const brush_t& b : node->brushlist )
	{
		bspLeafBrushes.push_back( b.original->outputNum );
	}

	leaf.numBSPLeafBrushes = bspLeafBrushes.size() - leaf.firstBSPLeafBrush;

	/* emit leaf surfaces */
	if ( node->opaque ) {
		return;
	}

	/* add the drawSurfRef_t drawsurfs */
	leaf.firstBSPLeafSurface = bspLeafSurfaces.size();
	for ( const drawSurfRef_t *dsr = node->drawSurfReferences; dsr; dsr = dsr->nextRef )
	{
		bspLeafSurfaces.push_back( dsr->outputNum );
	}

	leaf.numBSPLeafSurfaces = bspLeafSurfaces.size() - leaf.firstBSPLeafSurface;
}


/*
   EmitDrawNode_r()
   recursively emit the bsp nodes
 */

static int EmitDrawNode_r( node_t *node ){
	/* check for leafnode */
	if ( node->planenum == PLANENUM_LEAF ) {
		EmitLeaf( node );
		return -int( bspLeafs.size() );
	}

	/* emit a node */
	const int id = bspNodes.size();
	{
		bspNode_t& bnode = bspNodes.emplace_back();

		bnode.minmax.mins = node->minmax.mins;
		bnode.minmax.maxs = node->minmax.maxs;

		if ( node->planenum & 1 ) {
			Error( "WriteDrawNodes_r: odd planenum" );
		}
		bnode.planeNum = node->planenum;
	}

	//
	// recursively output the other nodes
	//
	for ( int i = 0; i < 2; i++ )
	{
		// reference node by id, as it may be reallocated
		if ( node->children[i]->planenum == PLANENUM_LEAF ) {
			bspNodes[id].children[i] = -int( bspLeafs.size() + 1 );
			EmitLeaf( node->children[i] );
		}
		else
		{
			bspNodes[id].children[i] = bspNodes.size();
			EmitDrawNode_r( node->children[i] );
		}
	}

	return id;
}



/*
   ============
   SetModelNumbers
   ============
 */
void SetModelNumbers(){
	int models = 1;
	for ( std::size_t i = 1; i < entities.size(); ++i ) {
		if ( !entities[i].brushes.empty() || entities[i].patches ) {
			char value[16];
			sprintf( value, "*%i", models );
			models++;
			entities[i].setKeyValue( "model", value );
		}
	}

}




/*
   SetLightStyles()
   sets style keys for entity lights
 */

void SetLightStyles(){
	int j, numStyles;
	char value[ 10 ];
	char lightTargets[ MAX_SWITCHED_LIGHTS ][ 64 ];
	int lightStyles[ MAX_SWITCHED_LIGHTS ];
	int numStrippedLights = 0;

	/* -keeplights option: force lights to be kept and ignore what the map file says */
	if ( keepLights ) {
		entities[0].setKeyValue( "_keepLights", "1" );
	}

	/* ydnar: determine if we keep lights in the bsp */
	entities[ 0 ].read_keyvalue( keepLights, "_keepLights" );

	/* any light that is controlled (has a targetname) must have a unique style number generated for it */
	numStyles = 0;
	for ( std::size_t i = 1; i < entities.size(); ++i )
	{
		entity_t& e = entities[ i ];

		if ( !e.classname_prefixed( "light" ) ) {
			continue;
		}
		const char *t;
		if ( !e.read_keyvalue( t, "targetname" ) ) {
			/* ydnar: strip the light from the BSP file */
			if ( !keepLights ) {
				e.epairs.clear();
				numStrippedLights++;
			}

			/* next light */
			continue;
		}

		/* get existing style */
		const int style = e.intForKey( "style" );
		if ( style < LS_NORMAL || style > LS_NONE ) {
			Error( "Invalid lightstyle (%d) on entity %zu", style, i );
		}

		/* find this targetname */
		for ( j = 0; j < numStyles; j++ )
			if ( lightStyles[ j ] == style && strEqual( lightTargets[ j ], t ) ) {
				break;
			}

		/* add a new style */
		if ( j >= numStyles ) {
			if ( numStyles == MAX_SWITCHED_LIGHTS ) {
				Error( "MAX_SWITCHED_LIGHTS (%d) exceeded, reduce the number of lights with targetnames", MAX_SWITCHED_LIGHTS );
			}
			strcpy( lightTargets[ j ], t );
			lightStyles[ j ] = style;
			numStyles++;
		}

		/* set explicit style */
		sprintf( value, "%d", 32 + j );
		e.setKeyValue( "style", value );

		/* set old style */
		if ( style != LS_NORMAL ) {
			sprintf( value, "%d", style );
			e.setKeyValue( "switch_style", value );
		}
	}

	/* emit some statistics */
	Sys_FPrintf( SYS_VRB, "%9d light entities stripped\n", numStrippedLights );
}



/*
   BeginBSPFile()
   starts a new bsp file
 */

void BeginBSPFile(){
}



/*
   EndBSPFile()
   finishes a new bsp and writes to disk
 */

void EndBSPFile( bool do_write ){
	Sys_FPrintf( SYS_VRB, "--- EndBSPFile ---\n" );

	EmitPlanes();

	numBSPEntities = entities.size();
	UnparseEntities();

	if ( do_write ) {
		/* write the surface extra file */
		WriteSurfaceExtraFile( source );

		/* write the bsp */
		auto path = StringOutputStream( 256 )( source, ".bsp" );
		Sys_Printf( "Writing %s\n", path.c_str() );
		WriteBSPFile( path );
	}
}



/*
   EmitBrushes()
   writes the brush list to the bsp
 */

void EmitBrushes( brushlist_t& brushes, int *firstBrush, int *numBrushes ){
	/* set initial brush */
	if ( firstBrush != NULL ) {
		*firstBrush = bspBrushes.size();
	}
	if ( numBrushes != NULL ) {
		*numBrushes = 0;
	}

	/* walk list of brushes */
	for ( brush_t& b : brushes )
	{
		/* get bsp brush */
		b.outputNum = bspBrushes.size();
		bspBrush_t& db = bspBrushes.emplace_back();
		if ( numBrushes != NULL ) {
			( *numBrushes )++;
		}

		db.shaderNum = EmitShader( b.contentShader->shader, &b.contentShader->contentFlags, &b.contentShader->surfaceFlags );
		db.firstSide = bspBrushSides.size();

		/* walk sides */
		db.numSides = 0;
		for ( side_t& side : b.sides )
		{
			/* set output number to bogus initially */
			side.outputNum = -1;

			/* emit side */
			side.outputNum = bspBrushSides.size();
			bspBrushSide_t& cp = bspBrushSides.emplace_back();
			db.numSides++;
			cp.planeNum = side.planenum;

			/* emit shader */
			if ( side.shaderInfo ) {
				cp.shaderNum = EmitShader( side.shaderInfo->shader, &side.shaderInfo->contentFlags, &side.shaderInfo->surfaceFlags );
			}
			else if( side.bevel ) { /* emit surfaceFlags for bevels to get correct physics at walkable brush edges and vertices */
				cp.shaderNum = EmitShader( NULL, NULL, &side.surfaceFlags );
			}
			else{
				cp.shaderNum = EmitShader( NULL, NULL, NULL );
			}
		}
	}
}




/*
   EmitEntityPartitions()
   writes the entitiy partitions
 */
void EmitEntityPartitions()
{
	bspEntityPartitions_t& p = bspEntityPartitions.emplace_back();
	memcpy(p.partitions, "01* env fx script snd spawn", 27);
}

/* helpers */
bool VertexLarger( Vector3 a, Vector3 b )
{
	if (a.x() > b.x())
		if (a.y() > b.y())
			if (a.z() > b.z())
				return true;

	return false;
}

struct tempMesh_t
{
	MinMax minmax;
	String64 shader;
	/* Parallel */
	std::vector<Vector3> Vertices;
	std::vector<Vector3> Normals;
	std::vector<Vector2> UVs;

	std::vector<uint16_t> Triangles;
};

bool FloatLargerEqual( float a, float b )
{
	return ( a > b ) || fabs( a - b ) < EQUAL_EPSILON;
}

bool FloatSmallerEqual( float a, float b )
{
	return ( a < b ) || fabs( a - b ) < EQUAL_EPSILON;
}

// I do indeed hate this
// This crap is funny as hell
bool MinMaxIntersecting( MinMax a, MinMax b)
{
	return	( FloatLargerEqual( a.maxs.x(), b.mins.x() ) && FloatSmallerEqual( a.maxs.x(), b.maxs.x() ) ) ||
			( FloatLargerEqual( a.mins.x(), b.mins.x() ) && FloatSmallerEqual( a.mins.x(), b.maxs.x() ) ) ||
			( FloatLargerEqual( a.maxs.y(), b.mins.y() ) && FloatSmallerEqual( a.maxs.y(), b.maxs.y() ) ) ||
			( FloatLargerEqual( a.mins.y(), b.mins.y() ) && FloatSmallerEqual( a.mins.y(), b.maxs.y() ) ) || 
			( FloatLargerEqual( a.maxs.z(), b.mins.z() ) && FloatSmallerEqual( a.maxs.z(), b.maxs.z() ) ) ||
			( FloatLargerEqual( a.mins.z(), b.mins.z() ) && FloatSmallerEqual( a.mins.z(), b.maxs.z() ) );
}

bool VectorsEqual( Vector3 a, Vector3 b )
{
	return	(fabs(a.x() - b.x()) < EQUAL_EPSILON) &&
			(fabs(a.y() - b.y()) < EQUAL_EPSILON) &&
			(fabs(a.z() - b.z()) < EQUAL_EPSILON);

}

float VectorDotProduct(Vector3 v1, Vector3 v2) {
	return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
}

/*
   EmitMeshes()
   writes the mesh list to the bsp
 */
void EmitMeshes( const entity_t& e )
{
	/*
		Each side in radiant can have a different material
		while in the bsp we can only have one material per mesh.

		We get around this by first spliting all brushes/patches into sides,
		then combining the ones with the same material which are touching into
		one bspMesh_t.
	*/
	
	std::vector<tempMesh_t> tempMeshes;
	/* walk list of brushes */
	for (const brush_t& brush : e.brushes)
	{
		/* loop through sides */
		for (const side_t& side : brush.sides)
		{
			if (strcmp(side.shaderInfo->shader.c_str(), "textures/common/caulk") == 0)
				continue;


			tempMesh_t& mesh = tempMeshes.emplace_back();
			mesh.shader = side.shaderInfo->shader;
			/* loop through vertices */
			for (const Vector3& vertex : side.winding)
			{
				/* Check against aabb */
				/* Very cool piece of code I'm really proud of :) */
				if (vertex.x() > mesh.minmax.maxs.x())
					mesh.minmax.maxs.x() = vertex.x();
				if (vertex.y() > mesh.minmax.maxs.y())
					mesh.minmax.maxs.y() = vertex.y();
				if (vertex.z() > mesh.minmax.maxs.z())
					mesh.minmax.maxs.z() = vertex.z();
				
				if (vertex.x() < mesh.minmax.mins.x())
					mesh.minmax.mins.x() = vertex.x();
				if (vertex.y() < mesh.minmax.mins.y())
					mesh.minmax.mins.y() = vertex.y();
				if (vertex.z() < mesh.minmax.mins.z())
					mesh.minmax.mins.z() = vertex.z();

				/* Calculate it's UV */
				/* or not */
				Vector2 UV;
				UV.x() = VectorDotProduct( vertex, side.texMat[0] );
				UV.y() = VectorDotProduct( vertex, side.texMat[1] );

				//UV.x() /= mesh.minmax.maxs.x() - mesh.minmax.mins.x();
				//UV.y() /= mesh.minmax.maxs.z() - mesh.minmax.mins.z();

				
				/* Calculate it's normal */
				Vector3 normal;
				std::vector<Vector3> sideNormals;
				for (const side_t& s : brush.sides)
				{
					for (const Vector3& v : s.winding)
					{
						if (VectorCompare(vertex, v))
						{
							normal = Vector3(s.plane.a, s.plane.b, s.plane.c);
							VectorNormalize(normal);
							sideNormals.push_back(normal);
							break;
						}
					}
				}
				
				for (const Vector3& n : sideNormals)
				{
					normal = Vector3(n.x() + normal.x(), n.y() + normal.y(), n.z() + normal.z());
				}
				
				
				VectorNormalize(normal);

				/* save */
				mesh.Vertices.emplace_back(vertex);
				mesh.Normals.emplace_back(normal);
				mesh.UVs.emplace_back(UV);
			}

			/* Create triangles for side */
			for (std::size_t i = 0; i < side.winding.size() - 2; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					int vert_index = j == 0 ? 0 : i + j;
					mesh.Triangles.emplace_back(vert_index);
				}
			}
		}
	}

	/* walk list of patches */
	/* one day */
	/*
	parseMesh_t* patch;
	while (patch != nullptr)
	{
		patch = patch->next;
	}
	*/

	/* Loop through meshes and combine the ones touching */
	/* While this isn't fast, it's better than my last approach */
	std::size_t index = 0;
	std::size_t iterationsSinceCombine = 0;
	while ( true )
	{
		/* std::out_of_range can't comprehend this  */
		if ( index >= tempMeshes.size() )
			index = 0;

		/* No more meshes to combine; break from teh loop */
		if ( iterationsSinceCombine >= tempMeshes.size() )
			break;

		/* Get mesh which we then compare to the rest, maybe combine, maybe not */
		tempMesh_t &mesh1 = tempMeshes.at( index );
		
		for ( std::size_t i = 0; i < tempMeshes.size(); i++ )
		{
			/* We dont want to compare the same mesh */
			if ( index == i )
				continue;
			//Sys_FPrintf(SYS_VRB, "false\n");
			tempMesh_t &mesh2 = tempMeshes.at(i);
			
			//Sys_FPrintf(SYS_VRB, "mesh1: maxs: %f, %f, %f; mins: %f, %f, %f\n", mesh1.minmax.maxs.x(), mesh1.minmax.maxs.y(), mesh1.minmax.maxs.z(), mesh1.minmax.mins.x(), mesh1.minmax.mins.y(), mesh1.minmax.mins.z());
			//Sys_FPrintf(SYS_VRB, "mesh2: maxs: %f, %f, %f; mins: %f, %f, %f\n", mesh2.minmax.maxs.x(), mesh2.minmax.maxs.y(), mesh2.minmax.maxs.z(), mesh2.minmax.mins.x(), mesh2.minmax.mins.y(), mesh2.minmax.mins.z());

			/* check if they have the same shader */
			if ( strcmp( mesh1.shader.c_str(), mesh2.shader.c_str() ) != 0 )
				continue;


			/* Check if they're intersecting */
			if ( !MinMaxIntersecting( mesh1.minmax, mesh2.minmax ) )
				continue;
			//Sys_FPrintf(SYS_VRB, "true\n");
			/* Combine them */
			//if (mesh2.minmax.maxs.x() > mesh1.minmax.maxs.x() && mesh2.minmax.maxs.y() > mesh1.minmax.maxs.y() && mesh2.minmax.maxs.z() > mesh1.minmax.maxs.z())
			//	mesh1.minmax.maxs = mesh2.minmax.maxs;

			//if (mesh2.minmax.mins.x() < mesh1.minmax.mins.x() && mesh2.minmax.maxs.y() < mesh1.minmax.mins.y() && mesh2.minmax.mins.z() < mesh1.minmax.mins.z())
			//	mesh1.minmax.mins = mesh2.minmax.mins;

			/* Triangles */
			for ( uint16_t& triIndex : mesh2.Triangles )
			{
				mesh1.Triangles.emplace_back( triIndex + mesh1.Vertices.size() );
			}

			/* All these are parallel so we chillin */
			for ( std::size_t v = 0; v < mesh2.Vertices.size(); v++ )
			{
				mesh1.Vertices.emplace_back(mesh2.Vertices.at(v));
				mesh1.Normals.emplace_back(mesh2.Normals.at(v));
				mesh1.UVs.emplace_back(mesh2.UVs.at(v));
			}

			for (Vector3& vertex : mesh1.Vertices)
			{
				if (vertex.x() > mesh1.minmax.maxs.x())
					mesh1.minmax.maxs.x() = vertex.x();
				if (vertex.y() > mesh1.minmax.maxs.y())
					mesh1.minmax.maxs.y() = vertex.y();
				if (vertex.z() > mesh1.minmax.maxs.z())
					mesh1.minmax.maxs.z() = vertex.z();

				if (vertex.x() < mesh1.minmax.mins.x())
					mesh1.minmax.mins.x() = vertex.x();
				if (vertex.y() < mesh1.minmax.mins.y())
					mesh1.minmax.mins.y() = vertex.y();
				if (vertex.z() < mesh1.minmax.mins.z())
					mesh1.minmax.mins.z() = vertex.z();
			}

			/* Delete mesh we combined as to not create duplicates */
			tempMeshes.erase( tempMeshes.begin() + i );
			iterationsSinceCombine = 0;
		}

		iterationsSinceCombine++;
		index++;
	}


	/* 
		We now have a list of meshes with matching materials.
		All that's left is converting into bsp structs :)
	*/

	Sys_FPrintf(SYS_VRB, "pain suffering\n");
	for (const tempMesh_t& tempMesh : tempMeshes)
	{
		bspMesh_t& mesh = bspMeshes.emplace_back();
		mesh.const0 = 4294967040; // :)
		mesh.first_vertex = bspVertexLitBump.size();
		mesh.vertex_count = tempMesh.Vertices.size();
		mesh.tri_offset = bspMeshIndices.size();
		mesh.tri_count = tempMesh.Triangles.size() / 3;

		MinMax aabb;

		/* Save vertices and vertexnormals */
		for (std::size_t i = 0; i < tempMesh.Vertices.size(); i++)
		{
			Vector3 vertex = tempMesh.Vertices.at(i);
			/* Check against aabb */
			if (vertex.x() > aabb.maxs.x())
				aabb.maxs.x() = vertex.x();
			if (vertex.y() > aabb.maxs.y())
				aabb.maxs.y() = vertex.y();
			if (vertex.z() > aabb.maxs.z())
				aabb.maxs.z() = vertex.z();

			if (vertex.x() < aabb.mins.x())
				aabb.mins.x() = vertex.x();
			if (vertex.y() < aabb.mins.y())
				aabb.mins.y() = vertex.y();
			if (vertex.z() < aabb.mins.z())
				aabb.mins.z() = vertex.z();

			bspVertexLitBump_t& litVertex = bspVertexLitBump.emplace_back();
			litVertex.minus_one = -1;
			litVertex.uv0 = tempMesh.UVs.at(i);

			/* Save vert */
			for (uint16_t j = 0; j < bspVertices.size(); j++)
			{
				if (VectorCompare(vertex, bspVertices.at(j)))
				{
					litVertex.vertex_index = j;
					goto normal;
				}
			}

			{
				litVertex.vertex_index = bspVertices.size();
				bspVertices.emplace_back(tempMesh.Vertices.at(i));
			}

			normal:;

			for (uint16_t j = 0; j < bspVertexNormals.size(); j++)
			{
				if (VectorCompare(tempMesh.Normals.at(i), bspVertexNormals.at(j)))
				{
					litVertex.normal_index = j;
					goto end;
				}
			}

			{
				litVertex.normal_index = bspVertexNormals.size();
				bspVertexNormals.emplace_back(tempMesh.Normals.at(i));
			}

			end:;
		}

		/* Save triangles */
		for (uint16_t triangle : tempMesh.Triangles)
		{
			for (uint32_t j = 0; j < bspVertexLitBump.size(); j++)
			{
				if (VectorCompare(bspVertices.at(bspVertexLitBump.at(j).vertex_index),tempMesh.Vertices.at(triangle)))
				{
					bspMeshIndex_t& index = bspMeshIndices.emplace_back();
					index = j;
					break;
				}
			}
		}

		/* Save MeshBounds */
		bspMeshBounds_t& meshBounds = bspMeshBounds.emplace_back();
		meshBounds.origin = (aabb.maxs + aabb.mins) / 2;
		meshBounds.extents = (aabb.maxs - aabb.mins) / 2;

		Sys_FPrintf(SYS_VRB, "mesh1: maxs: %f, %f, %f; mins: %f, %f, %f\n", aabb.maxs.x(), aabb.maxs.y(), aabb.maxs.z(), aabb.mins.x(), aabb.mins.y(), aabb.mins.z());
	}
}

void EmitModels()
{
	bspModel_t_new &m = bspModels_new.emplace_back();
	m.mesh_count = bspMeshes.size();
}

/*
   BeginModel()
   sets up a new brush model
 */

void BeginModel( const entity_t& e ){

}




/*
   EndModel()
   finish a model's processing
 */

void EndModel( const entity_t& e, node_t *headnode ){
	
}
