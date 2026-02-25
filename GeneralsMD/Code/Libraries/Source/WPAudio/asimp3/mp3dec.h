// asimp3/mp3dec.h - Stub header for Asimp3 MP3 decoder
// Minimal stub for compilation. Used by AUD_DSoundDriver.cpp.
#pragma once
#ifndef __ASIMP3_MP3DEC_STUB_H__
#define __ASIMP3_MP3DEC_STUB_H__

#include "mss.h"

// MP3 decoder types
typedef struct {
    ASISTREAM *asi;
    void *data;
    int data_size;
    int data_offset;
    int sample_rate;
    int channels;
    int bits_per_sample;
} MP3_DATA;

// MP3 transfer callback type
typedef int (*MP3_TRANSFER_CB)(void* user, void* dest, int bytes_requested, int offset);

// MP3 functions (stubs)
#ifdef __cplusplus
extern "C" {
#endif

inline int MP3_transfer(void* user, void* dest, int bytes_requested, int offset) { return 0; }

#ifdef __cplusplus
}
#endif

#endif // __ASIMP3_MP3DEC_STUB_H__
