// test_chunk_02_bgfx_buffers.cpp
// Verifies BGFXVertexBuffer8, BGFXIndexBuffer8, BGFXSurface8, BGFXTexture8
// Lock/Unlock mechanics: CPU memory is allocated, accessible, and correctly sized.
// No bgfx runtime needed — tests CPU-side structs only (bgfx is linked for
// ~BGFXTexture8, which never reaches bgfx::destroy here).
//
// A failed CHECK does not stop the test, so each pointer is checked before it is
// written through: a broken Lock gives a FAIL line rather than an access violation.

#include "always.h"
#include "RenderTypes.h"

#include "test_harness.h"

#include <string.h>

TEST(vertex_buffer_lock) {
    BGFXVertexBuffer8 vb(64);
    CHECK_EQ(vb.m_size, 64u);
    CHECK(vb.m_data != nullptr);
    CHECK_EQ(vb.m_refs, 1);

    BYTE* ptr = nullptr;
    HRESULT hr = vb.Lock(0, 64, &ptr, 0);
    CHECK_EQ(hr, D3D_OK);
    CHECK(ptr == vb.m_data);
    if (!ptr || ptr != vb.m_data) return;

    memset(ptr, 0x42, 64);
    vb.Unlock();

    CHECK_EQ(vb.m_data[0], 0x42);
    CHECK_EQ(vb.m_data[63], 0x42);

    // Lock with offset
    BYTE* ptr2 = nullptr;
    vb.Lock(16, 16, &ptr2, 0);
    CHECK(ptr2 == vb.m_data + 16);
    vb.Unlock();
}

TEST(index_buffer_lock) {
    BGFXIndexBuffer8 ib(32);
    CHECK_EQ(ib.m_size, 32u);
    CHECK(ib.m_data != nullptr);

    BYTE* ptr = nullptr;
    HRESULT hr = ib.Lock(0, 32, &ptr, 0);
    CHECK_EQ(hr, D3D_OK);
    CHECK(ptr == ib.m_data);
    if (!ptr || ptr != ib.m_data) return;

    uint16_t* indices = reinterpret_cast<uint16_t*>(ptr);
    for (int i = 0; i < 16; ++i) indices[i] = (uint16_t)i;
    ib.Unlock();

    CHECK_EQ(*(uint16_t*)ib.m_data, 0);
    CHECK_EQ(*(uint16_t*)(ib.m_data + 30), 15);
}

TEST(surface_lock_rect) {
    // 4x4 ARGB8 surface: pitch = 4*4 = 16 bytes, total = 64 bytes
    BGFXSurface8 surf(4, 4, D3DFMT_A8R8G8B8);
    CHECK_EQ(surf.m_width, 4u);
    CHECK_EQ(surf.m_height, 4u);
    CHECK_EQ(surf.m_pitch, 16);
    CHECK(surf.m_data != nullptr);

    D3DLOCKED_RECT lr;
    memset(&lr, 0xAB, sizeof(lr));  // LockRect has to write both fields
    HRESULT hr = surf.LockRect(&lr, nullptr, 0);
    CHECK_EQ(hr, D3D_OK);
    CHECK_EQ(lr.Pitch, 16);
    CHECK(lr.pBits == surf.m_data);
    if (!lr.pBits || lr.pBits != surf.m_data) return;

    // Write a red pixel at (0,0) in ARGB8 = 0xFFFF0000
    uint32_t* row0 = reinterpret_cast<uint32_t*>(lr.pBits);
    row0[0] = 0xFFFF0000u;
    surf.UnlockRect();

    CHECK_EQ(*reinterpret_cast<uint32_t*>(surf.m_data), 0xFFFF0000u);
}

TEST(surface_pitch_r5g6b5) {
    // 2x1 R5G6B5 surface: bpp=2, pitch=4
    BGFXSurface8 surf565(2, 1, D3DFMT_R5G6B5);
    CHECK_EQ(surf565.m_pitch, 4);
}

TEST(texture_surface_level0) {
    BGFXTexture8 tex(8, 8, D3DFMT_A8R8G8B8);
    CHECK(tex.m_surface != nullptr);
    CHECK_EQ(tex.m_refs, 1);
    // Not == 1: GetLevelCount() is hard-wired to 1 and CreateTexture ignores Levels
    // (RenderTypes.h:1263, :1307). That is the level-0-only mip defect, not a contract.
    CHECK(tex.GetLevelCount() >= 1);

    // GetSurfaceLevel(0) must AddRef and return the surface
    IDirect3DSurface8* pSurf = nullptr;
    HRESULT hr = tex.GetSurfaceLevel(0, &pSurf);
    CHECK_EQ(hr, D3D_OK);
    CHECK(pSurf != nullptr);
    CHECK(pSurf == tex.m_surface);
    if (!pSurf || pSurf != tex.m_surface) return;
    CHECK_EQ(tex.m_surface->m_refs, 2);
    // Release the extra ref we just got
    pSurf->Release();
    CHECK_EQ(tex.m_surface->m_refs, 1);
}

// Level > 0 hands back a throwaway surface of the mip's size and only level 0 is ever
// uploaded (RenderTypes.h:1268-1278): the level-0-only mip defect, so its result is not
// pinned. This used to assert nullptr, which would bring back the crash the dummy exists
// to prevent: textureloader.cpp:276-283 locks, fills and Releases whatever comes back,
// without a null check. Pinned instead: no crash, a usable surface, and no leak (the old
// test never released it).
TEST(texture_surface_level1_DEFECT_no_crash_no_leak) {
    BGFXTexture8 tex(8, 8, D3DFMT_A8R8G8B8);
    IDirect3DSurface8* const poison = reinterpret_cast<IDirect3DSurface8*>(0xDEADBEEFu);
    IDirect3DSurface8* pSurf2 = poison;
    HRESULT hr = tex.GetSurfaceLevel(1, &pSurf2);
    CHECK_EQ(hr, D3D_OK);
    CHECK(pSurf2 != poison);    // the OUT param was written
    CHECK(pSurf2 != nullptr);   // callers dereference it
    if (!pSurf2 || pSurf2 == poison) return;

    D3DLOCKED_RECT lr;
    memset(&lr, 0, sizeof(lr));
    CHECK_EQ(pSurf2->LockRect(&lr, nullptr, 0), D3D_OK);
    CHECK(lr.pBits != nullptr);
    CHECK_EQ(pSurf2->UnlockRect(), D3D_OK);
    // Release's count is not checked: a real mip owned by the texture would keep a reference.
    pSurf2->Release();
}
