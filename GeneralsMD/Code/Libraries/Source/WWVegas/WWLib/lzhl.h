#pragma once
#ifndef LZHL_H
#define LZHL_H

// LZHL compression stub - proprietary library removed
// Minimal declarations for compilation

#include <cstddef>

// ---------------------------------------------------------------------------
// Handle typedefs
// ---------------------------------------------------------------------------
typedef void* LZHL_CHANDLE;
typedef void* LZHL_DCHANDLE;

// ---------------------------------------------------------------------------
// Stub functions
// ---------------------------------------------------------------------------

inline LZHL_CHANDLE LZHLCreateCompressor(void)
{
    return NULL;
}

inline void LZHLDestroyCompressor(LZHL_CHANDLE handle)
{
    (void)handle;
}

inline size_t LZHLCompressBlock(LZHL_CHANDLE handle, void* dest, size_t destSize, const void* src, size_t srcSize)
{
    (void)handle; (void)dest; (void)destSize; (void)src; (void)srcSize;
    return 0;
}

inline LZHL_DCHANDLE LZHLCreateDecompressor(void)
{
    return NULL;
}

inline void LZHLDestroyDecompressor(LZHL_DCHANDLE handle)
{
    (void)handle;
}

inline int LZHLDecompressBlock(LZHL_DCHANDLE handle, void* dest, size_t* destSize, const void* src, size_t srcSize)
{
    (void)handle; (void)dest; (void)destSize; (void)src; (void)srcSize;
    return 0;
}

inline size_t LZHLCompressorCalcMaxBuf(size_t inSize)
{
    (void)inSize;
    return 0;
}

#endif // LZHL_H
