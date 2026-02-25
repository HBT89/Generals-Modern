/**
 * D3DX8 Core - Stub header for compilation only
 */
#pragma once
#ifndef _D3DX8CORE_H_
#define _D3DX8CORE_H_

#include <windows.h>
#include "d3d8.h"

#ifdef __cplusplus

/* ------------------------------------------------------------------ */
/*  ID3DXBuffer                                                        */
/* ------------------------------------------------------------------ */

struct ID3DXBuffer : public IUnknown {
    virtual LPVOID __stdcall GetBufferPointer() = 0;
    virtual DWORD  __stdcall GetBufferSize() = 0;
};

typedef ID3DXBuffer* LPD3DXBUFFER;

/* ------------------------------------------------------------------ */
/*  ID3DXFont                                                          */
/* ------------------------------------------------------------------ */

struct ID3DXFont : public IUnknown {
    virtual HRESULT __stdcall GetDevice(LPDIRECT3DDEVICE8* ppDevice) = 0;
    virtual HRESULT __stdcall GetLogFont(LOGFONTA* pLogFont) = 0;
    virtual HRESULT __stdcall Begin() = 0;
    virtual INT     __stdcall DrawTextA(LPCSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) = 0;
    virtual INT     __stdcall DrawTextW(LPCWSTR pString, INT Count, LPRECT pRect, DWORD Format, D3DCOLOR Color) = 0;
    virtual HRESULT __stdcall End() = 0;
    virtual HRESULT __stdcall OnLostDevice() = 0;
    virtual HRESULT __stdcall OnResetDevice() = 0;
};

typedef ID3DXFont* LPD3DXFONT;

/* ------------------------------------------------------------------ */
/*  ID3DXSprite                                                        */
/* ------------------------------------------------------------------ */

struct ID3DXSprite : public IUnknown {
    virtual HRESULT __stdcall GetDevice(LPDIRECT3DDEVICE8* ppDevice) = 0;
    virtual HRESULT __stdcall Begin() = 0;
    virtual HRESULT __stdcall Draw(LPDIRECT3DTEXTURE8 pSrcTexture,
                                   const RECT* pSrcRect,
                                   const D3DXVECTOR2* pScaling,
                                   const D3DXVECTOR2* pRotationCenter,
                                   FLOAT Rotation,
                                   const D3DXVECTOR2* pTranslation,
                                   D3DCOLOR Color) = 0;
    virtual HRESULT __stdcall DrawTransform(LPDIRECT3DTEXTURE8 pSrcTexture,
                                            const RECT* pSrcRect,
                                            const D3DXMATRIX* pTransform,
                                            D3DCOLOR Color) = 0;
    virtual HRESULT __stdcall End() = 0;
    virtual HRESULT __stdcall OnLostDevice() = 0;
    virtual HRESULT __stdcall OnResetDevice() = 0;
};

typedef ID3DXSprite* LPD3DXSPRITE;

/* ------------------------------------------------------------------ */
/*  ID3DXRenderToSurface                                               */
/* ------------------------------------------------------------------ */

typedef struct _D3DXRTS_DESC {
    UINT                Width;
    UINT                Height;
    D3DFORMAT           Format;
    BOOL                DepthStencil;
    D3DFORMAT           DepthStencilFormat;
} D3DXRTS_DESC;

struct ID3DXRenderToSurface : public IUnknown {
    virtual HRESULT __stdcall GetDevice(LPDIRECT3DDEVICE8* ppDevice) = 0;
    virtual HRESULT __stdcall GetDesc(D3DXRTS_DESC* pDesc) = 0;
    virtual HRESULT __stdcall BeginScene(LPDIRECT3DSURFACE8 pSurface, const D3DVIEWPORT8* pViewport) = 0;
    virtual HRESULT __stdcall EndScene() = 0;
    virtual HRESULT __stdcall OnLostDevice() = 0;
    virtual HRESULT __stdcall OnResetDevice() = 0;
};

typedef ID3DXRenderToSurface* LPD3DXRENDERTOSURFACE;

/* ------------------------------------------------------------------ */
/*  Function stubs                                                     */
/* ------------------------------------------------------------------ */

inline HRESULT D3DXCreateFont(
    LPDIRECT3DDEVICE8 pDevice, HFONT hFont, LPD3DXFONT* ppFont)
{
    (void)pDevice; (void)hFont; (void)ppFont;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateFontIndirect(
    LPDIRECT3DDEVICE8 pDevice, const LOGFONTA* pLogFont, LPD3DXFONT* ppFont)
{
    (void)pDevice; (void)pLogFont; (void)ppFont;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateSprite(
    LPDIRECT3DDEVICE8 pDevice, LPD3DXSPRITE* ppSprite)
{
    (void)pDevice; (void)ppSprite;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateRenderToSurface(
    LPDIRECT3DDEVICE8 pDevice, UINT Width, UINT Height, D3DFORMAT Format,
    BOOL DepthStencil, D3DFORMAT DepthStencilFormat,
    LPD3DXRENDERTOSURFACE* ppRenderToSurface)
{
    (void)pDevice; (void)Width; (void)Height; (void)Format;
    (void)DepthStencil; (void)DepthStencilFormat; (void)ppRenderToSurface;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateBuffer(DWORD NumBytes, LPD3DXBUFFER* ppBuffer)
{
    (void)NumBytes; (void)ppBuffer;
    return E_NOTIMPL;
}

inline HRESULT D3DXAssembleShader(
    LPCVOID pSrcData, UINT SrcDataLen, DWORD Flags,
    LPD3DXBUFFER* ppConstants, LPD3DXBUFFER* ppCompiledShader,
    LPD3DXBUFFER* ppCompilationErrors)
{
    (void)pSrcData; (void)SrcDataLen; (void)Flags;
    (void)ppConstants; (void)ppCompiledShader; (void)ppCompilationErrors;
    return E_NOTIMPL;
}

inline HRESULT D3DXAssembleShaderFromFileA(
    LPCSTR pSrcFile, DWORD Flags,
    LPD3DXBUFFER* ppConstants, LPD3DXBUFFER* ppCompiledShader,
    LPD3DXBUFFER* ppCompilationErrors)
{
    (void)pSrcFile; (void)Flags;
    (void)ppConstants; (void)ppCompiledShader; (void)ppCompilationErrors;
    return E_NOTIMPL;
}

#ifdef UNICODE
#define D3DXAssembleShaderFromFile D3DXAssembleShaderFromFileW
#else
#define D3DXAssembleShaderFromFile D3DXAssembleShaderFromFileA
#endif

#endif /* __cplusplus */

#endif /* _D3DX8CORE_H_ */
