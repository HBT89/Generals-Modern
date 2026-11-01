// test_basetype_float.cpp
// Pins the BaseType.h float helpers the simulation converts with (REAL_TO_INT,
// REAL_TO_INT_FLOOR/CEIL, fast_float_trunc/floor/ceil, fast_float2long_round), bit for bit,
// in the FPU mode the game runs in. Replays and multiplayer CRCs depend on every one of these
// results, EA's quirks included, so a change here is a simulation change.
// Header-only: no engine, no game data. MSVC x86 inline asm, so Win32 only.
//
// How the expected values were derived. The helpers are:
//   trunc:      mov ecx,[f]; shr ecx,23; sub cl,127 (CF = exponent < 127); eax = CF ? 0 :
//               0xff800000; sar eax,cl (the count is cl & 31); and [f],eax.
//   floor/ceil: an SSE subss/addss of 0x3F7FFFFF (0.99999994f), rounded per MXCSR.RC, then trunc.
//   round:      fld/fistp, rounded per the x87 RC; out of range, Inf and NaN store 0x80000000.
// Those exact instruction sequences were transcribed to GNU inline asm and run on an x86-64 CPU
// (32-bit operands; g++ -masm=intel -O0 and -O2 -ffp-contract=off -msse2 -frounding-math), with
// the x87 control word and MXCSR set as test_fp_mode.h sets them. That produced every row here,
// and agreed with a separate C model of the same instructions. MSVC was not available, so CI is
// the first run of the real __asm.

#include "test_harness.h"
#include "test_fp_mode.h"

#include <limits.h>
#include <string.h>

#if !defined(_M_IX86)
#error test_basetype_float pins MSVC x86 inline asm; it only builds for Win32.
#endif

// Keep the floor/ceil add where the source puts it, after the _controlfp calls below.
// Before BaseType.h, so its inline functions are covered too.
#pragma fenv_access (on)

// After the harness and the CRT headers: it #defines min, max, NULL, TRUE and FALSE.
#include "Lib/BaseType.h"

namespace {
    // What fistp stores for anything it cannot represent: the x87 "integer indefinite".
    const Int kIndef = INT_MIN;

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

    // CHECK_EQ only prints the expression; these also print the rounding mode, the input and
    // both values, so a CI log is enough to see what the code produced.
    void check_bits(const char *mode, unsigned in, const char *what, unsigned got, unsigned want, int line) {
        test_check(got == want, what, __FILE__, line);
        if (got != want)
            printf("       %s, in 0x%08X: got 0x%08X, want 0x%08X\n", mode, in, got, want);
    }

    void check_int(const char *mode, unsigned in, const char *what, Int got, Int want, int line) {
        test_check(got == want, what, __FILE__, line);
        if (got != want)
            printf("       %s, in 0x%08X: got %d, want %d\n", mode, in, got, want);
    }

    struct FloatRow {
        unsigned in;                       // input bits
        unsigned bTrunc, bFloor, bCeil;    // fast_float_trunc / _floor / _ceil result bits
        Int toInt, toIntFloor, toIntCeil;  // REAL_TO_INT / REAL_TO_INT_FLOOR / REAL_TO_INT_CEIL
        Int toLong;                        // fast_float2long_round(in)
    };

    // Round-to-nearest, as setFPMode leaves it. "nudge" rows are EA's floor/ceil quirk: the
    // 0.99999994 nudge does not survive the add, so a whole number moves by one.
    const FloatRow kRows[] = {
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000,  0,  0,  0,  0 },
        { 0x80000000, 0x00000000, 0x00000000, 0x00000000,  0,  0,  0,  0 },  // -0: all +0
        { 0x3E800000, 0x00000000, 0x00000000, 0x3F800000,  0,  0,  1,  0 },  // 0.25
        { 0xBE800000, 0x00000000, 0xBF800000, 0x00000000,  0, -1,  0,  0 },  // -0.25: trunc gives +0
        { 0x3F000000, 0x00000000, 0x00000000, 0x3F800000,  0,  0,  1,  0 },  // 0.5: round ties to even
        { 0xBF000000, 0x00000000, 0xBF800000, 0x00000000,  0, -1,  0,  0 },  // -0.5
        { 0x3F400000, 0x00000000, 0x00000000, 0x3F800000,  0,  0,  1,  1 },  // 0.75
        { 0xBF400000, 0x00000000, 0xBF800000, 0x00000000,  0, -1,  0, -1 },  // -0.75
        { 0x3F7FFFFF, 0x00000000, 0x00000000, 0x3F800000,  0,  0,  1,  1 },  // 0.99999994
        { 0xBF7FFFFF, 0x00000000, 0xBF800000, 0x00000000,  0, -1,  0, -1 },
        { 0x3F800000, 0x3F800000, 0x3F800000, 0x40000000,  1,  1,  2,  1 },  // 1: nudge, ceil 2 (tie to even)
        { 0xBF800000, 0xBF800000, 0xC0000000, 0xBF800000, -1, -2, -1, -1 },  // -1: nudge, floor -2
        { 0x3FC00000, 0x3F800000, 0x3F800000, 0x40000000,  1,  1,  2,  2 },  // 1.5
        { 0xBFC00000, 0xBF800000, 0xC0000000, 0xBF800000, -1, -2, -1, -2 },  // -1.5
        { 0x40000000, 0x40000000, 0x40000000, 0x40400000,  2,  2,  3,  2 },  // 2: nudge
        { 0xC0000000, 0xC0000000, 0xC0400000, 0xC0000000, -2, -3, -2, -2 },  // -2: nudge
        { 0x40200000, 0x40000000, 0x40000000, 0x40400000,  2,  2,  3,  2 },  // 2.5: round ties to even
        { 0xC0200000, 0xC0000000, 0xC0400000, 0xC0000000, -2, -3, -2, -2 },  // -2.5
        { 0x40600000, 0x40400000, 0x40400000, 0x40800000,  3,  3,  4,  4 },  // 3.5
        { 0xC0600000, 0xC0400000, 0xC0800000, 0xC0400000, -3, -4, -3, -4 },  // -3.5
        { 0x40700000, 0x40400000, 0x40400000, 0x40800000,  3,  3,  4,  4 },  // 3.75
        { 0xC0700000, 0xC0400000, 0xC0800000, 0xC0400000, -3, -4, -3, -4 },  // -3.75
        { 0x41200000, 0x41200000, 0x41200000, 0x41300000, 10, 10, 11, 10 },  // 10: nudge, ceil 11
        { 0xC1200000, 0xC1200000, 0xC1300000, 0xC1200000, -10, -11, -10, -10 },  // -10: nudge, floor -11
        { 0x41280000, 0x41200000, 0x41200000, 0x41300000, 10, 10, 11, 10 },  // 10.5
        { 0xC1280000, 0xC1200000, 0xC1300000, 0xC1200000, -10, -11, -10, -10 },
        { 0x412E6666, 0x41200000, 0x41200000, 0x41300000, 10, 10, 11, 11 },  // 10.9f
        { 0xC12E6666, 0xC1200000, 0xC1300000, 0xC1200000, -10, -11, -10, -11 },
        { 0x3089705F, 0x00000000, 0x00000000, 0x00000000,  0,  0,  0,  0 },  // 1e-9f: nudge, ceil 0
        { 0xB089705F, 0x00000000, 0x00000000, 0x00000000,  0,  0,  0,  0 },  // -1e-9f: nudge, floor 0
        { 0x33000000, 0x00000000, 0x00000000, 0x3F800000,  0,  0,  1,  0 },  // 2^-25: the add ties up to 1
        { 0x32FFFFFE, 0x00000000, 0x00000000, 0x00000000,  0,  0,  0,  0 },  // just below: rounds to 0.99999994
        { 0x00000001, 0x00000000, 0x00000000, 0x00000000,  0,  0,  0,  0 },  // smallest denormal
        { 0x80000001, 0x00000000, 0x00000000, 0x00000000,  0,  0,  0,  0 },
        { 0x44160000, 0x44160000, 0x44160000, 0x44164000, 600, 600, 601, 600 },  // 600: nudge
        { 0x44870800, 0x44870000, 0x44870000, 0x44872000, 1080, 1080, 1081, 1080 },  // 1080.25
        { 0x4AFFFFFF, 0x4AFFFFFE, 0x4AFFFFFE, 0x4B000000, 8388607, 8388607, 8388608, 8388608 },  // 8388607.5
        { 0x4B000000, 0x4B000000, 0x4B000000, 0x4B000001, 8388608, 8388608, 8388609, 8388608 },  // 2^23: nudge
        { 0xCB000000, 0xCB000000, 0xCB000001, 0xCB000000, -8388608, -8388609, -8388608, -8388608 },
        { 0x4B7FFFFF, 0x4B7FFFFF, 0x4B7FFFFF, 0x4B800000, 16777215, 16777215, 16777216, 16777215 },  // nudge
        { 0x4B800000, 0x4B800000, 0x4B800000, 0x4B800000, 16777216, 16777216, 16777216, 16777216 },  // 2^24: ulp 2, exact
        { 0xCB800000, 0xCB800000, 0xCB800000, 0xCB800000, -16777216, -16777216, -16777216, -16777216 },
        { 0x4E6E6B28, 0x4E6E6B28, 0x4E6E6B28, 0x4E6E6B28, 1000000000, 1000000000, 1000000000, 1000000000 },
        { 0xCE6E6B28, 0xCE6E6B28, 0xCE6E6B28, 0xCE6E6B28, -1000000000, -1000000000, -1000000000, -1000000000 },
        { 0x4EFFFFFF, 0x4EFFFFFF, 0x4EFFFFFF, 0x4EFFFFFF, 2147483520, 2147483520, 2147483520, 2147483520 },  // largest below 2^31
        { 0xCEFFFFFF, 0xCEFFFFFF, 0xCEFFFFFF, 0xCEFFFFFF, -2147483520, -2147483520, -2147483520, -2147483520 },
        { 0x4F000000, 0x4F000000, 0x4F000000, 0x4F000000, kIndef, kIndef, kIndef, kIndef },  // 2^31: out of range
        { 0xCF000000, 0xCF000000, 0xCF000000, 0xCF000000, kIndef, kIndef, kIndef, kIndef },  // -2^31: INT_MIN, in range
        { 0x4F800000, 0x4F800000, 0x4F800000, 0x4F800000, kIndef, kIndef, kIndef, kIndef },  // 2^32
        { 0x4F9502F9, 0x4F800000, 0x4F800000, 0x4F800000, kIndef, kIndef, kIndef, kIndef },  // 5e9: shift count wraps, mantissa cleared
        { 0xCF9502F9, 0xCF800000, 0xCF800000, 0xCF800000, kIndef, kIndef, kIndef, kIndef },
        { 0x501502F9, 0x50000000, 0x50000000, 0x50000000, kIndef, kIndef, kIndef, kIndef },  // 1e10
        { 0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000, kIndef, kIndef, kIndef, kIndef },  // +Inf
        { 0xFF800000, 0xFF800000, 0xFF800000, 0xFF800000, kIndef, kIndef, kIndef, kIndef },  // -Inf
        { 0x7FC00000, 0x7F800000, 0x7F800000, 0x7F800000, kIndef, kIndef, kIndef, kIndef },  // qNaN: trunc makes it +Inf
        { 0xFFC00000, 0xFF800000, 0xFF800000, 0xFF800000, kIndef, kIndef, kIndef, kIndef },  // -qNaN: -Inf
    };

    // fast_float2long_round inputs for the rounding-mode rows below.
    const unsigned kRoundIn[8] = {
        0x3F000000, 0x3FC00000, 0x40200000,  // 0.5, 1.5, 2.5
        0xBF000000, 0xBFC00000, 0xC0200000,  // -0.5, -1.5, -2.5
        0x401FFFFF, 0x40200001               // either side of 2.5
    };

    struct ModeRow {
        unsigned rc;
        const char *name;
        Int toLong[8];       // fast_float2long_round(kRoundIn[k])
        Int ceilTen;         // REAL_TO_INT_CEIL(10.0f)
        Int floorMinusTen;   // REAL_TO_INT_FLOOR(-10.0f)
        Int ceilTiny;        // REAL_TO_INT_CEIL(1e-9f)
        Int floorMinusTiny;  // REAL_TO_INT_FLOOR(-1e-9f)
    };

    const ModeRow kModes[] = {
        { _RC_NEAR, "near", { 0, 2, 2,  0, -2, -2, 2, 3 }, 11, -11, 0,  0 },
        { _RC_CHOP, "chop", { 0, 1, 2,  0, -1, -2, 2, 2 }, 10, -10, 0,  0 },
        { _RC_UP,   "up",   { 1, 2, 3,  0, -1, -2, 3, 3 }, 11, -10, 1,  0 },
        { _RC_DOWN, "down", { 0, 1, 2, -1, -2, -3, 2, 2 }, 10, -11, 0, -1 },
    };
}

TEST(game_fp_mode_replica_sets_near_and_24_bit) {
    GameFPMode mode;
    const unsigned int cw = _controlfp(0, 0);
    CHECK_EQ(cw & _MCW_RC, (unsigned int)_RC_NEAR);
    CHECK_EQ(cw & _MCW_PC, (unsigned int)_PC_24);
}

TEST(fast_float_table_in_game_fp_mode) {
    GameFPMode mode;
    const int count = (int)(sizeof(kRows) / sizeof(kRows[0]));
    for (int row = 0; row < count; ++row) {
        const FloatRow &r = kRows[row];
        const float f = runtime_float(r.in);
        check_bits("near", r.in, "fast_float_trunc", bits_of(fast_float_trunc(f)), r.bTrunc, __LINE__);
        check_bits("near", r.in, "fast_float_floor", bits_of(fast_float_floor(f)), r.bFloor, __LINE__);
        check_bits("near", r.in, "fast_float_ceil", bits_of(fast_float_ceil(f)), r.bCeil, __LINE__);
        check_int("near", r.in, "REAL_TO_INT", REAL_TO_INT(f), r.toInt, __LINE__);
        check_int("near", r.in, "REAL_TO_INT_FLOOR", (Int)REAL_TO_INT_FLOOR(f), r.toIntFloor, __LINE__);
        check_int("near", r.in, "REAL_TO_INT_CEIL", (Int)REAL_TO_INT_CEIL(f), r.toIntCeil, __LINE__);
        check_int("near", r.in, "fast_float2long_round", (Int)fast_float2long_round(f), r.toLong, __LINE__);
    }
}

// EA's quirk, named so it shows up on its own (the table above has it too). In the game's
// round-to-nearest mode, fast_float_ceil of every whole number from 1 to 2^24 - 1 comes back
// one too high, and fast_float_floor of every whole number from -1 to -(2^24 - 1) one too low.
// Deliberately pinned, not fixed: REAL_TO_INT_CEIL/FLOOR feed the simulation, so a fix changes
// replays and multiplayer CRCs. (Reforged fixed it in its BaseType.h, so its simulation and
// ours differ here.)
TEST(ceil_floor_nudge_overshoots_whole_numbers_DEFECT) {
    GameFPMode mode;
    CHECK_EQ(bits_of(fast_float_ceil(runtime_float(0x41200000))), 0x41300000u);    // ceil(10) == 11
    CHECK_EQ(bits_of(fast_float_ceil(runtime_float(0x3F800000))), 0x40000000u);    // ceil(1) == 2
    CHECK_EQ(bits_of(fast_float_floor(runtime_float(0xC1200000))), 0xC1300000u);   // floor(-10) == -11
    CHECK_EQ((Int)REAL_TO_INT_CEIL(runtime_float(0x44160000)), 601);               // 600
    CHECK_EQ((Int)REAL_TO_INT_FLOOR(runtime_float(0xBF800000)), -2);               // -1
    CHECK_EQ((Int)REAL_TO_INT_CEIL(runtime_float(0x3F000000)), 1);                 // 0.5: fine off whole numbers
}

// The same helpers in the other three rounding modes, to show what depends on the mode. On x86,
// _controlfp sets the rounding field of both the x87 control word (fistp) and MXCSR (the SSE add
// inside floor/ceil). REAL_TO_INT never depends on it: trunc makes the value whole before fistp.
TEST(fast_float_rounding_modes) {
    GameFPMode mode;
    const int modeCount = (int)(sizeof(kModes) / sizeof(kModes[0]));
    for (int m = 0; m < modeCount; ++m) {
        const ModeRow &r = kModes[m];
        _controlfp(r.rc, _MCW_RC);

        check_int(r.name, 0x40700000, "REAL_TO_INT(3.75)", REAL_TO_INT(runtime_float(0x40700000)), 3, __LINE__);
        check_int(r.name, 0xC0700000, "REAL_TO_INT(-3.75)", REAL_TO_INT(runtime_float(0xC0700000)), -3, __LINE__);
        for (int k = 0; k < 8; ++k)
            check_int(r.name, kRoundIn[k], "fast_float2long_round", (Int)fast_float2long_round(runtime_float(kRoundIn[k])), r.toLong[k], __LINE__);
        check_int(r.name, 0x41200000, "REAL_TO_INT_CEIL(10)", (Int)REAL_TO_INT_CEIL(runtime_float(0x41200000)), r.ceilTen, __LINE__);
        check_int(r.name, 0xC1200000, "REAL_TO_INT_FLOOR(-10)", (Int)REAL_TO_INT_FLOOR(runtime_float(0xC1200000)), r.floorMinusTen, __LINE__);
        check_int(r.name, 0x3089705F, "REAL_TO_INT_CEIL(1e-9)", (Int)REAL_TO_INT_CEIL(runtime_float(0x3089705F)), r.ceilTiny, __LINE__);
        check_int(r.name, 0xB089705F, "REAL_TO_INT_FLOOR(-1e-9)", (Int)REAL_TO_INT_FLOOR(runtime_float(0xB089705F)), r.floorMinusTiny, __LINE__);
    }
}
