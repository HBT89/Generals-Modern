#pragma once
#ifndef WSYS_FILE_H
#define WSYS_FILE_H

// wsys File stub - proprietary library removed
// Minimal declarations for compilation

#include <cstddef>

struct File
{
    void* handle;
    int mode;
};

inline File* FileOpen(const char* filename, int mode)
{
    (void)filename; (void)mode;
    return NULL;
}

inline void FileClose(File* file)
{
    (void)file;
}

inline int FileRead(File* file, void* buffer, int size)
{
    (void)file; (void)buffer; (void)size;
    return 0;
}

inline int FileSeek(File* file, int offset, int origin)
{
    (void)file; (void)offset; (void)origin;
    return 0;
}

inline int FileTell(File* file)
{
    (void)file;
    return 0;
}

#endif // WSYS_FILE_H
