/**
 * D3DX8 Effect - Stub header for compilation only
 */
#pragma once
#ifndef _D3DX8EFFECT_H_
#define _D3DX8EFFECT_H_

#include <windows.h>
#include "d3d8.h"
#include "d3dx8core.h"

#ifdef __cplusplus

/* ------------------------------------------------------------------ */
/*  D3DXEFFECT_DESC                                                    */
/* ------------------------------------------------------------------ */

typedef struct _D3DXEFFECT_DESC {
    UINT Parameters;
    UINT Techniques;
} D3DXEFFECT_DESC;

typedef struct _D3DXTECHNIQUE_DESC {
    LPCSTR Name;
    UINT   Passes;
} D3DXTECHNIQUE_DESC;

typedef struct _D3DXPASS_DESC {
    LPCSTR Name;
} D3DXPASS_DESC;

typedef struct _D3DXPARAMETER_DESC {
    LPCSTR Name;
    LPCSTR Semantic;
    DWORD  Type;
    DWORD  Class;
    UINT   Elements;
    UINT   Annotations;
    UINT   Bytes;
} D3DXPARAMETER_DESC;

typedef LPCSTR D3DXHANDLE;

/* ------------------------------------------------------------------ */
/*  ID3DXEffect                                                        */
/* ------------------------------------------------------------------ */

struct ID3DXEffect : public IUnknown {
    virtual HRESULT __stdcall GetDesc(D3DXEFFECT_DESC* pDesc) = 0;
    virtual HRESULT __stdcall GetParameterDesc(D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc) = 0;
    virtual HRESULT __stdcall GetTechniqueDesc(D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc) = 0;
    virtual HRESULT __stdcall GetPassDesc(D3DXHANDLE hPass, D3DXPASS_DESC* pDesc) = 0;
    virtual D3DXHANDLE __stdcall GetParameter(D3DXHANDLE hParameter, UINT Index) = 0;
    virtual D3DXHANDLE __stdcall GetParameterByName(D3DXHANDLE hParameter, LPCSTR pName) = 0;
    virtual D3DXHANDLE __stdcall GetParameterBySemantic(D3DXHANDLE hParameter, LPCSTR pSemantic) = 0;
    virtual D3DXHANDLE __stdcall GetTechnique(UINT Index) = 0;
    virtual D3DXHANDLE __stdcall GetTechniqueByName(LPCSTR pName) = 0;
    virtual D3DXHANDLE __stdcall GetPass(D3DXHANDLE hTechnique, UINT Index) = 0;
    virtual HRESULT __stdcall SetValue(D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes) = 0;
    virtual HRESULT __stdcall GetValue(D3DXHANDLE hParameter, LPVOID pData, UINT Bytes) = 0;
    virtual HRESULT __stdcall SetBool(D3DXHANDLE hParameter, BOOL b) = 0;
    virtual HRESULT __stdcall GetBool(D3DXHANDLE hParameter, BOOL* pb) = 0;
    virtual HRESULT __stdcall SetInt(D3DXHANDLE hParameter, INT n) = 0;
    virtual HRESULT __stdcall GetInt(D3DXHANDLE hParameter, INT* pn) = 0;
    virtual HRESULT __stdcall SetFloat(D3DXHANDLE hParameter, FLOAT f) = 0;
    virtual HRESULT __stdcall GetFloat(D3DXHANDLE hParameter, FLOAT* pf) = 0;
    virtual HRESULT __stdcall SetVector(D3DXHANDLE hParameter, const D3DXVECTOR4* pVector) = 0;
    virtual HRESULT __stdcall GetVector(D3DXHANDLE hParameter, D3DXVECTOR4* pVector) = 0;
    virtual HRESULT __stdcall SetMatrix(D3DXHANDLE hParameter, const D3DXMATRIX* pMatrix) = 0;
    virtual HRESULT __stdcall GetMatrix(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) = 0;
    virtual HRESULT __stdcall SetTexture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE8 pTexture) = 0;
    virtual HRESULT __stdcall GetTexture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE8* ppTexture) = 0;
    virtual HRESULT __stdcall SetTechnique(D3DXHANDLE hTechnique) = 0;
    virtual D3DXHANDLE __stdcall GetCurrentTechnique() = 0;
    virtual HRESULT __stdcall ValidateTechnique(D3DXHANDLE hTechnique) = 0;
    virtual HRESULT __stdcall FindNextValidTechnique(D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique) = 0;
    virtual HRESULT __stdcall Begin(UINT* pPasses, DWORD Flags) = 0;
    virtual HRESULT __stdcall Pass(UINT Pass) = 0;
    virtual HRESULT __stdcall End() = 0;
    virtual HRESULT __stdcall OnLostDevice() = 0;
    virtual HRESULT __stdcall OnResetDevice() = 0;
    virtual HRESULT __stdcall CloneEffect(LPDIRECT3DDEVICE8 pDevice, ID3DXEffect** ppEffect) = 0;
};

typedef ID3DXEffect* LPD3DXEFFECT;

/* ------------------------------------------------------------------ */
/*  D3DXCreateEffect stub                                              */
/* ------------------------------------------------------------------ */

inline HRESULT D3DXCreateEffect(
    LPDIRECT3DDEVICE8 pDevice, LPCVOID pSrcData, UINT SrcDataLen,
    LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
    (void)pDevice; (void)pSrcData; (void)SrcDataLen;
    (void)ppEffect; (void)ppCompilationErrors;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateEffectFromFileA(
    LPDIRECT3DDEVICE8 pDevice, LPCSTR pSrcFile,
    LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
    (void)pDevice; (void)pSrcFile;
    (void)ppEffect; (void)ppCompilationErrors;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateEffectFromResourceA(
    LPDIRECT3DDEVICE8 pDevice, HMODULE hSrcModule, LPCSTR pSrcResource,
    LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
    (void)pDevice; (void)hSrcModule; (void)pSrcResource;
    (void)ppEffect; (void)ppCompilationErrors;
    return E_NOTIMPL;
}

#ifdef UNICODE
#define D3DXCreateEffectFromFile D3DXCreateEffectFromFileW
#else
#define D3DXCreateEffectFromFile D3DXCreateEffectFromFileA
#endif

#endif /* __cplusplus */

#endif /* _D3DX8EFFECT_H_ */
