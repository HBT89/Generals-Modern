# Phase 1 – Modernization Layer (Updated)

## 🎯 Goal
Modernize Command & Conquer: Generals Zero Hour for modern hardware by:
- Enabling high-resolution (4K+) rendering
- Fixing UI scaling and layout
- Improving camera and input responsiveness
- Preparing the engine for new UI elements and larger map mechanics
- Migrating the rendering backend from Direct3D 8 (DX8) to BGFX for cross-platform, modern graphics support
See the [[Generals-Modern-main/README|README]] for basic information
---

## ✅ Completed So Far

### Rendering Backend Modernization
- Analyzed and mapped all DX8/D3D8 usages in the codebase, especially in WW3D2 and related folders
- Created `BGFXWrapper.h` and `BGFXWrapper.cpp` to provide a BGFX-based interface mimicking the old DX8Wrapper
- Updated `ww3d.cpp` and other key files to use `BGFXWrapper` instead of `DX8Wrapper` for device, window, and resolution management
- Removed all Direct3D 8 includes (`<d3d8.h>`, `<d3d8types.h>`, etc.) from headers and replaced D3D8 types with BGFX-agnostic placeholders
- Marked all major DX8 header files as obsolete, wrapped their DX8 code in `#if 0` blocks, and added TODOs for BGFX porting
- Commented out or removed all D3D8 includes and references in key source files, replacing them with BGFX stubs and TODOs
- Replaced all `#include "dx8wrapper.h"` with `#include "BGFXWrapper.h"` in all relevant files
- Added TODO comments and stubs in all affected files to guide future BGFX implementation
- Used file and text search tools to systematically identify and process all DX8-related code
- For each `.cpp` file processed, commented out all DX8/D3D includes and logic, added `// TODO: BGFX PORT` comments, and ensured `BGFXWrapper.h` is included
- In files like `dazzle.cpp`, `ww3dformat.cpp`, and `texture.cpp`, commented out all DX8Wrapper usages and replaced them with TODOs and stubs for BGFX
- Ensured that all TODOs are clearly marked for future BGFX implementation
- No compile errors in processed files after DX8 removal and BGFX stubbing

#### Shader and Pipeline Modernization
- Inventoried all legacy and new shader files, including `.vsh`, `.psh`, `.sc`, and C++ pipeline files
- Migrated and refactored all major shaders (bump diffuse, bump specular, cubemap, basic/simple) to BGFX-compatible `.sc` format, ensuring feature parity with DX8 originals
- Standardized all BGFX shaders to use valid GLSL syntax, explicit mat3 construction, and BGFX best practices (no HLSL types or matrix subscripts)
- Cleaned up and deduplicated `varying.def.sc` to ensure all attributes and varyings match shader usage
- Fixed shader compilation errors by correcting attribute declarations, `$input` lines, and matrix math
- Updated and documented the migration process in `reshade-automate.md` and `reshade-inventory.md`
- Provided detailed explainers for each C++ shader pipeline and utility class, including code reviews and migration notes
- Compared and explained differences between legacy and modernized shaders (e.g., self-shadowed vs. regular bump/specular)
- Ensured all migrated shaders and pipelines compile and function cross-platform with BGFX

### High-Resolution and UI Modernization
- ~~Set MaxCameraHeight = 600+ in GameData.ini~~
- ~~Set EnforceMaxCameraHeight = No~~
- Identified all INI and engine source files related to resolution, UI scaling, and camera
- Documented all required changes for UI and HUD scaling, camera/input tuning, and resolution support

---

## 🧱 Core Feature Set

### 1. High-Resolution Rendering Support
- Support for 1440p, 4K, ultrawide
- Proper world scaling and camera behavior
- Native support via source code (no wrapper required)
- BGFX backend for modern GPU support

### 2. UI and HUD Scaling
- Adjust ControlBarScheme.ini for widescreen alignment
- Adjust ControlBarResizer.ini for HUD repositioning
- Support alternate screen layouts and minimap positions

### 3. Camera and Input Tuning
- Tune scroll cutoff and responsiveness
- Prepare for further camera and input improvements

### 4. Controls and Hotkeys
- Investigate mouse remap support (e.g., left-click for move)
- Support Shift + Arrows for fast camera movement
- Expand mouse zone limits for ultrawide setups

---

## 🛠 Implementation Tasks

| Task | File | Type |
|------|------|------|
| Add support for native resolution detection | Display.cpp | Engine source |
| Add selectable resolutions to UI | ShellMenu / OptionsMenu | Engine source |
| Fix UI stretching | ControlBarScheme.ini | INI |
| Rescale HUD regions | ControlBarResizer.ini | INI |
| Increase zoom and camera height | GameData.ini | INI |
| Validate fullscreen/windowed modes | Display::setDisplayMode | Engine source |
| **Remove all DX8/D3D8 code, headers, and logic** | WW3D2/* | Engine source |
| **Implement BGFX device/context management** | BGFXWrapper.cpp | Engine source |
| **Replace all buffer, render state, and draw calls with BGFX equivalents** | WW3D2/* | Engine source |
| **Migrate all texture and shader management to BGFX** | WW3D2/* | Engine source |
| **Update all format conversion logic** | formconv.h, ww3dformat.cpp | Engine source |
| **Test and debug BGFX-based rendering path** | All | Engine source |
| **Update documentation and developer notes** | Upgrade Docs/* | Docs |

---

## 🚧 General TODO Plan (BGFX Migration)

1. **Device/Context Management**
   - Implement BGFX initialization, shutdown, and reset logic in `BGFXWrapper`
   - Replace all device creation, loss, and reset handling with BGFX lifecycle management

2. **Buffer and Render State Management**
   - Replace all vertex/index buffer creation and usage with BGFX buffer APIs
   - Map DX8 render states to BGFX state flags and pipeline state objects
   - Remove all DX8-specific buffer locking/unlocking and replace with BGFX update/submit logic

3. **Texture and Shader Management**
   - Replace all texture creation, loading, and binding with BGFX texture APIs
   - Migrate shader loading, compilation, and binding to BGFX shader system
   - Update all sampler and texture state logic to BGFX

4. **Rendering Logic**
   - Replace all draw calls (Draw_Triangles, Draw_Primitives, etc) with BGFX submit calls
   - Update all render pass and frame management to BGFX's view/encoder system
   - Ensure all render target and viewport logic is BGFX-compatible

5. **Format and Type Conversion**
   - Replace all D3DFORMAT, D3D types, and conversion functions with BGFX or project-native equivalents
   - Update all color, matrix, and vector math to be compatible with BGFX

6. **Testing and Integration**
   - Test each subsystem (device, buffers, textures, shaders, rendering) in isolation
   - Integrate subsystems and test full rendering pipeline
   - Remove all remaining DX8/D3D code and obsolete TODOs
Currently in the process of refactoring [[wwshader2bgfx]]

---

## ✅ Completion Criteria

- Game launches and renders correctly at 2560×1440 or higher
- HUD and sidebar are correctly aligned and visible
- Camera can zoom further and pan smoothly
- User-selectable resolutions are available in options menu
- No crash on startup or resolution switch
- All rendering and resource management is handled by BGFX (no DX8 code remains)

---

## 🔒 Future Enhancements

- Add config UI for advanced graphics settings
- Implement dynamic UI scaling for DPI or resolution shifts
- Add in-game toggle for layout modes (left/right bar)
- Expand BGFX backend for advanced features (shadows, post-processing, etc)
