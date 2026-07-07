# BGFXMASTER — Shader Development Master Plan
*Sub-page of Phase 1 – Graphics.md*
*Last updated: 2026-03-13*

---

## Current Status

| Phase | Status | Summary |
|-------|--------|---------|
| Phase A — Terrain texture fix | ✅ VERIFIED | Terrain renders with textures — confirmed 2026-03-13 |
| Phase A — Remaining shell map bugs | IN PROGRESS | See bug list below |
| Phase B — GPU lighting | NOT STARTED | |
| Phase C — Advanced features | NOT STARTED | |
| Phase D — RT/PBR prep | NOT STARTED | |

---

## Rendering System Status

| System | Status | Notes |
|--------|--------|-------|
| Shell map terrain geometry | ✅ WORKING | Terrain renders with textures + shading |
| Main menu UI — bottom buttons | PARTIAL | LOAD/OPTIONS/CREDITS/EXIT GAME render; top 2 (CAMPAIGN/SKIRMISH) lose text |
| Main menu UI — debug grid | BUG | Blue ruler border around entire viewport |
| Main menu UI — terrain seam | BUG | Flat triangle artifact lower-left of terrain |
| Skirmish in-game | BROKEN | Crashes on launch (TheScriptEngine exception) |
| 3D unit models | NOT STARTED | No lit shader hooked up |
| Water | NOT STARTED | Shader not connected |
| Shroud / Fog of war | NOT STARTED | |
| Particle effects | NOT STARTED | Alpha modes not wired |
| Sound | STUBBED | Phase 2 |

---

## Phase A — Terrain Texture Fix ✅ COMPLETE

### Root Cause (confirmed)

Two distinct failure paths caused `BGFXWrapper::Textures[0] == NULL` at `SubmitDraw` time:

1. **`Is_Texturing_Enabled()` guard** — `TextureClass::Apply()` explicitly calls `Set_DX8_Texture(stage, NULL)` when texturing is globally disabled, zeroing the slot even if a valid texture was passed in.

2. **Early-return in `setShader()`** — Subsequent terrain tiles call `setShader(ST_TERRAIN_BASE, 0)` with the same shader+pass, hitting the early-return at line 2789 and skipping the mirror loop entirely. Per-tile texture changes via `setShaderTexture()` are lost.

### Fix Applied — `W3DShaderManager.cpp::setShader()`

Moved the texture mirror loop before the early-return and replaced `Set_Texture()` (routes through `Apply()`) with `Peek_D3D_Base_Texture()` + `Set_DX8_Texture()` (bypasses the `Is_Texturing_Enabled()` guard).

```cpp
Int W3DShaderManager::setShader(ShaderTypes shader, Int pass)
{
    // BGFX: Mirror BEFORE early-return — fixes both failure modes.
    // Peek_D3D_Base_Texture() + Set_DX8_Texture() bypasses Is_Texturing_Enabled guard.
    for (int i = 0; i < 8; i++) {
        if (!m_Textures[i]) continue;
        IDirect3DBaseTexture8* d3dTex = m_Textures[i]->Peek_D3D_Base_Texture();
        if (d3dTex)
            DX8Wrapper::Set_DX8_Texture(i, d3dTex);   // direct: bypasses Apply()
        else
            DX8Wrapper::Set_Texture(i, m_Textures[i]); // fallback: triggers lazy Init/Upload
    }

    if (shader == m_currentShader && pass == m_currentShaderPass)
        return TRUE;
    // ... rest unchanged
```

**Why no second loop after `set()`:** Confirmed via code inspection that terrain shader `set()` implementations (`TerrainShader2Stage`, `FlatTerrainShader2Stage`, `TerrainShaderPixelShader`) do NOT call `Set_Texture(0, NULL)`. The `Set_Texture(NULL)` calls at lines 248/426/648/868 are in screen filter classes (`ScreenDefaultFilter`, `ScreenBWFilter`, etc.) — completely different code path. The `reset()` method uses `_Get_D3D_Device8()->SetTexture()` which is a stub no-op. One pre-early-return loop is sufficient.

### All Phase A Changes

| File | Change |
|------|--------|
| `wwshade2bgfx/vs_terrain.sc` | CREATED — pass-through VS with dual UV sets, BGRA→RGBA swizzle |
| `wwshade2bgfx/fs_terrain.sc` | CREATED — single texture × vertex color (Phase B stubs commented) |
| `wwshade2bgfx/varying.def.sc` | MODIFIED — added `a_texcoord1`, `v_texcoord1`, `v_normal`, `v_worldPos` |
| `WW3D2/BGFXWrapper.h` | MODIFIED — `ShaderProgramType` enum, `Set_Active_Shader_Type()`, light uniform handles |
| `WW3D2/BGFXWrapper.cpp` | MODIFIED — program registry `s_programs[SP_COUNT]`, terrain program compiled in `Init()`, `SubmitDraw()` dispatches by `s_activeProgram`; fixed `SP_MESH_DEFAULT` namespace qualifier |
| `W3DShaderManager.cpp` | MODIFIED — shader type → program dispatch switch; texture mirror loop (pre-early-return, direct `Peek_D3D_Base_Texture()` path); `TerrainShader2Stage::set()` pass 0/1 use `DX8Wrapper::Set_Texture()` |
| `Upgrade Docs/BGFXMASTER.md` | This document |

### Phase A Verification — ✅ PASSED (2026-03-13)

- Terrain renders sandy desert with texture colors, blending, and shading — confirmed visually
- LOAD / OPTIONS / CREDITS / EXIT GAME buttons visible with text

### Remaining Shell Map Bugs (Priority Order)

**Bug 1 — Blue debug grid border** (viewport artifact)
- Blue ruler/grid outline around entire rendering area
- Likely: `bgfx::setViewRect` or scissor rect set to debug dimensions; or `BGFX_DEBUG_WIREFRAME` flag left on

**Bug 2 — Top 2 menu buttons lose text** (UI texture/font issue)
- CAMPAIGN and SKIRMISH buttons go gray after initial animation
- Known from memory: "top 3 lose text → terrain loads → only EXIT GAME survives"
- Likely: font glyph texture being evicted/recycled by terrain texture uploads (TexHandleCache collision, or atlas re-upload overwriting glyph data)

**Bug 3 — Terrain triangle seam** (geometry/LOD artifact)
- Flat triangle visible lower-left of terrain
- Likely: terrain LOD/stitching vertex that doesn't get a texture UV assigned

**Bug 4 — Skirmish crash** (TheScriptEngine::UPDATE exception on map load)
- Separate from rendering — scripting/game logic issue

---

## Phase B — GPU Lighting (NEXT)

**Goal:** Remove CPU `doTheDynamicLight()` loop from `HeightMap.cpp`; move lighting to GPU uniforms.

### Files to Modify

- `BGFXWrapper.cpp` — Wire `Set_Light()` to upload `u_lights[]` uniform buffer (currently stores but doesn't upload)
- `HeightMap.cpp` — Delete `doTheDynamicLight()` CPU loop (lines 175–261); terrain lit per-fragment
- `BGFXWrapper.cpp` — Remove CPU `TransformWVP()` for terrain draws; move to VS (`u_modelViewProj`)

### Files to Create

- `wwshade2bgfx/vs_lit.sc` — generic lit VS with full WVP (replaces `vs_basic.sc` for models)
- `wwshade2bgfx/fs_lit.sc` — 8-light loop, attenuation, Blinn-Phong specular

### Uniform Layout

```
u_lights[8][4] — 8 lights × 4 vec4:
  [0] type(0=dir,1=point,2=spot), enabled, pad, pad
  [1] position.xyz, range
  [2] direction.xyz, inner_cone
  [3] color.rgb, intensity
```

### Reuse

Dirty-flag tracking pattern at `BGFXWrapper.cpp` line 981–1014 — clone for `LIGHTS_CHANGED` → uniform upload trigger.

### Verification

- Frame time drops measurably (CPU no longer touches terrain vertices per frame)
- Dynamic lights from GLA tunnels/napalm update without CPU vertex re-upload

---

## Phase C — Advanced Features (DX8-impossible)

**Goal:** Features unlocked by BGFX that were impossible under DX8.

#### Per-Unit Lighting
- `u_unitLightOverride` vec4 — set per draw call via `bgfx::setUniform()` before each unit submit
- Terrain ignores it (stays 0), units read it for faction glow / damage tint

#### Multi-Spectral Light Casting
- Add `vec4 u_lightSpectral` per light — separate R/G/B attenuation (colored shadows, laser tints)

#### Advanced Alpha Modes
- `u_alphaMode` uniform replaces hardcoded blend state in `SubmitDraw()`
- Modes: 0=opaque, 1=alpha-test, 2=alpha-blend, 3=additive, 4=premultiplied
- Fixes black fringe on smoke/particles

#### High-Resolution / Ultrawide
- `u_resolution` vec4 — passed every frame, enables aspect-correct UV tiling

**New shader files:**
- `vs_unit.sc` / `fs_unit.sc` — unit shader with per-unit light override + specular
- `fs_terrain_adv.sc` — 4-layer terrain blend (road, dirt, grass, rock)

Not planned in detail until Phase B is verified.

---

## Phase D — Ray Tracing / PBR Prep

**Goal:** Architecture that doesn't block adding RT or PBR later.

- All normals output in **world space** — RT requirement
- G-Buffer layout defined as struct: `gPosition | gNormal | gAlbedo | gMetallicRoughness`
- `s_shadowMap` bound to 1×1 white stub in all shaders
- `bgfx::getCaps()->supported` check for RT extension — fails gracefully to raster
- Probe slots reserved: `s_irradianceProbe`, `s_prefilteredEnv` — stub samplers return `vec4(1,1,1,1)`

Not planned in detail until Phase C is verified.

---

## Architecture Reference

### View System (keep as-is)
```
View 0 — 3D scene   — depth test ON,  depth write ON
View 1 — 2D UI      — depth test OFF, depth write OFF
```

### Shader Program Registry
```cpp
enum ShaderProgramType {
    SP_MESH_DEFAULT = 0,  // generic fallback
    SP_TERRAIN_BASE,      // base terrain texture + directional light
    SP_TERRAIN_NOISE1,    // base + 1 noise/cloud overlay
    SP_TERRAIN_NOISE2,    // base + 2nd noise layer
    SP_TERRAIN_NOISE12,   // base + both noise layers
    SP_SHROUD,            // shroud projection
    SP_ROAD,              // road surface
    SP_UNIT_LIT,          // generic lit unit (Phase B)
    SP_UNIT_BUMP,         // bump-mapped unit (Phase B)
    SP_WATER,             // water surface (Phase B)
    SP_PARTICLE,          // particles / effects (Phase C)
    SP_UI,                // 2D UI elements
    SP_COUNT
};
```

### Reuse Notes

- `bgfx::setUniform()` pattern: established for `s_uTexTransform[]` (`BGFXWrapper.cpp` ~line 856) — clone for light uniforms
- Dirty-flag tracking (`BGFXWrapper.cpp` lines 981–1014) — reuse for `LIGHTS_CHANGED` → uniform upload
- `UploadSurface()` lazy texture upload — keep as-is
- `varying.def.sc` — extend in-place, don't replace (bump shaders still work)
- `vs_bumpdiff.sc` tangent-space transform — reference for world-space normal pass-through
