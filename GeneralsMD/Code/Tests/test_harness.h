// Taken unchanged from Zero Hour Reforged's GeneralsMD/Code/Tests/test_harness.h by Olcay Seygan
// (github.com/olcayseygan/CnCGeneralsZH-Reforged, commit c13a474f), GPL-3.0-or-later.
// The CLAUDE.md it mentions is Reforged's.
/*
 * Minimal self-registering test harness.
 *
 * No framework on purpose (see CLAUDE.md): a test is one TEST(name) { ... }
 * block, registration happens at static-init time, and test_main.cpp supplies
 * main().  Every binary links this plus the lib under test and is registered
 * with CTest, so `ctest -C Release` runs the lot.
 *
 *   TEST(vector3_dot) {
 *     CHECK_NEAR(Vector3::Dot_Product(a, b), 32.0f, 1e-5f);
 *   }
 */
#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

#include <stdio.h>
#include <string.h>
#include <math.h>

void test_check(bool ok, const char *expr, const char *file, int line);
void test_register(const char *name, void (*fn)(void));

struct TestRegistrar
{
	TestRegistrar(const char *name, void (*fn)(void)) { test_register(name, fn); }
};

#define TEST(name)                                                    \
	static void name(void);                                           \
	static TestRegistrar test_reg_##name(#name, name);                \
	static void name(void)

#define CHECK(expr) test_check(!!(expr), #expr, __FILE__, __LINE__)

#define CHECK_EQ(a, b)                                                \
	test_check((a) == (b), #a " == " #b, __FILE__, __LINE__)

#define CHECK_NE(a, b)                                                \
	test_check((a) != (b), #a " != " #b, __FILE__, __LINE__)

/* Absolute tolerance. Fine for the value ranges this codebase deals in
   (unit vectors, world coordinates in the hundreds). */
#define CHECK_NEAR(a, b, eps)                                         \
	test_check(fabs(double(a) - double(b)) <= double(eps),            \
	           #a " ~= " #b, __FILE__, __LINE__)

/* The casts matter: StringClass converts to const char*, but it also has an
   operator!=(const char*) that would take over the null check and call
   _tcscmp(buffer, NULL).  It goes through a function rather than a macro body
   so each side is evaluated exactly once - the macro used to name (a) twice,
   which ran any side effect in the argument twice. */
inline void test_check_str(const char *a, const char *b, const char *expr,
                           const char *file, int line)
{
	test_check(a != 0 && b != 0 && !strcmp(a, b), expr, file, line);
}

#define CHECK_STR(a, b)                                               \
	test_check_str((const char *)(a), (const char *)(b),              \
	               #a " streq " #b, __FILE__, __LINE__)

#define CHECK_MEM(a, b, n)                                            \
	test_check(!memcmp((a), (b), (n)), #a " memeq " #b, __FILE__, __LINE__)

#endif /* TEST_HARNESS_H */
