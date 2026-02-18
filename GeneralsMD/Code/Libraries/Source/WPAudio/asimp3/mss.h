// mss.h - Stub for asimp3 Miles Sound System MP3 decoder
// Minimal stub to allow compilation. Replace with real SDK for audio.

#pragma once
#ifndef __ASIMP3_MSS_STUB_H__
#define __ASIMP3_MSS_STUB_H__

// The asimp3 package provides Miles-compatible MP3 decoding.
// This stub defines the minimum needed for AUD_DSoundDriver.cpp to compile.

typedef void* HPROVIDER;
typedef void* HSAMPLE;

// ASI codec interface
typedef void* HASISTREAM;

inline HASISTREAM AIL_ASI_stream_open(unsigned int, void*, int) { return NULL; }
inline int AIL_ASI_stream_process(HASISTREAM, void*, int) { return 0; }
inline void AIL_ASI_stream_close(HASISTREAM) {}
inline int AIL_ASI_stream_attribute(HASISTREAM, int) { return 0; }
inline void AIL_ASI_stream_seek(HASISTREAM, int) {}

#endif // __ASIMP3_MSS_STUB_H__
