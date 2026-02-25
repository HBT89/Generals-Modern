/**
 * Direct3D 8 - Stub header for compilation only
 */
#pragma once
#ifndef _D3D8_H_
#define _D3D8_H_

#include <windows.h>
#include <objbase.h>

#include "d3d8types.h"
#include "d3d8caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  GUIDs                                                              */
/* ------------------------------------------------------------------ */

/* IID_IDirect3D8 / IID_IDirect3DDevice8 etc. - defined as externs     */
/* The game only needs them to compile, not link against real GUIDs.    */

/* ------------------------------------------------------------------ */
/*  SDK version                                                        */
/* ------------------------------------------------------------------ */

#define D3D_SDK_VERSION 220

/* ------------------------------------------------------------------ */
/*  Forward declarations                                               */
/* ------------------------------------------------------------------ */

#ifdef __cplusplus

struct IDirect3D8;
struct IDirect3DDevice8;
struct IDirect3DResource8;
struct IDirect3DBaseTexture8;
struct IDirect3DTexture8;
struct IDirect3DCubeTexture8;
struct IDirect3DVolumeTexture8;
struct IDirect3DVertexBuffer8;
struct IDirect3DIndexBuffer8;
struct IDirect3DSurface8;
struct IDirect3DVolume8;
struct IDirect3DSwapChain8;

/* ------------------------------------------------------------------ */
/*  Pointer typedefs                                                   */
/* ------------------------------------------------------------------ */

typedef IDirect3D8*              LPDIRECT3D8;
typedef IDirect3DDevice8*        LPDIRECT3DDEVICE8;
typedef IDirect3DResource8*      LPDIRECT3DRESOURCE8;
typedef IDirect3DBaseTexture8*   LPDIRECT3DBASETEXTURE8;
typedef IDirect3DTexture8*       LPDIRECT3DTEXTURE8;
typedef IDirect3DCubeTexture8*   LPDIRECT3DCUBETEXTURE8;
typedef IDirect3DVolumeTexture8* LPDIRECT3DVOLUMETEXTURE8;
typedef IDirect3DVertexBuffer8*  LPDIRECT3DVERTEXBUFFER8;
typedef IDirect3DIndexBuffer8*   LPDIRECT3DINDEXBUFFER8;
typedef IDirect3DSurface8*       LPDIRECT3DSURFACE8;
typedef IDirect3DVolume8*        LPDIRECT3DVOLUME8;
typedef IDirect3DSwapChain8*     LPDIRECT3DSWAPCHAIN8;

typedef IDirect3D8*              PDIRECT3D8;
typedef IDirect3DDevice8*        PDIRECT3DDEVICE8;
typedef IDirect3DTexture8*       PDIRECT3DTEXTURE8;
typedef IDirect3DSurface8*       PDIRECT3DSURFACE8;
typedef IDirect3DVertexBuffer8*  PDIRECT3DVERTEXBUFFER8;
typedef IDirect3DIndexBuffer8*   PDIRECT3DINDEXBUFFER8;

/* ------------------------------------------------------------------ */
/*  IDirect3DResource8                                                 */
/* ------------------------------------------------------------------ */

struct IDirect3DResource8 : public IUnknown {
    virtual HRESULT __stdcall GetDevice(IDirect3DDevice8** ppDevice) = 0;
    virtual HRESULT __stdcall SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) = 0;
    virtual HRESULT __stdcall GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) = 0;
    virtual HRESULT __stdcall FreePrivateData(REFGUID refguid) = 0;
    virtual DWORD   __stdcall SetPriority(DWORD PriorityNew) = 0;
    virtual DWORD   __stdcall GetPriority() = 0;
    virtual void    __stdcall PreLoad() = 0;
    virtual D3DRESOURCETYPE __stdcall GetType() = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3DBaseTexture8                                              */
/* ------------------------------------------------------------------ */

struct IDirect3DBaseTexture8 : public IDirect3DResource8 {
    virtual DWORD __stdcall SetLOD(DWORD LODNew) = 0;
    virtual DWORD __stdcall GetLOD() = 0;
    virtual DWORD __stdcall GetLevelCount() = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3DTexture8                                                  */
/* ------------------------------------------------------------------ */

struct IDirect3DTexture8 : public IDirect3DBaseTexture8 {
    virtual HRESULT __stdcall GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) = 0;
    virtual HRESULT __stdcall GetSurfaceLevel(UINT Level, IDirect3DSurface8** ppSurfaceLevel) = 0;
    virtual HRESULT __stdcall LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) = 0;
    virtual HRESULT __stdcall UnlockRect(UINT Level) = 0;
    virtual HRESULT __stdcall AddDirtyRect(const RECT* pDirtyRect) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3DCubeTexture8                                              */
/* ------------------------------------------------------------------ */

struct IDirect3DCubeTexture8 : public IDirect3DBaseTexture8 {
    virtual HRESULT __stdcall GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) = 0;
    virtual HRESULT __stdcall GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface8** ppCubeMapSurface) = 0;
    virtual HRESULT __stdcall LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) = 0;
    virtual HRESULT __stdcall UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level) = 0;
    virtual HRESULT __stdcall AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3DVolumeTexture8                                            */
/* ------------------------------------------------------------------ */

struct IDirect3DVolumeTexture8 : public IDirect3DBaseTexture8 {
    virtual HRESULT __stdcall GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc) = 0;
    virtual HRESULT __stdcall GetVolumeLevel(UINT Level, IDirect3DVolume8** ppVolumeLevel) = 0;
    virtual HRESULT __stdcall LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags) = 0;
    virtual HRESULT __stdcall UnlockBox(UINT Level) = 0;
    virtual HRESULT __stdcall AddDirtyBox(const D3DBOX* pDirtyBox) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3DVertexBuffer8                                             */
/* ------------------------------------------------------------------ */

struct IDirect3DVertexBuffer8 : public IDirect3DResource8 {
    virtual HRESULT __stdcall Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags) = 0;
    virtual HRESULT __stdcall Unlock() = 0;
    virtual HRESULT __stdcall GetDesc(D3DVERTEXBUFFER_DESC* pDesc) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3DIndexBuffer8                                              */
/* ------------------------------------------------------------------ */

struct IDirect3DIndexBuffer8 : public IDirect3DResource8 {
    virtual HRESULT __stdcall Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags) = 0;
    virtual HRESULT __stdcall Unlock() = 0;
    virtual HRESULT __stdcall GetDesc(D3DINDEXBUFFER_DESC* pDesc) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3DSurface8                                                  */
/* ------------------------------------------------------------------ */

struct IDirect3DSurface8 : public IUnknown {
    virtual HRESULT __stdcall GetDevice(IDirect3DDevice8** ppDevice) = 0;
    virtual HRESULT __stdcall SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) = 0;
    virtual HRESULT __stdcall GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) = 0;
    virtual HRESULT __stdcall FreePrivateData(REFGUID refguid) = 0;
    virtual HRESULT __stdcall GetContainer(REFIID riid, void** ppContainer) = 0;
    virtual HRESULT __stdcall GetDesc(D3DSURFACE_DESC* pDesc) = 0;
    virtual HRESULT __stdcall LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) = 0;
    virtual HRESULT __stdcall UnlockRect() = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3DVolume8                                                   */
/* ------------------------------------------------------------------ */

struct IDirect3DVolume8 : public IUnknown {
    virtual HRESULT __stdcall GetDevice(IDirect3DDevice8** ppDevice) = 0;
    virtual HRESULT __stdcall SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) = 0;
    virtual HRESULT __stdcall GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) = 0;
    virtual HRESULT __stdcall FreePrivateData(REFGUID refguid) = 0;
    virtual HRESULT __stdcall GetContainer(REFIID riid, void** ppContainer) = 0;
    virtual HRESULT __stdcall GetDesc(D3DVOLUME_DESC* pDesc) = 0;
    virtual HRESULT __stdcall LockBox(D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags) = 0;
    virtual HRESULT __stdcall UnlockBox() = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3DSwapChain8                                                */
/* ------------------------------------------------------------------ */

struct IDirect3DSwapChain8 : public IUnknown {
    virtual HRESULT __stdcall Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) = 0;
    virtual HRESULT __stdcall GetBackBuffer(UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface8** ppBackBuffer) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3DDevice8                                                   */
/* ------------------------------------------------------------------ */

struct IDirect3DDevice8 : public IUnknown {
    /*** IDirect3DDevice8 methods ***/
    virtual HRESULT __stdcall TestCooperativeLevel() = 0;
    virtual UINT    __stdcall GetAvailableTextureMem() = 0;
    virtual HRESULT __stdcall ResourceManagerDiscardBytes(DWORD Bytes) = 0;
    virtual HRESULT __stdcall GetDirect3D(IDirect3D8** ppD3D8) = 0;
    virtual HRESULT __stdcall GetDeviceCaps(D3DCAPS8* pCaps) = 0;
    virtual HRESULT __stdcall GetDisplayMode(D3DDISPLAYMODE* pMode) = 0;
    virtual HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) = 0;
    virtual HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8* pCursorBitmap) = 0;
    virtual void    __stdcall SetCursorPosition(int X, int Y, DWORD Flags) = 0;
    virtual BOOL    __stdcall ShowCursor(BOOL bShow) = 0;
    virtual HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain8** pSwapChain) = 0;
    virtual HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) = 0;
    virtual HRESULT __stdcall Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) = 0;
    virtual HRESULT __stdcall GetBackBuffer(UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface8** ppBackBuffer) = 0;
    virtual HRESULT __stdcall GetRasterStatus(D3DGAMMARAMP* pRasterStatus) = 0;
    virtual void    __stdcall SetGammaRamp(DWORD Flags, const D3DGAMMARAMP* pRamp) = 0;
    virtual void    __stdcall GetGammaRamp(D3DGAMMARAMP* pRamp) = 0;
    virtual HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture8** ppTexture) = 0;
    virtual HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture8** ppVolumeTexture) = 0;
    virtual HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture8** ppCubeTexture) = 0;
    virtual HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer8** ppVertexBuffer) = 0;
    virtual HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer8** ppIndexBuffer) = 0;
    virtual HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable, IDirect3DSurface8** ppSurface) = 0;
    virtual HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface8** ppSurface) = 0;
    virtual HRESULT __stdcall CreateImageSurface(UINT Width, UINT Height, D3DFORMAT Format, IDirect3DSurface8** ppSurface) = 0;
    virtual HRESULT __stdcall CopyRects(IDirect3DSurface8* pSourceSurface, const RECT* pSourceRectsArray, UINT cRects, IDirect3DSurface8* pDestinationSurface, const POINT* pDestPointsArray) = 0;
    virtual HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture8* pSourceTexture, IDirect3DBaseTexture8* pDestinationTexture) = 0;
    virtual HRESULT __stdcall GetFrontBuffer(IDirect3DSurface8* pDestSurface) = 0;
    virtual HRESULT __stdcall SetRenderTarget(IDirect3DSurface8* pRenderTarget, IDirect3DSurface8* pNewZStencil) = 0;
    virtual HRESULT __stdcall GetRenderTarget(IDirect3DSurface8** ppRenderTarget) = 0;
    virtual HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface8** ppZStencilSurface) = 0;
    virtual HRESULT __stdcall BeginScene() = 0;
    virtual HRESULT __stdcall EndScene() = 0;
    virtual HRESULT __stdcall Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) = 0;
    virtual HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) = 0;
    virtual HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) = 0;
    virtual HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) = 0;
    virtual HRESULT __stdcall SetViewport(const D3DVIEWPORT8* pViewport) = 0;
    virtual HRESULT __stdcall GetViewport(D3DVIEWPORT8* pViewport) = 0;
    virtual HRESULT __stdcall SetMaterial(const D3DMATERIAL8* pMaterial) = 0;
    virtual HRESULT __stdcall GetMaterial(D3DMATERIAL8* pMaterial) = 0;
    virtual HRESULT __stdcall SetLight(DWORD Index, const D3DLIGHT8* pLight) = 0;
    virtual HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT8* pLight) = 0;
    virtual HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable) = 0;
    virtual HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable) = 0;
    virtual HRESULT __stdcall SetClipPlane(DWORD Index, const float* pPlane) = 0;
    virtual HRESULT __stdcall GetClipPlane(DWORD Index, float* pPlane) = 0;
    virtual HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) = 0;
    virtual HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) = 0;
    virtual HRESULT __stdcall BeginStateBlock() = 0;
    virtual HRESULT __stdcall EndStateBlock(DWORD* pToken) = 0;
    virtual HRESULT __stdcall ApplyStateBlock(DWORD Token) = 0;
    virtual HRESULT __stdcall CaptureStateBlock(DWORD Token) = 0;
    virtual HRESULT __stdcall DeleteStateBlock(DWORD Token) = 0;
    virtual HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, DWORD* pToken) = 0;
    virtual HRESULT __stdcall SetClipStatus(const D3DCLIPSTATUS8* pClipStatus) = 0;
    virtual HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS8* pClipStatus) = 0;
    virtual HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture8** ppTexture) = 0;
    virtual HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture8* pTexture) = 0;
    virtual HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) = 0;
    virtual HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) = 0;
    virtual HRESULT __stdcall ValidateDevice(DWORD* pNumPasses) = 0;
    virtual HRESULT __stdcall GetInfo(DWORD DevInfoID, void* pDevInfoStruct, DWORD DevInfoStructSize) = 0;
    virtual HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries) = 0;
    virtual HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) = 0;
    virtual HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber) = 0;
    virtual HRESULT __stdcall GetCurrentTexturePalette(UINT* PaletteNumber) = 0;
    virtual HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) = 0;
    virtual HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount) = 0;
    virtual HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) = 0;
    virtual HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) = 0;
    virtual HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer8* pDestBuffer, DWORD Flags) = 0;
    virtual HRESULT __stdcall CreateVertexShader(const DWORD* pDeclaration, const DWORD* pFunction, DWORD* pHandle, DWORD Usage) = 0;
    virtual HRESULT __stdcall SetVertexShader(DWORD Handle) = 0;
    virtual HRESULT __stdcall GetVertexShader(DWORD* pHandle) = 0;
    virtual HRESULT __stdcall DeleteVertexShader(DWORD Handle) = 0;
    virtual HRESULT __stdcall SetVertexShaderConstant(DWORD Register, const void* pConstantData, DWORD ConstantCount) = 0;
    virtual HRESULT __stdcall GetVertexShaderConstant(DWORD Register, void* pConstantData, DWORD ConstantCount) = 0;
    virtual HRESULT __stdcall GetVertexShaderDeclaration(DWORD Handle, void* pData, DWORD* pSizeOfData) = 0;
    virtual HRESULT __stdcall GetVertexShaderFunction(DWORD Handle, void* pData, DWORD* pSizeOfData) = 0;
    virtual HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8* pStreamData, UINT Stride) = 0;
    virtual HRESULT __stdcall GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8** ppStreamData, UINT* pStride) = 0;
    virtual HRESULT __stdcall SetIndices(IDirect3DIndexBuffer8* pIndexData, UINT BaseVertexIndex) = 0;
    virtual HRESULT __stdcall GetIndices(IDirect3DIndexBuffer8** ppIndexData, UINT* pBaseVertexIndex) = 0;
    virtual HRESULT __stdcall CreatePixelShader(const DWORD* pFunction, DWORD* pHandle) = 0;
    virtual HRESULT __stdcall SetPixelShader(DWORD Handle) = 0;
    virtual HRESULT __stdcall GetPixelShader(DWORD* pHandle) = 0;
    virtual HRESULT __stdcall DeletePixelShader(DWORD Handle) = 0;
    virtual HRESULT __stdcall SetPixelShaderConstant(DWORD Register, const void* pConstantData, DWORD ConstantCount) = 0;
    virtual HRESULT __stdcall GetPixelShaderConstant(DWORD Register, void* pConstantData, DWORD ConstantCount) = 0;
    virtual HRESULT __stdcall GetPixelShaderFunction(DWORD Handle, void* pData, DWORD* pSizeOfData) = 0;
    virtual HRESULT __stdcall DrawRectPatch(UINT Handle, const float* pNumSegs, const void* pRectPatchInfo) = 0;
    virtual HRESULT __stdcall DrawTriPatch(UINT Handle, const float* pNumSegs, const void* pTriPatchInfo) = 0;
    virtual HRESULT __stdcall DeletePatch(UINT Handle) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirect3D8                                                         */
/* ------------------------------------------------------------------ */

struct IDirect3D8 : public IUnknown {
    /*** IDirect3D8 methods ***/
    virtual HRESULT __stdcall RegisterSoftwareDevice(void* pInitializeFunction) = 0;
    virtual UINT    __stdcall GetAdapterCount() = 0;
    virtual HRESULT __stdcall GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER8* pIdentifier) = 0;
    virtual UINT    __stdcall GetAdapterModeCount(UINT Adapter) = 0;
    virtual HRESULT __stdcall EnumAdapterModes(UINT Adapter, UINT Mode, D3DDISPLAYMODE* pMode) = 0;
    virtual HRESULT __stdcall GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) = 0;
    virtual HRESULT __stdcall CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed) = 0;
    virtual HRESULT __stdcall CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) = 0;
    virtual HRESULT __stdcall CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType) = 0;
    virtual HRESULT __stdcall CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) = 0;
    virtual HRESULT __stdcall GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS8* pCaps) = 0;
    virtual HMONITOR __stdcall GetAdapterMonitor(UINT Adapter) = 0;
    virtual HRESULT __stdcall CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface) = 0;
};

/* ------------------------------------------------------------------ */
/*  Direct3DCreate8                                                    */
/* ------------------------------------------------------------------ */

/* Declare as a function pointer typedef for stub purposes */
typedef IDirect3D8* (WINAPI *LPDIRECT3DCREATE8)(UINT SDKVersion);

/* Inline stub so code that calls Direct3DCreate8 can compile */
inline IDirect3D8* WINAPI Direct3DCreate8(UINT SDKVersion) {
    (void)SDKVersion;
    return nullptr;
}

#else /* !__cplusplus - C interface stubs */

typedef struct IDirect3D8       IDirect3D8;
typedef struct IDirect3DDevice8 IDirect3DDevice8;
typedef struct IDirect3DTexture8 IDirect3DTexture8;
typedef struct IDirect3DSurface8 IDirect3DSurface8;
typedef struct IDirect3DVertexBuffer8 IDirect3DVertexBuffer8;
typedef struct IDirect3DIndexBuffer8 IDirect3DIndexBuffer8;
typedef struct IDirect3DBaseTexture8 IDirect3DBaseTexture8;
typedef struct IDirect3DCubeTexture8 IDirect3DCubeTexture8;
typedef struct IDirect3DResource8 IDirect3DResource8;

typedef IDirect3D8*              LPDIRECT3D8;
typedef IDirect3DDevice8*        LPDIRECT3DDEVICE8;
typedef IDirect3DTexture8*       LPDIRECT3DTEXTURE8;
typedef IDirect3DSurface8*       LPDIRECT3DSURFACE8;
typedef IDirect3DVertexBuffer8*  LPDIRECT3DVERTEXBUFFER8;
typedef IDirect3DIndexBuffer8*   LPDIRECT3DINDEXBUFFER8;
typedef IDirect3DBaseTexture8*   LPDIRECT3DBASETEXTURE8;
typedef IDirect3DCubeTexture8*   LPDIRECT3DCUBETEXTURE8;

#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif

#endif /* _D3D8_H_ */
