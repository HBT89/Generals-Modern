// Taken from Zero Hour Reforged's GeneralsMD/Code/Tests/test_main.cpp by Olcay Seygan
// (github.com/olcayseygan/CnCGeneralsZH-Reforged, commit c13a474f), GPL-3.0-or-later.
// Unchanged except the exit-code sentence below (comment corrected here: main() returns
// the number of failed tests, not failed checks).
/*
 * Runner for the micro-harness in test_harness.h.  One of these is linked into
 * every test binary; the tests themselves register from static initialisers.
 *
 * Exit code is the number of failed tests (capped at 125 so it stays a valid
 * process exit status), which is what CTest keys off.
 */
#include "test_harness.h"

#include <stdlib.h>

namespace
{
	/* Fixed-capacity registry: static-init order is unspecified, so a
	   std::vector here would risk being constructed after the first
	   registrar runs.  A POD array is zero-initialised before any of it. */
	const int MAX_TESTS = 512;

	struct Entry
	{
		const char *name;
		void (*fn)(void);
	};

	Entry g_tests[MAX_TESTS];
	int g_numTests;

	const char *g_currentTest;
	int g_currentFails;
	int g_totalChecks;
	int g_totalFails;
	int g_failedTests;
}

void test_register(const char *name, void (*fn)(void))
{
	if (g_numTests >= MAX_TESTS)
	{
		fprintf(stderr, "test registry full, raise MAX_TESTS\n");
		exit(1);
	}
	g_tests[g_numTests].name = name;
	g_tests[g_numTests].fn = fn;
	++g_numTests;
}

void test_check(bool ok, const char *expr, const char *file, int line)
{
	++g_totalChecks;
	if (ok)
		return;

	++g_currentFails;
	++g_totalFails;

	/* Strip the directory so failures stay readable in a narrow console. */
	const char *base = strrchr(file, '\\');
	if (!base)
		base = strrchr(file, '/');
	printf("  FAIL %s:%d  %s\n", base ? base + 1 : file, line, expr);
}

int main(int argc, char *argv[])
{
	/* Optional substring filter, so a single failing test can be re-run alone:
	   test_wwlib.exe crc  */
	const char *filter = (argc > 1) ? argv[1] : 0;
	int ran = 0;

	for (int i = 0; i < g_numTests; ++i)
	{
		if (filter && !strstr(g_tests[i].name, filter))
			continue;

		g_currentTest = g_tests[i].name;
		g_currentFails = 0;
		++ran;

		g_tests[i].fn();

		if (g_currentFails)
		{
			++g_failedTests;
			printf("FAIL %s (%d)\n", g_currentTest, g_currentFails);
		}
	}

	printf("%d tests, %d checks, %d failed\n", ran, g_totalChecks, g_totalFails);

	if (!ran)
	{
		printf("no tests matched\n");
		return 1;
	}

	return g_failedTests > 125 ? 125 : g_failedTests;
}
