#include "always.h"
#include "matrix3d.h"
#include "vector3.h"
#include "RenderTypes.h" // replaces d3d8.h
#include <srVector3.hpp>
#include <srMatrix4x3.hpp>

#include "test_harness.h"

// Mostly a compile test: these headers have to build together. Nothing here may
// instantiate a type with a vtable (IDirect3DDevice8, BGFXTexture8...), which would
// need bgfx at link time; this binary links only the harness.
TEST(stub_headers_compile_together) {
    D3DFORMAT fmt = D3DFMT_A8R8G8B8;
    CHECK_EQ(D3DFormatBytesPerPixel(fmt), 4u);

    srVector3 sv3;
    CHECK_EQ(sv3.x, 0.0f);
    CHECK_EQ(sv3.y, 0.0f);
    CHECK_EQ(sv3.z, 0.0f);

    Vector3 v3(1.0f, 2.0f, 3.0f);
    CHECK_EQ(v3.X, 1.0f);
    CHECK_EQ(v3.Y, 2.0f);
    CHECK_EQ(v3.Z, 3.0f);
}
