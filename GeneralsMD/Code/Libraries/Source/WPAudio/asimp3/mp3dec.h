// mp3dec.h - Stub for asimp3 MP3 decoder
// Minimal stub to allow compilation. Replace with real SDK or minimp3 for audio.

#pragma once
#ifndef __ASIMP3_MP3DEC_STUB_H__
#define __ASIMP3_MP3DEC_STUB_H__

// MP3 decoder function stubs
// The real asimp3 SDK provides Miles-compatible MP3 decode functions.

typedef void* MP3_DECODER;

inline MP3_DECODER MP3_decoder_open(void*, int, int) { return NULL; }
inline int MP3_decoder_read(MP3_DECODER, void*, int) { return 0; }
inline void MP3_decoder_close(MP3_DECODER) {}
inline int MP3_decoder_seek(MP3_DECODER, int) { return 0; }

#endif // __ASIMP3_MP3DEC_STUB_H__
