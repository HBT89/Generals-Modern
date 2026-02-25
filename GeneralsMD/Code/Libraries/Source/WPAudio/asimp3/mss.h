// asimp3/mss.h - Stub header for Asimp3 MSS compatibility
// Minimal stub for compilation. Used by AUD_DSoundDriver.cpp.
#pragma once
#ifndef __ASIMP3_MSS_STUB_H__
#define __ASIMP3_MSS_STUB_H__

// ASISTREAM handle type
typedef void* ASISTREAM;

// ASI callback types
typedef int (*ASI_STREAM_CALLBACK)(void* user, void* dest, int bytes_requested, int offset);

// ASI stream functions (stubs)
#ifdef __cplusplus
extern "C" {
#endif

inline ASISTREAM ASI_stream_open(unsigned int freq, int bits, int channels) { return NULL; }
inline int       ASI_stream_process(ASISTREAM stream, void* buffer, int buffer_size) { return 0; }
inline void      ASI_stream_close(ASISTREAM stream) {}
inline int       ASI_stream_seek(ASISTREAM stream, int offset) { return 0; }
inline int       ASI_stream_attribute(ASISTREAM stream, int attrib) { return 0; }
inline void      ASI_stream_set_preference(ASISTREAM stream, int pref, int value) {}

#ifdef __cplusplus
}
#endif

#endif // __ASIMP3_MSS_STUB_H__
