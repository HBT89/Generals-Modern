// bink.h - Stub header for RAD Bink Video SDK
// Minimal stub to allow compilation without the proprietary Bink SDK.
// Replace with real bink.h from RAD Game Tools for video playback.

#pragma once
#ifndef __BINK_STUB_H__
#define __BINK_STUB_H__

#include <windows.h>

// Bink handle
typedef struct BINK {
    unsigned int Width;
    unsigned int Height;
    unsigned int Frames;
    unsigned int FrameNum;
    unsigned int FrameRate;
    unsigned int FrameRateDiv;
    unsigned int ReadError;
    void* LastFrame;
} BINK;

typedef BINK* HBINK;

// Bink surface types
#define BINKSURFACE32    3
#define BINKSURFACE24    4
#define BINKSURFACE565   5
#define BINKSURFACE555   6
#define BINKSURFACEGENERIC 7

// Bink copy flags
#define BINKCOPYALL      0x80000000

// Bink buffer structure
typedef struct {
    int Width;
    int Height;
    int WindowWidth;
    int WindowHeight;
    int SurfaceType;
    void* Buffer;
    int BufferPitch;
} BINKBUFFER;

typedef BINKBUFFER* HBINKBUFFER;

#ifdef __cplusplus
extern "C" {
#endif

// Core Bink functions - stubs
inline HBINK BinkOpen(const char* name, unsigned int flags) { return NULL; }
inline HBINK BinkOpenMem(void* mem, unsigned int size, unsigned int flags) { return NULL; }
inline void BinkClose(HBINK bink) {}

inline int BinkWait(HBINK bink) { return 0; }
inline void BinkNextFrame(HBINK bink) {}
inline int BinkDoFrame(HBINK bink) { return 0; }
inline int BinkCopyToBuffer(HBINK bink, void* dest, int pitch, int height, int x, int y, unsigned int flags) { return 0; }
inline void BinkGoto(HBINK bink, unsigned int frame, int flags) {}

// Bink sound
inline int BinkSetSoundSystem(void* system, void* driver) { return 0; }
inline int BinkSetSoundOnOff(HBINK bink, int on) { return 0; }
inline int BinkOpenMiles(void* driver) { return 0; }
inline int BinkOpenDirectSound(unsigned int freq) { return 0; }

// Bink buffer
inline HBINKBUFFER BinkBufferOpen(HWND hwnd, int width, int height, int flags) { return NULL; }
inline void BinkBufferClose(HBINKBUFFER buf) {}
inline int BinkBufferLock(HBINKBUFFER buf) { return 0; }
inline int BinkBufferUnlock(HBINKBUFFER buf) { return 0; }
inline void BinkBufferBlit(HBINKBUFFER buf, HBINK bink, int x, int y) {}
inline void BinkBufferSetOffset(HBINKBUFFER buf, int x, int y) {}

// Bink flags
#define BINKNOFILLIOBUF     0x00000800
#define BINKNOSKIP          0x00080000
#define BINKPRELOADALL      0x00002000
#define BINKSNDTRACK        0x00004000

#ifdef __cplusplus
}
#endif

#endif // __BINK_STUB_H__
