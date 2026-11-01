// test_chunk_01_dx8_stubs.cpp
// Verifies all IDirect3DDevice8 GET methods with OUT params properly initialize
// those params to safe values (null/zero) instead of leaving them uninitialized.
// Header-only code, but it needs bgfx at link time: instantiating the device emits
// its vtable, whose CreateTexture news a BGFXTexture8, whose destructor references
// bgfx::destroy. Nothing here calls bgfx::init.
//
// The all-zero struct checks pin what the stub reports today. A device that tracks
// real state (the last viewport or transform set, real caps) would change them.

#include "always.h"
#include "RenderTypes.h"

#include "test_harness.h"

#include <string.h>

#define POISON8(p)  memset(p, 0xAB, sizeof(*p))

TEST(get_stream_source_nulls_outs) {
    IDirect3DDevice8 dev;
    IDirect3DVertexBuffer8* vb = reinterpret_cast<IDirect3DVertexBuffer8*>(0xDEADBEEFu);
    UINT stride = 0xDEADBEEFu;
    dev.GetStreamSource(0, &vb, &stride);
    CHECK(vb == nullptr);
    CHECK_EQ(stride, 0u);
}

TEST(get_indices_nulls_outs) {
    IDirect3DDevice8 dev;
    IDirect3DIndexBuffer8* ib = reinterpret_cast<IDirect3DIndexBuffer8*>(0xDEADBEEFu);
    UINT baseIdx = 0xDEADBEEFu;
    dev.GetIndices(&ib, &baseIdx);
    CHECK(ib == nullptr);
    CHECK_EQ(baseIdx, 0u);
}

TEST(get_texture_nulls_out) {
    IDirect3DDevice8 dev;
    IDirect3DBaseTexture8* tex = reinterpret_cast<IDirect3DBaseTexture8*>(0xDEADBEEFu);
    dev.GetTexture(0, &tex);
    CHECK(tex == nullptr);
}

TEST(get_texture_stage_state_zeroes_out) {
    IDirect3DDevice8 dev;
    DWORD val = 0xDEADBEEFu;
    dev.GetTextureStageState(0, D3DTSS_COLOROP, &val);
    CHECK_EQ(val, 0u);
}

TEST(get_viewport_zeroes_struct) {
    IDirect3DDevice8 dev;
    D3DVIEWPORT8 vp;
    POISON8(&vp);
    dev.GetViewport(&vp);
    D3DVIEWPORT8 z; memset(&z, 0, sizeof(z));
    CHECK_MEM(&vp, &z, sizeof(z));
}

TEST(get_back_buffer_nulls_out) {
    IDirect3DDevice8 dev;
    IDirect3DSurface8* surf = reinterpret_cast<IDirect3DSurface8*>(0xDEADBEEFu);
    dev.GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surf);
    CHECK(surf == nullptr);
}

TEST(validate_device_zeroes_passes) {
    IDirect3DDevice8 dev;
    DWORD passes = 0xDEADBEEFu;
    dev.ValidateDevice(&passes);
    CHECK_EQ(passes, 0u);
}

TEST(get_material_zeroes_struct) {
    IDirect3DDevice8 dev;
    D3DMATERIAL8 mat;
    POISON8(&mat);
    dev.GetMaterial(&mat);
    D3DMATERIAL8 z; memset(&z, 0, sizeof(z));
    CHECK_MEM(&mat, &z, sizeof(z));
}

TEST(get_transform_zeroes_matrix) {
    IDirect3DDevice8 dev;
    D3DMATRIX mtx;
    POISON8(&mtx);
    dev.GetTransform(D3DTS_WORLD, &mtx);
    D3DMATRIX z; memset(&z, 0, sizeof(z));
    CHECK_MEM(&mtx, &z, sizeof(z));
}

TEST(get_vertex_shader_zeroes_handle) {
    IDirect3DDevice8 dev;
    DWORD h = 0xDEADBEEFu;
    dev.GetVertexShader(&h);
    CHECK_EQ(h, 0u);
}

TEST(get_pixel_shader_zeroes_handle) {
    IDirect3DDevice8 dev;
    DWORD h = 0xDEADBEEFu;
    dev.GetPixelShader(&h);
    CHECK_EQ(h, 0u);
}

TEST(end_state_block_zeroes_handle) {
    IDirect3DDevice8 dev;
    DWORD h = 0xDEADBEEFu;
    dev.EndStateBlock(&h);
    CHECK_EQ(h, 0u);
}

TEST(get_display_mode_zeroes_struct) {
    IDirect3DDevice8 dev;
    D3DDISPLAYMODE dm;
    POISON8(&dm);
    dev.GetDisplayMode(&dm);
    D3DDISPLAYMODE z; memset(&z, 0, sizeof(z));
    CHECK_MEM(&dm, &z, sizeof(z));
}

TEST(get_creation_parameters_zeroes_struct) {
    IDirect3DDevice8 dev;
    D3DDEVICE_CREATION_PARAMETERS cp;
    POISON8(&cp);
    dev.GetCreationParameters(&cp);
    D3DDEVICE_CREATION_PARAMETERS z; memset(&z, 0, sizeof(z));
    CHECK_MEM(&cp, &z, sizeof(z));
}

TEST(get_device_caps_zeroes_struct) {
    IDirect3DDevice8 dev;
    D3DCAPS8 caps;
    POISON8(&caps);
    dev.GetDeviceCaps(&caps);
    D3DCAPS8 z; memset(&z, 0, sizeof(z));
    CHECK_MEM(&caps, &z, sizeof(z));
}
