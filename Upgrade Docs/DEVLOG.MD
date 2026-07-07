# Generals-Modern BGFX Migration — Dev Log

## Project Goal
Migrate Command & Conquer Generals Zero Hour rendering from DirectX 8 to BGFX (cross-platform, modern graphics API).

---

## Current Status — 2026-03-11

### What Works
- Full build pipeline: all libraries → GameEngine → GameEngineDevice → GeneralsZH.exe (zero errors)
- Game launches, window creates, resolution management working
- Main menu loads with all **6 buttons visible**: SOLO PLAY, MULTIPLAYER, LOAD, OPTIONS, CREDITS, EXIT GAME
- Generals Zero Hour logo renders correctly
- EXIT GAME popup works (confirmed interactive)
- Shell map terrain renders as 3D geometry (flat gray — textures missing, but geometry/camera correct)
- Transition animations working (buttons animate in, scale-up + fade)
- TSS state pollution fix applied: terrain ALPHAOP=MODULATE no longer persists into UI draws

### Active Bug: Top 2 Buttons Briefly Invisible During Terrain Fade-In
- SOLO PLAY and MULTIPLAYER disappear for ~8 seconds when terrain starts rendering
- Matches duration of the "black fade-in" overlay animation (terrain transition)
- They reappear after the fade completes — this is animation-related, not permanent
- Under investigation: Z-order of the fade overlay vs. button draw order, or ScaleUp sub-animation

### Known Issues (Not Yet Fixed)
- [ ] Terrain textures not binding — terrain renders as flat vertex colors (gray/sand)
- [ ] SOLO PLAY + MULTIPLAYER temporarily invisible during terrain fade-in (ongoing)
- [ ] Terrain camera angle wrong — zoomed into water surface instead of overview
- [ ] Texture atlas flash during initial load (UV sub-rect binding issue)
- [ ] Skirmish game crashes (TheScriptEngine::UPDATE exception)
- [ ] ~235 ObjectReskin template-not-found warnings
- [ ] Blue grid/ruler border visible at screen edges (debug artifact)
- [ ] Sound system stubbed (no audio)

---

## Session History

---

## 2026-03-11 — Session 8+: Main Menu UI & Shell Map Rendering

### Milestones
- **All 6 buttons now visible** after resolution fix (1024x768 instead of ultrawide)
- **EXIT GAME button works** — popup confirmed interactive without force-close
- **State pollution fixed**: TSS reset on 3D→2D switch prevents terrain ALPHAOP=MODULATE from killing UI
- **Terrain geometry rendering**: 3D shell map visible (beach/sand geometry, water animating)
- **Root-caused button disappearance**: `MainMenuShutdown` was being called on shell game start via `Shell::push()` shutdown chain; traced and confirmed by logging. GAME_SHELL mode now correctly bypasses `hideShell()` but sub-animation issue remains for top 2 buttons.

### Key Fixes Applied

#### TSS State Pollution Fix
**Problem**: Terrain rendering sets `ALPHAOP=D3DTOP_MODULATE` via `Set_DX8_Texture_Stage_State()`. Since `Apply_Render_State_Changes()` is a no-op, this value persists in `TextureStageStates[]`. UI draws that follow see MODULATE and multiply vertex alpha × texture alpha. Terrain vertices have `diffuse.alpha=0`, making all subsequent UI elements invisible.

**Fix** (in `BGFXWrapper.cpp::Set_View_Identity()`):
```cpp
bool was3D = !s_viewIsIdentity;
s_viewIsIdentity = true;
if (was3D) {
    TextureStageStates[0][D3DTSS_ALPHAOP]   = D3DTOP_SELECTARG1;
    TextureStageStates[0][D3DTSS_ALPHAARG1]  = D3DTA_TEXTURE;
    TextureStageStates[0][D3DTSS_ALPHAARG2]  = D3DTA_CURRENT;
    TextureStageStates[0][D3DTSS_COLOROP]    = D3DTOP_MODULATE;
    TextureStageStates[0][D3DTSS_COLORARG1]  = D3DTA_TEXTURE;
    TextureStageStates[0][D3DTSS_COLORARG2]  = D3DTA_DIFFUSE;
}
```

#### TSS-Aware Vertex Alpha Fix
**Problem**: BGFX shader always multiplies `texture.alpha × vertex.alpha`. DX8 fixed-function with `ALPHAOP=SELECTARG1 + ALPHAARG1=TEXTURE` ignores vertex alpha entirely. UI vertices with `diffuse.alpha=0` (default) were transparent.

**Fix** (in `SubmitDraw` and `Draw_Sorting_IB_VB`):
```cpp
if ((dst[k].diffuse >> 24) == 0) {
    unsigned alphaOp   = s_TSS ? s_TSS[0][D3DTSS_ALPHAOP]   : D3DTOP_SELECTARG1;
    unsigned alphaArg1 = s_TSS ? s_TSS[0][D3DTSS_ALPHAARG1] : D3DTA_TEXTURE;
    if (alphaOp == D3DTOP_SELECTARG1 && (alphaArg1 & 0x0F) == D3DTA_TEXTURE)
        dst[k].diffuse |= 0xFF000000;
}
```
Static pointer `s_TSS = TextureStageStates` initialized in `Init()` (file-scope functions can't access class members directly).

#### DX8 TSS Defaults Initialization
`TextureStageStates[]` is zero-initialized (arrays). DX8 default for `ALPHAOP` is `SELECTARG1=2`, not `DISABLE=1`. Added explicit initialization in `BGFXWrapper::Init()` after bgfx init:
```cpp
TextureStageStates[0][D3DTSS_ALPHAOP]   = D3DTOP_SELECTARG1;   // 2
TextureStageStates[0][D3DTSS_ALPHAARG1]  = D3DTA_TEXTURE;       // 2
TextureStageStates[0][D3DTSS_ALPHAARG2]  = D3DTA_CURRENT;       // 1
```

#### Resolution Fix
**Problem**: At ultrawide resolution (3440×1440), the transition system calculates button positions and animations based on 800×600 or 1024×768 logical coordinates. Buttons at ultrawide positions are off-screen during animations, causing the "top 3 buttons disappear" behavior.

**Fix** (in `BGFXWrapper.cpp::Init()`): Override game's requested resolution to use monitor's native resolution with proportional scaling clamped to `1024×768` borderless window.

#### `Apply_Render_State_Changes()` is a No-Op
**Critical Architecture Note**: `Apply_Render_State_Changes()` is implemented as:
```cpp
void BGFXWrapper::Apply_Render_State_Changes() { render_state_changed = 0; }
```
The DX8 ShaderClass code that would set ALPHAOP through the wrapper never actually applies because this function is a no-op. All TSS values written via `Set_DX8_Texture_Stage_State()` go directly into the `TextureStageStates[]` array. This means the TSS reset approach is correct — we manage the array manually rather than through shader apply.

### Debugging Infrastructure Added
- `[LOGIC]` log entries in `GameLogicDispatch.cpp::prepareNewGame()` — logs gameMode and isShellGame
- `[SHELL]` log entries in `Shell::hideShell()` and `Shell::push()` — traces shell hide/push operations
- `[MENU]` log entry in `MainMenu.cpp::MainMenuShutdown()` — traces shutdown trigger source
- All logs append to `bgfx_loading.log`

---

## 2026-03-09 — Session 7: 3D Terrain Rendering & UI Depth Fix

### Milestones
- Shell map terrain geometry renders (W3DShaderManager init restored)
- 3D/2D depth separation via BGFX view system (view 0 = terrain, view 1 = UI)
- Texture handle cache prevents bgfx handle recycling collisions
- Flip_To_Primary no-op fix (prevented double frame call clearing UI)

### Key Fixes
- **W3DShaderManager::init() had early `return;`** — bypassed all terrain shader setup, terrain drew nothing
- **CameraClass::Apply() was `#if 0`'d** — restored; sets proper view/proj transforms for 3D scene
- **Set_View_Identity was flag-only** — now actually writes identity matrix; UI z-values were -1 (outside BGFX [0,1] range), clipping all UI draws
- **Matrix convention**: WW3D uses column-vector M*v (translation in column 3 = indices [3][7][11]). Initial implementation used row-vector v*M convention → wrong z values
- **W3DShaderManager reset() functions** must restore blend states (D3DRS_ALPHABLENDENABLE etc.), not just textures — terrain blend states leaked into UI draws

---

## 2026-03-08 — Session 6: Texture Pipeline & INI Parsing

### Milestones
- Texture upload pipeline working (REAL texture bindings in draw log)
- ZH .big file loading order correct (ZH overrides base Generals)
- INI parsing resilient (try/catch around blocks, nested block skip)
- ObjectReskin gracefully handles missing source template

### Key Fixes
- **Texture upload timing**: TextureLoader is async; textures created empty, data applied later via `Apply_New_Surface()`
- **Registry path**: `GetStringFromGeneralsRegistry` returned ZH install root, not base game path
- **Double loading**: INI files loaded twice in some paths; second pass couldn't find templates from first

---

## 2026-02-25 — Session 5: GameEngine + GameEngineDevice Build

### Milestones
- GameEngine.lib compiles with 0 errors
- GameEngineDevice.lib compiles with 0 errors
- GeneralsZH.exe links and launches
- Main menu visible for the first time

### Key Fixes
- `Render2DClass::Set_Coordinate_Range()` division by zero (called before resolution known) — added guard
- Alpha blending enabled in Set_Shader (UI was all white rectangles)
- `MILES_HANDLE` defined as `unsigned long` in audiblesound.h

---

## 2026-02-22 — Session 4: wwshade + GameEngine Start

### Milestones
- **wwshade.lib compiles with 0 errors**
- GameEngine compilation started

### Key Fix
- `D3DXVECTOR4` missing `operator const void*()` in DX90SDK stub — added to RenderTypes.h

---

## 2026-02-21 — Session 3: ww3d2 Clean Build

### Milestones
- **ww3d2.lib compiles with 0 errors** (from ~1,557 errors at session start)
- **wwshade down to 36 errors** (from 183)

### Key Fixes
- `W3DMPO_GLUE(DynamicMeshModel)` macro was removed — restored; required by all W3D classes inheriting W3DMPO
- `shattersystem.cpp` needed 14 missing DynamicMeshClass methods — added as stubs in dynamesh.h
- `D3DCMPFUNC` was anonymous enum — changed to named typedef
- `Matrix4` undeclared in sortingrenderer.cpp — added `#include "sr_util.h"` (has `typedef Matrix4x4 Matrix4`)
- `bgfx::submit(0)` needed program handle — changed to `bgfx::submit(0, BGFX_INVALID_HANDLE)`

---

## 2026-02-20 — Session 2: BGFX Architecture

### Milestones
- ww3d2 errors reduced from ~1,004 to ~93
- BGFXWrapper.cpp/h created
- BGFXVertexBuffer, BGFXIndexBuffer created
- BGFX shader .sc files created
- dx8wrapper/dx8caps replaced with BGFX equivalents
- dx8fvf.h vertex format structs restored

### Architecture Decisions
- `RenderTypes.h` is single source of truth for all D3D8 placeholder types
- Guard macros block DX90SDK stub headers: `_D3D8TYPES_H_`, `_D3D8_H_`, `_D3DX8MATH_H_`, etc.
- `#define DX8Wrapper BGFXWrapper` for drop-in replacement

---

## 2026-02-19 — Session 1: Project Setup & Library Builds

### Milestones
- CMake build system created from scratch (CMakeLists.txt)
- All base libraries built: wwdebug, wwlib, wwmath, wwdownload, compression, wwsaveload, wwutil, benchmark, GameSpy stubs, eabrowserdispatch, profile_lib, debug_lib

### Key Fixes Established
- `AnimateWindow` Win32 collision → `#define AnimateWindow _Win32_AnimateWindow` redirect
- `Matrix4` → `Matrix4x4` rename → typedef in sr_util.h
- `MILES_HANDLE` = `unsigned long`
- `DLINK_ITERATOR` needs `&` for pointer-to-member in modern MSVC
- Placement `new[]` conflict in GameMemory.h — commented out
- PreRTS.h: `<fstream.h>` → `<fstream>`, removed `<mapicode.h>` and `<snmp.h>`

---

## Build Status — 2026-03-11

| Target | Status | Notes |
|--------|--------|-------|
| Base Libraries | DONE | wwdebug, wwlib, wwmath, etc. |
| ww3d2 | DONE | |
| wwshade | DONE | |
| GameEngine | DONE | |
| GameEngineDevice | DONE | |
| GeneralsZH.exe | DONE | Launches, main menu functional |
| Shell Map Terrain | PARTIAL | Geometry renders, no textures |
| Main Menu UI | PARTIAL | All 6 buttons visible; top 2 flicker during fade-in |
| Skirmish | BROKEN | Script engine crash on map load |
| Campaign | BROKEN | Object loading failures |
| Sound | STUBBED | Phase 2 |
| Multiplayer | NOT STARTED | Phase 3 |

---

## Architecture Reference

### BGFX Rendering Layer
- `BGFXWrapper.cpp` (~2100+ lines) replaces `DX8Wrapper` via `#define DX8Wrapper BGFXWrapper`
- Uses D3D11 backend, runtime HLSL shader compilation via d3dcompiler
- View 0 = 3D terrain scene, View 1 = 2D UI overlay
- `Apply_Render_State_Changes()` is intentionally a no-op — state managed manually via TextureStageStates[] array
- TSS defaults must be set explicitly (arrays zero-init; DX8 defaults are non-zero)

### Critical DX8→BGFX Differences
| DX8 Behavior | BGFX Behavior | Fix Applied |
|---|---|---|
| ALPHAOP=SELECTARG1 ignores vertex alpha | Shader always multiplies tex×vert alpha | Alpha fix in SubmitDraw/Draw_Sorting_IB_VB |
| State machine resets between draws | RenderStates[] array persists across draws | TSS reset on 3D→2D switch in Set_View_Identity() |
| ShaderClass::Invalidate() resets pipeline | No equivalent | W3DShaderManager reset() functions restore blend states |
| ALPHAOP default = SELECTARG1 (2) | Array zero-init = DISABLE (0) | Explicit init in BGFXWrapper::Init() |

### DX8 TSS Enum Values (d3d8types.h)
```
D3DTSS_COLOROP=1, D3DTSS_COLORARG1=2, D3DTSS_COLORARG2=3
D3DTSS_ALPHAOP=4, D3DTSS_ALPHAARG1=5, D3DTSS_ALPHAARG2=6
D3DTOP_DISABLE=1, D3DTOP_SELECTARG1=2, D3DTOP_MODULATE=4
D3DTA_DIFFUSE=0, D3DTA_CURRENT=1, D3DTA_TEXTURE=2
```

### Shell Map / Main Menu Flow
1. `Shell::showShell()` pushes `Menus/MainMenu.wnd`
2. `MainMenuInit()` calls `TheShell->showShellMap(TRUE)` → sends `MSG_NEW_GAME(GAME_SHELL)`
3. `GameLogic::prepareNewGame(GAME_SHELL)` — `isInShellGame()=TRUE` → shell NOT hidden
4. `startNewGame()` first call: sets `m_startNewGame=TRUE`, returns
5. Render loop runs, MainMenu transition fires at frame ~40 (7s), 78 UI draws appear
6. `startNewGame()` second call (from update loop): loads shell map, `m_loadScreenRender=TRUE` during load
7. Load completes, `m_loadScreenRender=FALSE`, 3D terrain renders (frame ~80, ~10.7s)
8. TSS reset fires on first 2D draw after 3D, UI draws resume with correct ALPHAOP
