# Game Load & Render Sequence — Generals-Modern BGFX Port

Updated: 2026-03-05

---

## Phase 1: GameEngine::init() — Subsystem Load Order
`GameEngine/Source/Common/GameEngine.cpp:261`

Called as: `WinMain` → `GameMain` → `TheGameEngine->init(argc, argv)`

| # | Subsystem | Notes |
|---|-----------|-------|
| 1 | TheLocalFileSystem | Local disk access |
| 2 | TheArchiveFileSystem | .big archive loading (after local FS) |
| 3 | TheWritableGlobalData | GameData.ini |
| 4 | TheGameText | String tables |
| 5 | TheScienceStore | Tech tree |
| 6 | TheMultiplayerSettings | MP config |
| 7 | TheTerrainTypes | Terrain INI |
| 8 | TheTerrainRoads | Roads INI |
| 9 | TheGlobalLanguageData | Language |
| 10 | TheCDManager | Asset/CD management |
| 11 | **TheAudio** | Miles Sound System (currently skip-init) |
| 12 | TheFunctionLexicon | createFunctionLexicon() |
| 13 | TheModuleFactory | createModuleFactory() |
| 14 | TheMessageStream | createMessageStream() |
| 15 | TheSidesList | Factions |
| 16 | TheCaveSystem | Cave logic |
| 17 | TheRankInfoStore | Rank.ini |
| 18 | ThePlayerTemplateStore | PlayerTemplate.ini |
| 19 | TheParticleSystemManager | Particles |
| 20 | TheFXListStore | FXList.ini |
| 21 | TheWeaponStore | Weapon.ini |
| 22 | TheObjectCreationListStore | ObjectCreationList.ini |
| 23 | TheLocomotorStore | Locomotor.ini |
| 24 | TheSpecialPowerStore | SpecialPower.ini |
| 25 | TheDamageFXStore | DamageFX.ini |
| 26 | TheArmorStore | Armor.ini |
| 27 | TheBuildAssistant | Build system |
| 28 | TheThingFactory | Object.ini (large) |
| 29 | TheUpgradeCenter | Upgrade.ini |
| **30** | **TheGameClient** | **★ Triggers W3DDisplay::init() → WW3D::Init() → BGFXWrapper::Init()** |
| 31 | TheAI | AIData.ini |
| 32 | TheGameLogic | createGameLogic() |
| 33 | TheTeamFactory | |
| 34 | TheCrateSystem | Crate.ini |
| 35 | ThePlayerList | |
| 36 | TheRecorder | Replay |
| 37 | TheRadar | |
| 38 | TheVictoryConditions | |
| 39+ | TheMetaMap, TheActionManager, TheGameState… | Remaining systems |

After all subsystems: `TheSubsystemList->postProcessLoadAll()`

---

## Phase 2: W3DDisplay::init() — Renderer Setup
`GameEngineDevice/Source/W3DDevice/GameClient/W3DDisplay.cpp:~655`

Triggered by TheGameClient init (subsystem #30 above).

```
Display::init()                          base class
TheW3DFileSystem = NEW W3DFileSystem
WWMath::Init()
Create RTS3DInterfaceScene, RTS2DScene, RTS3DScene
Create global lights
m_assetManager = NEW W3DAssetManager
★ WW3D::Init(ApplicationHWnd)           → ww3d.cpp:268
    BGFXWrapper::Init(hwnd, false)       → BGFXWrapper.cpp:161
        bgfx::renderFrame()              single-threaded mode
        init.platformData.nwh = hwnd     ★ must be on Init struct, NOT setPlatformData
        bgfx::init(init)                 D3D11, SUCCEEDED at feature level 12.1
        BGFXWrapper::Do_Onetime_Device_Dependent_Inits()
            MissingTexture::_Init()
            TextureFilterClass::_Init_Filters()
            TheDX8MeshRenderer.Init()
            BoxRenderObjClass::Init()
            VertexMaterialClass::Init()
            PointGroupClass::_Init()
            ShatterSystem::Init()
            TextureLoader::Init()
WW3D::Set_Prelit_Mode / Enable_Static_Sort_Lists / etc.
WW3D::Set_Render_Device(0, width, height, bits, windowed)
    BGFXWrapper::Set_Render_Device()     stores resolution, calls bgfx::reset()
GameLOD config
initAssets(), init2DScene(), init3DScene()
W3DShaderManager::init()
W3DDebugDisplay init
```

---

## Phase 3: GameEngine::execute() — Main Loop
`GameEngine/Source/Common/GameEngine.cpp:836`

```
while (!m_quitting) {
    GameEngine::update()                 per-frame
        TheRadar->UPDATE()
        TheAudio->UPDATE()
        TheGameClient->UPDATE()          ★ drives rendering (see Phase 4)
        TheMessageStream->propagateMessages()
        TheNetwork->UPDATE()
        TheGameLogic->UPDATE()
    // frame rate limiting (Sleep/spin)
}
delete TheGameEngine
```

---

## Phase 4: W3DDisplay::draw() — Per-Frame Render
`GameEngineDevice/Source/W3DDevice/GameClient/W3DDisplay.cpp:1690`

Called each frame via TheGameClient->UPDATE()

```
[skip if minimized]
updateAverageFPS()
dynamic LOD / terrain LOD

★ Device check (line 1870):
  DX8Wrapper::_Get_D3D_Device8()          returns &s_stubDevice (BGFXWrapper.h)
  ->TestCooperativeLevel()                 STUB returns D3D_OK ✓ render loop runs

updateViews()                             camera math
TheParticleSystemManager->update()
TheWaterRenderObj->updateRenderTargetTextures()   [if water type 2]
TheW3DProjectedShadowManager->updateRenderTargetTextures()

★ WW3D::Begin_Render(clear=true, clearZ=true, black, waterOpacity)  [line 1909]
    BGFXWrapper::Clear(color, z+stencil, Vector3(0,0,0), opacity, 1.0f, 0)
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR|DEPTH|STENCIL, 0x000000ff, 1.0, 0)
    BGFXWrapper::Begin_Scene()
        bgfx::setViewRect(0, 0, 0, width, height)
        bgfx::touch(0)

if loadScreenRender:
    TheInGameUI->draw()  TheMouse->draw()  WW3D::End_Render()  [continue]

drawViews()                               3D scene — all draw calls STUB (no-op)
TheInGameUI->DRAW()                       2D UI   — all draw calls STUB (no-op)
TheMouse->DRAW()                          cursor  — STUB
[video buffer draw if active]
[letterbox]
[debug display]

WW3D::End_Render()
    BGFXWrapper::End_Scene(flip=true)
        bgfx::frame()                     ★ REAL — presents frame to D3D11
        FrameCount++
```

---

## Phase 5: What's REAL vs STUB Right Now

### REAL (actually calls bgfx::)
| Function | bgfx call |
|----------|-----------|
| BGFXWrapper::Init() | bgfx::init() with D3D11, platformData.nwh=hwnd |
| Begin_Scene() | setViewRect(0,...), touch(0) |
| Clear() | setViewClear(0, flags, rgba, z, stencil) |
| End_Scene(true) | bgfx::frame() |
| Flip_To_Primary() | bgfx::frame() |
| Set_Viewport() | setViewRect(0, x, y, w, h) |
| Set_Render_Device() | bgfx::reset(w, h, flags) |

### STUB (no-op — black screen)
| Area | What's missing |
|------|----------------|
| Geometry draw calls | Draw_Triangles, Draw_Strip — submits nothing to bgfx |
| Textures | CreateTexture allocates BGFXTexture8 but never calls bgfx::createTexture2D |
| Shaders | No bgfx program created; nothing to draw with |
| Render states | SetRenderState, SetTextureStageState — all no-op |
| 2D UI rendering | Render2DClass uses DX8 quads → STUB |
| Material/shader setup | Set_DX8_Material, Set_Shader → no-op |

### What the Screen Shows
- Black background: bgfx IS clearing (BGFX_CLEAR_COLOR to 0x000000ff)
- No geometry: all draw calls are stubs — nothing submitted to bgfx
- Result: solid black window, correct behavior given current state

---

## Next Steps to Get Pixels
To go from black to rendering ANYTHING:

1. **2D colored quad** (simplest): Implement `BGFXWrapper::Draw_Triangles` for 2D using a bgfx transient vertex buffer + minimal flat-color shader → UI would render as solid-color shapes
2. **Texture sampling**: Make BGFXTexture8::Lock/Unlock upload pixel data to a real bgfx TextureHandle via bgfx::updateTexture2D
3. **Full pipeline**: wire shader programs (vs/fs compiled from bgfx shaders) into Set_Shader calls

The render pipeline infrastructure (begin/clear/frame) is working. The gap is geometry submission.
