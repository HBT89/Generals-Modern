/* gnu_regex.h - Stub header for GNU regex library
 * Provides minimal type definitions and function stubs
 * to satisfy compilation of regexpr.cpp
 */

#ifndef GNU_REGEX_H
#define GNU_REGEX_H

#include <stddef.h>

/* Syntax flag type */
typedef unsigned long reg_syntax_t;

/* Syntax flag constants used by regexpr.cpp */
#define RE_CHAR_CLASSES          0x0001
#define RE_CONTEXT_INDEP_ANCHORS 0x0002
#define RE_CONTEXT_INDEP_OPS     0x0004
#define RE_CONTEXT_INVALID_OPS   0x0008
#define RE_INTERVALS             0x0010
#define RE_NO_BK_BRACES          0x0020
#define RE_NO_BK_PARENS          0x0040
#define RE_NO_BK_VBAR            0x0080
#define RE_NO_EMPTY_RANGES       0x0100

/* Compiled regex pattern structure */
typedef struct {
    void *buffer;
    unsigned long allocated;
    unsigned long used;
    reg_syntax_t syntax;
    char *fastmap;
    unsigned char *translate;
    size_t re_nsub;
    unsigned int can_be_null : 1;
    unsigned int regs_allocated : 2;
    unsigned int fastmap_accurate : 1;
    unsigned int no_sub : 1;
    unsigned int not_bol : 1;
    unsigned int not_eol : 1;
    unsigned int newline_anchor : 1;
} regex_t;

/* Match data structure */
typedef int regoff_t;

typedef struct {
    unsigned num_regs;
    regoff_t *start;
    regoff_t *end;
} regmatch_t;

/* Stub function declarations */
static inline reg_syntax_t re_set_syntax(reg_syntax_t syntax) {
    (void)syntax;
    return 0;
}

static inline const char *re_compile_pattern(const char *pattern, size_t length, regex_t *preg) {
    (void)pattern;
    (void)length;
    (void)preg;
    return "regex stub - not implemented";
}

static inline int re_match(regex_t *preg, const char *string, size_t length, int start, regmatch_t *regs) {
    (void)preg;
    (void)string;
    (void)length;
    (void)start;
    (void)regs;
    return -1;  /* No match */
}

static inline void regfree(regex_t *preg) {
    (void)preg;
}

#endif /* GNU_REGEX_H */
