// test_harness_armed_control.cpp
// Fails on purpose. CMakeLists.txt registers it with WILL_FAIL, so ctest passes it only
// while a failing CHECK still makes a test binary exit non-zero. If that ever broke,
// every other test would pass vacuously, as the assert()-based ones did in Release;
// this one would go red instead.

#include "test_harness.h"

TEST(armed_control_fails_on_purpose) {
    CHECK(1 == 1);
    CHECK(1 == 2);
}
