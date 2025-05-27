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
 *                 Project Name : Commando	                              * 
 *                                                                         * 
 *                     $Archive:: /Commando/Code/ww3d2/dynamesh.h         $* 
 *                                                                         * 
 *                      $Author:: Greg_h                                  $* 
 *                                                                         * 
 *                     $Modtime:: 12/03/01 4:20p                          $* 
 *                                                                         * 
 *                    $Revision:: 15                                      $* 
 *                                                                         * 
 *-------------------------------------------------------------------------*/


#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DYNAMESH_H
#define DYNAMESH_H

#include "meshgeometry.h"
#include "meshmatdesc.h"
#include "matinfo.h"
#include "rendobj.h"
#include "polyinfo.h"
#include <bgfx/bgfx.h>
#include "BGFXVertexBuffer.h"
#include "BGFXIndexBuffer.h"

class	ShaderClass;
class	IntersectionClass;
class	IntersectionResultClass;

/*
** DynamicMeshModel: used for low-level rendering of DynamicMeshClass.
** It is composed of the same two classes (one base, one embedded as
** MeshModelClass, so many of its features are similar (see meshmdl.h)
*/
class DynamicMeshModel : public MeshGeometryClass
{
	// BGFX: Removed W3DMPO_GLUE(DynamicMeshModel) macro, obsolete with BGFX

public:

	DynamicMeshModel(unsigned int max_polys, unsigned int max_verts);
	DynamicMeshModel(unsigned int max_polys, unsigned int max_verts, MaterialInfoClass *mat_info);
	DynamicMeshModel(const DynamicMeshModel &src);
	~DynamicMeshModel(void);

	// Inherited from MeshGeometryClass
	virtual void	Compute_Plane_Equations(void);
	virtual void	Compute_Vertex_Normals(void);
	virtual void	Compute_Bounds(Vector3 * verts);

	// Reset mesh (with existing max polygon and max vertex counts)
	void	Reset(void);

	// Render mesh
	void	Render(RenderInfoClass & rinfo);

	// Set current polygon and vertex counts
	void	Set_Counts(int pnum, int vnum)	{ DynamicMeshPNum = pnum;	DynamicMeshVNum = vnum;	}

	// Access to material stuff:
	unsigned *	Get_Color_Array(int color_array_index) { return MatDesc->Get_Color_Array(color_array_index); }
	Vector2 *	Get_UV_Array(int uv_array_index) { return MatDesc->Get_UV_Array_By_Index(uv_array_index); }

	void			Set_Single_Material(VertexMaterialClass * vmat, int pass=0) { MatDesc->Set_Single_Material(vmat, pass); }
	void			Set_Single_Texture(TextureClass * tex, int pass=0, int stage=0) { MatDesc->Set_Single_Texture(tex, pass, stage); }
	void			Set_Single_Shader(ShaderClass shader, int pass=0) { MatDesc->Set_Single_Shader(shader, pass); }
	
	void			Set_Material(int vidx, VertexMaterialClass * vmat, int pass=0) { MatDesc->Set_Material(vidx, vmat, pass); }
	void			Set_Shader(int pidx, ShaderClass shader, int pass=0)  { MatDesc->Set_Shader(pidx, shader, pass); }
	void			Set_Texture(int pidx, TextureClass * tex, int pass=0, int stage=0)  { MatDesc->Set_Texture(pidx, tex, pass, stage); }
	void			Set_Pass_Count(int passes)	{ MatDesc->Set_Pass_Count(passes); }
	int			Get_Pass_Count(void) const	{ return MatDesc->Get_Pass_Count(); }

	// Create the array (if it doesn't exist), fill it with the supplied value.
	void			Initialize_Texture_Array(int pass, int stage, TextureClass *texture = NULL);
	void			Initialize_Material_Array(int pass, VertexMaterialClass *vmat = NULL);

	// Accessors to material info:
	MaterialInfoClass		*Peek_Material_Info(void)			{ return MatInfo; }
	MaterialInfoClass		*Get_Material_Info(void)			{ if (MatInfo) MatInfo->Add_Ref(); return MatInfo;}
	void Set_Material_Info(MaterialInfoClass *mat_info)
	{ 
		if (MatInfo) 
			MatInfo->Release_Ref(); 
		WWASSERT(MatInfo != 0);
		MatInfo = mat_info; 
		MatInfo->Add_Ref(); 
	}

	// New geometry accessors (non-const)
	TriIndex *	Get_Non_Const_Polygon_Array(void);
	Vector3 *	Get_Non_Const_Vertex_Normal_Array(void);

	// BGFX: Vertex/index buffer accessors
	BGFXVertexBuffer& Get_Vertex_Buffer() { return m_vb; }
	BGFXIndexBuffer& Get_Index_Buffer() { return m_ib; }

private:

	// These are current counts, as opposed to those in the mesh geometry and
	// material description which are actually maximum counts.
	int						DynamicMeshPNum;
	int						DynamicMeshVNum;

	// All non-geometry properties (uvs, colors, textures, shaders, etc)
	MeshMatDescClass *	MatDesc;

	// Lists of textures and vertex materials for ease of processing
	MaterialInfoClass *	MatInfo;

	// BGFX buffers (optional: cache for dynamic meshes)
	BGFXVertexBuffer m_vb;
	BGFXIndexBuffer m_ib;
};

	// function to clone a dynamic screen mesh class
	virtual RenderObjClass * 		Clone(void) const	{ return NEW_REF( DynamicScreenMeshClass, (*this)); }

	// class id of this render object
	virtual int	Class_ID(void) const	{ return CLASSID_DYNASCREENMESH; }

	// Remap locations to match a screen
	virtual void Location( float x, float y, float z = 0.0f);

	// For moving a vertex after the DynaMesh has already been created.
   virtual void Move_Vertex(int index, float x, float y, float z = 0.0f);

	// Set position
	virtual void Set_Position(const Vector3 &v);

	virtual void Reset( void);

	virtual void Set_Aspect(float aspect) { Aspect=aspect; };

protected:

	//	 Aspect Ratio of the virtual screen.
	//  1.0 gives a -1,-1 to 1,1 display
	//  3/4 givs a -1,-3/4 to 1,3/4 display
	float		Aspect;

	// tells when the triangle needs to be back flipped
	virtual	bool	Flip_Face( void) { return !DynamicMeshClass::Flip_Face(); }
};

#endif	// DYNAMESH