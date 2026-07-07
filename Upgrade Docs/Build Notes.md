# Build Notes — Generals Modern (Zero Hour)

## Progress So Far

### Build System
- [x] Full CMake build system replacing legacy `rts.dsw` / MSVC 6.0 project
- [x] Targets Win32 (x86) using VS2022 Build Tools (MSVC 19.44)
- [x] All libraries compile: wwdebug, wwlib, wwmath, wwdownload, compression, wwsaveload, wwutil, benchmark, ww3d2, wwshade, wwaudio, all GameSpy stubs, eabrowserdispatch, profile_lib, debug_lib
- [x] GameEngine.lib, GameEngineDevice.lib link successfully
- [x] GeneralsZH.exe builds and launches

### Rendering (DX8 → BGFX)
- [x] BGFXWrapper.cpp/h created as DX8-to-BGFX translation layer (untracked file, survives git reset)
- [x] All DX8/D3D8 includes replaced with BGFX stubs
- [x] BGFX initializes with D3D11 backend (feature level 12.1)
- [x] Window creation and resolution management working
- [x] Forced windowed mode with ultrawide-aware resolution scaling
- [x] Texture upload pipeline (TGA/DDS from .big archives → BGFX textures)
- [x] 2D rendering (UI, menus, buttons, text) working
- [x] Set_Shader translates ShaderClass blend/alpha/depth states to D3D render states
- [x] Alpha blending working for UI elements
- [x] CPU-side vertex transformation (TransformWVP) using WW3D column-vector convention (M*v)
- [x] CameraClass::Apply() re-enabled — sets viewport, projection, and view transforms
- [x] W3DShaderManager::init() restored — initializes 2-stage fallback shaders for terrain
- [x] Shell map terrain rendering visible (3D terrain with water waves on main menu)
- [x] Set_View_Identity / Set_World_Identity write actual identity matrices (fixes 2D z-clipping)
- [x] UI alignment corrected for ultrawide displays
- [x] Texture upload pipeline confirmed working (draw log shows 195/200 draws use REAL textures)
- [x] BGFX view separation: view 0 = 3D scene, view 1 = 2D UI overlay (prevents z-ordering issues)
- [x] Unified timestamped trace logging (bgfx_trace.log) replaces scattered per-file logs
- [x] Texture update path fixed (destroy+recreate for immutable bgfx textures)
- [x] W3DShaderManager reset() functions restore blend states after terrain passes
- [x] Flip_To_Primary no-op fix — prevented double bgfx::frame() call that was clearing UI every other frame
- [x] Texture handle cache (TexHandleCache) — prevents bgfx handle recycling collision between font and terrain
- [x] View 1 depth clear — UI overlay clears depth so 2D draws aren't rejected by terrain depth buffer
- [x] DX8 texture stage state tracking (ALPHAOP/ALPHAARG) — vertex alpha fix respects DX8 stage states
- [x] DX8 TSS defaults initialized (SELECTARG1 + TEXTURE) matching vanilla DX8 device defaults
- [ ] **STATE POLLUTION (CRITICAL)**: Terrain rendering changes TextureStageStates (ALPHAOP→MODULATE) which persists into UI draws. Since Apply_Render_State_Changes() is a no-op in BGFX, the ShaderClass never resets TSS. Fix built (TSS reset on 3D→2D switch) but NOT YET DEPLOYED/TESTED.
- [ ] **TRANSITION ANIMATION**: All 6 main menu buttons ARE visible briefly during transition animation (~0:15), then top 3 (SOLO PLAY, MULTIPLAYER, LOAD) lose text. The ScaleUp transition may be repositioning/resizing them in a way that causes re-render with wrong state.
- [ ] **TEXTURE ATLAS FLASH**: At ~0:14 during load, the entire UI texture atlas renders as a sprite sheet in the button area. Suggests Image class UV sub-rect binding issue during initial texture load.
- [ ] Full 3D world rendering (models, effects)
- [ ] Shader migration (vertex/pixel shaders to BGFX .sc format)
- [ ] Terrain camera position — shell map camera appears zoomed into water surface instead of overview. Camera waypoints load correctly but projection may need FOV/aspect fix.

### Game Functionality
- [x] Main menu loads and renders (faction emblems, buttons, text visible — needs full UI interaction testing)
- [x] Shell map loads and renders (3D terrain background visible on main menu)
- [x] Skirmish setup screen functional (player list, faction selection, map selection)
- [x] Campaign selection screen works (faction portraits render)
- [x] Campaign mission loads past INI parsing into actual gameplay (briefly — instant win/lose)
- [x] "YOU ARE DEFEATED" result screen renders with proper image and text
- [x] CD check bypassed (IsFirstCDPresent always returns TRUE)
- [x] Base Generals .big files load from ZH_Generals\ subdirectory (ZH overrides base game)
- [ ] Campaign missions play through (objects missing due to ObjectReskin failures)
- [ ] Skirmish games play through (crashes during map object loading)
- [ ] Multiplayer (not attempted)

### INI Parsing Resilience
- [x] try/catch around INI block parsing — errors skip block instead of crashing
- [x] Nested block skip logic (finds End at column 0 for top-level blocks)
- [x] ObjectReskin gracefully handles missing source template (logs warning, parses block anyway)
- [x] Object parse errors logged with file/line info
- [x] Release builds show actual crash reason in MessageBox (not generic message)
- [ ] ~235 ObjectReskin source-not-found warnings (base objects exist but reskin lookup timing issue)
- [ ] 3 Object blocks fail in campaign map.ini (ChinaJetCargoPlane, AircraftCropDuster, BaikonurRocketDetonation)

---

## How We Did It

### Build System Migration
1. Created `CMakeLists.txt` from scratch at `GeneralsMD/Code/CMakeLists.txt`
2. Configured CMake for Win32 (x86) with VS2022 generators
3. Key compiler flags: `/Zc:forScope-` (deprecated but needed), `/FS` (parallel PDB), `/permissive` (loose conformance), `NOMINMAX`, `Z_PREFIX`
4. Build order enforced: Libraries → GameEngine → GameEngineDevice → GeneralsZH.exe

### Stub/Replacement Libraries
Created stub headers for all unavailable SDKs:
- **DirectX 8**: Full COM interface stubs in `Libraries/Source/DX90SDK/Include/` (d3d8.h, d3d8types.h, etc.)
- **Miles Sound System**: Minimal Mss.H stub (AILCALLBACK defined, MILES_HANDLE NOT defined here)
- **Surrender Engine**: srVector/srMatrix stubs in WW3D2/
- **GameSpy**: Stub implementations for peer, gp, ghttp, gstats
- **Bink Video**: bink.h stub (BinkOpen returns NULL)
- **Granny Animation**: granny.h stub
- **SafeDisk**: CdaPfn.h stub
- **LZH-Light**: lzhl.h stub

### BGFX Rendering Layer
- `BGFXWrapper.cpp` (~2000+ lines) replaces `DX8Wrapper` via `#define DX8Wrapper BGFXWrapper`
- Implements: Init, shutdown, device reset, texture creation/upload, vertex/index buffers, render state management, 2D draw calls
- Uses bgfx with D3D11 backend, runtime HLSL shader compilation via d3dcompiler
- Handles the DX8→BGFX state translation: blend modes, alpha test, depth test, cull mode
- Resolution scaling for ultrawide monitors (proportional fit with 80px taskbar margin)

### Asset Loading
- ZH .big files load from exe directory (take priority)
- Base Generals .big files load from `ZH_Generals\` subdirectory (fill gaps)
- Archive file system uses first-loaded-wins for duplicate paths — ZH overrides base game naturally
- Registry-based Generals install path removed; uses local `ZH_Generals\` only

---

## Why We Had Issues

### Compiler Modernization
- **`/Zc:forScope-` deprecated**: MSVC 19.44 ignores this flag. Every for-loop that leaked its iterator variable had to be fixed manually (Player.cpp, PopupLadderSelect.cpp, FlightDeckBehavior.cpp, W3DModelDraw.cpp, etc.)
- **`AnimateWindow` collision**: Win32 API function name collides with game class method. Fixed with `#define AnimateWindow _Win32_AnimateWindow` before `<windows.h>`, `#undef` after.
- **`Matrix4` → `Matrix4x4`**: Surrender engine renamed this type. Added typedef in sr_util.h.
- **Placement `new[]` conflict**: GameMemory.h defines placement new/delete that conflicts with stdlib. Commented out the conflicting operators.
- **`DLINK_ITERATOR` pointer-to-member**: Modern MSVC requires explicit `&` for pointer-to-member in macros.
- **Missing headers**: `<fstream.h>` → `<fstream>`, removed `<mapicode.h>` and `<snmp.h>` (unavailable, unused).

### Rendering Pipeline
- **No actual DX8 runtime**: The game was written for Direct3D 8. We replaced the entire rendering backend with BGFX, which required implementing every DX8 function the game calls.
- **Set_Shader was empty**: All UI elements rendered as white rectangles because alpha blending was never enabled. Had to implement the full ShaderClass → D3D render state translation.
- **Texture upload timing**: The game uses a background thread (TextureLoader) to load textures asynchronously. Textures are created empty, then data is uploaded later via Apply_New_Surface. Many textures still appear empty.
- **Render2DClass coordinate crash**: `Set_Coordinate_Range()` was called before resolution was known, causing division by zero (±inf). Added guard.
- **Matrix multiplication convention**: WW3D Matrix4x4 uses column-vector convention (M*v) with translation in column 3 (indices [3],[7],[11]). Our initial TransformWVP used row-vector convention (v*M), producing wrong z values (-131 to -497 instead of [0,1]). Switching to M*v fixed terrain rendering.
- **CameraClass::Apply() was disabled**: Wrapped in `#if 0` with comment about porting to BGFX. The function just stores matrices in arrays — perfectly safe. Re-enabling it gave the 3D scene proper view/projection transforms.
- **W3DShaderManager::init() was skipped**: Had early `return;` bypassing all shader initialization. Terrain shader pass count was 0, so terrain drew nothing. Restoring the init loop (only 2-stage fallbacks, not DX8-device-dependent code) fixed terrain rendering.
- **Set_View_Identity was flag-only**: Only set a flag without writing identity to the transform array. SubmitDraw reads transforms directly from the array. With leftover camera view matrix (translation at [2][3]=-1), 2D UI vertices got z=-1 (outside BGFX [0,1] depth range), clipping all UI. Fixed by actually writing identity matrices.
- **Terrain vertex diffuse alpha=0x00**: DX8 fixed-function pipeline ignored this (ALPHAOP=DISABLE), but BGFX shader processes alpha. Terrain pass 0 disables alpha blending, so this is OK for opaque pass.
- **IMPORTANT — Alpha blending is a systemic DX8→BGFX issue**: In DX8's fixed-function pipeline, blend states were implicitly managed by the hardware state machine — shaders like `ShaderClass::Invalidate()` would reset the pipeline to known defaults. In BGFX, all DX8 render states persist in a flat `RenderStates[]` array. Every W3DShaderManager shader `reset()` function must explicitly restore blend states (`D3DRS_ALPHABLENDENABLE`, `D3DRS_SRCBLEND`, `D3DRS_DESTBLEND`) or they leak into subsequent draws (e.g. terrain's DESTCOLOR/ZERO blend corrupts UI's SRCALPHA/INVSRCALPHA). This is NOT a one-off fix — every multi-pass shader in the engine has this problem. The `reset()` functions were originally just "clean up textures" but now must also clean up ALL modified render states.

### INI Parsing
- **ObjectReskin blocks crash**: The original code throws `INI_INVALID_DATA` when the source template isn't found. In our build, load order from .big archives causes some source templates to not exist when referenced. Fixed by catching the exception and parsing the block anyway.
- **Nested block skip**: When an Object block throws mid-parse, the file position is corrupt. Simple "skip to End" logic hit sub-block End tokens. Fixed by checking for End at column 0 only (top-level End is unindented).
- **Missing base game assets**: ZH .big files override base game files with the same path. If INIZH.big contains `factionbuilding.ini`, it replaces the base game version entirely. The ZH version may only have ObjectReskin additions, not the original Object definitions.
- **Double loading**: Some INI files are loaded twice (initial load + override pass). The second pass can't find templates from the first pass if the factory was reset between loads.

### Asset Dependencies
- **No `INI.big` initially**: Only ZH .big files were present. All base game objects (GLAHole, GenericOptTree, Bush01, etc.) were missing, causing 238+ ObjectReskin failures and campaign instant-win/lose.
- **Registry path wrong**: `GetStringFromGeneralsRegistry` returned the ZH install root, not the base game subfolder. Loading `*.big` from there duplicated all ZH files as "base game" files, causing 1569 INI errors and crashes.

---

## What Remains

### Critical (Game Unplayable Without)
- [x] Terrain texture loading works (draw log confirms REAL texture bindings — needs screenshot verification)
- [x] Terrain/UI depth separation via BGFX views (needs screenshot verification with clean menu)
- [ ] UI stability: menu buttons disappear when shell map terrain begins rendering
- [ ] Fix skirmish crash during map object loading (crashes between TerrainLogic->newMap and preloadAssets)
- [ ] Fix ObjectReskin template lookup failure (objects created but not found in hashmap immediately after)
- [ ] Fix campaign Object block parse failures (ChinaJetCargoPlane, AircraftCropDuster, BaikonurRocketDetonation)
- [ ] Implement 3D model rendering (units, buildings, trees)

### Important (Playable But Broken Without)
- [ ] Fix loading screen (shows texture atlas instead of proper background)
- [ ] Implement vertex/pixel shader support in BGFX (migrate .vsh/.psh to .sc format)
- [ ] Sound system (currently stubbed — OpenAL Soft migration per Phase 2)

### Polish
- [ ] True ultrawide rendering (currently scales 4:3 to fit, doesn't expand viewport)
- [ ] Blue ruler/grid border visible around game window edges
- [ ] Fullscreen mode support (currently forced windowed)
- [ ] Resolution selection in options menu
- [ ] Remove all diagnostic logging (bgfx_loading.log)
- [ ] Remove hardcoded file paths in logging

### Future Phases
- [ ] Phase 2: Sound (Miles → OpenAL Soft)
- [ ] Phase 3: Networking (GameSpy replacement)
- [ ] Custom music, sounds, movies, models
- [ ] Mod support improvements
