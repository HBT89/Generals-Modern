#include "always.h"
#include "matrix3d.h"
#include "vector3.h"
#include "RenderTypes.h" // replaces d3d8.h
#include <srVector3.hpp>
#include <srMatrix4x3.hpp>
#include <stdio.h>
int main() {
    printf("Stub compilation test passed!\n");
    D3DFORMAT fmt = D3DFMT_A8R8G8B8;
    srVector3 sv3;
    Vector3 v3(1.0f, 2.0f, 3.0f);
    (void)fmt; (void)sv3; (void)v3;
    return 0;
}
