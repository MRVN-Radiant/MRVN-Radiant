
#include "remap.h"
#include "bspfile_abstract.h"



/*
	EmitMeshes()
	Emits shared meshes for a given entity
*/
void Shared::MakeMeshes( const entity_t &e )
{
	// Multiple entities can have meshes, make sure we clear before making new meshes
	Shared::meshes.clear();

	Sys_FPrintf( SYS_VRB, "--- SharedMeshes ---\n" );

	// Loop through brushes
	for ( const brush_t &brush : e.brushes )
	{
		// Loop through sides
		for ( const side_t &side : brush.sides )
		{
			// Skip bevels
			if ( side.bevel )
				continue;

			// Check flags
			if ( side.shaderInfo->compileFlags & C_NODRAW )
				continue;

			// Check for degenerate windings
			if( side.winding.size() < 3 ) {
				Sys_FPrintf( SYS_WRN, "        Skipping degenerate winding!\n" );
				continue;
			}			
			
			Shared::Mesh_t &mesh = Shared::meshes.emplace_back();
			mesh.shaderInfo = side.shaderInfo;

			Vector3 normal = side.plane.normal();

			// Loop through vertices
			for ( const Vector3 &vertex : side.winding )
			{
				// Extend AABB
				mesh.minmax.extend( vertex );

				// Texturing
				// This is taken from surface.cpp, It somewhat works
				// TODO: Make this work better
				Vector2 st;
				Vector3 vTranslated, texX, texY;
				float x, y;
				ComputeAxisBase(side.plane.normal(), texX, texY);
				vTranslated = vertex + e.originbrush_origin;
				x = vector3_dot(vTranslated, texX);
				y = vector3_dot(vTranslated, texY);
				st[0] = side.texMat[0][0] * x + side.texMat[0][1] * y + side.texMat[0][2];
				st[1] = side.texMat[1][0] * x + side.texMat[1][1] * y + side.texMat[1][2];
				
				
				Shared::Vertex_t &sv = mesh.vertices.emplace_back();
				sv.xyz = vertex;
				sv.st = st;
				sv.normal = normal;
			}

			// Create triangles for side
			for ( std::size_t i = 0; i < side.winding.size() - 2; i++ )
			{
				for ( int j = 0; j < 3; j++ )
				{
					int vert_index = j == 0 ? 0 : i + j;
					mesh.triangles.emplace_back( vert_index );
				}
			}
		}
	}

	// Loop through patches
	parseMesh_t* patch;
	patch = e.patches;
	while( patch != NULL ) {
		mesh_t patchMesh = patch->mesh;
		
		Shared::Mesh_t &mesh = Shared::meshes.emplace_back();
		mesh.shaderInfo = patch->shaderInfo;
		
		// Get vertices
		for( int index = 0; index < ( patchMesh.width * patchMesh.height ); index++ ) {
			Shared::Vertex_t &vertex = mesh.vertices.emplace_back();
				
			vertex.xyz = patchMesh.verts[ index ].xyz;
			vertex.normal = patchMesh.verts[ index ].normal;
			vertex.st = patchMesh.verts[ index ].st;

			mesh.minmax.extend( vertex.xyz );
		}
		
		// Make triangles
		for( int x = 0; x < ( patchMesh.width - 1 ); x++ ) {
			for( int y = 0; y < ( patchMesh.height - 1 ); y++ ) {
				int index = x + y * patchMesh.width;
				
				mesh.triangles.emplace_back( index );
				mesh.triangles.emplace_back( index + patchMesh.width );
				mesh.triangles.emplace_back( index + patchMesh.width + 1 );

				mesh.triangles.emplace_back( index );
				mesh.triangles.emplace_back( index + patchMesh.width + 1 );
				mesh.triangles.emplace_back( index + 1 );
			}
		}
		patch = patch->next;
	}
	

	// Combine all meshes based on shaderInfo and AABB tests
	std::size_t index = 0;
	std::size_t iterationsSinceCombine = 0;
	while ( true )
	{
		// std::out_of_range can't comprehend this
		if ( index >= Shared::meshes.size() )
			index = 0;

		// No more meshes to combine; break from the loop
		if ( iterationsSinceCombine >= Shared::meshes.size() )
			break;

		// Get mesh which we then compare to the rest, maybe combine, maybe not
		Shared::Mesh_t &mesh1 = Shared::meshes.at( index );

		for ( std::size_t i = 0; i < Shared::meshes.size(); i++ )
		{
			// We dont want to compare the same mesh
			if ( index == i )
				continue;

			Shared::Mesh_t &mesh2 = Shared::meshes.at( i );

			// Check if they have the same shader
			if ( !striEqual( mesh1.shaderInfo->shader.c_str(), mesh2.shaderInfo->shader.c_str() ) )
				continue;

			// Check if they're intersecting
			if ( !mesh1.minmax.test( mesh2.minmax ) )
				continue;


			// Combine them
			// Triangles
			for ( uint16_t &triIndex : mesh2.triangles )
				mesh1.triangles.emplace_back( triIndex + mesh1.vertices.size() );
			
			// Copy over vertices
			mesh1.vertices.insert( mesh1.vertices.end(), mesh2.vertices.begin(), mesh2.vertices.end() );

			// Update minmax
			mesh1.minmax.extend( mesh2.minmax );
			

			// Delete mesh we combined as to not create duplicates
			Shared::meshes.erase( Shared::meshes.begin() + i );
			iterationsSinceCombine = 0;
			break;
		}

		iterationsSinceCombine++;
		index++;
	}

	Sys_Printf( "%9zu shared meshes\n", Shared::meshes.size() );
}

void Shared::MakeVisReferences()
{
	/* Meshes */
	for ( std::size_t i = 0; i < Shared::meshes.size(); i++ )
	{
		Shared::Mesh_t &mesh = Shared::meshes.at( i );
		Shared::visRef_t &ref = Shared::visRefs.emplace_back();

		ref.minmax = mesh.minmax;
		ref.index = i;
	}

	/* Props */

	Sys_Printf( "%9zu shared vis references\n", Shared::visRefs.size() );
}

/*
	CalculateSAH
	calculates the surface-area-heurestic for a BVH2 Tree
*/
float CalculateSAH( std::vector<Shared::visRef_t> &refs, int axis, float pos ) {
	MinMax parentMinMax;
	MinMax childMinMaxs[2] = {};
	std::size_t childRefCounts[2] = {};

	// Count and create AABBs from references
	for ( Shared::visRef_t &ref : refs ) {
		parentMinMax.extend( ref.minmax );

		float refPos = ref.minmax.maxs[axis];
		if ( refPos < pos ) {
			childRefCounts[0]++;
			childMinMaxs[0].extend( ref.minmax );
		}
		else {
			childRefCounts[1]++;
			childMinMaxs[1].extend( ref.minmax );
		}
	}

	
	float parentArea = parentMinMax.area();

	float cost = .5f;
	for ( int i = 0; i < 2; i++ ) {
		if( childRefCounts[i] != 0 ) {
			float childArea = childMinMaxs[i].area();
			cost += childRefCounts[i] * childArea;
		}
	}

	cost /= parentArea;
	//Sys_Printf("%f\n", cost);

	return cost;
}

/*
	MakeVisTree
	Creates a node and either stops or tries to create children
*/
Shared::visNode_t Shared::MakeVisTree( std::vector<Shared::visRef_t> refs, float parentCost ) {
	Shared::visNode_t node;

	// Create MinMax of our node
	MinMax minmax;
	for ( Shared::visRef_t &ref : refs )
		minmax.extend( ref.minmax );

	node.minmax = minmax;

	// Check if a vis ref is large enough to reference directly
	std::vector<Shared::visRef_t> visRefs = refs;
	refs.clear();
	for (Shared::visRef_t& ref : visRefs)
	{
		if (minmax.surrounds(ref.minmax) && ref.minmax.area() / minmax.area() > 0.8)
			node.refs.emplace_back(ref);
		else
			refs.emplace_back(ref);
	}

	// Check all possible ways of splitting
	float bestCost = 0;
	float bestPos = 0;
	int bestAxis = 0;

	bestCost = 1e30f;
	float pos;
	for ( int axis = 0; axis < 3; axis++ ) {
		for ( std::size_t i = 0; i < refs.size(); i++ ) {
			pos = refs.at(i).minmax.mins[axis];

			float cost = CalculateSAH(refs, axis, pos);
			//Sys_Printf("%f\n", cost);
			if (cost < bestCost)
			{
				bestCost = cost;
				bestPos = pos;
				bestAxis = axis;
			}
		}
	}
	//Sys_Printf("%f < %f\n", bestCost, parentCost);
	if ( bestCost >= parentCost )
	{
		if( refs.size() > 255 )
			Sys_FPrintf( SYS_WRN, "CellAABBNode references more than 255 refs\n" );

		for ( Shared::visRef_t &ref : refs )
			node.refs.emplace_back( ref );

		return node;
	}
	
	MinMax nodes[2];
	nodes[0] = minmax;
	nodes[1] = minmax;
	std::vector<Shared::visRef_t> nodeRefs[2];

	nodes[0].maxs[bestAxis] = bestPos;
	nodes[1].mins[bestAxis] = bestPos;

	for ( Shared::visRef_t &ref : refs )
	{
		for( int i = 0; i < 2; i++ ) {
			if( ref.minmax.test( nodes[i] ) ) {
				nodeRefs[i].emplace_back( ref );
				break;
			}
		}
	}

	//Sys_Printf("%li; %li; %li; %li\n", nodeRefs[0].size(), nodeRefs[1].size(), nodeRefs[2].size(), nodeRefs[3].size());

	for( int i = 0; i < 2; i++ )
		if( nodeRefs[i].size() != 0 )
			node.children.emplace_back( Shared::MakeVisTree( nodeRefs[i], bestCost ) );

	return node;
}
