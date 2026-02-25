/**
 * D3DX8 Texture Helpers - Stub header for compilation only
 */
#pragma once
#ifndef _D3DX8TEX_H_
#define _D3DX8TEX_H_

#include <windows.h>
#include "d3d8.h"

/* ------------------------------------------------------------------ */
/*  Filter defines                                                     */
/* ------------------------------------------------------------------ */

#define D3DX_FILTER_NONE      0x00000001
#define D3DX_FILTER_POINT     0x00000002
#define D3DX_FILTER_LINEAR    0x00000003
#define D3DX_FILTER_TRIANGLE  0x00000004
#define D3DX_FILTER_BOX       0x00000005
#define D3DX_FILTER_MIRROR_U  0x00010000
#define D3DX_FILTER_MIRROR_V  0x00020000
#define D3DX_FILTER_MIRROR_W  0x00040000
#define D3DX_FILTER_MIRROR    0x00070000
#define D3DX_FILTER_DITHER    0x00080000
#define D3DX_FILTER_SRGB      0x00100000

#define D3DX_DEFAULT          ((UINT)-1)
#define D3DX_DEFAULT_NONPOW2  ((UINT)-2)

/* ------------------------------------------------------------------ */
/*  Image file format                                                  */
/* ------------------------------------------------------------------ */

typedef enum _D3DXIMAGE_FILEFORMAT {
    D3DXIFF_BMP         = 0,
    D3DXIFF_JPG         = 1,
    D3DXIFF_TGA         = 2,
    D3DXIFF_PNG         = 3,
    D3DXIFF_DDS         = 4,
    D3DXIFF_PPM         = 5,
    D3DXIFF_DIB         = 6,
    D3DXIFF_FORCE_DWORD = 0x7fffffff
} D3DXIMAGE_FILEFORMAT;

/* ------------------------------------------------------------------ */
/*  D3DXIMAGE_INFO                                                     */
/* ------------------------------------------------------------------ */

typedef struct _D3DXIMAGE_INFO {
    UINT                Width;
    UINT                Height;
    UINT                Depth;
    UINT                MipLevels;
    D3DFORMAT           Format;
    D3DRESOURCETYPE     ResourceType;
    D3DXIMAGE_FILEFORMAT ImageFileFormat;
} D3DXIMAGE_INFO;

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Function stubs - all return E_NOTIMPL                              */
/* ------------------------------------------------------------------ */

inline HRESULT D3DXGetImageInfoFromFileA(
    LPCSTR pSrcFile, D3DXIMAGE_INFO* pSrcInfo)
{
    (void)pSrcFile; (void)pSrcInfo;
    return E_NOTIMPL;
}

inline HRESULT D3DXGetImageInfoFromFileW(
    LPCWSTR pSrcFile, D3DXIMAGE_INFO* pSrcInfo)
{
    (void)pSrcFile; (void)pSrcInfo;
    return E_NOTIMPL;
}

inline HRESULT D3DXGetImageInfoFromFileInMemory(
    LPCVOID pSrcData, UINT SrcDataSize, D3DXIMAGE_INFO* pSrcInfo)
{
    (void)pSrcData; (void)SrcDataSize; (void)pSrcInfo;
    return E_NOTIMPL;
}

inline HRESULT D3DXGetImageInfoFromResourceA(
    HMODULE hSrcModule, LPCSTR pSrcResource, D3DXIMAGE_INFO* pSrcInfo)
{
    (void)hSrcModule; (void)pSrcResource; (void)pSrcInfo;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateTextureFromFileA(
    LPDIRECT3DDEVICE8 pDevice, LPCSTR pSrcFile, LPDIRECT3DTEXTURE8* ppTexture)
{
    (void)pDevice; (void)pSrcFile; (void)ppTexture;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateTextureFromFileW(
    LPDIRECT3DDEVICE8 pDevice, LPCWSTR pSrcFile, LPDIRECT3DTEXTURE8* ppTexture)
{
    (void)pDevice; (void)pSrcFile; (void)ppTexture;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateTextureFromFileExA(
    LPDIRECT3DDEVICE8 pDevice, LPCSTR pSrcFile, UINT Width, UINT Height,
    UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
    DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey,
    D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
    LPDIRECT3DTEXTURE8* ppTexture)
{
    (void)pDevice; (void)pSrcFile; (void)Width; (void)Height;
    (void)MipLevels; (void)Usage; (void)Format; (void)Pool;
    (void)Filter; (void)MipFilter; (void)ColorKey;
    (void)pSrcInfo; (void)pPalette; (void)ppTexture;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateTextureFromFileExW(
    LPDIRECT3DDEVICE8 pDevice, LPCWSTR pSrcFile, UINT Width, UINT Height,
    UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
    DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey,
    D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
    LPDIRECT3DTEXTURE8* ppTexture)
{
    (void)pDevice; (void)pSrcFile; (void)Width; (void)Height;
    (void)MipLevels; (void)Usage; (void)Format; (void)Pool;
    (void)Filter; (void)MipFilter; (void)ColorKey;
    (void)pSrcInfo; (void)pPalette; (void)ppTexture;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateTextureFromFileInMemory(
    LPDIRECT3DDEVICE8 pDevice, LPCVOID pSrcData, UINT SrcDataSize,
    LPDIRECT3DTEXTURE8* ppTexture)
{
    (void)pDevice; (void)pSrcData; (void)SrcDataSize; (void)ppTexture;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateTextureFromFileInMemoryEx(
    LPDIRECT3DDEVICE8 pDevice, LPCVOID pSrcData, UINT SrcDataSize,
    UINT Width, UINT Height, UINT MipLevels, DWORD Usage,
    D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter,
    D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo,
    PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE8* ppTexture)
{
    (void)pDevice; (void)pSrcData; (void)SrcDataSize;
    (void)Width; (void)Height; (void)MipLevels; (void)Usage;
    (void)Format; (void)Pool; (void)Filter; (void)MipFilter;
    (void)ColorKey; (void)pSrcInfo; (void)pPalette; (void)ppTexture;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateTextureFromResourceA(
    LPDIRECT3DDEVICE8 pDevice, HMODULE hSrcModule, LPCSTR pSrcResource,
    LPDIRECT3DTEXTURE8* ppTexture)
{
    (void)pDevice; (void)hSrcModule; (void)pSrcResource; (void)ppTexture;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateTexture(
    LPDIRECT3DDEVICE8 pDevice, UINT Width, UINT Height,
    UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
    LPDIRECT3DTEXTURE8* ppTexture)
{
    (void)pDevice; (void)Width; (void)Height;
    (void)MipLevels; (void)Usage; (void)Format; (void)Pool;
    (void)ppTexture;
    return E_NOTIMPL;
}

inline HRESULT D3DXFilterTexture(
    LPDIRECT3DBASETEXTURE8 pBaseTexture, const PALETTEENTRY* pPalette,
    UINT SrcLevel, DWORD Filter)
{
    (void)pBaseTexture; (void)pPalette; (void)SrcLevel; (void)Filter;
    return E_NOTIMPL;
}

inline HRESULT D3DXLoadSurfaceFromSurface(
    LPDIRECT3DSURFACE8 pDestSurface, const PALETTEENTRY* pDestPalette,
    const RECT* pDestRect, LPDIRECT3DSURFACE8 pSrcSurface,
    const PALETTEENTRY* pSrcPalette, const RECT* pSrcRect,
    DWORD Filter, D3DCOLOR ColorKey)
{
    (void)pDestSurface; (void)pDestPalette; (void)pDestRect;
    (void)pSrcSurface; (void)pSrcPalette; (void)pSrcRect;
    (void)Filter; (void)ColorKey;
    return E_NOTIMPL;
}

inline HRESULT D3DXLoadSurfaceFromMemory(
    LPDIRECT3DSURFACE8 pDestSurface, const PALETTEENTRY* pDestPalette,
    const RECT* pDestRect, LPCVOID pSrcMemory, D3DFORMAT SrcFormat,
    UINT SrcPitch, const PALETTEENTRY* pSrcPalette,
    const RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
{
    (void)pDestSurface; (void)pDestPalette; (void)pDestRect;
    (void)pSrcMemory; (void)SrcFormat; (void)SrcPitch;
    (void)pSrcPalette; (void)pSrcRect; (void)Filter; (void)ColorKey;
    return E_NOTIMPL;
}

inline HRESULT D3DXLoadSurfaceFromFileA(
    LPDIRECT3DSURFACE8 pDestSurface, const PALETTEENTRY* pDestPalette,
    const RECT* pDestRect, LPCSTR pSrcFile, const RECT* pSrcRect,
    DWORD Filter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo)
{
    (void)pDestSurface; (void)pDestPalette; (void)pDestRect;
    (void)pSrcFile; (void)pSrcRect; (void)Filter;
    (void)ColorKey; (void)pSrcInfo;
    return E_NOTIMPL;
}

inline HRESULT D3DXLoadSurfaceFromFileInMemory(
    LPDIRECT3DSURFACE8 pDestSurface, const PALETTEENTRY* pDestPalette,
    const RECT* pDestRect, LPCVOID pSrcData, UINT SrcDataSize,
    const RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey,
    D3DXIMAGE_INFO* pSrcInfo)
{
    (void)pDestSurface; (void)pDestPalette; (void)pDestRect;
    (void)pSrcData; (void)SrcDataSize; (void)pSrcRect;
    (void)Filter; (void)ColorKey; (void)pSrcInfo;
    return E_NOTIMPL;
}

inline HRESULT D3DXSaveSurfaceToFileA(
    LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat,
    LPDIRECT3DSURFACE8 pSrcSurface, const PALETTEENTRY* pSrcPalette,
    const RECT* pSrcRect)
{
    (void)pDestFile; (void)DestFormat; (void)pSrcSurface;
    (void)pSrcPalette; (void)pSrcRect;
    return E_NOTIMPL;
}

inline HRESULT D3DXSaveTextureToFileA(
    LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat,
    LPDIRECT3DBASETEXTURE8 pSrcTexture, const PALETTEENTRY* pSrcPalette)
{
    (void)pDestFile; (void)DestFormat; (void)pSrcTexture; (void)pSrcPalette;
    return E_NOTIMPL;
}

inline HRESULT D3DXCreateCubeTextureFromFileA(
    LPDIRECT3DDEVICE8 pDevice, LPCSTR pSrcFile,
    LPDIRECT3DCUBETEXTURE8* ppCubeTexture)
{
    (void)pDevice; (void)pSrcFile; (void)ppCubeTexture;
    return E_NOTIMPL;
}

inline HRESULT D3DXCheckTextureRequirements(
    LPDIRECT3DDEVICE8 pDevice, UINT* pWidth, UINT* pHeight,
    UINT* pNumMipLevels, DWORD Usage, D3DFORMAT* pFormat, D3DPOOL Pool)
{
    (void)pDevice; (void)pWidth; (void)pHeight;
    (void)pNumMipLevels; (void)Usage; (void)pFormat; (void)Pool;
    return E_NOTIMPL;
}

/* Unicode/ANSI mappings */
#ifdef UNICODE
#define D3DXCreateTextureFromFile        D3DXCreateTextureFromFileW
#define D3DXCreateTextureFromFileEx      D3DXCreateTextureFromFileExW
#define D3DXGetImageInfoFromFile         D3DXGetImageInfoFromFileW
#else
#define D3DXCreateTextureFromFile        D3DXCreateTextureFromFileA
#define D3DXCreateTextureFromFileEx      D3DXCreateTextureFromFileExA
#define D3DXGetImageInfoFromFile         D3DXGetImageInfoFromFileA
#define D3DXSaveSurfaceToFile            D3DXSaveSurfaceToFileA
#define D3DXSaveTextureToFile            D3DXSaveTextureToFileA
#define D3DXLoadSurfaceFromFile          D3DXLoadSurfaceFromFileA
#define D3DXCreateTextureFromResource    D3DXCreateTextureFromResourceA
#define D3DXGetImageInfoFromResource     D3DXGetImageInfoFromResourceA
#define D3DXCreateCubeTextureFromFile    D3DXCreateCubeTextureFromFileA
#endif

#ifdef __cplusplus
}
#endif

#endif /* _D3DX8TEX_H_ */
