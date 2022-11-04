
#include "remap.h"
#include "bspfile_abstract.h"



float CalculateSAH( std::vector<Shared::visRef_t> &refs, int axis, float pos )
{
	MinMax left;
	std::size_t leftCount = 0;
	MinMax right;
	std::size_t rightCount = 0;

	for ( Shared::visRef_t &ref : refs )
	{
		float refPos = ref.minmax.maxs[axis];
		if ( refPos < pos )
		{
			leftCount++;
			left.extend( ref.minmax );
		}
		else
		{
			rightCount++;
			right.extend( ref.minmax );
		}
	}

	float cost = 0;
	if ( leftCount != 0 )
		cost += leftCount * left.area();
	if ( rightCount != 0 )
		cost += rightCount * right.area();

	return cost;
}




/*
	EmitMeshes()
	Emits shared meshes for a given entity
*/
void Shared::MakeMeshes( const entity_t &e )
{
	/* Multiple entities can have meshes, make sure we clear before making new meshes */
	Shared::meshes.clear();

	Sys_FPrintf( SYS_VRB, "--- SharedMeshes ---\n" );

	/* Brushes */
	for ( const brush_t &brush : e.brushes )
	{
		/* loop through sides */
		for ( const side_t &side : brush.sides )
		{
			/* Skip bevels */
			if ( side.bevel )
				continue;

			/* This should check flags, but those dont work rn */
			if ( striEqual( side.shaderInfo->shader.c_str(), "textures/tools/toolsnodraw" ) )
				continue;
			
			Shared::Mesh_t &mesh = Shared::meshes.emplace_back();
			mesh.shaderInfo = side.shaderInfo;

			Vector3 normal = side.plane.normal();//Vector3(0.0f,0.0f,1.0f);

			/* Loop through vertices */
			for ( const Vector3 &vertex : side.winding )
			{
				/* Update AABB */
				mesh.minmax.extend( vertex );

				/* Texturing */
				/* this is taken from surface.cpp, It doesn't work :) */
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

			/* Create triangles for side */
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

	/* Patches */
	
	parseMesh_t *patch;
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
		}
		
		// Make triangles
		for( int x = 0; x < ( patchMesh.width - 1 ); x++ ) {
			for( int y = 0; y < ( patchMesh.height - 1 ); y++ ) {
				int index = x + y * patchMesh.width;
				Sys_Printf( "%i : %i = %i\n",x , y, index );
				
				mesh.triangles.emplace_back( index );
				mesh.triangles.emplace_back( index + patchMesh.width );
				mesh.triangles.emplace_back( index + patchMesh.width + 1 );

				mesh.triangles.emplace_back( index );
				mesh.triangles.emplace_back( index + patchMesh.width + 1 );
				mesh.triangles.emplace_back( index + 1 );
				
			}
		}

		Sys_Printf( "Vertices: %li\n", mesh.vertices.size() );
		Sys_Printf( "Triangles: %li\n", mesh.triangles.size() / 3 );
		
		
		patch = patch->next;
	}
	

	/* Combine */
	std::size_t index = 0;
	std::size_t iterationsSinceCombine = 0;
	while ( true )
	{
		/* std::out_of_range can't comprehend this  */
		if ( index >= Shared::meshes.size() )
			index = 0;

		/* No more meshes to combine; break from the loop */
		if ( iterationsSinceCombine >= Shared::meshes.size() )
			break;

		/* Get mesh which we then compare to the rest, maybe combine, maybe not */
		Shared::Mesh_t &mesh1 = Shared::meshes.at( index );

		for ( std::size_t i = 0; i < Shared::meshes.size(); i++ )
		{
			/* We dont want to compare the same mesh */
			if ( index == i )
				continue;

			Shared::Mesh_t &mesh2 = Shared::meshes.at( i );

			/* check if they have the same shader */
			if ( !striEqual( mesh1.shaderInfo->shader.c_str(), mesh2.shaderInfo->shader.c_str() ) )
				continue;

			/* Check if they're intersecting */
			if ( !mesh1.minmax.test( mesh2.minmax ) )
				continue;


			/* Combine them */
			/* Triangles */
			for ( uint16_t &triIndex : mesh2.triangles )
				mesh1.triangles.emplace_back( triIndex + mesh1.vertices.size() );
			
			/* Copy over vertices */
			mesh1.vertices.insert( mesh1.vertices.end(), mesh2.vertices.begin(), mesh2.vertices.end() );

			/* Update minmax */
			mesh1.minmax.extend( mesh2.minmax );
			

			/* Delete mesh we combined as to not create duplicates */
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

Shared::visNode_t Shared::MakeVisTree( std::vector<Shared::visRef_t> refs, float parentCost )
{
	Shared::visNode_t node;

	/* Make minmax of refs */
	MinMax minmax;
	for ( Shared::visRef_t &ref : refs )
		minmax.extend( ref.minmax );

	node.minmax = minmax;

	{
		
		std::vector<Shared::visRef_t> visRefs = refs;
		refs.clear();

		for ( Shared::visRef_t &ref : visRefs )
		{
			//Sys_Printf("amonsus %i\n", ref.index);
			if ( ref.minmax.area() / minmax.area() > 0.7 )
				node.refs.emplace_back( ref );
			else
				refs.emplace_back( ref );
		}
		
		/*
		std::size_t i = 0;
		for (Shared::visRef_t& ref : refs)
		{
			Sys_Printf("amonsus %i\n", ref.index);
			if (ref.minmax.area() / minmax.area() > 0.7)
			{
				refs.erase(refs.begin() + i);
				node.refs.emplace_back(ref);
				i--;
			}
			i++;
		}
		*/
	}


	float bestCost, bestPos = 0;
	int bestAxis = 0;

	bestCost = 1e30f;

	/* Loop through each axis and test possible splits */
	for ( int axis = 0; axis < 3; axis++ )
	{
		for ( Shared::visRef_t &ref : refs )
		{
			float pos = ref.minmax.mins[axis];
			float cost = CalculateSAH( refs, axis, pos );
			
			if ( cost < bestCost )
			{
				bestCost = cost;
				bestPos = pos;
				bestAxis = axis;//Sys_Printf("bestCost: %f; bestPos: %f; bestAxis: %i\n", bestCost, bestPos, bestAxis);
			}
		}
	}
	
	if ( bestCost >= parentCost || refs.size() < 5 )
	{
		for ( Shared::visRef_t &ref : refs )
			node.refs.emplace_back( ref );

		return node;
	}

	MinMax left = minmax;
	MinMax right = minmax;

	std::vector<Shared::visRef_t> leftRefs;
	std::vector<Shared::visRef_t> rightRefs;

	left.mins[bestAxis] = bestPos;
	right.maxs[bestAxis] = bestPos;


	for ( Shared::visRef_t &ref : refs )
	{
		if ( ref.minmax.test( left ) )
		{
			leftRefs.emplace_back( ref );
			continue;
		}

		if ( ref.minmax.test( right ) )
			rightRefs.emplace_back( ref );
	}
	

	if( leftRefs.size() != 0 )
		node.children.emplace_back( Shared::MakeVisTree( leftRefs, bestCost ) );
	if( rightRefs.size() != 0 )
		node.children.emplace_back( Shared::MakeVisTree( rightRefs, bestCost ) );

	return node;
}
