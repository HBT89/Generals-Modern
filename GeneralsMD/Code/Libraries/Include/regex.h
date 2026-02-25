#pragma once
#ifndef REGEX_STUB_H
#define REGEX_STUB_H

// GNU regex stub - proprietary/platform-specific library removed
// Minimal declarations for compilation

#include <cstddef>

// ---------------------------------------------------------------------------
// Regex error codes
// ---------------------------------------------------------------------------
#define REG_NOERROR   0
#define REG_NOMATCH   1
#define REG_BADPAT    2
#define REG_ECOLLATE  3
#define REG_ECTYPE    4
#define REG_EESCAPE   5
#define REG_ESUBREG   6
#define REG_EBRACK    7
#define REG_EPAREN    8
#define REG_EBRACE    9
#define REG_BADBR     10
#define REG_ERANGE    11
#define REG_ESPACE    12
#define REG_BADRPT    13

// ---------------------------------------------------------------------------
// Regex compile flags
// ---------------------------------------------------------------------------
#define REG_EXTENDED  1
#define REG_ICASE     2
#define REG_NOSUB     4
#define REG_NEWLINE   8

// ---------------------------------------------------------------------------
// Regex exec flags
// ---------------------------------------------------------------------------
#define REG_NOTBOL    1
#define REG_NOTEOL    2

// ---------------------------------------------------------------------------
// Regex structures
// ---------------------------------------------------------------------------
typedef struct
{
    void* buffer;
    size_t allocated;
    size_t used;
    size_t re_nsub;
} regex_t;

typedef int regoff_t;

typedef struct
{
    regoff_t rm_so;
    regoff_t rm_eo;
} regmatch_t;

// ---------------------------------------------------------------------------
// Stub functions
// ---------------------------------------------------------------------------

inline int regcomp(regex_t* preg, const char* pattern, int cflags)
{
    (void)preg; (void)pattern; (void)cflags;
    if (preg)
    {
        preg->buffer = NULL;
        preg->allocated = 0;
        preg->used = 0;
        preg->re_nsub = 0;
    }
    return 0;
}

inline int regexec(const regex_t* preg, const char* string, size_t nmatch, regmatch_t pmatch[], int eflags)
{
    (void)preg; (void)string; (void)nmatch; (void)pmatch; (void)eflags;
    return REG_NOMATCH;
}

inline void regfree(regex_t* preg)
{
    (void)preg;
}

inline size_t regerror(int errcode, const regex_t* preg, char* errbuf, size_t errbuf_size)
{
    (void)errcode; (void)preg;
    if (errbuf && errbuf_size > 0)
    {
        errbuf[0] = '\0';
    }
    return 0;
}

#endif // REGEX_STUB_H
