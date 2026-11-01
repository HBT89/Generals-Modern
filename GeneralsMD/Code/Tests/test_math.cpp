#include "always.h"
#include "vector3.h"
#include "matrix3d.h"

#include "test_harness.h"

// Every product here involves 0 or 1, so the results are exact and compared exactly.
// No aliasing case: the 3-argument Cross_Product asserts result != &a (vector3.h:376).

TEST(vector3_dot_product) {
    Vector3 a(1.0f, 0.0f, 0.0f);
    Vector3 b(0.0f, 1.0f, 0.0f);
    CHECK_EQ(Vector3::Dot_Product(a, b), 0.0f);
}

TEST(vector3_cross_product) {
    Vector3 a(1.0f, 0.0f, 0.0f);
    Vector3 b(0.0f, 1.0f, 0.0f);
    Vector3 cross;
    Vector3::Cross_Product(a, b, &cross);
    CHECK_EQ(cross.X, 0.0f);
    CHECK_EQ(cross.Y, 0.0f);
    CHECK_EQ(cross.Z, 1.0f);
}
