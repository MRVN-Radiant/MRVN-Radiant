/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2021, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS int32_tERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/** @file  HL1FileData.h
 *  @brief Definition of in-memory structures for the
 *         Half-Life 1 MDL file format.
 */

/**
 * Structs taken from https://github.com/IJARika/respawn-mdl with permission
 */

#ifndef AI_R5FILEDATA_INCLUDED
#define AI_R5FILEDATA_INCLUDED

#define MAX_NUM_LODS 8
#define MAX_NUM_BONES_PER_VERT 3

#include <assimp/Compiler/pushpack1.h>
#include <assimp/types.h>

namespace Assimp {
namespace MDL {
namespace RespawnR5 {

using vec2_t = float[2];
using vec3_t = float[3];
using vec4_t = float[4];

struct studiohdr_t
{
	int32_t id; // Model format ID, such as "IDST" (0x49 0x44 0x53 0x54)
	int32_t version; // Format version number, such as 48 (0x30,0x00,0x00,0x00)
	int32_t checksum; // This has to be the same in the phy and vtx files to load!
	int32_t sznameindex; // This has been moved from studiohdr2 to the front of the main header.
	char name[64]; // The internal name of the model, padding with null bytes.
	                // Typically "my_model.mdl" will have an internal name of "my_model"
	int32_t length; // Data size of MDL file in bytes.
 
	vec3_t eyeposition;	// ideal eye position

	vec3_t illumposition;	// illumination center
	
	vec3_t hull_min;		// ideal movement hull size
	vec3_t hull_max;			

	vec3_t view_bbmin;		// clipping bounding box
	vec3_t view_bbmax;		
 
	int32_t flags;
  
	int32_t numbones; // bones
	int32_t boneindex;
 
	int32_t numbonecontrollers; // bone controllers
	int32_t bonecontrollerindex;
 
	int32_t numhitboxsets;
	int32_t hitboxsetindex;
 
    // unused now
	int32_t numlocalanim; // animations/poses
	int32_t localanimindex; // animation descriptions
 
	int32_t numlocalseq; // sequences
	int32_t	localseqindex;
 
	int32_t activitylistversion; // initialization flag - have the sequences been indexed?
 
	// mstudiotexture_t
	// int16_t rpak path
	// raw textures
    int32_t materialtypesindex; // index into an array of uint8_t sized material type enums for each material used by the model
	int32_t numtextures; // the material limit exceeds 128, probably 256.
	int32_t textureindex;
 
	// this should always only be one, unless using vmts.
	// raw textures search paths
	int32_t numcdtextures;
	int32_t cdtextureindex;
 
	// replaceable textures tables
	int32_t numskinref;
	int32_t numskinfamilies;
	int32_t skinindex;
 
	int32_t numbodyparts;		
	int32_t bodypartindex;
 
	int32_t numlocalattachments;
	int32_t localattachmentindex;
 
	int32_t numlocalnodes;
	int32_t localnodeindex;
	int32_t localnodenameindex;
 
	int32_t numunknodes; // ???
	int32_t nodedataindexindex; // index into an array of int32_t sized offsets that read into the data for each node
 
	int32_t meshindex; // offset to model meshes

    // all flex related model vars and structs are stripped in respawn source
	int32_t deprecated_numflexcontrollers;
	int32_t deprecated_flexcontrollerindex;
 
	int32_t deprecated_numflexrules;
	int32_t deprecated_flexruleindex;
 
	int32_t numikchains;
	int32_t ikchainindex;

	// mesh panels for using rui on models, primarily for weapons
	int32_t numruimeshes;
	int32_t ruimeshindex;
 
	int32_t numlocalposeparameters;
	int32_t localposeparamindex;
 
	int32_t surfacepropindex;
 
	int32_t keyvalueindex;
	int32_t keyvaluesize;
 
	int32_t numlocalikautoplaylocks;
	int32_t localikautoplaylockindex;
 
	float mass;
	int32_t contents;
	
	// unused for packed models
    // technically still functional though I am unsure why you'd want to use it
	int32_t numincludemodels;
	int32_t includemodelindex;
	
	int32_t virtualModel; // size placeholder

	int32_t bonetablebynameindex;
	
	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	uint8_t constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	uint8_t rootLOD;
	
	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	uint8_t numAllowedRootLODs;

	uint8_t unused;

	float fadeDistance;
	
	float gathersize; // what. from r5r struct

    int32_t deprecated_numflexcontrollerui;
	int32_t deprecated_flexcontrolleruiindex;

	float flVertAnimFixedPointScale; // to be verified
    int32_t surfacepropLookup; // saved in the file

	int32_t sourceFilenameOffset; // doesn't actually need to be written pretty sure, only four bytes when not present.

	int32_t numsrcbonetransform;
	int32_t srcbonetransformindex;

	int32_t	illumpositionattachmentindex;
	
	int32_t linearboneindex;

    // unsure what this is for but it exists for jigglbones
	int32_t numprocbones;
    int32_t procbonetableindex;
    int32_t linearprocboneindex;

    // depreciated as they are removed in 12.1
    int32_t m_nBoneFlexDriverCount;
	int32_t m_nBoneFlexDriverIndex;
    
    int32_t m_nPerTriAABBIndex;
    int32_t m_nPerTriAABBNodeCount;
    int32_t m_nPerTriAABBLeafCount;
    int32_t m_nPerTriAABBVertCount;

	// always "" or "Titan"
    int32_t unkstringindex;

	// this is now used for combined files in rpak, vtx, vvd, and vvc are all combined while vphy is separate.
	// the indexes are added to the offset in the rpak mdl_ header.
	// vphy isn't vphy, looks like a heavily modified vphy.
	int32_t vtxindex; // VTX
	int32_t vvdindex; // VVD / IDSV
	int32_t vvcindex; // VVC / IDCV 
	int32_t vphyindex; // VPHY / IVPS

	int32_t vtxsize;
	int32_t vvdsize;
	int32_t vvcsize;
	int32_t vphysize; // still used in models using vg

    // unused in apex, gets cut in 12.1
    int32_t unkmemberindex1;
    int32_t numunkmember1;

    // mostly seen on '_animated' suffixed models
    // manually declared bone followers are no longer stored in kvs under 'bone_followers', they are now stored in an array of ints with the bone index.
    int32_t numbonefollowers; // numBoneFollowers
    int32_t bonefollowerindex;

    // BVH4 size (?)
    vec3_t mins;
    vec3_t maxs; // seem to be the same as hull size

    int32_t unk3_v54[3];

	int32_t bvh4index; // bvh4 tree

	int16_t unk4_v54[2]; // same as unk3_v54_v121, 2nd might be base for other offsets?

    // new in apex vertex weight file for verts that have more than three weights
	int32_t vvwindex; // index will come last after other vertex files
	int32_t vvwsize;
} PACK_STRUCT;

struct mstudiobodyparts_t
{
	int32_t sznameindex;
	int32_t nummodels;
	int32_t base;
	int32_t modelindex; // index into models array
} PACK_STRUCT;

struct mstudiomodel_t
{
	char name[64];

	int32_t unkindex2; // goes to bones sometimes, string index

    int32_t type;

	float boundingradius;

	int32_t nummeshes;	
	int32_t meshindex;

	// cache purposes
	int32_t numvertices; // number of unique vertices/normals/texcoords
	int32_t vertexindex; // vertex vec3_t
	int32_t tangentsindex; // tangents vec3_t

	int32_t numattachments;
	int32_t attachmentindex;

    int32_t deprecated_numeyeballs;
	int32_t deprecated_eyeballindex;

	int32_t pad[4];

    int32_t colorindex; // vertex color
                    // offset by colorindex number of bytes into vvc vertex colors
    int32_t uv2index; // vertex second uv map
                  // offset by uv2index number of bytes into vvc secondary uv map
} PACK_STRUCT;

struct mstudio_meshvertexloddata_t
{
    int32_t modelvertexdataUnusedPad; // likely has none of the funny stuff because unused

    int32_t numLODVertexes[MAX_NUM_LODS]; // depreciated starting with rmdl v14(?)
} PACK_STRUCT;

struct mstudiomesh_t
{
	int32_t material;

	int32_t modelindex;

	int32_t numvertices; // number of unique vertices/normals/texcoords
	int32_t vertexoffset; // vertex mstudiovertex_t

	// Access thin/fat mesh vertex data (only one will return a non-NULL result)
	
	int32_t deprecated_numflexes; // vertex animation
	int32_t deprecated_flexindex;

	// special codes for material operations
	int32_t deprecated_materialtype;
	int32_t deprecated_materialparam;

	// a unique ordinal for this mesh
	int32_t meshid;

	vec3_t center;

	mstudio_meshvertexloddata_t vertexloddata;
    
	char unk[8]; // these are suposed to be filled on load, however this isn't true??
} PACK_STRUCT;

// ------------------------------------------------------------------------------------------------
// vg


#include <assimp/Compiler/poppack1.h>

} // namespace Respawn
} // namespace MDL
} // namespace Assimp

#endif // AI_R5FILEDATA_INCLUDED