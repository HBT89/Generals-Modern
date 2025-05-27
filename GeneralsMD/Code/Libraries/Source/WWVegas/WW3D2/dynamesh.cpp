/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*************************************************************************** 
 ***    C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S     *** 
 *************************************************************************** 
 *                                                                         * 
 *                 Project Name : Commando                                 * 
 *                                                                         * 
 *                     $Archive:: /Commando/Code/ww3d2/dynamesh.cpp       $* 
 *                                                                         * 
 *                      $Author:: Greg_h                                  $* 
 *                                                                         * 
 *                     $Modtime:: 12/03/01 4:50p                          $* 
 *                                                                         * 
 *                    $Revision:: 25                                      $* 
 *                                                                         * 
 *-------------------------------------------------------------------------* 
 * Functions:                                                              * 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "dynamesh.h"
#include "BGFXVertexBuffer.h"
#include "BGFXIndexBuffer.h"
#include "BGFXWrapper.h"
#include "sortingrenderer.h"
#include "rinfo.h"
#include "camera.h"
#include "formconv.h"
#include <bgfx/bgfx.h>



/*
** DynamicMeshModel implementation
*/

DynamicMeshModel::DynamicMeshModel(unsigned int max_polys, unsigned int max_verts) :
	MeshGeometryClass(),
	DynamicMeshPNum(0),
	DynamicMeshVNum(0),
	MatDesc(NULL),
	MatInfo(NULL)
{
	MatInfo = NEW_REF(MaterialInfoClass, ());

	MatDesc =  W3DNEW MeshMatDescClass;
	MatDesc->Set_Polygon_Count(max_polys);
	MatDesc->Set_Vertex_Count(max_verts);

	Reset_Geometry(max_polys, max_verts);
}

DynamicMeshModel::DynamicMeshModel(unsigned int max_polys, unsigned int max_verts, MaterialInfoClass *mat_info) :
	MeshGeometryClass(),
	DynamicMeshPNum(0),
	DynamicMeshVNum(0),
	MatDesc(NULL),
	MatInfo(NULL)
{
	MatInfo = mat_info;
	MatInfo->Add_Ref();

	MatDesc = W3DNEW MeshMatDescClass;
	MatDesc->Set_Polygon_Count(max_polys);
	MatDesc->Set_Vertex_Count(max_verts);

	Reset_Geometry(max_polys, max_verts);
}

DynamicMeshModel::DynamicMeshModel(const DynamicMeshModel &src) :
	MeshGeometryClass(src),
	DynamicMeshPNum(src.DynamicMeshPNum),
	DynamicMeshVNum(src.DynamicMeshVNum),
	MatDesc(NULL),
	MatInfo(NULL)
{
	// Copy the material info structure.
	MatInfo = NEW_REF(MaterialInfoClass, (*(src.MatInfo)));


	// [SKB: Feb 21 2002 @ 11:47pm] :
	// Moved before the remapping cause I don't like referencing null.
	MatDesc = W3DNEW MeshMatDescClass;

	// remap!
	MaterialRemapperClass remapper(src.MatInfo, MatInfo);
	remapper.Remap_Mesh(src.MatDesc, MatDesc);
}

DynamicMeshModel::~DynamicMeshModel(void)
{
	if (MatDesc) {
		delete MatDesc;
		MatDesc = NULL;
	}
	REF_PTR_RELEASE(MatInfo);
}

void DynamicMeshModel::Compute_Plane_Equations(void)
{
	// Make sure the arrays are allocated before we do this
	get_vert_normals();
	Vector4 * planes = get_planes(true);

	// Set the poly and vertex counts to the dynamic counts, call the base class function, then
	// set them back.
	int old_poly_count = PolyCount;
	int old_vert_count = VertexCount;
	PolyCount = DynamicMeshPNum;
	VertexCount = DynamicMeshVNum;

	MeshGeometryClass::Compute_Plane_Equations(planes);

	PolyCount = old_poly_count;
	VertexCount = old_vert_count;
}

void DynamicMeshModel::Compute_Vertex_Normals(void)
{
	// Make sure the arrays are allocated before we do this
	Vector3 * vnorms = get_vert_normals();
	get_planes(true);

	// Set the poly and vertex counts to the dynamic counts, call the base class function, then
	// set them back.
	int old_poly_count = PolyCount;
	int old_vert_count = VertexCount;
	PolyCount = DynamicMeshPNum;
	VertexCount = DynamicMeshVNum;

	MeshGeometryClass::Compute_Vertex_Normals(vnorms);

	PolyCount = old_poly_count;
	VertexCount = old_vert_count;
}

void DynamicMeshModel::Compute_Bounds(Vector3 * verts)
{
	// Set the poly and vertex counts to the dynamic counts, call the base class function, then
	// set them back.
	int old_poly_count = PolyCount;
	int old_vert_count = VertexCount;
	PolyCount = DynamicMeshPNum;
	VertexCount = DynamicMeshVNum;

	MeshGeometryClass::Compute_Bounds(verts);

	PolyCount = old_poly_count;
	VertexCount = old_vert_count;
}

void DynamicMeshModel::Reset(void)
{
	Set_Counts(0, 0);
	int polycount = Get_Polygon_Count();
	int vertcount = Get_Vertex_Count();
	Reset_Geometry(polycount, vertcount);
	MatDesc->Reset(polycount, vertcount, 1);
	REF_PTR_RELEASE(MatInfo);
	MatInfo = NEW_REF(MaterialInfoClass, ());
}

void DynamicMeshModel::Render(RenderInfoClass & rinfo)
{
    // Prepare vertex data
    const Vector3* locs = Get_Vertex_Array();
    const Vector3* normals = Get_Vertex_Normal_Array();
    const Vector2* uvs = MatDesc->Get_UV_Array_By_Index(0, false);
    const unsigned* colors = MatDesc->Get_Color_Array(0, false);
    // TODO: Add support for additional UVs/colors as needed
    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
        uint32_t color;
    };
    std::vector<Vertex> vertices(DynamicMeshVNum);
    for (int i = 0; i < DynamicMeshVNum; ++i) {
        vertices[i].x = locs[i].X;
        vertices[i].y = locs[i].Y;
        vertices[i].z = locs[i].Z;
        vertices[i].nx = normals ? normals[i].X : 0.0f;
        vertices[i].ny = normals ? normals[i].Y : 0.0f;
        vertices[i].nz = normals ? normals[i].Z : 0.0f;
        vertices[i].u = uvs ? uvs[i].X : 0.0f;
        vertices[i].v = uvs ? uvs[i].Y : 0.0f;
        vertices[i].color = colors ? colors[i] : 0xFFFFFFFF;
    }
    // Prepare index data
    const TriIndex* tris = Get_Polygon_Array();
    std::vector<uint16_t> indices(DynamicMeshPNum * 3);
    for (int i = 0; i < DynamicMeshPNum; ++i) {
        indices[i * 3 + 0] = (uint16_t)tris[i][0];
        indices[i * 3 + 1] = (uint16_t)tris[i][1];
        indices[i * 3 + 2] = (uint16_t)tris[i][2];
    }
    // Setup BGFX vertex layout
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
    // Create BGFX buffers
    BGFXVertexBuffer vb;
    vb.Create(vertices.data(), sizeof(Vertex), DynamicMeshVNum, layout);
    BGFXIndexBuffer ib;
    ib.Create(indices.data(), sizeof(uint16_t), DynamicMeshPNum * 3);
    // Submit draw call (shader/texture/material setup omitted for brevity)
    bgfx::setVertexBuffer(0, vb.GetHandle());
    bgfx::setIndexBuffer(ib.GetHandle());
    // TODO: Set uniforms, textures, and submit with the correct shader
    bgfx::submit(0); // Use view 0 for now
}

void DynamicMeshModel::Initialize_Texture_Array(int pass, int stage, TextureClass *texture)
{
	TexBufferClass * texlist = MatDesc->Get_Texture_Array(pass, 0, true);
	for (int lp = 0; lp < PolyCount; lp++) {
		texlist->Set_Element(lp, texture);
	}
}

void DynamicMeshModel::Initialize_Material_Array(int pass, VertexMaterialClass *vmat)
{
	MatBufferClass * vertmatlist = MatDesc->Get_Material_Array(pass, true);
	for (int lp = 0; lp < VertexCount; lp++) {
		vertmatlist->Set_Element(lp, vmat);
	}
}

void DynamicMeshClass::Render(RenderInfoClass & rinfo)
{
	if (Is_Not_Hidden_At_All() == false)	return;	

	// test for an empty mesh..
	if (PolyCount == 0 ) return;

	// If static sort lists are enabled and this mesh has a sort level, put it on the list instead
	// of rendering it.

	if (WW3D::Are_Static_Sort_Lists_Enabled() && SortLevel != SORT_LEVEL_NONE) {

		WW3D::Add_To_Static_Sort_List(this, SortLevel);

	} else {

		const FrustumClass & frustum = rinfo.Camera.Get_Frustum();

		if (CollisionMath::Overlap_Test(frustum, Get_Bounding_Box()) != CollisionMath::OUTSIDE) {
			DX8Wrapper::Set_Transform(D3DTS_WORLD, Transform);
			Model->Render(rinfo);
		}
	}
}

bool DynamicMeshClass::End_Vertex()
{
	// check that we have room for a new vertex
	WWASSERT(VertCount < Model->Get_Vertex_Count());

	// if we are a multi-material object record the material
	int pass = Get_Pass_Count();
	while (pass--) {
		if (MultiVertexMaterial[pass]) {
			VertexMaterialClass *mat = Peek_Material_Info()->Get_Vertex_Material(VertexMaterialIdx[pass]);
			Model->Set_Material(VertCount, mat, pass);
			REF_PTR_RELEASE(mat);
		}

	}

	// if we are multi colored, record the color
	for (int color_array_index = 0; color_array_index < MAX_COLOR_ARRAYS; color_array_index++) {
		if (MultiVertexColor[color_array_index]) {
//			Vector4 * color = &((Model->Get_Color_Array(color_array_index))[VertCount]);
//			color->X = CurVertexColor[color_array_index].X;
//			color->Y = CurVertexColor[color_array_index].Y;
//			color->Z = CurVertexColor[color_array_index].Z;
//			color->W = CurVertexColor[color_array_index].W;
			unsigned * color = &((Model->Get_Color_Array(color_array_index))[VertCount]);
			*color=DX8Wrapper::Convert_Color_Clamp(CurVertexColor[color_array_index]);
		}
	}

	// mark this vertex as being complete
	VertCount++;
	TriVertexCount++;

	// if we have 3 or more vertices, add a new poly
	if (TriVertexCount >= 3) {
	
		// check that we have room for a new poly
		WWASSERT(PolyCount < Model->Get_Polygon_Count());

		// set vertex indices
		TriIndex *poly = &(Model->Get_Non_Const_Polygon_Array())[PolyCount];
		if (TriMode == TRI_MODE_STRIPS) {
			(*poly)[0] = VertCount-3;
			(*poly)[1] = VertCount-2;
			(*poly)[2] = VertCount-1;

			// for every other tri, reverse vertex order
			if (Flip_Face()) {
				(*poly)[1] = VertCount-1;
				(*poly)[2] = VertCount-2;
			}
		} else {
			(*poly)[0] = FanVertex;
			(*poly)[1] = VertCount-2;
			(*poly)[2] = VertCount-1;
		}

		// check each pass
		int pass = Get_Pass_Count();
		while (pass--) {

			// If we are multi texture
			if (MultiTexture[pass]) {
				TextureClass *tex = Peek_Material_Info()->Get_Texture(TextureIdx[pass]);
				Model->Set_Texture(PolyCount, tex, pass);
				REF_PTR_RELEASE(tex);
			}
		}

		// increase the count and record that we have a new material
		PolyCount++;
		Model->Set_Counts(PolyCount, VertCount);
	}
	return true;
}

/******************************************************************
**
** DynamicMeshClass
**
*******************************************************************/
DynamicMeshClass::DynamicMeshClass(int max_poly, int max_vert) :
	Model(NULL),
	PolyCount(0),
	VertCount(0),
	TriVertexCount(0),
	FanVertex(0),
	TriMode(TRI_MODE_STRIPS),
	SortLevel(SORT_LEVEL_NONE)
{
	int pass = MAX_PASSES;
	while (pass--) {
		MultiTexture[pass] = false;
		TextureIdx[pass] = -1;

		MultiVertexMaterial[pass] = false;
		VertexMaterialIdx[pass] = -1;
	}

	for (int color_array_index = 0; color_array_index < MAX_COLOR_ARRAYS; color_array_index++) {
		MultiVertexColor[color_array_index] = false;
		CurVertexColor[color_array_index].Set(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Model = NEW_REF(DynamicMeshModel, (max_poly, max_vert));
}

DynamicMeshClass::DynamicMeshClass(int max_poly, int max_vert, MaterialInfoClass *mat_info) :
	Model(NULL),
	PolyCount(0),
	VertCount(0),
	TriVertexCount(0),
	FanVertex(0),
	TriMode(TRI_MODE_STRIPS),
	SortLevel(SORT_LEVEL_NONE)
{
	int pass = MAX_PASSES;
	while (pass--) {
		MultiTexture[pass] = false;
		TextureIdx[pass] = -1;

		MultiVertexMaterial[pass] = false;
		VertexMaterialIdx[pass] = -1;
	}

	for (int color_array_index = 0; color_array_index < MAX_COLOR_ARRAYS; color_array_index++) {
		MultiVertexColor[color_array_index] = false;
		CurVertexColor[color_array_index].Set(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Model = NEW_REF(DynamicMeshModel, (max_poly, max_vert, mat_info));
}

DynamicMeshClass::DynamicMeshClass(const DynamicMeshClass & src) :
	RenderObjClass(src),
	Model(NULL),
	PolyCount(src.PolyCount),
	VertCount(src.VertCount),
	TriVertexCount(src.TriVertexCount),
	FanVertex(src.FanVertex),
	TriMode(src.TriMode),
	SortLevel(src.SortLevel)
{
	int pass = MAX_PASSES;
	while (pass--) {
		MultiTexture[pass] = src.MultiTexture[pass];
		TextureIdx[pass] = src.TextureIdx[pass];

		MultiVertexMaterial[pass] = src.MultiVertexMaterial[pass];
		VertexMaterialIdx[pass] = src.VertexMaterialIdx[pass];

		MultiVertexColor[pass] = src.MultiVertexColor[pass];
		CurVertexColor[pass]  = src.CurVertexColor[pass];
	}

	for (int color_array_index = 0; color_array_index < MAX_COLOR_ARRAYS; color_array_index++) {
		MultiVertexColor[color_array_index] = src.MultiVertexColor[color_array_index];
		CurVertexColor[color_array_index] = src.CurVertexColor[color_array_index];
	}

	Model = NEW_REF(DynamicMeshModel, (*(src.Model)));
}

void DynamicMeshClass::Resize(int max_polys, int max_verts)
{
	Reset();

	REF_PTR_RELEASE(Model);
	Model = NEW_REF(DynamicMeshModel, (max_polys, max_verts));

	// reset all the texture & vertex material indices
	int pass = MAX_PASSES;
	while (pass--) {
		TextureIdx[pass] = -1;
		VertexMaterialIdx[pass] = -1;
		MultiVertexMaterial[pass] = false;
	}
}

DynamicMeshClass::~DynamicMeshClass()
{	
	REF_PTR_RELEASE(Model);
}

RenderObjClass * DynamicMeshClass::Clone(void) const
{
	return NEW_REF(DynamicMeshClass, (*this));
}

void DynamicMeshClass::Location(float x, float y, float z)
{
	Vector3 * loc = Model->Get_Vertex_Array();
	assert(loc);

	loc[VertCount].X = x;
	loc[VertCount].Y = y;
	loc[VertCount].Z = z;
}

/*
** For moving a vertex after the DynaMesh has already been created.
*/
void DynamicMeshClass::Move_Vertex(int index, float x, float y, float z)
{
	Vector3 * loc = Model->Get_Vertex_Array();
	assert(loc);
	loc[index][0] = x;
	loc[index][1] = y;
	loc[index][2] = z;
}	 

/*
** Get a vertex value.
*/
void DynamicMeshClass::Get_Vertex(int index, float &x, float &y, float &z)
{
	Vector3 * loc = Model->Get_Vertex_Array();
	assert(loc);
	x = loc[index][0];
	y = loc[index][1];
	z = loc[index][2];
}


/*
** Offset the entire mesh
*/
void DynamicMeshClass::Translate_Vertices(const Vector3 & offset)
{
	Vector3 * loc = Model->Get_Vertex_Array();
	assert(loc);
	for (int i=0; i < Get_Num_Vertices(); i++) {
		loc[i].X += offset.X;
		loc[i].Y += offset.Y;
		loc[i].Z += offset.Z;
	}
	
	Set_Dirty_Bounds();
	Set_Dirty_Planes();
}

int DynamicMeshClass::Set_Vertex_Material(int idx, int pass)
{
	assert(idx < Peek_Material_Info()->Vertex_Material_Count());
	VertexMaterialIdx[pass] = idx;
	if (!MultiVertexMaterial[pass]) {
		// WWASSERT( VertexMaterialIdx[pass] == 0);
		VertexMaterialClass *mat = Peek_Material_Info()->Get_Vertex_Material(VertexMaterialIdx[pass]);
		Model->Set_Single_Material(mat, pass);
		mat->Release_Ref();
	}
	return VertexMaterialIdx[pass];
}

int DynamicMeshClass::Set_Vertex_Material(VertexMaterialClass *material, bool dont_search, int pass)
{
	// Check if same as the last vertex material
	if (Peek_Material_Info()->Vertex_Material_Count() && (VertexMaterialIdx[pass] != -1) && Peek_Material_Info()->Peek_Vertex_Material(VertexMaterialIdx[pass]) == material) {
		return VertexMaterialIdx[pass];
	}

	// if there are vertex materials in the list then we may have just jumped
	// to becoming a multi-vertex-material object.  Take care of that here.
	if ((!MultiVertexMaterial[pass]) && Peek_Material_Info()->Vertex_Material_Count() && (VertexMaterialIdx[pass] != -1) && Peek_Material_Info()->Peek_Vertex_Material(VertexMaterialIdx[pass]) != material) {

		// allocate the array of per-vertex vertex material overrides
		VertexMaterialClass *mat = Peek_Material_Info()->Get_Vertex_Material(VertexMaterialIdx[pass]);
		Model->Initialize_Material_Array(pass, mat);
		mat->Release_Ref();

		// flag that we need to write the per -vertex vertex material override array
		MultiVertexMaterial[pass] = true;
	}

	// add the material to the material info class if we cant find it in the 
	// list.  if we are not supposed to search the list for it then just add
	// it.
	if (!dont_search) {
		for (int lp = 0, found = 0; lp < Peek_Material_Info()->Vertex_Material_Count(); lp ++) {
			VertexMaterialClass *mat = Peek_Material_Info()->Get_Vertex_Material(lp);
			if (material == mat) {
				VertexMaterialIdx[pass] = lp;
				found = true;
				mat->Release_Ref();
				break;
			}
			mat->Release_Ref();
		}
		if (!found) {
			Peek_Material_Info()->Add_Vertex_Material(material);
			VertexMaterialIdx[pass] = Peek_Material_Info()->Vertex_Material_Count() - 1;
		}
	} else {
		Peek_Material_Info()->Add_Vertex_Material(material);
		VertexMaterialIdx[pass] = Peek_Material_Info()->Vertex_Material_Count() - 1;
	}

	if (!MultiVertexMaterial[pass]) {
		Model->Set_Single_Material(Peek_Material_Info()->Peek_Vertex_Material(VertexMaterialIdx[pass]), pass);
	}
	return(VertexMaterialIdx[pass]);
}

int DynamicMeshClass::Set_Texture(int idx, int pass)
{
	WWASSERT(idx < Peek_Material_Info()->Texture_Count());
	TextureIdx[pass] = idx;
	if (!MultiTexture[pass]) {
		TextureClass *tex = Peek_Material_Info()->Get_Texture(TextureIdx[pass]);
		Model->Set_Single_Texture(tex, pass);
		tex->Release_Ref();
	}
	return TextureIdx[pass];
}

int DynamicMeshClass::Set_Texture(TextureClass *texture, bool dont_search, int pass)
{
	// Check if same as the last texture
	if (Peek_Material_Info()->Texture_Count() && (TextureIdx[pass] != -1) && Peek_Material_Info()->Peek_Texture(TextureIdx[pass]) == texture) {
		return TextureIdx[pass];
	}

	// if there are textures in the list then we may have just jumped
	// to becoming a multi-texture object.  Take care of that here.
	if ((!MultiTexture[pass]) && Peek_Material_Info()->Texture_Count() && (TextureIdx[pass] != -1) && Peek_Material_Info()->Peek_Texture(TextureIdx[pass]) != texture) {

		// allocate the array of per polygon material over-rides
		TextureClass *tex = Peek_Material_Info()->Get_Texture(TextureIdx[pass]);
		Model->Initialize_Texture_Array(pass, 0, tex);
		tex->Release_Ref();

		// flag that we need to write the per polygon material overide array
		MultiTexture[pass] = true;
	}

	// add the material to the material info class if we cant find it in the 
	// list.  if we are not supposed to search the list for it then just add
	// it.
	if (!dont_search) {
		for (int lp = 0, found = 0; lp < Peek_Material_Info()->Texture_Count(); lp ++) {
			TextureClass *tex = Peek_Material_Info()->Get_Texture(lp);
			if (texture == tex) {
				TextureIdx[pass] = lp;
				found = true;
				tex->Release_Ref();
				break;
			}
			tex->Release_Ref();
		}
		if (!found) {
			Peek_Material_Info()->Add_Texture(texture);
			TextureIdx[pass] = Peek_Material_Info()->Texture_Count() - 1;
		}
	} else {
		Peek_Material_Info()->Add_Texture(texture);
		TextureIdx[pass] = Peek_Material_Info()->Texture_Count() - 1;
	}

	if (!MultiTexture[pass]) {
		TextureClass *tex = Peek_Material_Info()->Get_Texture(TextureIdx[pass]);
		Model->Set_Single_Texture(tex, pass);
		tex->Release_Ref();
	}
	return(TextureIdx[pass]);
}

/*
**
*/
// Remap locations to match a screen
void DynamicScreenMeshClass::Location( float x, float y, float z)	
{	
	DynamicMeshClass::Location( (x * 2) - 1, Aspect - (y * 2 * Aspect), 0); 
}

// For moving a vertex after the DynaMesh has already been created.
void DynamicScreenMeshClass::Move_Vertex(int index, float x, float y, float z)	
{	
	DynamicMeshClass::Move_Vertex( index, (x * 2) - 1, Aspect - (y * 2 * Aspect), 0); 
}

// Set position
void DynamicScreenMeshClass::Set_Position(const Vector3 &v)	
{ 
	DynamicMeshClass::Set_Position(Vector3(v.X * 2, -(v.Y * 2 * Aspect), 0)); 
}

void DynamicScreenMeshClass::Reset( void )		
{	
	Reset_Flags();	
	Reset_Mesh_Counters();	
}


