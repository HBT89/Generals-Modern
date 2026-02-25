// bink.h - Stub header for RAD Bink Video SDK
// Minimal stub to allow compilation without the proprietary Bink SDK.
#pragma once
#ifndef __BINK_STUB_H__
#define __BINK_STUB_H__

#include <windows.h>

// Bink handle type
typedef struct BINK {
    unsigned int Width;
    unsigned int Height;
    unsigned int Frames;
    unsigned int FrameNum;
    unsigned int FrameRate;
    unsigned int FrameRateDiv;
    unsigned int ReadError;
    void* LastDecompFrame;
} BINK;

typedef BINK* HBINK;

// Bink surface types
#define BINKSURFACE32       3
#define BINKSURFACE32R      4
#define BINKSURFACE32A      5
#define BINKSURFACE32RA     6
#define BINKSURFACE24       7
#define BINKSURFACE24R      8
#define BINKSURFACE555      0
#define BINKSURFACE565      1
#define BINKSURFACEMASK     15

// Bink copy flags
#define BINKCOPYALL         0x80000000

// Bink open flags
#define BINKNOTHREADEDIO    0x00800000

// Stub function declarations
#ifdef __cplusplus
extern "C" {
#endif

inline HBINK BinkOpen(const char* name, unsigned int flags) { return NULL; }
inline void  BinkClose(HBINK bink) {}
inline int   BinkWait(HBINK bink) { return 0; }
inline void  BinkNextFrame(HBINK bink) {}
inline int   BinkDoFrame(HBINK bink) { return 0; }
inline int   BinkCopyToBuffer(HBINK bink, void* buffer, int pitch, unsigned int height, unsigned int x, unsigned int y, unsigned int flags) { return 0; }
inline void  BinkGoto(HBINK bink, unsigned int frame, int flags) {}
inline void  BinkSetVolume(HBINK bink, unsigned int trackid, int volume) {}
inline void  BinkSetSoundTrack(unsigned int total_tracks, unsigned int* tracks) {}
inline void  BinkSetSoundSystem(void* open, unsigned int param) {}
inline void* BinkOpenMiles(unsigned int param) { return NULL; }
inline void  BinkSetSoundOnOff(HBINK bink, int onoff) {}
inline void  BinkSetIO(void* io) {}
inline void  BinkSetMemory(void* alloc, void* free) {}
inline int   BinkPause(HBINK bink, int pause) { return 0; }
inline void  BinkSetFrameRate(unsigned int rate, unsigned int ratediv) {}
inline unsigned int BinkGetError(void) { return 0; }

// Bink buffer helpers
inline void* BinkOpenDirectSound(unsigned int param) { return NULL; }

#ifdef __cplusplus
}
#endif

#endif // __BINK_STUB_H__
