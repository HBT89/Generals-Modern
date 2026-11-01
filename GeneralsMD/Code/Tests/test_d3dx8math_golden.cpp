// test_d3dx8math_golden.cpp
// Pins the D3DX math the simulation runs, bit for bit: D3DXVec4Transform and D3DXVec4Dot from
// our DX90SDK stub (Libraries/Source/DX90SDK/Include/d3dx8math.h), called the way
// BezierSegment.cpp and BezFwdIterator.cpp call them for DumbProjectileBehavior's flight path.
// No other GameLogic code reaches D3DX. Header-only: no engine, no game data.
//
// The goldens were computed from the stub header itself with g++ x86-64 at -O0 and -O2, using
// -ffp-contract=off -msse2. That rounds each + and * to IEEE single, as MSVC x86 /arch:SSE2
// /fp:precise does. A numpy float32 step-by-step model gave the same bits. Rows marked "exact" do
// not depend on how intermediates are evaluated. The others would change if MSVC evaluated float
// expressions in double; the first test below checks for that. They pin our plain C++ stub, not
// retail d3dx8.lib, so they say nothing about compatibility with retail replays.
//
// Every test runs in the game's FPU mode (test_fp_mode.h), as the simulation does. For SSE2
// code that changes nothing; if any of this were compiled to x87, its 24-bit precision would
// round each step to single just as SSE does. The goldens were also checked that way: g++
// -m80387 -mfpmath=387 in the same mode gives the same bits.

#include <d3dx8math.h>

#include "test_harness.h"
#include "test_fp_mode.h"

#include <string.h>

namespace {
    // BezierSegment::s_bezBasisMatrix (BezierSegment.cpp:241), copied because it is protected.
    // It is symmetric, so it cannot tell a row-vector transform from a column-vector one; kAsym
    // below can.
    const D3DXMATRIX kBezBasis(
        -1.0f,  3.0f, -3.0f,  1.0f,
         3.0f, -6.0f,  3.0f,  0.0f,
        -3.0f,  3.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,  0.0f);

    const D3DXMATRIX kAsym(
         1.0f,  2.0f,  3.0f,  4.0f,
         5.0f,  6.0f,  7.0f,  8.0f,
         9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f);

    // A projectile-like curve: P0..P3 = (10.5,-20.25,5) (40.123,15.7,80) (70.9,50.3,80)
    // (101,85.5,3.25), as float bits.
    const unsigned kControl[4][3] = {
        { 0x41280000, 0xC1A20000, 0x40A00000 },
        { 0x42207DF4, 0x417B3333, 0x42A00000 },
        { 0x428DCCCD, 0x42493333, 0x42A00000 },
        { 0x42CA0000, 0x42AB0000, 0x40500000 },
    };

    // Read through a volatile, so nothing below is folded at compile time.
    inline float runtime_float(unsigned bits) {
        volatile unsigned v = bits;
        const unsigned u = v;
        float f;
        memcpy(&f, &u, sizeof(f));
        return f;
    }

    inline unsigned bits_of(float f) {
        unsigned u;
        memcpy(&u, &f, sizeof(u));
        return u;
    }

    // CHECK_EQ only prints the expression; this also prints the row and both values, so a CI
    // log is enough to see what the code produced.
    void check_bits(unsigned row, const char *what, float got, unsigned want, int line) {
        const unsigned g = bits_of(got);
        test_check(g == want, what, __FILE__, line);
        if (g != want)
            printf("       row 0x%08X: got 0x%08X, want 0x%08X\n", row, g, want);
    }

    void check_vec4(unsigned row, const D3DXVECTOR4 &v, const unsigned want[4], int line) {
        check_bits(row, "x", v.x, want[0], line);
        check_bits(row, "y", v.y, want[1], line);
        check_bits(row, "z", v.z, want[2], line);
        check_bits(row, "w", v.w, want[3], line);
    }

    D3DXVECTOR4 control_coords(int axis) {
        return D3DXVECTOR4(runtime_float(kControl[0][axis]), runtime_float(kControl[1][axis]),
                           runtime_float(kControl[2][axis]), runtime_float(kControl[3][axis]));
    }

    // BezierSegment::evaluateBezSegmentAtT (BezierSegment.cpp:105-116), statement for statement.
    void evaluate_at_t(float tValue, float out[3]) {
        D3DXVECTOR4 tVec(tValue * tValue * tValue, tValue * tValue, tValue, 1);

        D3DXVECTOR4 xCoords = control_coords(0);
        D3DXVECTOR4 yCoords = control_coords(1);
        D3DXVECTOR4 zCoords = control_coords(2);

        D3DXVECTOR4 tResult;
        D3DXVec4Transform(&tResult, &tVec, &kBezBasis);

        out[0] = D3DXVec4Dot(&xCoords, &tResult);
        out[1] = D3DXVec4Dot(&yCoords, &tResult);
        out[2] = D3DXVec4Dot(&zCoords, &tResult);
    }

    struct Vec4Row { unsigned in; unsigned want[4]; };
    struct Vec3Row { unsigned in; unsigned want[3]; };

    // Bezier weights: D3DXVec4Transform(t^3, t^2, t, 1) by the basis, keyed by t.
    const Vec4Row kWeights[] = {
        { 0x00000000, { 0x3F800000, 0x00000000, 0x00000000, 0x00000000 } },  // 0, exact
        { 0x3E800000, { 0x3ED80000, 0x3ED80000, 0x3E100000, 0x3C800000 } },  // 0.25, exact
        { 0x3F000000, { 0x3E000000, 0x3EC00000, 0x3EC00000, 0x3E000000 } },  // 0.5, exact
        { 0x3F800000, { 0x00000000, 0x00000000, 0x00000000, 0x3F800000 } },  // 1, exact
        { 0x3DCCCCCD, { 0x3F3A9FBE, 0x3E78D4FE, 0x3CDD2F1B, 0x3A83126F } },  // 0.1f
        { 0x3EAAAAAB, { 0x3E97B428, 0x3EE38E36, 0x3E638E3B, 0x3D17B427 } },  // 1/3
        { 0x3F333333, { 0x3CDD2F00, 0x3E418940, 0x3EE1CABC, 0x3EAF9DB2 } },  // 0.7f
        { 0x3F666666, { 0x3A831800, 0x3CDD2F00, 0x3E78D500, 0x3F3A9FBD } },  // 0.9f
    };

    // evaluateBezSegmentAtT(t) on the curve above, keyed by t: x, y, z.
    const Vec3Row kEvaluate[] = {
        { 0x00000000, { 0x41280000, 0xC1A20000, 0x40A00000 } },  // 0: P0, exact
        { 0x3E800000, { 0x42039EBD, 0x40CFACCC, 0x423CA400 } },  // 0.25
        { 0x3F000000, { 0x425E48D6, 0x4203A000, 0x42742000 } },  // 0.5: the split getApproximateLength makes
        { 0x3F800000, { 0x42CA0000, 0x42AB0000, 0x40500000 } },  // 1: P3, exact
        { 0x3DCCCCCD, { 0x419B5B86, 0xC1180E8A, 0x41C9FC6B } },  // 0.1f
        { 0x3EAAAAAB, { 0x4221C269, 0x417527D1, 0x425BBDA0 } },  // 1/3
        { 0x3F333333, { 0x42938DA4, 0x4257B7A6, 0x424E9959 } },  // 0.7f
        { 0x3F666666, { 0x42B7E72D, 0x4295E97E, 0x41BFCB43 } },  // 0.9f
    };
}

// A canary for everything below. 1 + 2^-24 is a tie between 1.0 and the next float up, and
// single precision rounds it to 1.0, so the difference is 0. Evaluated in double it would be
// 2^-24 (0x33800000), and the non-exact goldens would move.
TEST(float_expressions_evaluate_in_single_precision) {
    GameFPMode mode;
    const float one = runtime_float(0x3F800000);
    const float e = runtime_float(0x33800000);
    const float d = (one + e) - one;
    CHECK_EQ(bits_of(d), 0x00000000u);
}

TEST(vec4transform_bezier_weights) {
    GameFPMode mode;
    const int count = (int)(sizeof(kWeights) / sizeof(kWeights[0]));
    for (int row = 0; row < count; ++row) {
        const float t = runtime_float(kWeights[row].in);
        D3DXVECTOR4 tVec(t * t * t, t * t, t, 1);
        D3DXVECTOR4 w;
        D3DXVECTOR4 *ret = D3DXVec4Transform(&w, &tVec, &kBezBasis);
        CHECK(ret == &w);
        check_vec4(kWeights[row].in, w, kWeights[row].want, __LINE__);
    }
}

// BezFwdIterator::start (BezFwdIterator.cpp:64-71): each axis of the control points by the basis.
TEST(vec4transform_bezier_control_coords) {
    GameFPMode mode;
    static const unsigned want[3][4] = {
        { 0xBFEA5E80, 0x405D9180, 0x42B1BCEE, 0x41280000 },  // px
        { 0x3FF999C0, 0xC08199A0, 0x42D7B333, 0xC1A20000 },  // py
        { 0xBFE00000, 0xC3610000, 0x43610000, 0x40A00000 },  // pz, exact
    };
    for (int axis = 0; axis < 3; ++axis) {
        const D3DXVECTOR4 p = control_coords(axis);
        D3DXVECTOR4 cVec;
        D3DXVec4Transform(&cVec, &p, &kBezBasis);
        check_vec4((unsigned)axis, cVec, want[axis], __LINE__);
    }
}

TEST(vec4dot_bezier_evaluate_at_t) {
    GameFPMode mode;
    const int count = (int)(sizeof(kEvaluate) / sizeof(kEvaluate[0]));
    for (int row = 0; row < count; ++row) {
        float out[3];
        evaluate_at_t(runtime_float(kEvaluate[row].in), out);
        check_bits(kEvaluate[row].in, "x", out[0], kEvaluate[row].want[0], __LINE__);
        check_bits(kEvaluate[row].in, "y", out[1], kEvaluate[row].want[1], __LINE__);
        check_bits(kEvaluate[row].in, "z", out[2], kEvaluate[row].want[2], __LINE__);
    }
}

// out = v * M (row vector): with kAsym the transpose would give (30,70,110,150) for (1,2,3,4).
TEST(vec4transform_is_row_vector_times_matrix) {
    GameFPMode mode;
    static const unsigned exact[4] = { 0x42B40000, 0x42C80000, 0x42DC0000, 0x42F00000 };  // 90 100 110 120
    static const unsigned frac[4] = { 0x416CCCCD, 0x41800000, 0x4189999A, 0x41933333 };
    D3DXVECTOR4 out;

    const D3DXVECTOR4 v(runtime_float(0x3F800000), runtime_float(0x40000000),
                        runtime_float(0x40400000), runtime_float(0x40800000));  // 1 2 3 4
    D3DXVec4Transform(&out, &v, &kAsym);
    check_vec4(0, out, exact, __LINE__);

    const D3DXVECTOR4 f(runtime_float(0x3DCCCCCD), runtime_float(0xBE4CCCCD),
                        runtime_float(0x3E99999A), runtime_float(0x3F800000));  // 0.1 -0.2 0.3 1
    D3DXVec4Transform(&out, &f, &kAsym);
    check_vec4(1, out, frac, __LINE__);
}

// D3DXVec4Dot only reads its arguments, so the same vector on both sides is fine.
TEST(vec4dot_aliased_arguments) {
    GameFPMode mode;
    const D3DXVECTOR4 q(runtime_float(0x3DCCCCCD), runtime_float(0xBE4CCCCD),
                        runtime_float(0x3E99999A), runtime_float(0x3FC00000));  // 0.1 -0.2 0.3 1.5
    const D3DXVECTOR4 copy = q;
    check_bits(0, "Vec4Dot(&q, &q)", D3DXVec4Dot(&q, &q), 0x4018F5C3, __LINE__);
    check_bits(1, "Vec4Dot(&q, &copy)", D3DXVec4Dot(&q, &copy), 0x4018F5C3, __LINE__);
}

// The stub's D3DXVec4Transform (d3dx8math.h:416-421) writes pOut->x and then reads pV->x again
// for y, z and w, so an aliased call (pOut == pV) gets y, z and w wrong; real D3DX works into a
// temporary. The simulation never aliases (BezierSegment.cpp:112, BezFwdIterator.cpp:69-71), so
// lockstep is unaffected, and the fix belongs in the stub, not here. Only what is right either
// way is pinned: the call completes, returns pOut, and x is the non-aliased x.
TEST(vec4transform_aliased_DEFECT_only_x_pinned) {
    GameFPMode mode;
    D3DXVECTOR4 v(runtime_float(0x3F800000), runtime_float(0x40000000),
                  runtime_float(0x40400000), runtime_float(0x40800000));  // 1 2 3 4
    D3DXVECTOR4 *ret = D3DXVec4Transform(&v, &v, &kAsym);
    CHECK(ret == &v);
    check_bits(0, "aliased x", v.x, 0x42B40000, __LINE__);  // 90
}
