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

#ifndef AI_R2FILEDATA_INCLUDED
#define AI_R2FILEDATA_INCLUDED

#define MAX_NUM_LODS 8
#define MAX_NUM_BONES_PER_VERT 3

#include <assimp/Compiler/pushpack1.h>
#include <assimp/types.h>

namespace Assimp {
namespace MDL {
namespace RespawnR2 {

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
  
    // highest observed: 250
    // max is definitely 256 because 8bit uint limit
    int32_t numbones; // bones
	int32_t boneindex;
 
    int32_t numbonecontrollers; // bone controllers
	int32_t bonecontrollerindex;
 
    int32_t numhitboxsets;
	int32_t hitboxsetindex;
 
    int32_t numlocalanim; // animations/poses
	int32_t localanimindex; // animation descriptions
 
    int32_t numlocalseq; // sequences
	int32_t	localseqindex;
 
	int32_t activitylistversion; // initialization flag - have the sequences been indexed? set on load
	int32_t eventsindexed;
 
    // mstudiotexture_t
    // short rpak path
	// raw textures
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
 
    int32_t deprecated_numflexdesc;
	int32_t deprecated_flexdescindex;
 
    int32_t deprecated_numflexcontrollers;
	int32_t deprecated_flexcontrollerindex;
 
    int32_t deprecated_numflexrules;
	int32_t deprecated_flexruleindex;
 
    int32_t numikchains;
	int32_t ikchainindex;

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
 
    // external animations, models, etc.
	int32_t numincludemodels;
	int32_t includemodelindex;
    
    int32_t virtualModel; // should be void

    // animblock is either completely cut, this is because they no longer use .ani files.

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

	float fadeDistance; // set to -1 to never fade. set above 0 if you want it to fade out, distance is in feet.
                        // player/titan models seem to inherit this value from the first model loaded in menus.
                        // works oddly on entities, probably only meant for static props

	int32_t deprecated_numflexcontrollerui;
	int32_t deprecated_flexcontrolleruiindex;
    
    float flVertAnimFixedPointScale;
    int32_t surfacepropLookup; // this index must be cached by the loader, not saved in the file

    // this is in all shipped models, probably part of their asset bakery. it should be 0x2CC.
    // doesn't actually need to be written pretty sure, only four bytes when not present.
    // this is not completely true as some models simply have nothing, such as animation models.
	int32_t sourceFilenameOffset;

    int32_t numsrcbonetransform;
	int32_t srcbonetransformindex;

	int32_t	illumpositionattachmentindex;
	
	int32_t linearboneindex;

	int32_t m_nBoneFlexDriverCount;
	int32_t m_nBoneFlexDriverIndex;
    
    // for static props (and maybe others)
    // Precomputed Per-Triangle AABB data
    int32_t m_nPerTriAABBIndex;
    int32_t m_nPerTriAABBNodeCount;
    int32_t m_nPerTriAABBLeafCount;
    int32_t m_nPerTriAABBVertCount;

    // always "" or "Titan"
    int32_t unkstringindex;

    // ANIs are no longer used and this is reflected in many structs
    // Start of interal file data
    int32_t vtxindex; // VTX
	int32_t vvdindex; // VVD / IDSV
    int32_t vvcindex; // VVC / IDCV 
    int32_t vphyindex; // VPHY / IVPS

    int32_t vtxsize; // VTX
    int32_t vvdsize; // VVD / IDSV
    int32_t vvcsize; // VVC / IDCV 
    int32_t vphysize; // VPHY / IVPS

    // this data block is related to the vphy, if it's not present the data will not be written
    // definitely related to phy, apex phy has this merged into it
    int32_t unkindex; // section between vphy and vtx.?
    int32_t numunk; // only seems to be used when phy has one solid

    // mostly seen on '_animated' suffixed models
	// manually declared bone followers are no longer stored in kvs under 'bone_followers', they are now stored in an array of ints with the bone index.
	int32_t numbonefollowers; // numBoneFollowers
	int32_t bonefollowerindex;

    int32_t unused1[60];
} PACK_STRUCT;

struct mstudiotexture_t
{
    int32_t sznameindex;

	int32_t unused_flags;
	int32_t used;

	int32_t unused[8];
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

    int32_t type;

    float boundingradius;

    int32_t nummeshes;	
    int32_t meshindex;

    // cache purposes
    int32_t numvertices; // number of unique vertices/normals/texcoords
    int32_t vertexindex; // vertex vec3_t
                     // offset by vertexindex number of bytes into vvd verts
    int32_t tangentsindex; // tangents vec3_t
                       // offset by tangentsindex number of bytes into vvd tangents

    int32_t numattachments;
    int32_t attachmentindex;

    int32_t deprecated_numeyeballs;
    int32_t deprecated_eyeballindex;

    int32_t pad[4];

    int32_t colorindex; // vertex color
                    // offset by colorindex number of bytes into vvc vertex colors
    int32_t uv2index; // vertex second uv map
                  // offset by uv2index number of bytes into vvc secondary uv map

    int32_t unused[4];
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
                      // offset by vertexoffset number of verts into vvd vertexes, relative to the models offset

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
    
    char unk[8]; // set on load

	int32_t unused[6]; // remove as appropriate
} PACK_STRUCT;

// ------------------------------------------------------------------------------------------------
// vtx

struct FileHeader_t
{
    // file version as defined by OPTIMIZED_MODEL_FILE_VERSION (currently 7)
    int32_t version;

    // hardware params that affect how the model is to be optimized.
    int32_t vertCacheSize;
    uint16_t maxBonesPerStrip;
    uint16_t maxBonesPerFace;
    int32_t maxBonesPerVert;

    // must match checkSum in the .mdl
    int32_t checkSum;

    int32_t numLODs; // Also specified in ModelHeader_t's and should match

    // Offset to materialReplacementList Array. one of these for each LOD, 8 in total
    int32_t materialReplacementListOffset;

    // Defines the size and location of the body part array
    int32_t numBodyParts;
    int32_t bodyPartOffset;
} PACK_STRUCT;

struct BodyPartHeader_t
{
    // Model array
    int32_t numModels;
    int32_t modelOffset;
} PACK_STRUCT;

struct ModelHeader_t
{
    //LOD mesh array
    int32_t numLODs;   //This is also specified in FileHeader_t
    int32_t lodOffset;
} PACK_STRUCT;

struct ModelLODHeader_t
{
    //Mesh array
    int32_t numMeshes;
    int32_t meshOffset;

    float switchPoint;
} PACK_STRUCT;

struct MeshHeader_t
{
    int32_t numStripGroups;
    int32_t stripGroupHeaderOffset;

    uint8_t flags;
} PACK_STRUCT;

struct StripGroupHeader_t
{
    // These are the arrays of all verts and indices for this mesh.  strips index into this.
    int32_t numVerts;
    int32_t vertOffset;

    int32_t numIndices;
    int32_t indexOffset;

    int32_t numStrips;
    int32_t stripOffset;

    uint8_t flags;

    // The following fields are only present if MDL version is >=49
    // Points to an array of unsigned shorts (16 bits each)
    int32_t numTopologyIndices;
    int32_t topologyOffset;
} PACK_STRUCT;

struct StripHeader_t
{
    int32_t numIndices;
    int32_t indexOffset;

    int32_t numVerts;    
    int32_t vertOffset;

    int16_t numBones;

    uint8_t flags;

    int32_t numBoneStateChanges;
    int32_t boneStateChangeOffset;

    // MDL Version 49 and up only
    int32_t numTopologyIndices;
    int32_t topologyOffset;
} PACK_STRUCT;

struct Vertex_t
{
    // these index into the mesh's vert[origMeshVertID]'s bones
    uint8_t boneWeightIndex[MAX_NUM_BONES_PER_VERT];
    uint8_t numBones;

    uint16_t origMeshVertID;

    // for sw skinned verts, these are indices into the global list of bones
    // for hw skinned verts, these are hardware bone indices
    uint8_t boneID[MAX_NUM_BONES_PER_VERT];
} PACK_STRUCT;

// ------------------------------------------------------------------------------------------------
// vvd

struct vertexFileHeader_t
{
    int32_t id; // MODEL_VERTEX_FILE_ID
    int32_t version; // MODEL_VERTEX_FILE_VERSION
    int32_t checksum; // same as studiohdr_t, ensures sync

    int32_t numLODs; // num of valid lods
    int32_t numLODVertexes[MAX_NUM_LODS]; // num verts for desired root lod

    // in vvc this is the offset to vertex color
    int32_t numFixups; // num of vertexFileFixup_t

    // in vvc this is the offset into a second uv layer
    int32_t fixupTableStart; // offset from base to fixup table
    int32_t vertexDataStart; // offset from base to vertex block
    int32_t tangentDataStart; // offset from base to tangent block
} PACK_STRUCT;

struct mstudioboneweight_t
{
    float	weight[MAX_NUM_BONES_PER_VERT];
    unsigned char bone[MAX_NUM_BONES_PER_VERT]; // set to unsigned so we can read it
    uint8_t	numbones;
} PACK_STRUCT;

struct mstudiovertex_t
{
    mstudioboneweight_t	m_BoneWeights;
    vec3_t m_vecPosition;
    vec3_t m_vecNormal;
    vec2_t m_vecTexCoord;
} PACK_STRUCT;

#include <assimp/Compiler/poppack1.h>

} // namespace Respawn
} // namespace MDL
} // namespace Assimp

#endif // AI_R2FILEDATA_INCLUDED