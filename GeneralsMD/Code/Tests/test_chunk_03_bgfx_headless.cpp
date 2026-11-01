// test_chunk_03_bgfx_headless.cpp
// Verifies bgfx can init, execute a frame, and shut down cleanly in Noop renderer
// mode (no window or GPU required). Catches linker issues and bgfx API breakage
// before running the full game build.

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "test_harness.h"

// The only test in this binary: renderFrame-before-init and bgfx's global state allow
// one init/shutdown cycle per process.
TEST(bgfx_noop_init_frame_shutdown) {
    // Single-threaded mode: renderFrame before init suppresses bgfx's render thread.
    // Required when the caller owns the main loop (same as the game).
    bgfx::renderFrame();

    bgfx::Init init;
    init.type             = bgfx::RendererType::Noop;
    init.resolution.width  = 0;
    init.resolution.height = 0;
    // No HWND needed for Noop renderer
    init.platformData.nwh  = nullptr;

    const bool ok = bgfx::init(init);
    CHECK(ok);
    if (!ok) return;
    CHECK_EQ(bgfx::getRendererType(), bgfx::RendererType::Noop);

    // One frame cycle: clear, touch, frame
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    bgfx::touch(0);
    bgfx::frame();

    bgfx::shutdown();
}
