#pragma once
#ifndef BINK_H
#define BINK_H

// Bink Video stub - proprietary library removed
// Minimal declarations for compilation

#include <cstddef>

#ifdef _WIN32
#include <windows.h>
#endif

// ---------------------------------------------------------------------------
// Bink handle and structures
// ---------------------------------------------------------------------------
typedef void* HBINK;

typedef struct BINKIO
{
    void* data;
} BINKIO;

typedef struct BINKSND
{
    void* data;
} BINKSND;

typedef struct BINKRECT
{
    int left, top, right, bottom;
} BINKRECT;

typedef struct BINK
{
    unsigned int Width;
    unsigned int Height;
    unsigned int Frames;
    unsigned int FrameNum;
    unsigned int FrameRate;
    unsigned int FrameRateDiv;
    unsigned int ReadError;
    void* FrameBuffers;
} BINK;

// ---------------------------------------------------------------------------
// Bink stub functions
// ---------------------------------------------------------------------------

inline HBINK BinkOpen(const char* name, unsigned int flags)
{
    (void)name; (void)flags;
    return NULL;
}

inline void BinkClose(HBINK bink)
{
    (void)bink;
}

inline int BinkWait(HBINK bink)
{
    (void)bink;
    return 0;
}

inline int BinkDoFrame(HBINK bink)
{
    (void)bink;
    return 0;
}

inline void BinkNextFrame(HBINK bink)
{
    (void)bink;
}

inline int BinkCopyToBuffer(HBINK bink, void* dest, int destpitch, unsigned int destheight, unsigned int destx, unsigned int desty, unsigned int flags)
{
    (void)bink; (void)dest; (void)destpitch; (void)destheight; (void)destx; (void)desty; (void)flags;
    return 0;
}

inline void BinkSetVolume(HBINK bink, unsigned int trackid, int volume)
{
    (void)bink; (void)trackid; (void)volume;
}

inline int BinkSetSoundSystem(void* open, unsigned long param)
{
    (void)open; (void)param;
    return 0;
}

inline void BinkSetSoundTrack(unsigned int total, unsigned int* tracks)
{
    (void)total; (void)tracks;
}

inline void BinkSetMemory(void* (*alloc)(unsigned int), void (*free)(void*))
{
    (void)alloc; (void)free;
}

inline void BinkGoto(HBINK bink, unsigned int frame, int flags)
{
    (void)bink; (void)frame; (void)flags;
}

inline int BinkPause(HBINK bink, int pause)
{
    (void)bink; (void)pause;
    return 0;
}

// Bink surface type defines
#define BINKSURFACE8     0
#define BINKSURFACE24    1
#define BINKSURFACE32    2
#define BINKSURFACE565   3
#define BINKSURFACE555   4
#define BINKSURFACE5551  5
#define BINKSURFACE4444  6
#define BINKSURFACE32R   7
#define BINKSURFACE32A   8
#define BINKSURFACE32RA  9
#define BINKSURFACEYUY2  10

// Bink open flags
#define BINKFILEHANDLE   0x00800000
#define BINKNOSKIP       0x00080000
#define BINKPRELOADALL   0x00002000
#define BINKSNDTRACK     0x00004000
#define BINKOLDFRAMEFORMAT 0x00008000
#define BINKCOPYALL      0x00010000
#define BINKNOFRAMEBUFFERS 0x00000040
#define BINKUSETRIPLEBUFFERING 0x00000080

#endif // BINK_H
