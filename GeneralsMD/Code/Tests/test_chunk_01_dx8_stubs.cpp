// test_chunk_01_dx8_stubs.cpp
// Verifies all IDirect3DDevice8 GET methods with OUT params properly initialize
// those params to safe values (null/zero) instead of leaving them uninitialized.
// No link-time dependencies beyond the headers.

#include "always.h"
#include "RenderTypes.h"
#include <cassert>
#include <string.h>
#include <stdio.h>

#define POISON8(p)  memset(p, 0xAB, sizeof(*p))
#define POISONP(p)  (*(void**)(p) = (void*)0xDEADBEEFu)
#define POISOND(p)  (*(DWORD*)(p) = 0xDEADBEEFu)
#define POISONU(p)  (*(UINT*)(p)  = 0xDEADBEEFu)

int main() {
    IDirect3DDevice8 dev;

    // --- GetStreamSource ---
    {
        IDirect3DVertexBuffer8* vb = reinterpret_cast<IDirect3DVertexBuffer8*>(0xDEADBEEFu);
        UINT stride = 0xDEADBEEFu;
        dev.GetStreamSource(0, &vb, &stride);
        assert(vb == nullptr     && "GetStreamSource must null ppVertexBuffer");
        assert(stride == 0       && "GetStreamSource must zero pStride");
    }

    // --- GetIndices ---
    {
        IDirect3DIndexBuffer8* ib = reinterpret_cast<IDirect3DIndexBuffer8*>(0xDEADBEEFu);
        UINT baseIdx = 0xDEADBEEFu;
        dev.GetIndices(&ib, &baseIdx);
        assert(ib == nullptr     && "GetIndices must null ppIndexData");
        assert(baseIdx == 0      && "GetIndices must zero pBaseVertexIndex");
    }

    // --- GetTexture ---
    {
        IDirect3DBaseTexture8* tex = reinterpret_cast<IDirect3DBaseTexture8*>(0xDEADBEEFu);
        dev.GetTexture(0, &tex);
        assert(tex == nullptr    && "GetTexture must null ppTexture");
    }

    // --- GetTextureStageState ---
    {
        DWORD val = 0xDEADBEEFu;
        dev.GetTextureStageState(0, D3DTSS_COLOROP, &val);
        assert(val == 0          && "GetTextureStageState must zero pValue");
    }

    // --- GetViewport ---
    {
        D3DVIEWPORT8 vp;
        POISON8(&vp);
        dev.GetViewport(&vp);
        D3DVIEWPORT8 z; memset(&z, 0, sizeof(z));
        assert(memcmp(&vp, &z, sizeof(z)) == 0 && "GetViewport must zero struct");
    }

    // --- GetBackBuffer ---
    {
        IDirect3DSurface8* surf = reinterpret_cast<IDirect3DSurface8*>(0xDEADBEEFu);
        dev.GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surf);
        assert(surf == nullptr   && "GetBackBuffer must null ppBackBuffer");
    }

    // --- ValidateDevice ---
    {
        DWORD passes = 0xDEADBEEFu;
        dev.ValidateDevice(&passes);
        assert(passes == 0       && "ValidateDevice must zero pNumPasses");
    }

    // --- GetMaterial ---
    {
        D3DMATERIAL8 mat;
        POISON8(&mat);
        dev.GetMaterial(&mat);
        D3DMATERIAL8 z; memset(&z, 0, sizeof(z));
        assert(memcmp(&mat, &z, sizeof(z)) == 0 && "GetMaterial must zero struct");
    }

    // --- GetTransform ---
    {
        D3DMATRIX mtx;
        POISON8(&mtx);
        dev.GetTransform(D3DTS_WORLD, &mtx);
        D3DMATRIX z; memset(&z, 0, sizeof(z));
        assert(memcmp(&mtx, &z, sizeof(z)) == 0 && "GetTransform must zero matrix");
    }

    // --- GetVertexShader ---
    {
        DWORD h = 0xDEADBEEFu;
        dev.GetVertexShader(&h);
        assert(h == 0            && "GetVertexShader must zero handle");
    }

    // --- GetPixelShader ---
    {
        DWORD h = 0xDEADBEEFu;
        dev.GetPixelShader(&h);
        assert(h == 0            && "GetPixelShader must zero handle");
    }

    // --- EndStateBlock ---
    {
        DWORD h = 0xDEADBEEFu;
        dev.EndStateBlock(&h);
        assert(h == 0            && "EndStateBlock must zero handle");
    }

    // --- GetDisplayMode ---
    {
        D3DDISPLAYMODE dm;
        POISON8(&dm);
        dev.GetDisplayMode(&dm);
        D3DDISPLAYMODE z; memset(&z, 0, sizeof(z));
        assert(memcmp(&dm, &z, sizeof(z)) == 0 && "GetDisplayMode must zero struct");
    }

    // --- GetCreationParameters ---
    {
        D3DDEVICE_CREATION_PARAMETERS cp;
        POISON8(&cp);
        dev.GetCreationParameters(&cp);
        D3DDEVICE_CREATION_PARAMETERS z; memset(&z, 0, sizeof(z));
        assert(memcmp(&cp, &z, sizeof(z)) == 0 && "GetCreationParameters must zero struct");
    }

    // --- GetDeviceCaps ---
    {
        D3DCAPS8 caps;
        POISON8(&caps);
        dev.GetDeviceCaps(&caps);
        D3DCAPS8 z; memset(&z, 0, sizeof(z));
        assert(memcmp(&caps, &z, sizeof(z)) == 0 && "GetDeviceCaps must zero struct");
    }

    printf("DX8 stub OUT-param test passed (15 methods)!\n");
    return 0;
}
