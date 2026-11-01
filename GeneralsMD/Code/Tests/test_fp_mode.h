// test_fp_mode.h
// The FPU mode the game runs its simulation in, for tests that pin simulation math.
// setFPMode() (GameLogic.cpp:193) does _fpreset() and then sets round-to-nearest and 24-bit
// x87 precision. It is part of the engine, so it is replicated here rather than called.
// (BaseType.h:179 says the mode is CHOP; the CHOP line in setFPMode is commented out.)
// Construct one at the top of a TEST; the caller's RC and PC come back at scope exit.
#ifndef TEST_FP_MODE_H
#define TEST_FP_MODE_H

#include <float.h>

struct GameFPMode {
    unsigned int saved;
    GameFPMode() {
        saved = _controlfp(0, 0);
        _fpreset();
        _controlfp(_RC_NEAR | _PC_24, _MCW_RC | _MCW_PC);
    }
    ~GameFPMode() { _controlfp(saved, _MCW_RC | _MCW_PC); }
};

#endif // TEST_FP_MODE_H
