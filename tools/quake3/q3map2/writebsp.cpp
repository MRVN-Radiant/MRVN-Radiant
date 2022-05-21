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
	/* these values may actually be initialized if the file existed when loaded, so clear them explicitly */
	bspModels.clear();
	bspNodes.clear();
	bspBrushSides.clear();
	bspLeafSurfaces.clear();
	bspLeafBrushes.clear();

	/* leave leaf 0 as an error, because leafs are referenced as negative number nodes */
	bspLeafs.resize( 1 );

	/* ydnar: gs mods: set the first 6 drawindexes to 0 1 2 2 1 3 for triangles and quads */
	bspDrawIndexes = { 0, 1, 2, 0, 2, 3 };
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


float MatrixDeterminant(Vector3 a, Vector3 b, Vector3 c)
{
	double _d;
	_d = a.x() * (b.y() * c.z() - b.z() * c.y()) - a.y() * (b.x() * c.z() - b.z() * c.x()) - a.z() * (b.x() * c.y() - b.y() * c.x());

	return _d;
}


/*
   EmitMeshes()
   writes the mesh list to the bsp
 */
void EmitEntityPartitions()
{
	bspEntityPartitions_t& p = bspEntityPartitions.emplace_back();
	memcpy(p.partitions, "01* env fx script snd spawn", 27);
}

bool IsCloseEnough( Vector3 a, Vector3 b, float epsilon )
{
	if (fabs(a.x() - b.x()) < epsilon)
		if (fabs(a.y() - b.y()) < epsilon)
			if (fabs(a.z() - b.z()) < epsilon)
				return true;

	return false;
}

/*
   EmitMeshes()
   writes the mesh list to the bsp
 */
void EmitMeshes( const entity_t& e )
{
	/* As of now 1 brush = 1 mesh, this needs to change */
	

	/* walk list of brushes */
	for ( const brush_t &brush : e.brushes )
	{
		/* These are parallel */
		std::vector<bspVertices_t> meshVertices;
		std::vector<bspVertexNormals_t> meshVertexNormals;
		/* This isn't */
		std::vector<bspMeshIndices_t> meshIndices;


		/* Create Mesh entry */
		bspMeshes_t& mesh = bspMeshes.emplace_back();

		/* Create vertices and their normals for this brush */
		for ( const side_t &side : brush.sides )
		{
			/* Loop through vertices, only save unique ones */
			for ( const Vector3 &vertex : side.winding )
			{
				std::size_t index = 0;
				for ( bspVertices_t& v : meshVertices )
				{
					if ( IsCloseEnough( v.xyz, vertex, 0.001 ) )
						break;
					
					index++;
				}


				/* vertex doesn't exist, save it */
				if ( index == meshVertices.size() )
				{
					bspVertices_t &vert = meshVertices.emplace_back();
					vert.xyz = vertex;

					/* Calculate it's normal */
					std::vector<Vector3> sideNormals;
					for ( const side_t &s  : brush.sides )
					{
						for ( const Vector3 &v : s.winding )
						{
							if ( IsCloseEnough( vertex, v, 0.001 ) )
							{
								sideNormals.push_back( Vector3( s.plane.a, s.plane.b, s.plane.c ) );
								break;
							}
						}
					}
					
					Vector3 normal;
					for (const Vector3 &n : sideNormals)
					{
						normal = Vector3(n.x() + normal.x(), n.y() + normal.y(), n.z() + normal.z());
					}

					vector3_normalise( normal );

					bspVertexNormals_t &norm = meshVertexNormals.emplace_back();
					norm.xyz = normal;
				}
			}


			/* Make triangles for side */
			for (std::size_t i = 0; i < side.winding.size() - 2; i++)
			{
				for ( int j = 0; j < 3; j++ )
				{
					int vert_index = j == 0 ? 0 : i + j;

					Vector3 vertex;
					vertex = side.winding.at( vert_index );

					std::size_t index = 0;
					for ( bspVertices_t &v : meshVertices )
					{
						if ( IsCloseEnough( v.xyz, vertex, 0.001 ) )
							break;

						index++;
					}

					bspMeshIndices_t &mi = meshIndices.emplace_back();
					mi.index = index;
				}
			}
		}

		mesh.first_vertex = bspVertexLitBump.size();
		mesh.vertex_count = meshVertices.size();
		/* Merge into lumps */
		for ( uint32_t i = 0; i < meshVertices.size(); i++ )
		{
			bspVertices_t vertex = meshVertices.at( i );
			bspVertexNormals_t normal = meshVertexNormals.at( i );


			bspVertexLitBump_t& vlb = bspVertexLitBump.emplace_back();
			vlb.minus_one = -1;

			/* Save vertex */
			std::size_t vertex_index = 0;
			for ( bspVertices_t &v : bspVertices )
			{
				if ( IsCloseEnough( v.xyz, vertex.xyz, 0.001 ) )
					break;

				vertex_index++;
			}

			if ( vertex_index == bspVertices.size() )
			{
				bspVertices_t& vert = bspVertices.emplace_back();
				vert.xyz = vertex.xyz;
			}

			/* Save vertex normal */
			std::size_t normal_index = 0;
			for ( bspVertexNormals_t &n : bspVertexNormals )
			{
				if ( IsCloseEnough( n.xyz, normal.xyz, 0.001 ) )
					break;

				normal_index++;
			}

			if ( normal_index == bspVertexNormals.size() )
			{
				bspVertexNormals_t& norm = bspVertexNormals.emplace_back();
				norm.xyz = normal.xyz;
			}

			vlb.vertex_index = vertex_index;
			vlb.normal_index = normal_index;
		}

		mesh.tri_offset = bspMeshIndices.size();
		mesh.tri_count = meshIndices.size() / 3;
		/* Merge Mesh Indices */
		for ( bspMeshIndices_t &i : meshIndices )
		{
			bspVertices_t vertex;
			vertex.xyz = meshVertices.at( i.index ).xyz;

			uint16_t index = 0;
			for ( bspVertices_t& v : bspVertices )
			{
				if ( IsCloseEnough(v.xyz, vertex.xyz, 0.001 ) )
					break;

				index++;
			}

			bspMeshIndices_t &mi = bspMeshIndices.emplace_back();
			mi.index = index;
		}
	}
}


/*
   EmitFogs() - ydnar
   turns map fogs into bsp fogs
 */

void EmitFogs(){
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
	MinMax minmax;
	MinMax lgMinmax;          /* ydnar: lightgrid mins/maxs */

	/* bound the brushes */
	for ( const brush_t& b : e.brushes )
	{
		/* ignore non-real brushes (origin, etc) */
		if ( b.sides.empty() ) {
			continue;
		}
		minmax.extend( b.minmax );

		/* ydnar: lightgrid bounds */
		if ( b.compileFlags & C_LIGHTGRID ) {
			lgMinmax.extend( b.minmax );
		}
	}

	/* bound patches */
	for ( const parseMesh_t *p = e.patches; p; p = p->next )
	{
		for ( const bspDrawVert_t& vert : Span( p->mesh.verts, p->mesh.width * p->mesh.height ) )
			minmax.extend( vert.xyz );
	}

	/* get model */
	bspModel_t& mod = bspModels.emplace_back();

	/* ydnar: lightgrid mins/maxs */
	if ( lgMinmax.valid() ) {
		/* use lightgrid bounds */
		mod.minmax = lgMinmax;
	}
	else
	{
		/* use brush/patch bounds */
		mod.minmax = minmax;
	}

	/* note size */
	Sys_FPrintf( SYS_VRB, "BSP bounds: { %f %f %f } { %f %f %f }\n", minmax.mins[0], minmax.mins[1], minmax.mins[2], minmax.maxs[0], minmax.maxs[1], minmax.maxs[2] );
	if ( lgMinmax.valid() )
		Sys_FPrintf( SYS_VRB, "Lightgrid bounds: { %f %f %f } { %f %f %f }\n", lgMinmax.mins[0], lgMinmax.mins[1], lgMinmax.mins[2], lgMinmax.maxs[0], lgMinmax.maxs[1], lgMinmax.maxs[2] );

	/* set firsts */
	mod.firstBSPSurface = bspDrawSurfaces.size();
	mod.firstBSPBrush = bspBrushes.size();
}




/*
   EndModel()
   finish a model's processing
 */

void EndModel( const entity_t& e, node_t *headnode ){
	/* note it */
	Sys_FPrintf( SYS_VRB, "--- EndModel ---\n" );

	/* emit the bsp */
	bspModel_t& mod = bspModels.back();
	EmitDrawNode_r( headnode );

	/* set surfaces and brushes */
	mod.numBSPSurfaces = bspDrawSurfaces.size() - mod.firstBSPSurface;
	mod.firstBSPBrush = e.firstBrush;
	mod.numBSPBrushes = e.numBrushes;
}
