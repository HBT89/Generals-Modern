# Generals-Modern BGFX Migration — Dev Log

## Project Goal
Migrate Command & Conquer Generals Zero Hour rendering from DirectX 8 to BGFX (cross-platform).

---

## 2026-02-21 — Session 3: ww3d2 Builds Clean, wwshade In Progress

### Milestones
- **ww3d2.lib compiles with 0 errors** (down from ~1,557 at session start across multiple rounds)
- **wwshade down to 36 errors** (from 183)

### Major Problems Solved

#### 1. DynamicMeshModel was abstract (W3DMPO_GLUE macro removed)
- **Problem**: Previous BGFX port incorrectly removed `W3DMPO_GLUE(DynamicMeshModel)` macro, making the class abstract due to unimplemented `glueEnforcer()` pure virtual from W3DMPO base.
- **Solution**: Restored the macro in `dynamesh.h`.
- **Lesson**: W3DMPO_GLUE is required by ALL W3D classes inheriting from W3DMPO. Never remove it.

#### 2. shattersystem.cpp needed 14 missing DynamicMeshClass methods
- **Problem**: Methods like Set_Shader(shader, pass), Begin_Tri_Fan(), Location_Inline(), etc. were missing.
- **Solution**: Added all as inline stubs/forwarders in `dynamesh.h`.

#### 3. D3DCMPFUNC enum anonymous → named typedef
- **Problem**: `shader.cpp` referenced `D3DCMPFUNC` as a type but it was an anonymous enum.
- **Solution**: Changed to `typedef enum _D3DCMPFUNC { ... } D3DCMPFUNC;` in `RenderTypes.h`.

#### 4. Matrix4 undeclared in sortingrenderer.cpp
- **Problem**: 10 errors from Matrix4 not being in scope.
- **Solution**: Added `#include "sr_util.h"` (which has `typedef Matrix4x4 Matrix4`).

#### 5. IDirect3DIndexBuffer8 unknown type
- **Problem**: `dx8indexbuffer.h` didn't include the header defining the stub type.
- **Solution**: Added `#include "RenderTypes.h"`.

#### 6. bgfx::submit() wrong argument count
- **Problem**: `bgfx::submit(0)` needs a program handle.
- **Solution**: Changed to `bgfx::submit(0, BGFX_INVALID_HANDLE)`.

#### 7. D3DX math/shader types for wwshade
- **Problem**: wwshade needs D3DVSD_STREAM, D3DXVECTOR3/4, D3DXMATRIX, ID3DXBuffer, shader creation methods.
- **Solution**: Added all to RenderTypes.h with proper guards.

### Files Modified This Session
| File | Change |
|------|--------|
| `WW3D2/dynamesh.h` | Restored W3DMPO_GLUE, added 14 methods to DynamicMeshClass |
| `WW3D2/dynamesh.cpp` | Fixed bgfx::submit, MAX_PASSES qualifiers, Get_Num_Vertices→VertCount |
| `WW3D2/RenderTypes.h` | D3DCMPFUNC typedef, IDirect3DDevice8 stubs, D3DX math types, shader decl macros, ID3DXBuffer |
| `WW3D2/BGFXWrapper.h` | Added PROJECTED=256 to texture transform enum |
| `WW3D2/dx8indexbuffer.h` | Added #include "RenderTypes.h" |
| `WW3D2/sortingrenderer.cpp` | Added sr_util.h and ww3d.h includes |
| `WW3D2/shader.cpp` | Added ww3d.h include |
| `WW3D2/ww3dformat.cpp` | Added ww3d.h include |
| `WW3D2/render2dsentence.cpp` | Added ww3d.h, fixed uint16 casts for max() |
| `WW3D2/textdraw.h` | Vector3→Vector4 conversion fix |
| `WW3D2/font3d.h` | Added Peek_Texture(char) overload |
| `WW3D2/rendobj.cpp` | const_cast for strchr |
| `WW3D2/lightenvironment.cpp` | Added float type specifier |
| `WW3D2/meshmdlio.cpp` | Fixed clload→cload typo |
| `WW3D2/missingtexture.cpp` | Commented out D3dx8core.h (excluded from build) |
| `WW3D2/pointgr.cpp` | Replaced D3DXMatrixRotationZ with inline math |
| `WW3D2/formconv.cpp` | RGB565→R5G6B5 (correct bgfx name) |
| `WW3D2/assetmgr.cpp` | const_cast for strchr (2 places) |
| `WW3D2/hanimmgr.cpp` | const_cast for strchr |
| `WW3D2/hmorphanim.cpp` | const_cast for strstr (2 places) |
| `CMakeLists.txt` | Added missingtexture.cpp to exclusion list |

### Current Blocker: wwshade D3DXVECTOR4 conversion
- **36 errors** all the same: `D3DXVECTOR4` cannot convert to `const void*`
- Root cause: RenderTypes.h defines `_D3DX8MATH_H_` guard at line 15, so the D3DXVECTOR4 with `operator const void*()` in RenderTypes.h is never compiled. The DX90SDK stub version (without the operator) is used instead.
- **Fix needed**: Add `operator const void*() const { return this; }` to DX90SDK stub's D3DXVECTOR4, or restructure the guard.

---

## 2026-02-20 — Session 2: Major ww3d2 Error Reduction

### Milestones
- ww3d2 errors reduced from ~1,004 to ~93
- Created BGFXWrapper, BGFXVertexBuffer, BGFXIndexBuffer
- Created all BGFX shader .sc files
- Emptied/replaced dx8wrapper, dx8caps with BGFX equivalents
- Restored dx8fvf.h vertex format structs from .OLD backup

### Key Architecture Decisions
- RenderTypes.h is the single source of truth for all D3D8 placeholder types
- Guard macros block DX90SDK stub headers: `_D3D8TYPES_H_`, `_D3D8CAPS_H_`, `_D3D8_H_`, `_D3DX8MATH_H_`, `_D3DX8CORE_H_`, `_D3DX8TEX_H_`
- `#define DX8Wrapper BGFXWrapper` for drop-in replacement
- formconv.h BGFX conversion functions active, D3D ones in `#if 0`

---

## 2026-02-19 — Session 1: Project Setup & Library Builds

### Milestones
- CMake build system created (CMakeLists.txt)
- All base libraries built: wwdebug, wwlib, wwmath, wwdownload, compression, wwsaveload, wwutil, benchmark, GameSpy stubs, eabrowserdispatch, profile_lib, debug_lib
- Identified and documented all build fixes needed for modern MSVC

### Key Fixes Established
- `AnimateWindow` Win32 collision → #define redirect
- `Matrix4` → `Matrix4x4` rename + typedef
- `MILES_HANDLE` = unsigned long
- DLINK_ITERATOR needs `&` for pointer-to-member
- Placement new[] conflict in GameMemory.h
- PreRTS.h: `<fstream.h>` → `<fstream>`, remove `<mapicode.h>` and `<snmp.h>`

---

## Build Status Summary

| Target | Status | Errors |
|--------|--------|--------|
| Base Libraries | DONE | 0 |
| ww3d2 | DONE | 0 |
| wwshade | IN PROGRESS | 36 |
| GameEngine | NOT STARTED | — |
| GameEngineDevice | NOT STARTED | — |
| GeneralsZH.exe | NOT STARTED | — |
