// test_chunk_02_bgfx_buffers.cpp
// Verifies BGFXVertexBuffer8, BGFXIndexBuffer8, BGFXSurface8, BGFXTexture8
// Lock/Unlock mechanics: CPU memory is allocated, accessible, and correctly sized.
// No bgfx runtime needed — tests CPU-side structs only.

#include "always.h"
#include "RenderTypes.h"
#include <cassert>
#include <string.h>
#include <stdio.h>

int main() {
    // --- BGFXVertexBuffer8 ---
    {
        BGFXVertexBuffer8 vb(64);
        assert(vb.m_size == 64  && "VB: wrong size");
        assert(vb.m_data != nullptr && "VB: allocation failed");
        assert(vb.m_refs == 1   && "VB: initial refcount should be 1");

        BYTE* ptr = nullptr;
        HRESULT hr = vb.Lock(0, 64, &ptr, 0);
        assert(hr == D3D_OK     && "VB: Lock must return D3D_OK");
        assert(ptr == vb.m_data && "VB: Lock must return base pointer");

        memset(ptr, 0x42, 64);
        vb.Unlock();

        assert(vb.m_data[0]  == 0x42 && "VB: write via Lock not persisted (first)");
        assert(vb.m_data[63] == 0x42 && "VB: write via Lock not persisted (last)");

        // Lock with offset
        BYTE* ptr2 = nullptr;
        vb.Lock(16, 16, &ptr2, 0);
        assert(ptr2 == vb.m_data + 16 && "VB: Lock with offset wrong");
        vb.Unlock();

        printf("  BGFXVertexBuffer8 OK\n");
    }

    // --- BGFXIndexBuffer8 ---
    {
        BGFXIndexBuffer8 ib(32);
        assert(ib.m_size == 32  && "IB: wrong size");
        assert(ib.m_data != nullptr && "IB: allocation failed");

        BYTE* ptr = nullptr;
        HRESULT hr = ib.Lock(0, 32, &ptr, 0);
        assert(hr == D3D_OK     && "IB: Lock must return D3D_OK");

        uint16_t* indices = reinterpret_cast<uint16_t*>(ptr);
        for (int i = 0; i < 16; ++i) indices[i] = (uint16_t)i;
        ib.Unlock();

        assert(*(uint16_t*)ib.m_data == 0  && "IB: index 0 wrong");
        assert(*(uint16_t*)(ib.m_data + 30) == 15 && "IB: index 15 wrong");

        printf("  BGFXIndexBuffer8 OK\n");
    }

    // --- BGFXSurface8 ---
    {
        // 4x4 ARGB8 surface: pitch = 4*4 = 16 bytes, total = 64 bytes
        BGFXSurface8 surf(4, 4, D3DFMT_A8R8G8B8);
        assert(surf.m_width  == 4  && "Surface: wrong width");
        assert(surf.m_height == 4  && "Surface: wrong height");
        assert(surf.m_pitch  == 16 && "Surface: wrong pitch (expected 16 for 4px ARGB8)");
        assert(surf.m_data != nullptr && "Surface: allocation failed");

        D3DLOCKED_RECT lr;
        HRESULT hr = surf.LockRect(&lr, nullptr, 0);
        assert(hr == D3D_OK       && "Surface: LockRect must return D3D_OK");
        assert(lr.Pitch == 16     && "Surface: LockRect pitch wrong");
        assert(lr.pBits == surf.m_data && "Surface: LockRect pBits wrong");

        // Write a red pixel at (0,0) in ARGB8 = 0xFFFF0000
        uint32_t* row0 = reinterpret_cast<uint32_t*>(lr.pBits);
        row0[0] = 0xFFFF0000u;
        surf.UnlockRect();

        assert(*reinterpret_cast<uint32_t*>(surf.m_data) == 0xFFFF0000u
               && "Surface: pixel write not persisted");

        // 2x1 R5G6B5 surface: bpp=2, pitch=4
        BGFXSurface8 surf565(2, 1, D3DFMT_R5G6B5);
        assert(surf565.m_pitch == 4 && "Surface: R5G6B5 pitch should be 4");

        printf("  BGFXSurface8 OK\n");
    }

    // --- BGFXTexture8 ---
    {
        BGFXTexture8 tex(8, 8, D3DFMT_A8R8G8B8);
        assert(tex.m_surface != nullptr && "Texture: no level-0 surface");
        assert(tex.m_refs == 1          && "Texture: initial refcount should be 1");
        assert(tex.GetLevelCount() == 1 && "Texture: wrong mip count");

        // GetSurfaceLevel(0) must AddRef and return the surface
        IDirect3DSurface8* pSurf = nullptr;
        HRESULT hr = tex.GetSurfaceLevel(0, &pSurf);
        assert(hr == D3D_OK   && "Texture: GetSurfaceLevel failed");
        assert(pSurf != nullptr && "Texture: GetSurfaceLevel returned null");
        // Release the extra ref we just got
        pSurf->Release();

        // GetSurfaceLevel(1) on a 1-mip texture: returns D3D_OK but must null out ppSurface
        IDirect3DSurface8* pSurf2 = reinterpret_cast<IDirect3DSurface8*>(0xDEADBEEFu);
        hr = tex.GetSurfaceLevel(1, &pSurf2);
        assert(hr == D3D_OK   && "Texture: GetSurfaceLevel(1) must return D3D_OK");
        assert(pSurf2 == nullptr && "Texture: GetSurfaceLevel(1) must null ppSurface for out-of-range level");

        printf("  BGFXTexture8 OK\n");
    }

    printf("BGFX buffer Lock/Unlock test passed!\n");
    return 0;
}
