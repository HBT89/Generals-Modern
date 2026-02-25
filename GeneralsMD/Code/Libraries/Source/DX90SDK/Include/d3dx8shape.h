/**
 * D3DX8 Shape - Stub header for compilation only
 */
#pragma once
#ifndef _D3DX8SHAPE_H_
#define _D3DX8SHAPE_H_

#include <windows.h>
#include "d3d8.h"
#include "d3dx8core.h"

#ifdef __cplusplus

/* ------------------------------------------------------------------ */
/*  ID3DXMesh and related interfaces                                   */
/* ------------------------------------------------------------------ */

struct ID3DXBaseMesh : public IUnknown {
    virtual HRESULT __stdcall DrawSubset(DWORD AttribId) = 0;
    virtual DWORD   __stdcall GetNumFaces() = 0;
    virtual DWORD   __stdcall GetNumVertices() = 0;
    virtual DWORD   __stdcall GetFVF() = 0;
    virtual HRESULT __stdcall GetDeclaration(DWORD Declaration[]) = 0;
    virtual DWORD   __stdcall GetNumBytesPerVertex() = 0;
    virtual DWORD   __stdcall GetOptions() = 0;
    virtual HRESULT __stdcall GetDevice(LPDIRECT3DDEVICE8* ppDevice) = 0;
    virtual HRESULT __stdcall CloneMeshFVF(DWORD Options, DWORD FVF, LPDIRECT3DDEVICE8 pDevice, ID3DXBaseMesh** ppCloneMesh) = 0;
    virtual HRESULT __stdcall GetVertexBuffer(LPDIRECT3DVERTEXBUFFER8* ppVB) = 0;
    virtual HRESULT __stdcall GetIndexBuffer(LPDIRECT3DINDEXBUFFER8* ppIB) = 0;
    virtual HRESULT __stdcall LockVertexBuffer(DWORD Flags, BYTE** ppData) = 0;
    virtual HRESULT __stdcall UnlockVertexBuffer() = 0;
    virtual HRESULT __stdcall LockIndexBuffer(DWORD Flags, BYTE** ppData) = 0;
    virtual HRESULT __stdcall UnlockIndexBuffer() = 0;
    virtual HRESULT __stdcall GetAttributeTable(void* pAttribTable, DWORD* pAttribTableSize) = 0;
};

struct ID3DXMesh : public ID3DXBaseMesh {
    virtual HRESULT __stdcall LockAttributeBuffer(DWORD Flags, DWORD** ppData) = 0;
    virtual HRESULT __stdcall UnlockAttributeBuffer() = 0;
    virtual HRESULT __stdcall Optimize(DWORD Flags, const DWORD* pAdjacencyIn, DWORD* pAdjacencyOut, DWORD* pFaceRemap, LPD3DXBUFFER* ppVertexRemap, ID3DXMesh** ppOptMesh) = 0;
    virtual HRESULT __stdcall OptimizeInplace(DWORD Flags, const DWORD* pAdjacencyIn, DWORD* pAdjacencyOut, DWORD* pFaceRemap, LPD3DXBUFFER* ppVertexRemap) = 0;
};

typedef ID3DXBaseMesh* LPD3DXBASEMESH;
typedef ID3DXMesh*     LPD3DXMESH;

/* ------------------------------------------------------------------ */
/*  Shape creation stubs                                               */
/* ------------------------------------------------------------------ */

inline HRESULT D3DXCreateSphere(
    LPDIRECT3DDEVICE8 pDevice, FLOAT Radius, UINT Slices, UINT Stacks,
    LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency)
{
    (void)pDevice; (void)Radius; (void)Slices; (void)Stacks;
    (void)ppMesh; (void)ppAdjacency;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateBox(
    LPDIRECT3DDEVICE8 pDevice, FLOAT Width, FLOAT Height, FLOAT Depth,
    LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency)
{
    (void)pDevice; (void)Width; (void)Height; (void)Depth;
    (void)ppMesh; (void)ppAdjacency;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateCylinder(
    LPDIRECT3DDEVICE8 pDevice, FLOAT Radius1, FLOAT Radius2,
    FLOAT Length, UINT Slices, UINT Stacks,
    LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency)
{
    (void)pDevice; (void)Radius1; (void)Radius2; (void)Length;
    (void)Slices; (void)Stacks; (void)ppMesh; (void)ppAdjacency;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateTorus(
    LPDIRECT3DDEVICE8 pDevice, FLOAT InnerRadius, FLOAT OuterRadius,
    UINT Sides, UINT Rings,
    LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency)
{
    (void)pDevice; (void)InnerRadius; (void)OuterRadius;
    (void)Sides; (void)Rings; (void)ppMesh; (void)ppAdjacency;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreatePolygon(
    LPDIRECT3DDEVICE8 pDevice, FLOAT Length, UINT Sides,
    LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency)
{
    (void)pDevice; (void)Length; (void)Sides;
    (void)ppMesh; (void)ppAdjacency;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateMeshFVF(
    DWORD NumFaces, DWORD NumVertices, DWORD Options, DWORD FVF,
    LPDIRECT3DDEVICE8 pDevice, LPD3DXMESH* ppMesh)
{
    (void)NumFaces; (void)NumVertices; (void)Options; (void)FVF;
    (void)pDevice; (void)ppMesh;
    return E_NOTIMPL;
}

inline HRESULT D3DXComputeNormals(LPD3DXBASEMESH pMesh, const DWORD* pAdjacency) {
    (void)pMesh; (void)pAdjacency;
    return E_NOTIMPL;
}

inline HRESULT D3DXDeclaratorFromFVF(DWORD FVF, DWORD Declaration[]) {
    (void)FVF; (void)Declaration;
    return E_NOTIMPL;
}

inline HRESULT D3DXFVFFromDeclarator(const DWORD* pDeclarator, DWORD* pFVF) {
    (void)pDeclarator; (void)pFVF;
    return E_NOTIMPL;
}

#endif /* __cplusplus */

#endif /* _D3DX8SHAPE_H_ */
