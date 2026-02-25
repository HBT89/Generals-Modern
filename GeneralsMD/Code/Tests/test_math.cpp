#include "always.h"
#include "vector3.h"
#include "matrix3d.h"
#include <stdio.h>
#include <math.h>
int main() {
    Vector3 a(1.0f, 0.0f, 0.0f);
    Vector3 b(0.0f, 1.0f, 0.0f);
    float dot = Vector3::Dot_Product(a, b);
    if (fabs(dot) > 0.001f) { printf("FAIL: dot product\n"); return 1; }
    Vector3 cross;
    Vector3::Cross_Product(a, b, &cross);
    if (fabs(cross.Z - 1.0f) > 0.001f) { printf("FAIL: cross product\n"); return 1; }
    printf("Math test passed!\n");
    return 0;
}
