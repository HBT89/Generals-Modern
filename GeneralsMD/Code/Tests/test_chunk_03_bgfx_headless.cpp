// test_chunk_03_bgfx_headless.cpp
// Verifies bgfx can init, execute a frame, and shut down cleanly in Noop renderer
// mode (no window or GPU required). Catches linker issues and bgfx API breakage
// before running the full game build.

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <cassert>
#include <stdio.h>

int main() {
    // Single-threaded mode: renderFrame before init suppresses bgfx's render thread.
    // Required when the caller owns the main loop (same as the game).
    bgfx::renderFrame();

    bgfx::Init init;
    init.type             = bgfx::RendererType::Noop;
    init.resolution.width  = 0;
    init.resolution.height = 0;
    // No HWND needed for Noop renderer
    init.platformData.nwh  = nullptr;

    bool ok = bgfx::init(init);
    if (!ok) {
        printf("FAIL: bgfx::init(Noop) returned false\n");
        return 1;
    }

    // One frame cycle: clear, touch, frame
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    bgfx::touch(0);
    bgfx::frame();

    bgfx::shutdown();

    printf("BGFX headless init/frame/shutdown passed!\n");
    return 0;
}
