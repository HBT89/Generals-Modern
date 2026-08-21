// BGFXWrapper.cpp
// Implementation of BGFXWrapper for WW3D replacement
#include "BGFXWrapper.h"
#include "matrix4.h"
#include "matrix3d.h"
#include "dx8caps.h"
#include "rddesc.h"
#include "formconv.h"
#include "missingtexture.h"
#include "texturefilter.h"
#include "dx8renderer.h"
#include "dx8vertexbuffer.h"
#include "dx8indexbuffer.h"
#include "ww3d.h"
#include "vertmaterial.h"
#include "boxrobj.h"
#include "pointgr.h"
#include "shattersystem.h"
#include "textureloader.h"
#include "surfaceclass.h"
#include "sortingrenderer.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <d3dcompiler.h>
#include <string>
#include <vector>
#include <cstdarg>
#include <cmath>
#include "render2d.h"

// ============================================================================
// BGFX diagnostic callback — logs trace/fatal messages to bgfx_startup.log
// so we can understand why bgfx::init() fails.
// ============================================================================
struct BgfxDiagCallback : public bgfx::CallbackI
{
    FILE* lf = nullptr;

    BgfxDiagCallback() {
        lf = fopen("C:\\TheLab\\Development\\Generals-Modern\\bgfx_startup.log", "a");
        if (lf) { fprintf(lf, "[bgfx] DiagCallback created\n"); fflush(lf); }
    }
    ~BgfxDiagCallback() {
        if (lf) { fclose(lf); lf = nullptr; }
    }

    void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) override {
        if (lf) {
            fprintf(lf, "[bgfx FATAL] %s:%u code=%d  %s\n", _filePath, (unsigned)_line, (int)_code, _str);
            fflush(lf);
        }
        // Fatal is non-recoverable; allow bgfx internals to handle it
        abort();
    }

    void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) override {
        if (lf) {
            fprintf(lf, "[bgfx] %s:%u  ", _filePath, (unsigned)_line);
            vfprintf(lf, _format, _argList);
            fflush(lf);
        }
    }

    void profilerBegin(const char*, uint32_t, const char*, uint16_t) override {}
    void profilerBeginLiteral(const char*, uint32_t, const char*, uint16_t) override {}
    void profilerEnd() override {}
    uint32_t cacheReadSize(uint64_t) override { return 0; }
    bool cacheRead(uint64_t, void*, uint32_t) override { return false; }
    void cacheWrite(uint64_t, const void*, uint32_t) override {}
    void screenShot(const char*, uint32_t, uint32_t, uint32_t, const void*, uint32_t, bool) override {}
    void captureBegin(uint32_t, uint32_t, uint32_t, bgfx::TextureFormat::Enum, bool) override {}
    void captureEnd() override {}
    void captureFrame(const void*, uint32_t) override {}
};

static BgfxDiagCallback s_bgfxCallback;

// Global from dx8wrapper.cpp — referenced by CommandLine.cpp
int DX8Wrapper_PreserveFPU = 0;

// Static storage for texture handle cache (defined in RenderTypes.h)
TexHandleCache::Entry TexHandleCache::s_cache[TexHandleCache::MAX_CACHED] = {};
int TexHandleCache::s_count = 0;

// ============================================================================
// Unified timestamped trace log — all BGFX diagnostics in one place
// ============================================================================
static LARGE_INTEGER s_traceFreq = {};
static LARGE_INTEGER s_traceStart = {};
static FILE* s_traceFile = nullptr;
static int s_traceInitted = 0;

static void TraceInit() {
    if (s_traceInitted) return;
    s_traceInitted = 1;
    QueryPerformanceFrequency(&s_traceFreq);
    QueryPerformanceCounter(&s_traceStart);
    s_traceFile = fopen("C:\\TheLab\\Development\\Generals-Modern\\bgfx_trace.log", "w");
    if (s_traceFile) {
        fprintf(s_traceFile, "=== BGFX Trace Log started ===\n");
        fflush(s_traceFile);
    }
}

static void Trace(const char* category, const char* fmt, ...) {
    if (!s_traceInitted) TraceInit();
    if (!s_traceFile) return;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    double ms = (double)(now.QuadPart - s_traceStart.QuadPart) * 1000.0 / (double)s_traceFreq.QuadPart;
    fprintf(s_traceFile, "[%10.1f] %-6s ", ms, category);
    va_list args;
    va_start(args, fmt);
    vfprintf(s_traceFile, fmt, args);
    va_end(args);
    fprintf(s_traceFile, "\n");
    // Flush every line so we don't lose data on crash
    fflush(s_traceFile);
}

// ============================================================================
// Static Member Variable Definitions
// ============================================================================
DX8_CleanupHook* BGFXWrapper::m_pCleanupHook = nullptr;
RenderStateStruct BGFXWrapper::render_state;
unsigned BGFXWrapper::render_state_changed = 0;
Matrix4x4 BGFXWrapper::DX8Transforms[D3DTS_WORLD + 1];

bool BGFXWrapper::IsInitted = false;
bool BGFXWrapper::IsDeviceLost = false;
void* BGFXWrapper::Hwnd = nullptr;
unsigned BGFXWrapper::_MainThreadID = 0;
bool BGFXWrapper::_EnableTriangleDraw = true;

int BGFXWrapper::CurRenderDevice = 0;
int BGFXWrapper::ResolutionWidth = 800;
int BGFXWrapper::ResolutionHeight = 600;
int BGFXWrapper::BitDepth = 32;
int BGFXWrapper::TextureBitDepth = 32;
bool BGFXWrapper::IsWindowed = true;
D3DFORMAT BGFXWrapper::DisplayFormat = D3DFMT_A8R8G8B8;

D3DMATRIX BGFXWrapper::old_world;
D3DMATRIX BGFXWrapper::old_view;
D3DMATRIX BGFXWrapper::old_prj;

DWORD BGFXWrapper::Vertex_Shader = 0;
DWORD BGFXWrapper::Pixel_Shader = 0;
Vector4 BGFXWrapper::Vertex_Shader_Constants[MAX_VERTEX_SHADER_CONSTANTS];
Vector4 BGFXWrapper::Pixel_Shader_Constants[MAX_PIXEL_SHADER_CONSTANTS];

LightEnvironmentClass* BGFXWrapper::Light_Environment = nullptr;
RenderInfoClass* BGFXWrapper::Render_Info = nullptr;
DWORD BGFXWrapper::Vertex_Processing_Behavior = 0;
ZTextureClass* BGFXWrapper::Shadow_Map[MAX_SHADOW_MAPS] = {};
Vector3 BGFXWrapper::Ambient_Color(0.0f, 0.0f, 0.0f);

bool BGFXWrapper::world_identity = true;
unsigned BGFXWrapper::RenderStates[256] = {};
unsigned BGFXWrapper::TextureStageStates[MAX_TEXTURE_STAGES][32] = {};
IDirect3DBaseTexture8* BGFXWrapper::Textures[MAX_TEXTURE_STAGES] = {};

bool BGFXWrapper::FogEnable = false;
D3DCOLOR BGFXWrapper::FogColor = 0;

unsigned long BGFXWrapper::FrameCount = 0;
DX8Caps* BGFXWrapper::CurrentCaps = nullptr;
D3DADAPTER_IDENTIFIER8 BGFXWrapper::CurrentAdapterIdentifier = {};

IDirect3DDevice8 BGFXWrapper::s_stubDevice;
IDirect3D8 BGFXWrapper::s_stubD3D;

IDirect3DSurface8* BGFXWrapper::CurrentRenderTarget = nullptr;
IDirect3DSurface8* BGFXWrapper::CurrentDepthBuffer = nullptr;
IDirect3DSurface8* BGFXWrapper::DefaultRenderTarget = nullptr;
IDirect3DSurface8* BGFXWrapper::DefaultDepthBuffer = nullptr;

unsigned BGFXWrapper::DrawPolygonLowBoundLimit = 0;
bool BGFXWrapper::IsRenderToTexture = false;

int BGFXWrapper::ZBias = 0;
float BGFXWrapper::ZNear = 0.1f;
float BGFXWrapper::ZFar = 1000.0f;
Matrix4x4 BGFXWrapper::ProjectionMatrix;

bool BGFXWrapper::CurrentDX8LightEnables[4] = {};

bgfx::UniformHandle BGFXWrapper::s_uTexTransform[4] = {
    BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE
};
bgfx::UniformHandle BGFXWrapper::s_uTexCoordSource    = BGFX_INVALID_HANDLE;
bgfx::UniformHandle BGFXWrapper::s_uTexTransformFlags = BGFX_INVALID_HANDLE;

// Phase A lighting uniforms
bgfx::UniformHandle BGFXWrapper::s_uLightDir      = BGFX_INVALID_HANDLE;
bgfx::UniformHandle BGFXWrapper::s_uLightColor    = BGFX_INVALID_HANDLE;
bgfx::UniformHandle BGFXWrapper::s_uAmbientColor  = BGFX_INVALID_HANDLE;

// Track whether bgfx::init() actually succeeded (separate from IsInitted which
// is always true so the game engine doesn't abort). Guard all bgfx:: calls.
static bool s_bgfxReallyInitted = false;
static bool s_viewIsIdentity = false;  // true when Set_View_Identity active (2D mode)

// Per-frame draw counters for diagnostics
static int s_frame2D = 0;      // 2D draws this frame
static int s_frame3D = 0;      // 3D draws this frame
static int s_frameText = 0;    // text draws (2D with v > 8) this frame
static int s_frameReject = 0;  // rejected draws (inf/nan) this frame

// File-scope aliases for BGFXWrapper protected members.
// Initialized in BGFXWrapper::Init() (a member function) so that
// file-scope helper functions (SubmitDraw, etc.) can read them.
static unsigned*               s_RS  = nullptr; // → BGFXWrapper::RenderStates
static IDirect3DBaseTexture8** s_TX  = nullptr; // → BGFXWrapper::Textures
static unsigned              (*s_TSS)[32] = nullptr; // → BGFXWrapper::TextureStageStates

// Per-stage texture transform state (internal)
namespace {
    constexpr int kMaxStages = 4;
    Matrix4x4 s_texTransforms[kMaxStages];
    int s_texCoordSources[kMaxStages] = {};
    int s_texTransformFlags[kMaxStages] = {};
}

// ============================================================================
// Geometry submission state
// ============================================================================
// Shader program for all geometry (compiled once at Init time).
// Uses runtime D3DCompile so no offline shaderc is needed.
static bgfx::ProgramHandle  s_meshProgram   = BGFX_INVALID_HANDLE;

// ---- Shader Program Registry ------------------------------------------------
// Indexed by BGFXWrapper::ShaderProgramType. Invalid handles fall back to
// s_meshProgram so un-compiled slots never cause a crash.
static bgfx::ProgramHandle  s_programs[BGFXWrapper::SP_COUNT];
static int                  s_activeProgram = BGFXWrapper::SP_MESH_DEFAULT;

// Texture sampler uniforms.
// s_texSampler  → t0/s0  base terrain texture (and mesh texture)
// s_texSamplerB → t1/s1  blend terrain texture (same atlas, different UVs)
static bgfx::UniformHandle  s_texSampler    = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle  s_texSamplerB   = BGFX_INVALID_HANDLE;

// WVP matrix uniform — set per-draw; VS uses this to transform model-space vertices to clip space.
// GPU-side transform avoids the CPU perspective-divide sign-flip for behind-camera vertices.
static bgfx::UniformHandle  s_uWVP          = BGFX_INVALID_HANDLE;

// 1×1 white fallback texture bound when stage 0 is unset.
static bgfx::TextureHandle  s_defaultTexture = BGFX_INVALID_HANDLE;

// bgfx vertex layout matching VertexFormatXYZNDUV2
//   float x,y,z    (12 bytes) = Position
//   float nx,ny,nz (12 bytes) = Normal
//   uint8 r,g,b,a  ( 4 bytes) = Color0 (BGRA packed, normalized to [0..1])
//   float u0,v0    ( 8 bytes) = TexCoord0
//   float u1,v1    ( 8 bytes) = TexCoord1
//   Total: 44 bytes per vertex
static bgfx::VertexLayout   s_layoutXYZNDUV2;

// bgfx binary shader version (must match shaderc.cpp: BGFX_SHADER_BIN_VERSION)
static const uint8_t k_bgfxShaderVersion = 11;

// ============================================================================
// Runtime HLSL → bgfx shader compilation
// ============================================================================
// Compiles an HLSL string to DXBC via D3DCompile(), wraps it in bgfx's binary
// format, and returns a bgfx::ShaderHandle. No offline shaderc tool required.
// For shader binary format see bgfx/src/renderer_d3d11.cpp:ShaderD3D11::create()
static bgfx::ShaderHandle CompileHLSLToShader(const char* hlsl, bool isVertex)
{
    const char* profile = isVertex ? "vs_5_0" : "ps_5_0";

    ID3DBlob* code   = nullptr;
    ID3DBlob* errors = nullptr;
    HRESULT hr = D3DCompile(
        hlsl, strlen(hlsl),
        isVertex ? "vs_mesh" : "fs_mesh",   // source name (for error messages)
        nullptr, nullptr,                    // defines, include handler
        "main", profile,
        D3DCOMPILE_OPTIMIZATION_LEVEL1, 0,
        &code, &errors);

    if (errors)
    {
        Trace("SHADER", "%s compile %s: %s",
            isVertex ? "VS" : "FS",
            SUCCEEDED(hr) ? "warnings" : "ERRORS",
            (const char*)errors->GetBufferPointer());
        {
        }
        errors->Release();
    }

    if (FAILED(hr)) return BGFX_INVALID_HANDLE;

    // Strip debug/reflection data to reduce binary size
    ID3DBlob* stripped = nullptr;
    D3DStripShader(code->GetBufferPointer(), code->GetBufferSize(),
        D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_TEST_BLOBS,
        &stripped);
    if (stripped) { code->Release(); code = stripped; }

    // Build bgfx binary format:
    //   uint32  magic   (VSH or FSH + version)
    //   uint32  hashIn  (0 = don't care)
    //   uint32  hashOut (0 = don't care; present for version >= 6)
    //   uint16  count   (0 = no uniforms)
    //   uint32  dxbcSize
    //   uint8[] dxbc bytecode
    //   uint8   nul     (padding byte after dxbc)
    //   uint8   numAttrs (0 = skip attribute validation)
    //   uint16  cbSize  (0 = no constant buffer)
    uint32_t magic =
        isVertex
        ? BX_MAKEFOURCC('V', 'S', 'H', k_bgfxShaderVersion)
        : BX_MAKEFOURCC('F', 'S', 'H', k_bgfxShaderVersion);

    uint32_t hashIn  = 0;
    uint32_t hashOut = 0;
    uint32_t dxbcSize = (uint32_t)code->GetBufferSize();
    uint8_t  nul      = 0;

    // Vertex shaders declare a single user uniform "u_wvp" (mat4).
    // BGFX maps this by name to the cbuffer at D3D11 register b0, float4 offset 0.
    // Fragment shaders have no uniforms (count=0, cbSize=0).
    //
    // Uniform entry layout (binary format version >= 10):
    //   uint8  nameSize   = 5
    //   char[5] name      = "u_wvp"
    //   uint8  type       = 4 (UniformType::Mat4)
    //   uint8  num        = 1
    //   uint16 regIndex   = 0  (float4 offset 0 in the vs scratch / cbuffer)
    //   uint16 regCount   = 4  (mat4 = 4 float4 registers)
    //   uint16 texInfo    = 0  (not a texture, required for version >= 8)
    //   uint16 texFormat  = 0  (not a texture, required for version >= 10)
    //   Total: 16 bytes per entry
    struct WvpUniformEntry {
        uint8_t  nameSize  = 5;
        char     name[5]   = {'u','_','w','v','p'};
        uint8_t  type      = 4;   // UniformType::Mat4
        uint8_t  num       = 1;
        uint16_t regIndex  = 0;
        uint16_t regCount  = 4;
        uint16_t texInfo   = 0;
        uint16_t texFormat = 0;
    } wvpEntry;
    static_assert(sizeof(WvpUniformEntry) == 16, "WvpUniformEntry size mismatch");

    uint16_t count  = isVertex ? 1u : 0u;
    uint16_t cbSize = isVertex ? 64u : 0u; // mat4 = 4 float4s × 16 bytes = 64

    // Vertex shader attribute IDs (from bgfx vertexlayout.cpp s_attribToId table).
    // These tell bgfx which vertex inputs the shader uses so it builds the correct
    // D3D11 input layout.  Fragment shaders have numAttrs=0.
    static const uint16_t k_vsAttrs[] = {
        0x0001, // Position
        0x0002, // Normal
        0x0005, // Color0
        0x0010, // TexCoord0
        0x0011, // TexCoord1
    };
    const uint8_t numAttrs     = isVertex ? (uint8_t)BX_COUNTOF(k_vsAttrs) : 0;
    const uint16_t* attrsPtr   = isVertex ? k_vsAttrs : nullptr;
    uint32_t attrsSize         = (uint32_t)numAttrs * sizeof(uint16_t);
    uint32_t uniformEntrySize  = isVertex ? (uint32_t)sizeof(WvpUniformEntry) : 0u;

    uint32_t totalSize = 4 + 4 + 4 + 2 + uniformEntrySize + 4 + dxbcSize + 1 + 1 + attrsSize + 2;
    uint8_t* buf = new uint8_t[totalSize];
    uint8_t* p   = buf;

    auto wrt = [&](const void* data, size_t sz) { memcpy(p, data, sz); p += sz; };
    wrt(&magic,        4);
    wrt(&hashIn,       4);
    wrt(&hashOut,      4);
    wrt(&count,        2);
    if (isVertex) wrt(&wvpEntry, sizeof(wvpEntry));  // uniform entry between count and DXBC
    wrt(&dxbcSize,     4);
    wrt(code->GetBufferPointer(), dxbcSize);
    wrt(&nul,          1);
    wrt(&numAttrs,     1);
    if (attrsSize > 0) wrt(attrsPtr, attrsSize);
    wrt(&cbSize,       2);

    code->Release();

    bgfx::ShaderHandle sh = bgfx::createShader(bgfx::copy(buf, totalSize));
    delete[] buf;

    if (!bgfx::isValid(sh))
    {
        Trace("SHADER", "%s bgfx::createShader() FAILED", isVertex ? "VS" : "FS");
    }
    return sh;
}

// ============================================================================
// Shader HLSL source
// ============================================================================
// Vertex format: VertexFormatXYZNDUV2 (44 bytes)
// Semantics match bgfx's D3D11 s_attrib table (renderer_d3d11.cpp line 305)
//
// u_wvp (cbuffer b0) = proj * view * world, computed per-draw on CPU, uploaded
// via bgfx::setUniform(s_uWVP).  GPU transforms model-space positions to clip-space
// so the hardware handles near-plane clipping correctly — avoids the CPU sign-flip
// that occurs when clip_w ≤ 0 (behind-camera vertices).
static const char* k_vsMesh = R"(
cbuffer cbBuffer0 : register(b0)
{
    row_major float4x4 u_wvp;
};

struct VS_IN
{
    float3 a_position  : POSITION;
    float3 a_normal    : NORMAL;
    float4 a_color0    : COLOR;       // R8G8B8A8_UNORM → float4 [0..1] (NOT uint4)
    float2 a_texcoord0 : TEXCOORD0;
    float2 a_texcoord1 : TEXCOORD1;
};

struct VS_OUT
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR0;
    float2 uv0   : TEXCOORD0;
};

VS_OUT main(VS_IN i)
{
    VS_OUT o;
    o.pos   = mul(u_wvp, float4(i.a_position, 1.0f));
    // bgfx R8G8B8A8_UNORM reads game's BGRA memory as: r=Blue, g=Green, b=Red, a=Alpha
    // Swizzle to correct RGBA output: (r=R, g=G, b=B, a=A)
    o.color = float4(i.a_color0.z, i.a_color0.y, i.a_color0.x, i.a_color0.w);
    o.uv0   = i.a_texcoord0;
    return o;
}
)";

static const char* k_fsMesh = R"(
Texture2D    s_texColor : register(t0);
SamplerState s_sampler  : register(s0);

struct VS_OUT
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR0;
    float2 uv0   : TEXCOORD0;
};

float4 main(VS_OUT i) : SV_TARGET
{
    float4 tex = s_texColor.Sample(s_sampler, i.uv0);
    return tex * i.color;
}
)";

// ============================================================================
// Terrain shader HLSL (Phase A)
// ============================================================================
// VS: identical to k_vsMesh but passes both UV sets (uv0 + uv1) and the
//     world-space normal so the FS can drive GPU lighting in Phase B.
// FS: samples stage 0 (base terrain), multiplies by vertex color.
//     Vertex color contains baked directional+ambient light from HeightMap::updateVB().
//     Stage 1 (blend) and stage 2 (alpha mask) are declared and bound but not
//     combined yet — Phase C will activate the lerp() once the uniform
//     infrastructure for per-layer weights is in place.
static const char* k_vsTerrain = R"(
cbuffer cbBuffer0 : register(b0)
{
    row_major float4x4 u_wvp;
};

struct VS_IN
{
    float3 a_position  : POSITION;
    float3 a_normal    : NORMAL;
    float4 a_color0    : COLOR;
    float2 a_texcoord0 : TEXCOORD0;
    float2 a_texcoord1 : TEXCOORD1;
};

struct VS_OUT
{
    float4 pos       : SV_POSITION;
    float4 color     : COLOR0;
    float2 uv0       : TEXCOORD0;
    float2 uv1       : TEXCOORD1;
    float3 worldNorm : TEXCOORD2;
};

VS_OUT main(VS_IN i)
{
    VS_OUT o;
    o.pos      = mul(u_wvp, float4(i.a_position, 1.0f));
    // BGRA u8-normalised → RGBA float.
    o.color    = float4(i.a_color0.z, i.a_color0.y, i.a_color0.x, i.a_color0.w);
    o.uv0      = i.a_texcoord0;  // base/blend texture UVs
    o.uv1      = i.a_texcoord1;  // alpha-mask UVs
    // Normal is in world-space (HeightMap cross-product computation).
    // Pass through so Phase B FS can do dot(N, lightDir) per-fragment.
    o.worldNorm = normalize(i.a_normal);
    return o;
}
)";

static const char* k_fsTerrain = R"(
// Stage 0 — base terrain tile texture (always bound)
Texture2D    s_texColor : register(t0);
SamplerState s_sampler0 : register(s0);

// Stage 1 — secondary terrain tile (blend target, Phase C)
// Bound to 1x1 white stub until multi-texture blending is enabled.
Texture2D    s_texBlend : register(t1);
SamplerState s_sampler1 : register(s1);

// Stage 2 — alpha mask between stage0 and stage1 (Phase C)
Texture2D    s_texAlpha : register(t2);
SamplerState s_sampler2 : register(s2);

// Phase B lighting uniforms — declared now, driven from bgfx::setUniform() in Phase B.
// For Phase A they are ignored; vertex color carries pre-baked lighting.
// cbuffer LightParams : register(b1)
// {
//     float4 u_lightDir;      // world-space sun direction (xyz)
//     float4 u_lightColor;    // sun RGB + intensity
//     float4 u_ambientColor;  // sky RGB + fill factor
// };

struct VS_OUT
{
    float4 pos       : SV_POSITION;
    float4 color     : COLOR0;
    float2 uv0       : TEXCOORD0;
    float2 uv1       : TEXCOORD1;
    float3 worldNorm : TEXCOORD2;
};

float4 main(VS_OUT i) : SV_TARGET
{
    // Base terrain texture sampled at uv0.
    float4 base  = s_texColor.Sample(s_sampler0, i.uv0);

    // Phase C: blend second terrain texture layer using uv1 and vertex alpha.
    // i.color.a  = blend factor (0 = base only, 1 = blend only).
    // s_texBlend = same terrain atlas bound to stage 1 with different UVs per vertex.
    float4 blend = s_texBlend.Sample(s_sampler1, i.uv1);
    float3 col   = lerp(base.rgb, blend.rgb, i.color.a);

    // Pre-baked terrain lighting is in i.color.rgb (from vertex diffuse RGB).
    return float4(col * i.color.rgb, 1.0);
}
)";

// Uniform for screen size (passed to vertex shader)
static bgfx::UniformHandle s_uScreenSize = BGFX_INVALID_HANDLE;

// ============================================================================
// Initialization
// ============================================================================
bool BGFXWrapper::Init(void* hwnd, bool lite)
{
    if (IsInitted) return true;
    Hwnd = hwnd;

    TraceInit();
    Trace("INIT", "BGFXWrapper::Init hwnd=%p res=%dx%d windowed=%d", hwnd, ResolutionWidth, ResolutionHeight, (int)IsWindowed);

    // Expose protected members to file-scope helper functions.
    s_RS  = RenderStates;
    s_TX  = Textures;
    s_TSS = TextureStageStates;

    // Mirror DX8Wrapper::Init() which calls Set_Screen_Resolution() early so that
    // Render2DClass::Get_Screen_Resolution() returns valid dimensions before any
    // render2dsentence renderer calls Set_Coordinate_Range(Get_Screen_Resolution()).
    // Without this, ScreenResolution stays (0,0,0,0) → CoordinateScale = ±inf → all draws black.
    Render2DClass::Set_Screen_Resolution(RectClass(0, 0, ResolutionWidth, ResolutionHeight));

    // Single-threaded mode: tell bgfx the main thread IS the render thread.
    // Calling renderFrame() before init() suppresses bgfx's internal render thread.
    // This is required for game engines that drive their own main loop.
    bgfx::renderFrame();

    bgfx::Init init;
    init.type        = bgfx::RendererType::Direct3D11; // Explicit: only D3D11 is compiled in
    init.callback    = &s_bgfxCallback;                // Capture all trace/fatal messages

    // CRITICAL: set platformData directly in the Init struct.
    // bgfx::setPlatformData() sets a global that is overwritten by Context::init()
    // from init.platformData at line 1928 of bgfx.cpp, so we must set it here.
    init.platformData.nwh = hwnd;
    init.platformData.ndt = nullptr; // Not used on Windows

    init.resolution.width  = (ResolutionWidth  > 0) ? (uint32_t)ResolutionWidth  : 800u;
    init.resolution.height = (ResolutionHeight > 0) ? (uint32_t)ResolutionHeight : 600u;
    init.resolution.reset  = IsWindowed ? BGFX_RESET_NONE : BGFX_RESET_FULLSCREEN;

    // -----------------------------------------------------------------------
    // Transient buffer sizes.
    //
    // No geometry in this port is GPU-resident yet: every vertex and index
    // buffer is a malloc in system RAM and is re-uploaded through bgfx's
    // TRANSIENT buffers on every single frame. That makes the per-frame
    // transient pool, not VRAM, the hard ceiling on scene complexity.
    //
    // bgfx defaults to 6 MB vertex / 2 MB index (config.h:320, :324). At the
    // 44-byte XYZNDUV2 stride that is only ~143k vertices per frame, and the
    // shell map's terrain alone is 16 tiles x 4096 vertices x 2 passes ~= 131k
    // before a single unit or UI element is drawn. Measured consequence, from
    // a live run: "DROP transient exhausted: need v=4096 avail v=2734",
    // 5 dropped draws every frame, i.e. terrain tiles silently missing.
    //
    // These are a STOPGAP. The real fix is backing static geometry with real
    // bgfx vertex/index buffers so terrain stops being re-uploaded per frame;
    // once that lands these can come back down. bgfx honours these values
    // directly with no clamp against the compile-time macro (bgfx.cpp:2053).
    init.limits.transientVbSize = 48u << 20; // 48 MB ~= 1.1M vertices/frame
    init.limits.transientIbSize = 12u << 20; // 12 MB ~= 6.2M indices/frame

    bool bgfxOk = bgfx::init(init);
    s_bgfxReallyInitted = bgfxOk;
    Trace("INIT", "bgfx::init() %s renderer=%s", bgfxOk ? "SUCCEEDED" : "FAILED",
        bgfx::getRendererName(bgfx::getRendererType()));
    if (bgfxOk) {
        for (int i = 0; i < 4; ++i) {
            s_uTexTransform[i] = bgfx::createUniform(
                (std::string("u_texTransform") + std::to_string(i)).c_str(),
                bgfx::UniformType::Mat4);
        }
        s_uTexCoordSource = bgfx::createUniform("u_texCoordSource", bgfx::UniformType::Vec4);
        s_uTexTransformFlags = bgfx::createUniform("u_texTransformFlags", bgfx::UniformType::Vec4);

        // ----------------------------------------------------------------
        // Initialize D3D transform state to identity (DX8 device default).
        // Matrix4x4::Matrix4x4() does NOT initialize — must do it explicitly.
        // ----------------------------------------------------------------
        for (int i = 0; i <= D3DTS_WORLD; ++i)
            DX8Transforms[i].Make_Identity();

        // DX8 default texture stage states for stage 0:
        // ALPHAOP = SELECTARG1 (2), ALPHAARG1 = TEXTURE (2)
        // This means "use texture alpha, ignore vertex alpha" by default.
        TextureStageStates[0][D3DTSS_ALPHAOP]   = D3DTOP_SELECTARG1;   // 2
        TextureStageStates[0][D3DTSS_ALPHAARG1]  = D3DTA_TEXTURE;       // 2
        TextureStageStates[0][D3DTSS_ALPHAARG2]  = D3DTA_CURRENT;       // 1

        // ----------------------------------------------------------------
        // DX8 render state defaults (RenderStates[] is zero-initialized,
        // but DX8 has non-zero defaults for many states).
        // Without these, depth test stays disabled and terrain renders wrong.
        // ----------------------------------------------------------------
        RenderStates[D3DRS_ZENABLE]        = 1;                    // D3DZB_TRUE — depth test ON
        RenderStates[D3DRS_ZWRITEENABLE]   = 1;                    // TRUE — depth writes ON
        RenderStates[D3DRS_ZFUNC]          = 4;                    // D3DCMP_LESSEQUAL
        RenderStates[D3DRS_FILLMODE]       = 3;                    // D3DFILL_SOLID
        RenderStates[D3DRS_CULLMODE]       = 3;                    // D3DCULL_CCW
        RenderStates[D3DRS_SRCBLEND]       = 2;                    // D3DBLEND_ONE
        RenderStates[D3DRS_DESTBLEND]      = 1;                    // D3DBLEND_ZERO
        RenderStates[D3DRS_ALPHABLENDENABLE] = 0;                  // FALSE — no blend by default

        // ----------------------------------------------------------------
        // Vertex layout for VertexFormatXYZNDUV2 (44 bytes)
        // float xyz (12), float nxyz (12), uint8x4 BGRA (4), float uv0 (8), float uv1 (8)
        // ----------------------------------------------------------------
        s_layoutXYZNDUV2.begin()
            .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal,    3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)  // normalized [0..1]
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float)
            .end();

        // ----------------------------------------------------------------
        // Compile mesh shaders at runtime (no offline shaderc required).
        // CPU pre-projects vertices in SubmitDraw; VS just passes through.
        // ----------------------------------------------------------------
        // ----------------------------------------------------------------
        // Initialise program registry to BGFX_INVALID_HANDLE.
        // SubmitDraw falls back to s_meshProgram for any invalid slot.
        // ----------------------------------------------------------------
        for (int pi = 0; pi < SP_COUNT; ++pi)
            s_programs[pi] = BGFX_INVALID_HANDLE;

        // ----------------------------------------------------------------
        // WVP matrix uniform — MUST be created before bgfx::createProgram()
        // so ShaderD3D11::create() finds "u_wvp" in m_uniformReg when it
        // parses the binary uniform entries we declared in CompileHLSLToShader.
        // ----------------------------------------------------------------
        s_uWVP = bgfx::createUniform("u_wvp", bgfx::UniformType::Mat4);

        // ----------------------------------------------------------------
        // Compile default mesh program.
        // ----------------------------------------------------------------
        bgfx::ShaderHandle vsh = CompileHLSLToShader(k_vsMesh, true);
        bgfx::ShaderHandle fsh = CompileHLSLToShader(k_fsMesh, false);
        if (bgfx::isValid(vsh) && bgfx::isValid(fsh)) {
            s_meshProgram = bgfx::createProgram(vsh, fsh, true);
            s_programs[SP_MESH_DEFAULT] = s_meshProgram; // registry slot 0
            Trace("INIT", "Mesh shader program created OK stride=%u", s_layoutXYZNDUV2.getStride());

            // Sampler uniform — must be created so bgfx::setTexture() actually binds textures.
            // Name matches the HLSL Texture2D declaration "s_texColor" in k_fsMesh.
            // Without this, s_texSampler stays BGFX_INVALID_HANDLE and setTexture is skipped
            // → shader samples unbound slot → all draws are black.
            s_texSampler  = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
            s_texSamplerB = bgfx::createUniform("s_texBlend", bgfx::UniformType::Sampler);

            // 1×1 opaque-white default texture — bound when game has no stage-0 texture.
            // This lets vertex-color-only draws render correctly (white * vertex_color = vertex_color).
            static const uint8_t kWhite[4] = {0xFF, 0xFF, 0xFF, 0xFF}; // RGBA
            s_defaultTexture = bgfx::createTexture2D(1, 1, false, 1,
                bgfx::TextureFormat::RGBA8, 0, bgfx::copy(kWhite, sizeof(kWhite)));
        } else {
            Trace("INIT", "Mesh shader FAILED vsh=%s fsh=%s",
                bgfx::isValid(vsh)?"OK":"FAIL", bgfx::isValid(fsh)?"OK":"FAIL");
            if (bgfx::isValid(vsh)) bgfx::destroy(vsh);
            if (bgfx::isValid(fsh)) bgfx::destroy(fsh);
        }

        // ----------------------------------------------------------------
        // Compile terrain program (Phase A).
        // Uses the same sampler name "s_texColor" as k_fsMesh so s_texSampler
        // works for stage-0 binding without a separate uniform handle.
        // Stages 1 and 2 are declared in k_fsTerrain but not combined yet.
        // ----------------------------------------------------------------
        {
            bgfx::ShaderHandle tvsh = CompileHLSLToShader(k_vsTerrain, true);
            bgfx::ShaderHandle tfsh = CompileHLSLToShader(k_fsTerrain, false);
            if (bgfx::isValid(tvsh) && bgfx::isValid(tfsh)) {
                bgfx::ProgramHandle terrainProg = bgfx::createProgram(tvsh, tfsh, true);
                // Register for all terrain types — Phase B will split these further.
                s_programs[SP_TERRAIN_BASE]    = terrainProg;
                s_programs[SP_TERRAIN_NOISE1]  = terrainProg;
                s_programs[SP_TERRAIN_NOISE2]  = terrainProg;
                s_programs[SP_TERRAIN_NOISE12] = terrainProg;
                Trace("INIT", "Terrain shader program compiled OK");
            } else {
                Trace("INIT", "Terrain shader FAILED tvsh=%s tfsh=%s",
                    bgfx::isValid(tvsh)?"OK":"FAIL", bgfx::isValid(tfsh)?"OK":"FAIL");
                if (bgfx::isValid(tvsh)) bgfx::destroy(tvsh);
                if (bgfx::isValid(tfsh)) bgfx::destroy(tfsh);
                // Fall back: terrain uses mesh program (still better than crash)
                s_programs[SP_TERRAIN_BASE]    = s_meshProgram;
                s_programs[SP_TERRAIN_NOISE1]  = s_meshProgram;
                s_programs[SP_TERRAIN_NOISE2]  = s_meshProgram;
                s_programs[SP_TERRAIN_NOISE12] = s_meshProgram;
            }

            // Phase A lighting uniforms — created now, driven from Set_Light() in Phase B.
            // For Phase A these are unused by the shader (vertex color carries baked lighting).
            s_uLightDir     = bgfx::createUniform("u_lightDir",     bgfx::UniformType::Vec4);
            s_uLightColor   = bgfx::createUniform("u_lightColor",   bgfx::UniformType::Vec4);
            s_uAmbientColor = bgfx::createUniform("u_ambientColor", bgfx::UniformType::Vec4);
        }
    }
    // Initialize capabilities object — required by W3DShaderManager::getChipset()
    // and other subsystems that query GPU features. BGFXCaps queries bgfx::getCaps()
    // if BGFX is initialized, or returns safe modern-GPU defaults otherwise.
    if (!CurrentCaps) {
        CurrentCaps = new BGFXCaps(WW3D_FORMAT_UNKNOWN);
    }

    // Mirror DX8Wrapper::Set_Render_Device() which calls Do_Onetime_Device_Dependent_Inits()
    // after the D3D device is created. We call it here since BGFX init is our "device creation".
    Do_Onetime_Device_Dependent_Inits();

    // Always report success so game startup continues even if BGFX renderer failed
    IsInitted = true;
    return true;
}

void BGFXWrapper::Shutdown()
{
    if (IsInitted) {
        if (s_bgfxReallyInitted) {
            for (int i = 0; i < 4; ++i) {
                if (bgfx::isValid(s_uTexTransform[i])) bgfx::destroy(s_uTexTransform[i]);
                s_uTexTransform[i] = BGFX_INVALID_HANDLE;
            }
            if (bgfx::isValid(s_uTexCoordSource)) bgfx::destroy(s_uTexCoordSource);
            s_uTexCoordSource = BGFX_INVALID_HANDLE;
            if (bgfx::isValid(s_uTexTransformFlags)) bgfx::destroy(s_uTexTransformFlags);
            s_uTexTransformFlags = BGFX_INVALID_HANDLE;
            // Destroy program registry (skip slot 0 = s_meshProgram, destroyed below).
            // Terrain slots 1–4 all point to the same compiled program; destroy once.
            {
                bgfx::ProgramHandle terrainProg = s_programs[SP_TERRAIN_BASE];
                if (bgfx::isValid(terrainProg) && terrainProg.idx != s_meshProgram.idx)
                    bgfx::destroy(terrainProg);
                for (int pi = 0; pi < SP_COUNT; ++pi)
                    s_programs[pi] = BGFX_INVALID_HANDLE;
            }
            if (bgfx::isValid(s_meshProgram)) { bgfx::destroy(s_meshProgram); s_meshProgram = BGFX_INVALID_HANDLE; }
            if (bgfx::isValid(s_uScreenSize)) { bgfx::destroy(s_uScreenSize); s_uScreenSize = BGFX_INVALID_HANDLE; }
            if (bgfx::isValid(s_texSampler))  { bgfx::destroy(s_texSampler);  s_texSampler  = BGFX_INVALID_HANDLE; }
            if (bgfx::isValid(s_texSamplerB)) { bgfx::destroy(s_texSamplerB); s_texSamplerB = BGFX_INVALID_HANDLE; }
            if (bgfx::isValid(s_defaultTexture)) { bgfx::destroy(s_defaultTexture); s_defaultTexture = BGFX_INVALID_HANDLE; }
            // Phase A lighting uniforms
            if (bgfx::isValid(s_uLightDir))     { bgfx::destroy(s_uLightDir);     s_uLightDir     = BGFX_INVALID_HANDLE; }
            if (bgfx::isValid(s_uLightColor))   { bgfx::destroy(s_uLightColor);   s_uLightColor   = BGFX_INVALID_HANDLE; }
            if (bgfx::isValid(s_uAmbientColor)) { bgfx::destroy(s_uAmbientColor); s_uAmbientColor = BGFX_INVALID_HANDLE; }
            bgfx::shutdown();
            s_bgfxReallyInitted = false;
        }
        IsInitted = false;
    }
}

void BGFXWrapper::Do_Onetime_Device_Dependent_Inits(void)
{
    // Initialize subsystems that the original DX8Wrapper::Do_Onetime_Device_Dependent_Inits()
    // was responsible for. CurrentCaps must already be initialized before this is called.

    // Initialize the missing texture placeholder (already stubbed, no-op in BGFX port)
    MissingTexture::_Init();

    // Initialize texture filter lookup tables (reads CurrentCaps, then sets texture stage state
    // via BGFXWrapper stubs which are no-ops — safe to call)
    if (CurrentCaps) {
        TextureFilterClass::_Init_Filters(
            (TextureFilterClass::TextureFilterMode)WW3D::Get_Texture_Filter());
    }

    // Initialize DX8 mesh renderer (only allocates an FVF category list, no DX8 calls)
    TheDX8MeshRenderer.Init();

    // Initialize box render object materials (BGFX-compatible since boxrobj.cpp was fixed)
    BoxRenderObjClass::Init();

    // Initialize vertex material presets — CRITICAL: must come before PointGroupClass::_Init()
    VertexMaterialClass::Init();

    // Initialize point group lookup tables and index buffers
    // (DX8IndexBufferClass now backed by BGFXIndexBuffer8 CPU memory — safe)
    PointGroupClass::_Init();

    // Initialize shatter system (asset manager only, no DX8 calls)
    ShatterSystem::Init();

    // Start background texture loading thread
    TextureLoader::Init();

    // Note: Set_Default_Global_Render_States() is skipped — it sets DX8 render states
    // that don't apply to the BGFX backend. BGFX manages its own state.
}
void BGFXWrapper::Do_Onetime_Device_Dependent_Shutdowns(void) {}

// ============================================================================
// Scene Management
// ============================================================================
void BGFXWrapper::Begin_Scene(void)
{
    if (s_bgfxReallyInitted) {
        static int s_sceneLog = 0;
        if (s_sceneLog < 5) {
            Trace("SCENE", "Begin_Scene frame=%lu res=%dx%d", FrameCount, ResolutionWidth, ResolutionHeight);
            s_sceneLog++;
        }
        // View 0: 3D scene (terrain, models, etc.)
        bgfx::setViewRect(0, 0, 0, (uint16_t)ResolutionWidth, (uint16_t)ResolutionHeight);
        bgfx::touch(0);
        // View 1: 2D UI overlay — renders AFTER view 0.
        // MUST clear depth so UI draws are never rejected by terrain's depth values.
        // Color is NOT cleared — UI composites on top of the 3D scene.
        bgfx::setViewRect(1, 0, 0, (uint16_t)ResolutionWidth, (uint16_t)ResolutionHeight);
        bgfx::setViewClear(1, BGFX_CLEAR_DEPTH, 0, 1.0f, 0);
        bgfx::setViewMode(1, bgfx::ViewMode::Sequential);
        bgfx::touch(1);
        bgfx::setDebug(BGFX_DEBUG_NONE);
    }
}

void BGFXWrapper::End_Scene(bool flip_frame)
{
    if (flip_frame && s_bgfxReallyInitted) {
        // Log per-frame summary: first 10 frames, then every 10th up to 200, then every 100th
        bool logFrame = (FrameCount < 10) || (FrameCount < 200 && FrameCount % 10 == 0) || (FrameCount % 100 == 0);
        if (logFrame) {
            Trace("FRAME", "#%lu 2D=%d(text=%d) 3D=%d reject=%d",
                FrameCount, s_frame2D, s_frameText, s_frame3D, s_frameReject);
        }
        s_frame2D = 0; s_frame3D = 0; s_frameText = 0; s_frameReject = 0;
        s_activeProgram = SP_MESH_DEFAULT; // reset each frame; W3DShaderManager::setShader re-sets as needed
        bgfx::frame();
        FrameCount++;
    }
}

void BGFXWrapper::Flip_To_Primary(void)
{
    // NOTE: Do NOT call bgfx::frame() here — End_Scene already does it.
    // Double-frame would submit an empty frame that clears the screen.
    static int s_flipLog = 0;
    if (s_flipLog < 3) {
        Trace("SCENE", "Flip_To_Primary called (no-op, End_Scene handles frame submit)");
        s_flipLog++;
    }
}

void BGFXWrapper::Clear(bool clear_color, bool clear_z_stencil, const Vector3& color,
    float dest_alpha, float z, unsigned int stencil)
{
    uint16_t flags = 0;
    if (clear_color) flags |= BGFX_CLEAR_COLOR;
    if (clear_z_stencil) flags |= BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL;
    uint32_t rgba = ((uint32_t)(color.X * 255) << 24) |
                    ((uint32_t)(color.Y * 255) << 16) |
                    ((uint32_t)(color.Z * 255) << 8) |
                    ((uint32_t)(dest_alpha * 255));
    if (s_bgfxReallyInitted) bgfx::setViewClear(0, flags, rgba, z, stencil);
}

void BGFXWrapper::Set_Viewport(CONST D3DVIEWPORT8* pViewport)
{
    if (pViewport && s_bgfxReallyInitted) {
        bgfx::setViewRect(0, pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height);
        // Keep view 1 (2D overlay) at full resolution always — 2D uses its own identity projection
    }
}

// ============================================================================
// Device Management
// ============================================================================
bool BGFXWrapper::Has_Stencil(void) { return true; }
void BGFXWrapper::Get_Format_Name(unsigned int format, StringClass* tex_format) {}
bool BGFXWrapper::Set_Any_Render_Device(void) { return true; }
bool BGFXWrapper::Set_Render_Device(const char* dev_name, int width, int height, int bits, int windowed, bool resize_window) { return true; }
bool BGFXWrapper::Set_Render_Device(int dev, int resx, int resy, int bits, int windowed, bool resize_window, bool reset_device, bool restore_assets)
{
    if (resx > 0) ResolutionWidth = resx;
    if (resy > 0) ResolutionHeight = resy;
    if (bits > 0) BitDepth = bits;
    // Always force windowed mode — bgfx runs in a window, never true fullscreen.
    IsWindowed = true;

    // Force 1024x768 for now — matches the original game's target resolution.
    // The shell map camera, UI layout, and projections are all designed for 4:3.
    {
        int monW = GetSystemMetrics(SM_CXSCREEN);
        int monH = GetSystemMetrics(SM_CYSCREEN);

        // Override to 1024x768 (original game resolution)
        ResolutionWidth  = 1024;
        ResolutionHeight = 768;

        Trace("INIT", "SetRenderDevice resx=%d resy=%d windowed=%d mon=%dx%d final=%dx%d",
            resx, resy, (int)IsWindowed, monW, monH, ResolutionWidth, ResolutionHeight);

        if (Hwnd) {
            HWND hwnd = (HWND)Hwnd;
            SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, 0);
            SetWindowPos(hwnd, HWND_TOP, 0, 0, ResolutionWidth, ResolutionHeight,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);
            Trace("INIT", "Borderless fullscreen %dx%d", ResolutionWidth, ResolutionHeight);
        }
    }

    // Mirror DX8Wrapper::Set_Render_Device() — update screen resolution for Render2D instances
    Render2DClass::Set_Screen_Resolution(RectClass(0, 0, ResolutionWidth, ResolutionHeight));

    if (s_bgfxReallyInitted) {
        bgfx::reset(ResolutionWidth, ResolutionHeight, IsWindowed ? BGFX_RESET_NONE : BGFX_RESET_FULLSCREEN);
    }
    return true;
}
bool BGFXWrapper::Set_Next_Render_Device(void) { return true; }
bool BGFXWrapper::Toggle_Windowed(void)
{
    IsWindowed = !IsWindowed;
    if (s_bgfxReallyInitted) bgfx::reset(ResolutionWidth, ResolutionHeight, IsWindowed ? BGFX_RESET_NONE : BGFX_RESET_FULLSCREEN);
    return true;
}
int BGFXWrapper::Get_Render_Device_Count(void) { return 1; }
int BGFXWrapper::Get_Render_Device(void) { return 0; }
const char* BGFXWrapper::Get_Render_Device_Name(int device_index) { return "BGFX"; }
bool BGFXWrapper::Set_Device_Resolution(int width, int height, int bits, int windowed, bool resize_window) { return Set_Render_Device(-1, width, height, bits, windowed, resize_window); }

void BGFXWrapper::Get_Device_Resolution(int& set_w, int& set_h, int& set_bits, bool& set_windowed)
{
    set_w = ResolutionWidth; set_h = ResolutionHeight; set_bits = BitDepth; set_windowed = IsWindowed;
}

void BGFXWrapper::Get_Render_Target_Resolution(int& set_w, int& set_h, int& set_bits, bool& set_windowed)
{
    Get_Device_Resolution(set_w, set_h, set_bits, set_windowed);
}

bool BGFXWrapper::Reset_Device(bool reload_assets) { return true; }
WW3DFormat BGFXWrapper::getBackBufferFormat(void) { return WW3D_FORMAT_A8R8G8B8; }

// ============================================================================
// Transforms
// ============================================================================
void BGFXWrapper::Set_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix4x4& m)
{
    DX8Transforms[transform] = m;
    if (transform == D3DTS_WORLD) render_state_changed |= WORLD_CHANGED;
    if (transform == D3DTS_VIEW) {
        render_state_changed |= VIEW_CHANGED;
        bool was2D = s_viewIsIdentity;
        s_viewIsIdentity = false;
        if (was2D) {
            static int s_switchLog3D = 0;
            if (s_switchLog3D < 20) { Trace("STATE", "View -> CAMERA (3D mode)"); s_switchLog3D++; }
        }
        // Log camera view matrix (first 5 sets only to avoid log spam)
        static int s_viewMatLog = 0;
        if (s_viewMatLog < 5) {
            const float* f = reinterpret_cast<const float*>(&m);
            Trace("CAM", "ViewMat row0=(%.3f,%.3f,%.3f,%.3f) row1=(%.3f,%.3f,%.3f,%.3f)",
                f[0],f[1],f[2],f[3], f[4],f[5],f[6],f[7]);
            Trace("CAM", "        row2=(%.3f,%.3f,%.3f,%.3f) row3=(%.3f,%.3f,%.3f,%.3f)",
                f[8],f[9],f[10],f[11], f[12],f[13],f[14],f[15]);
            s_viewMatLog++;
        }
    }
    if (transform == D3DTS_PROJECTION) {
        static int s_projMatLog = 0;
        if (s_projMatLog < 5) {
            const float* f = reinterpret_cast<const float*>(&m);
            Trace("CAM", "ProjMat row0=(%.3f,%.3f,%.3f,%.3f) row1=(%.3f,%.3f,%.3f,%.3f)",
                f[0],f[1],f[2],f[3], f[4],f[5],f[6],f[7]);
            Trace("CAM", "        row2=(%.3f,%.3f,%.3f,%.3f) row3=(%.3f,%.3f,%.3f,%.3f)",
                f[8],f[9],f[10],f[11], f[12],f[13],f[14],f[15]);
            s_projMatLog++;
        }
    }
}

void BGFXWrapper::Set_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix3D& m)
{
    Matrix4x4 m4(m);
    Set_Transform(transform, m4);
}

void BGFXWrapper::Get_Transform(D3DTRANSFORMSTATETYPE transform, Matrix4x4& m)
{
    m = DX8Transforms[transform];
}

void BGFXWrapper::_Set_DX8_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix4x4& m) { Set_Transform(transform, m); }
void BGFXWrapper::_Set_DX8_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix3D& m) { Set_Transform(transform, m); }
void BGFXWrapper::_Get_DX8_Transform(D3DTRANSFORMSTATETYPE transform, Matrix4x4& m) { Get_Transform(transform, m); }
void BGFXWrapper::Set_World_Identity() {
    world_identity = true;
    render_state_changed |= WORLD_IDENTITY;
    // Actually write identity into the transform array so SubmitDraw sees it.
    Matrix4x4 id(true);
    DX8Transforms[D3DTS_WORLD] = id;
}
void BGFXWrapper::Set_View_Identity() {
    render_state_changed |= VIEW_IDENTITY;
    Matrix4x4 id(true);
    DX8Transforms[D3DTS_VIEW] = id;
    bool was3D = !s_viewIsIdentity;
    s_viewIsIdentity = true;
    if (was3D) {
        // Reset texture stage states to DX8 defaults when switching to 2D.
        // Terrain rendering sets ALPHAOP=MODULATE which pollutes UI draws.
        // DX8 default: ALPHAOP=SELECTARG1, ALPHAARG1=TEXTURE (ignore vertex alpha).
        TextureStageStates[0][D3DTSS_ALPHAOP]   = D3DTOP_SELECTARG1;
        TextureStageStates[0][D3DTSS_ALPHAARG1]  = D3DTA_TEXTURE;
        TextureStageStates[0][D3DTSS_ALPHAARG2]  = D3DTA_CURRENT;
        // Also reset COLOROP to default: MODULATE (texture * vertex color)
        TextureStageStates[0][D3DTSS_COLOROP]    = D3DTOP_MODULATE;
        TextureStageStates[0][D3DTSS_COLORARG1]  = D3DTA_TEXTURE;
        TextureStageStates[0][D3DTSS_COLORARG2]  = D3DTA_DIFFUSE;
        static int s_switchLog = 0;
        if (s_switchLog < 20) { Trace("STATE", "View -> IDENTITY (2D mode) — reset TSS defaults"); s_switchLog++; }
    }
}
bool BGFXWrapper::Is_World_Identity() { return world_identity; }
bool BGFXWrapper::Is_View_Identity() { return (render_state_changed & VIEW_IDENTITY) != 0; }

// ============================================================================
// Texture Transform (BGFX-specific)
// ============================================================================
void BGFXWrapper::Set_Texture_Transform(int stage, const Matrix4x4& mat)
{
    if (stage >= 0 && stage < kMaxStages) s_texTransforms[stage] = mat;
}

void BGFXWrapper::Set_Texture_Coord_Source(int stage, int source)
{
    if (stage >= 0 && stage < kMaxStages) s_texCoordSources[stage] = source;
}

void BGFXWrapper::Set_Texture_Transform_Flags(int stage, int flags)
{
    if (stage >= 0 && stage < kMaxStages) s_texTransformFlags[stage] = flags;
}

// ============================================================================
// Shader Program Registry — public API
// ============================================================================
void BGFXWrapper::Set_Active_Shader_Type(int type)
{
    if (type >= 0 && type < SP_COUNT)
        s_activeProgram = type;
    else
        s_activeProgram = SP_MESH_DEFAULT;
}

int BGFXWrapper::Get_Active_Shader_Type()
{
    return s_activeProgram;
}

void BGFXWrapper::Upload_Texture_Uniforms()
{
    if (!s_bgfxReallyInitted) return;
    for (int i = 0; i < 4; ++i) {
        if (bgfx::isValid(s_uTexTransform[i]))
            bgfx::setUniform(s_uTexTransform[i], &s_texTransforms[i], 1);
    }
    float coordSources[4] = { float(s_texCoordSources[0]), float(s_texCoordSources[1]), float(s_texCoordSources[2]), float(s_texCoordSources[3]) };
    float transformFlags[4] = { float(s_texTransformFlags[0]), float(s_texTransformFlags[1]), float(s_texTransformFlags[2]), float(s_texTransformFlags[3]) };
    if (bgfx::isValid(s_uTexCoordSource)) bgfx::setUniform(s_uTexCoordSource, coordSources, 1);
    if (bgfx::isValid(s_uTexTransformFlags)) bgfx::setUniform(s_uTexTransformFlags, transformFlags, 1);
}

// ============================================================================
// Lighting
// ============================================================================
void BGFXWrapper::Set_DX8_Light(int index, D3DLIGHT8* light)
{
    if (index < 0 || index >= 4) return;
    render_state.Lights[index] = *light;
    render_state.LightEnable[index] = true;
    CurrentDX8LightEnables[index] = true;
    render_state_changed |= (LIGHT0_CHANGED << index);
}

void BGFXWrapper::Set_Light_Environment(LightEnvironmentClass* light_env) { Light_Environment = light_env; }
void BGFXWrapper::Set_Fog(bool enable, const Vector3& color, float start, float end) { FogEnable = enable; FogColor = D3DCOLOR_COLORVALUE(color.X, color.Y, color.Z, 1.0f); }
void BGFXWrapper::Set_Ambient(const Vector3& color) { Ambient_Color = color; }

// ============================================================================
// Render State
// ============================================================================
void BGFXWrapper::Set_DX8_Render_State(D3DRENDERSTATETYPE state, unsigned value) { RenderStates[state] = value; }
void BGFXWrapper::Set_DX8_Clip_Plane(DWORD Index, CONST float* pPlane) {}
void BGFXWrapper::Set_DX8_Texture_Stage_State(unsigned stage, D3DTEXTURESTAGESTATETYPE state, unsigned value) {
    if (stage < MAX_TEXTURE_STAGES) {
        TextureStageStates[stage][state] = value;
        // Log alpha-related stage state changes for debugging
        if (stage == 0 && (state == D3DTSS_ALPHAOP || state == D3DTSS_ALPHAARG1 || state == D3DTSS_ALPHAARG2)) {
            static int s_tssLog = 0;
            if (s_tssLog < 80) {
                const char* name = (state == D3DTSS_ALPHAOP) ? "ALPHAOP" :
                                   (state == D3DTSS_ALPHAARG1) ? "ALPHAARG1" : "ALPHAARG2";
                Trace("TSS", "stage0 %s = %u", name, value);
                s_tssLog++;
            }
        }
    }
}
void BGFXWrapper::Set_DX8_Texture(unsigned int stage, IDirect3DBaseTexture8* texture) { if (stage < MAX_TEXTURE_STAGES) { Textures[stage] = texture; render_state_changed |= (TEXTURE0_CHANGED << stage); } }
void BGFXWrapper::Set_DX8_Material(const D3DMATERIAL8* mat) { (void)mat; render_state_changed |= MATERIAL_CHANGED; }
void BGFXWrapper::Set_DX8_ZBias(int zbias) { ZBias = zbias; }
void BGFXWrapper::Set_Projection_Transform_With_Z_Bias(const Matrix4x4& matrix, float znear, float zfar) { ZNear = znear; ZFar = zfar; ProjectionMatrix = matrix; Set_Transform(D3DTS_PROJECTION, matrix); }
void BGFXWrapper::Set_Gamma(float gamma, float bright, float contrast, bool calibrate, bool uselimit) {}
bool BGFXWrapper::Validate_Device(void) { return true; }

void BGFXWrapper::Set_Shader(const ShaderClass& shader) {
    // ----------------------------------------------------------------
    // Translate ShaderClass bit-packed state into D3D render states.
    // SubmitDraw already reads these render states to configure bgfx.
    // ----------------------------------------------------------------

    // --- Alpha blending ---
    // Source blend LUT: ZERO→D3DBLEND_ZERO, ONE→D3DBLEND_ONE, SRC_ALPHA→D3DBLEND_SRCALPHA,
    //   ONE_MINUS_SRC_ALPHA→D3DBLEND_INVSRCALPHA (original game maps to DESTCOLOR but we use INVSRCALPHA)
    static const DWORD srcBlendD3D[] = { D3DBLEND_ZERO, D3DBLEND_ONE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA };
    // Dest blend LUT: ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, SRC_ALPHA, ONE_MINUS_SRC_ALPHA
    static const DWORD dstBlendD3D[] = { D3DBLEND_ZERO, D3DBLEND_ONE, D3DBLEND_SRCCOLOR, D3DBLEND_INVSRCCOLOR, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA };

    int srcIdx = (int)shader.Get_Src_Blend_Func();
    int dstIdx = (int)shader.Get_Dst_Blend_Func();
    DWORD sf = (srcIdx >= 0 && srcIdx < 4) ? srcBlendD3D[srcIdx] : D3DBLEND_ONE;
    DWORD df = (dstIdx >= 0 && dstIdx < 6) ? dstBlendD3D[dstIdx] : D3DBLEND_ZERO;

    BOOL blendOn = (sf != D3DBLEND_ONE || df != D3DBLEND_ZERO) ? TRUE : FALSE;

    // Color mask disable → disable all output
    if (shader.Get_Color_Mask() != ShaderClass::COLOR_WRITE_ENABLE) {
        blendOn = FALSE;
    }

    Set_DX8_Render_State(D3DRS_ALPHABLENDENABLE, blendOn);
    if (blendOn) {
        Set_DX8_Render_State(D3DRS_SRCBLEND, sf);
        Set_DX8_Render_State(D3DRS_DESTBLEND, df);
    }

    // --- Alpha test ---
    if (shader.Get_Alpha_Test() == ShaderClass::ALPHATEST_ENABLE) {
        Set_DX8_Render_State(D3DRS_ALPHATESTENABLE, TRUE);
        if (sf == D3DBLEND_INVSRCALPHA) {
            Set_DX8_Render_State(D3DRS_ALPHAREF, 0xFF - 0x60);
            Set_DX8_Render_State(D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL);
        } else {
            Set_DX8_Render_State(D3DRS_ALPHAREF, 0x60);
            Set_DX8_Render_State(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
        }
    } else {
        Set_DX8_Render_State(D3DRS_ALPHATESTENABLE, FALSE);
    }

    // --- Depth ---
    Set_DX8_Render_State(D3DRS_ZFUNC, (DWORD)(int(shader.Get_Depth_Compare()) + 1));
    Set_DX8_Render_State(D3DRS_ZWRITEENABLE, (DWORD)shader.Get_Depth_Mask());
}
void BGFXWrapper::Get_Shader(ShaderClass& shader) {}
void BGFXWrapper::Set_Texture(unsigned stage, TextureBaseClass* texture) {
    // Call Apply() which handles lazy init (loads DDS if needed) then calls
    // Set_DX8_Texture internally — without this render2d.cpp texture calls are no-ops.
    if (texture) {
        texture->Apply(stage);
    } else {
        Set_DX8_Texture(stage, nullptr);
    }
}
void BGFXWrapper::Set_Material(const VertexMaterialClass* material) {}
void BGFXWrapper::Set_Light(unsigned index, const D3DLIGHT8* light) { if (light) Set_DX8_Light(index, const_cast<D3DLIGHT8*>(light)); }
void BGFXWrapper::Set_Light(unsigned index, const LightClass& light) {}
void BGFXWrapper::Apply_Render_State_Changes() { render_state_changed = 0; }
void BGFXWrapper::Apply_Default_State() {}
void BGFXWrapper::Invalidate_Cached_Render_States(void) {}

// ============================================================================
// Buffers
// ============================================================================
void BGFXWrapper::Set_Vertex_Buffer(const VertexBufferClass* vb, unsigned stream) {
    if (stream < MAX_VERTEX_STREAMS) {
        render_state.vertex_buffers[stream]      = const_cast<VertexBufferClass*>(vb);
        render_state.vertex_buffer_types[stream] = vb ? vb->Type() : BUFFER_TYPE_INVALID;
        if (stream == 0) {
            render_state.vba_offset = 0;
            render_state.vba_count  = vb ? (unsigned short)vb->Get_Vertex_Count() : 0;
        }
    }
    render_state_changed |= VERTEX_BUFFER_CHANGED;
}
void BGFXWrapper::Set_Vertex_Buffer(const DynamicVBAccessClass& vba_) {
    DynamicVBAccessClass& vba           = const_cast<DynamicVBAccessClass&>(vba_);
    render_state.vertex_buffer_types[0] = vba.Get_Type();
    render_state.vba_offset             = vba.VertexBufferOffset;
    render_state.vba_count              = vba.Get_Vertex_Count();
    render_state.vertex_buffers[0]      = vba.VertexBuffer;
    render_state_changed |= VERTEX_BUFFER_CHANGED | INDEX_BUFFER_CHANGED;
}
void BGFXWrapper::Set_Index_Buffer(const IndexBufferClass* ib, unsigned short index_base_offset) {
    render_state.index_buffer      = const_cast<IndexBufferClass*>(ib);
    render_state.index_buffer_type = ib ? ib->Type() : BUFFER_TYPE_INVALID;
    render_state.index_base_offset = index_base_offset;
    render_state.iba_offset        = 0;
    render_state_changed |= INDEX_BUFFER_CHANGED;
}
void BGFXWrapper::Set_Index_Buffer(const DynamicIBAccessClass& iba_, unsigned short index_base_offset) {
    DynamicIBAccessClass& iba      = const_cast<DynamicIBAccessClass&>(iba_);
    render_state.index_base_offset = index_base_offset;
    render_state.index_buffer_type = iba.Get_Type();
    render_state.iba_offset        = iba.IndexBufferOffset;
    render_state.index_buffer      = iba.IndexBuffer;
    render_state_changed |= INDEX_BUFFER_CHANGED;
}
void BGFXWrapper::Get_Render_State(RenderStateStruct& state) { state = render_state; }
void BGFXWrapper::Set_Render_State(const RenderStateStruct& state) { render_state = state; }
void BGFXWrapper::Release_Render_State() {}

// ============================================================================
// Drawing
// ============================================================================

// Map D3DFORMAT → bgfx texture format.
// BGRA8 is the correct bgfx name for D3D's A8R8G8B8 / X8R8G8B8 (bytes: B G R A).
// BC1/2/3 are the bgfx names for DXT1/3/5.
static bgfx::TextureFormat::Enum D3DFormatToBGFX(D3DFORMAT fmt)
{
    switch (fmt) {
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:  return bgfx::TextureFormat::BGRA8;
    case D3DFMT_R5G6B5:    return bgfx::TextureFormat::R5G6B5;
    case D3DFMT_A1R5G5B5:  return bgfx::TextureFormat::RGB5A1;
    case D3DFMT_A4R4G4B4:  return bgfx::TextureFormat::RGBA4;
    case D3DFMT_DXT1:      return bgfx::TextureFormat::BC1;
    case D3DFMT_DXT2:
    case D3DFMT_DXT3:      return bgfx::TextureFormat::BC2;
    case D3DFMT_DXT5:      return bgfx::TextureFormat::BC3;
    default:               return bgfx::TextureFormat::BGRA8;
    }
}

// Compute the actual byte size of texture pixel data for the given surface.
// For DXT (block-compressed) formats this is much smaller than pitch×height.
// ddsfile.cpp ignores pitch when copying DXT data and uses a flat memcpy of
// exactly this many bytes starting at surf->m_data[0].
static uint32_t TextureDataSize(const BGFXSurface8* surf)
{
    uint32_t w = (uint32_t)surf->m_width;
    uint32_t h = (uint32_t)surf->m_height;
    switch (surf->m_format) {
    case D3DFMT_DXT1:
        // 8 bytes per 4×4 block
        return ((w + 3) / 4) * ((h + 3) / 4) * 8;
    case D3DFMT_DXT2: case D3DFMT_DXT3:
    case D3DFMT_DXT4: case D3DFMT_DXT5:
        // 16 bytes per 4×4 block
        return ((w + 3) / 4) * ((h + 3) / 4) * 16;
    default:
        return (uint32_t)surf->m_pitch * h;
    }
}

// Convert a 16-bit surface to a 32-bit BGRA8 buffer.
// D3D11 dropped support for most 16-bit formats (RGBA4, RGB5A1, R5G6B5 can be
// problematic or have wrong component order vs. D3D's naming).  Converting on
// the CPU is safe and simple.
static const bgfx::Memory* Convert16To32(const BGFXSurface8* surf)
{
    uint32_t n = (uint32_t)surf->m_width * (uint32_t)surf->m_height;
    const bgfx::Memory* mem = bgfx::alloc(n * 4);
    uint8_t* dst = mem->data;
    const uint16_t* src = (const uint16_t*)surf->m_data;

    switch (surf->m_format) {
    case D3DFMT_A4R4G4B4:
        // D3D: bits 15-12=A, 11-8=R, 7-4=G, 3-0=B  → BGRA8
        for (uint32_t i = 0; i < n; i++, src++, dst += 4) {
            uint16_t px = *src;
            uint8_t a = (px >> 12) & 0xF, r = (px >> 8) & 0xF;
            uint8_t g = (px >>  4) & 0xF, b = (px >> 0) & 0xF;
            dst[0] = (b << 4) | b;  dst[1] = (g << 4) | g;
            dst[2] = (r << 4) | r;  dst[3] = (a << 4) | a;
        }
        break;
    case D3DFMT_A1R5G5B5:
        // D3D: bit 15=A, 14-10=R, 9-5=G, 4-0=B  → BGRA8
        for (uint32_t i = 0; i < n; i++, src++, dst += 4) {
            uint16_t px = *src;
            uint8_t a = (px >> 15) & 0x1;
            uint8_t r = (px >> 10) & 0x1F, g = (px >> 5) & 0x1F, b = (px >> 0) & 0x1F;
            dst[0] = (b << 3) | (b >> 2);  dst[1] = (g << 3) | (g >> 2);
            dst[2] = (r << 3) | (r >> 2);  dst[3] = a ? 0xFF : 0x00;
        }
        break;
    case D3DFMT_R5G6B5:
        // D3D: bits 15-11=R, 10-5=G, 4-0=B  → BGRA8 (opaque)
        for (uint32_t i = 0; i < n; i++, src++, dst += 4) {
            uint16_t px = *src;
            uint8_t r = (px >> 11) & 0x1F, g = (px >> 5) & 0x3F, b = (px >> 0) & 0x1F;
            dst[0] = (b << 3) | (b >> 2);  dst[1] = (g << 2) | (g >> 4);
            dst[2] = (r << 3) | (r >> 2);  dst[3] = 0xFF;
        }
        break;
    default:
        break;
    }
    return mem;
}

// Upload a BGFXSurface8 to GPU, creating or updating the bgfx texture handle.
// Returns the handle idx (0xFFFF on failure).
static unsigned short UploadSurface(BGFXTexture8* tex)
{
    BGFXSurface8* surf = tex->m_surface;
    if (!surf || !surf->m_data) return tex->m_bgfxIdx;

    uint32_t sz = TextureDataSize(surf);
    if (sz == 0) return tex->m_bgfxIdx;

    // For X8R8G8B8, force alpha=0xFF so the "X" channel doesn't make quads transparent.
    if (surf->m_format == D3DFMT_X8R8G8B8) {
        uint8_t* p = (uint8_t*)surf->m_data;
        uint32_t fullSz = (uint32_t)surf->m_pitch * surf->m_height;
        for (uint32_t i = 3; i < fullSz; i += 4)
            p[i] = 0xFF;
    }

    // 16-bit formats: convert to BGRA8 on CPU (avoids D3D11 format support issues
    // and ensures correct component ordering vs. D3D's naming convention).
    bool needs16to32 = (surf->m_format == D3DFMT_A4R4G4B4 ||
                        surf->m_format == D3DFMT_A1R5G5B5 ||
                        surf->m_format == D3DFMT_R5G6B5);
    bgfx::TextureFormat::Enum fmt;
    const bgfx::Memory* mem;
    if (needs16to32) {
        fmt = bgfx::TextureFormat::BGRA8;
        mem = Convert16To32(surf);
    } else {
        fmt = D3DFormatToBGFX(surf->m_format);
        mem = bgfx::copy(surf->m_data, sz);
    }

    bool isCreate = (tex->m_bgfxIdx == 0xFFFF);

    if (isCreate) {
        // Try to reuse a cached handle (same dimensions) to avoid handle churn.
        unsigned short cached = TexHandleCache::Take(surf->m_width, surf->m_height);
        bgfx::TextureHandle th;
        if (cached != 0xFFFF) {
            // Reuse cached handle — just update its content.
            th.idx = cached;
            tex->m_bgfxIdx = cached;
            bgfx::updateTexture2D(th, 0, 0, 0, 0,
                (uint16_t)surf->m_width, (uint16_t)surf->m_height, mem);
            static int s_reuseLog = 0;
            if (s_reuseLog < 50) {
                Trace("TEX", "REUSE %ux%u d3dfmt=%d idx=%u (cached)",
                    (unsigned)surf->m_width, (unsigned)surf->m_height,
                    (int)surf->m_format, (unsigned)cached);
                s_reuseLog++;
            }
        } else {
            // No cached handle — create new mutable texture.
            th = bgfx::createTexture2D(
                (uint16_t)surf->m_width, (uint16_t)surf->m_height,
                false, 1, fmt, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, NULL);
            tex->m_bgfxIdx = th.idx;
            if (bgfx::isValid(th)) {
                bgfx::updateTexture2D(th, 0, 0, 0, 0,
                    (uint16_t)surf->m_width, (uint16_t)surf->m_height, mem);
            }
            static int s_createLog = 0;
            if (s_createLog < 200 || !bgfx::isValid(th)) {
                Trace("TEX", "CREATE %ux%u d3dfmt=%d bgfxfmt=%d sz=%u => idx=%u valid=%d",
                    (unsigned)surf->m_width, (unsigned)surf->m_height,
                    (int)surf->m_format, (int)fmt, sz,
                    (unsigned)th.idx, bgfx::isValid(th) ? 1 : 0);
                s_createLog++;
            }
        }
    } else {
        // Texture is mutable — just update content in place (handle stays stable).
        bgfx::TextureHandle th; th.idx = tex->m_bgfxIdx;
        bgfx::updateTexture2D(th, 0, 0, 0, 0,
            (uint16_t)surf->m_width, (uint16_t)surf->m_height, mem);
        static int s_updateLog = 0;
        if (s_updateLog < 100) {
            Trace("TEX", "UPDATE %ux%u d3dfmt=%d idx=%u",
                (unsigned)surf->m_width, (unsigned)surf->m_height,
                (int)surf->m_format, (unsigned)th.idx);
            s_updateLog++;
        }
    }
    tex->m_bgfxDirty = false;
    surf->m_dirty     = false;
    return tex->m_bgfxIdx;
}

// CPU World×View×Proj transform using WW3D column-vector convention: v' = M * v.
// Matrix4x4 is stored row-major: flat[row*4+col] = M[row][col].
// Translation lives in column 3 (indices [3],[7],[11]).
// Inputs: model-space (x,y,z), outputs: NDC (ox,oy,oz) and clip-w (ow).
static inline void TransformWVP(
    const float* world, const float* view, const float* proj,
    float x, float y, float z,
    float& ox, float& oy, float& oz, float& ow)
{
    // world * v  (column-vector: result_i = sum_j(M[i][j] * v[j]))
    float wx = world[0]*x + world[1]*y + world[2]*z  + world[3];
    float wy = world[4]*x + world[5]*y + world[6]*z  + world[7];
    float wz = world[8]*x + world[9]*y + world[10]*z + world[11];
    float ww = world[12]*x + world[13]*y + world[14]*z + world[15];
    // view * (world * v)
    float vx = view[0]*wx + view[1]*wy + view[2]*wz  + view[3]*ww;
    float vy = view[4]*wx + view[5]*wy + view[6]*wz  + view[7]*ww;
    float vz = view[8]*wx + view[9]*wy + view[10]*wz + view[11]*ww;
    float vw = view[12]*wx + view[13]*wy + view[14]*wz + view[15]*ww;
    // proj * (view * world * v)
    float cx = proj[0]*vx + proj[1]*vy + proj[2]*vz  + proj[3]*vw;
    float cy = proj[4]*vx + proj[5]*vy + proj[6]*vz  + proj[7]*vw;
    float cz = proj[8]*vx + proj[9]*vy + proj[10]*vz + proj[11]*vw;
    float cw = proj[12]*vx + proj[13]*vy + proj[14]*vz + proj[15]*vw;
    // Perspective divide → NDC
    if (fabsf(cw) > 1e-6f) { ox = cx/cw; oy = cy/cw; oz = cz/cw; }
    else                    { ox = 0.0f;  oy = 0.0f;  oz = 0.0f;  }
    ow = cw;
}

// Row-major 4×4 matrix multiply: C = A * B  (C[r][c] = sum_k A[r][k] * B[k][c])
// Used to compute WVP = proj * (view * world) for the GPU uniform upload.
static inline void MatMul4x4(const float* A, const float* B, float* C)
{
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++) {
            float s = 0.f;
            for (int k = 0; k < 4; k++)
                s += A[r*4+k] * B[k*4+c];
            C[r*4+c] = s;
        }
}

// Helper: submit one draw batch to bgfx using transient vertex + index buffers.
// Vertices are uploaded in model-space; the VS multiplies by u_wvp (proj*view*world).
// world/view/proj are row-major float[16] from DX8Transforms (passed by caller).
static void SubmitDraw(const BYTE* vdata, unsigned vb_start, unsigned v_count,
                       const uint16_t* src_idx, unsigned i_count, unsigned min_vi,
                       bool strip,
                       const float* world, const float* view, const float* proj)
{
    if (!s_bgfxReallyInitted || !bgfx::isValid(s_meshProgram)) return;
    if (v_count == 0 || i_count == 0) return;

    const VertexFormatXYZNDUV2* src = (const VertexFormatXYZNDUV2*)(vdata + vb_start * sizeof(VertexFormatXYZNDUV2));

    // -----------------------------------------------------------------------
    // Filter: skip draws where vertex positions contain ±inf or NaN.
    // Render2DClass writes ±inf when Set_Coordinate_Range() is called with a
    // zero-sized range (before screen dimensions are known). Submitting these
    // to the GPU wastes transient buffer space and produces no useful pixels.
    // -----------------------------------------------------------------------
    bool all_finite = true;
    for (unsigned k = 0; k < v_count; k++) {
        if (!std::isfinite(src[k].x) || !std::isfinite(src[k].y)) {
            all_finite = false;
            break;
        }
    }

    // Counters persist across calls (static local)
    static int s_totalDraws  = 0;
    static int s_finiteDraws = 0;
    static int s_infDraws    = 0;
    ++s_totalDraws;

    if (!all_finite) {
        ++s_infDraws;
        ++s_frameReject;
        static int s_infLogCount = 0;
        if (s_infLogCount < 3) {
            Trace("DRAW", "REJECT inf/nan v=%u i=%u vb=%u v0=(%.2f,%.2f,%.2f)",
                v_count, i_count, vb_start,
                v_count > 0 ? src[0].x : 0.f, v_count > 0 ? src[0].y : 0.f, v_count > 0 ? src[0].z : 0.f);
            ++s_infLogCount;
        }
        return;
    }
    ++s_finiteDraws;

    // Per-frame counters
    static bool s_seen3D = false;
    if (s_viewIsIdentity) {
        s_frame2D++;
        if (v_count > 8) s_frameText++;
        // Log vertex data for 2D draws — dump positions/alpha for first 60 after 3D starts
        if (s_seen3D) {
            static int s_vtxLog = 0;
            if (s_vtxLog < 60) {
                uint32_t d0 = src[0].diffuse;
                unsigned alphaOp = s_TSS ? s_TSS[0][D3DTSS_ALPHAOP] : D3DTOP_SELECTARG1;
                unsigned alphaArg1 = s_TSS ? s_TSS[0][D3DTSS_ALPHAARG1] : D3DTA_TEXTURE;
                Trace("VTXDBG", "2D v=%u pos0=(%.3f,%.3f) pos1=(%.3f,%.3f) diffuse=0x%08X aop=%u aarg=%u tex=%s",
                    v_count, src[0].x, src[0].y,
                    v_count > 1 ? src[1].x : 0.f, v_count > 1 ? src[1].y : 0.f,
                    d0, alphaOp, alphaArg1,
                    (s_TX && s_TX[0]) ? "YES" : "NO");
                s_vtxLog++;
            }
        }
    } else {
        s_frame3D++;
        s_seen3D = true;
    }

    // Concise draw logging: first 30, then every 1000th, plus first large draw
    static int s_validLogCount = 0;
    s_validLogCount++;
    bool logThis = (s_validLogCount <= 30 || (s_validLogCount % 1000 == 0));
    if (logThis) {
        bool is2D = s_viewIsIdentity;
        Trace("DRAW", "#%d %s v=%u i=%u blend=%d src=%d dst=%d zen=%d zwr=%d viewId=%s",
            s_validLogCount, (v_count >= 4096 ? "TERRAIN" : (is2D ? "2D" : "3D")),
            v_count, i_count,
            s_RS ? (int)s_RS[D3DRS_ALPHABLENDENABLE] : -1,
            s_RS ? (int)s_RS[D3DRS_SRCBLEND] : -1,
            s_RS ? (int)s_RS[D3DRS_DESTBLEND] : -1,
            s_RS ? (int)s_RS[D3DRS_ZENABLE] : -1,
            s_RS ? (int)s_RS[D3DRS_ZWRITEENABLE] : -1,
            is2D ? "1(UI)" : "0(3D)");
    }

    // BGFX: UI visibility diagnosis — log first 200 2D draws after terrain has loaded
    if (s_viewIsIdentity && s_seen3D) {
        static int s_ui2dCount = 0;
        if (s_ui2dCount < 200) {
            s_ui2dCount++;
            Trace("UI2D", "#%d v=%u diffuse=0x%08X alphaOp=%u colorOp=%u colorArg1=%u tex=%s",
                s_ui2dCount, v_count, src[0].diffuse,
                s_TSS ? s_TSS[0][D3DTSS_ALPHAOP]   : 99u,
                s_TSS ? s_TSS[0][D3DTSS_COLOROP]   : 99u,
                s_TSS ? s_TSS[0][D3DTSS_COLORARG1] : 99u,
                (s_TX && s_TX[0]) ? "REAL" : "DEFAULT");
        }
    }

    // Check BOTH transient pools before allocating EITHER. Allocating the vertex
    // buffer first and then failing the index check wastes the vertex allocation
    // and drops the draw regardless. Also log it: a silent `return` here is
    // geometry vanishing with no trace, and it gets more likely the busier the
    // scene gets, which is the worst possible failure signature to debug.
    if (bgfx::getAvailTransientVertexBuffer(v_count, s_layoutXYZNDUV2) < v_count ||
        bgfx::getAvailTransientIndexBuffer(i_count) < i_count) {
        ++s_frameReject;
        static int s_dropLog = 0;
        if (s_dropLog < 30) {
            ++s_dropLog;
            Trace("DRAW", "DROP transient exhausted: need v=%u i=%u avail v=%u i=%u",
                v_count, i_count,
                bgfx::getAvailTransientVertexBuffer(v_count, s_layoutXYZNDUV2),
                bgfx::getAvailTransientIndexBuffer(i_count));
        }
        return;
    }
    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(&tvb, v_count, s_layoutXYZNDUV2);

    VertexFormatXYZNDUV2* dst = (VertexFormatXYZNDUV2*)tvb.data;
    for (unsigned k = 0; k < v_count; k++) {
        dst[k] = src[k];
        // DX8 fixed-function: when stage 0 ops select the texture channel directly
        // (SELECTARG1+TEXTURE), the vertex color/alpha channel is irrelevant — DX8
        // ignores it.  Our BGFX shader always multiplies texture × vertex, so we
        // force the vertex channel to its identity value (0xFF) in those modes so
        // the multiply has no effect.  When MODULATE, vertex values are intentional.
        {
            unsigned colorOp   = s_TSS ? s_TSS[0][D3DTSS_COLOROP]   : D3DTOP_MODULATE;
            unsigned colorArg1 = s_TSS ? s_TSS[0][D3DTSS_COLORARG1] : D3DTA_TEXTURE;
            unsigned alphaOp   = s_TSS ? s_TSS[0][D3DTSS_ALPHAOP]   : D3DTOP_SELECTARG1;
            unsigned alphaArg1 = s_TSS ? s_TSS[0][D3DTSS_ALPHAARG1] : D3DTA_TEXTURE;
            bool selectTexRGB  = (colorOp == D3DTOP_SELECTARG1 && (colorArg1 & 0x0F) == D3DTA_TEXTURE);
            bool selectTexA    = (alphaOp == D3DTOP_SELECTARG1 && (alphaArg1 & 0x0F) == D3DTA_TEXTURE);
            if (selectTexRGB)                      dst[k].diffuse |= 0x00FFFFFF;  // RGB identity
            if (selectTexA || (dst[k].diffuse >> 24) == 0) {
                if (selectTexA)                    dst[k].diffuse |= 0xFF000000;  // A  identity
            }
        }
    }

    // Availability was already checked above, alongside the vertex pool.
    bgfx::TransientIndexBuffer tib;
    bgfx::allocTransientIndexBuffer(&tib, i_count);
    uint16_t* idst = reinterpret_cast<uint16_t*>(tib.data);

    // Rebase absolute VB indices onto the transient buffer, which starts at the
    // draw's first vertex. An index below min_vi underflows to ~65535 and one at
    // or past v_count points outside the buffer we just allocated; either makes
    // the GPU fetch vertices that are not ours. Validate rather than submit it.
    bool idx_ok = true;
    for (unsigned k = 0; k < i_count; ++k) {
        const unsigned raw = src_idx[k];
        if (raw < min_vi || (raw - min_vi) >= v_count) { idx_ok = false; break; }
        idst[k] = (uint16_t)(raw - min_vi);
    }
    if (!idx_ok) {
        ++s_frameReject;
        static int s_idxLog = 0;
        if (s_idxLog < 20) {
            ++s_idxLog;
            Trace("DRAW", "REJECT index out of range: i=%u v=%u min_vi=%u i0=%u",
                i_count, v_count, min_vi, (unsigned)src_idx[0]);
        }
        return;
    }

    bgfx::setVertexBuffer(0, &tvb);
    bgfx::setIndexBuffer(&tib);

    // -----------------------------------------------------------------------
    // Resolve stage-0 texture: upload if dirty, bind GPU handle (or fallback).
    // -----------------------------------------------------------------------
    bgfx::TextureHandle bindTex = s_defaultTexture;
    if (s_TX && s_TX[0]) {
        BGFXTexture8* tex = static_cast<BGFXTexture8*>(s_TX[0]);
        BGFXSurface8* surf = tex->m_surface;
        if (surf && surf->m_data && (tex->m_bgfxDirty || surf->m_dirty))
            UploadSurface(tex);
        if (tex->m_bgfxIdx != 0xFFFF) { bindTex.idx = tex->m_bgfxIdx; }
    }
    // Log texture binding for first 100 draws + all terrain draws (v>=4096)
    {
        static int s_texLogCount = 0;
        bool isTerrain = (v_count >= 4096);
        if (s_texLogCount < 100 || isTerrain) {
            BGFXTexture8* dbgTex = (s_TX && s_TX[0]) ? static_cast<BGFXTexture8*>(s_TX[0]) : nullptr;
            bool isDefault = !bgfx::isValid(bindTex) || bindTex.idx == s_defaultTexture.idx;
            static int s_terrTexLog = 0;
            if (s_texLogCount < 100 || (isTerrain && s_terrTexLog < 30)) {
                Trace("TEX", "bind #%d idx=0x%X %s v=%u tex=%p surf=%p",
                    s_texLogCount,
                    dbgTex ? dbgTex->m_bgfxIdx : 0xFFFF,
                    isDefault ? "DEFAULT" : "REAL", v_count,
                    dbgTex, dbgTex ? dbgTex->m_surface : nullptr);
                if (isTerrain) s_terrTexLog++;
            }
            ++s_texLogCount;
        }
    }

    if (bgfx::isValid(s_texSampler)) bgfx::setTexture(0, s_texSampler, bindTex);

    // For terrain draws: bind stage-1 (blend) texture so the terrain shader can
    // lerp between base (uv0) and blend (uv1) using the vertex alpha blend factor.
    // The game sets both stage 0 and stage 1 to the same terrain atlas texture
    // (m_stageZeroTexture) but with different per-vertex UV coordinates.
    if (s_activeProgram == BGFXWrapper::SP_TERRAIN_BASE && bgfx::isValid(s_texSamplerB)) {
        bgfx::TextureHandle bindTex1 = s_defaultTexture;
        if (s_TX && s_TX[1]) {
            BGFXTexture8* tex1 = static_cast<BGFXTexture8*>(s_TX[1]);
            BGFXSurface8* surf1 = tex1->m_surface;
            if (surf1 && surf1->m_data && (tex1->m_bgfxDirty || surf1->m_dirty))
                UploadSurface(tex1);
            if (tex1->m_bgfxIdx != 0xFFFF) bindTex1.idx = tex1->m_bgfxIdx;
        }
        bgfx::setTexture(1, s_texSamplerB, bindTex1);
    }

    // -----------------------------------------------------------------------
    // Build bgfx state from cached DX8 render states.
    // -----------------------------------------------------------------------
    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;

    // Alpha blending
    if (s_RS && s_RS[D3DRS_ALPHABLENDENABLE]) {
        auto d3dBlend = [](unsigned d) -> uint64_t {
            switch (d) {
                case D3DBLEND_ZERO:         return BGFX_STATE_BLEND_ZERO;
                case D3DBLEND_ONE:          return BGFX_STATE_BLEND_ONE;
                case D3DBLEND_SRCALPHA:     return BGFX_STATE_BLEND_SRC_ALPHA;
                case D3DBLEND_INVSRCALPHA:  return BGFX_STATE_BLEND_INV_SRC_ALPHA;
                case D3DBLEND_SRCCOLOR:     return BGFX_STATE_BLEND_SRC_COLOR;
                case D3DBLEND_INVSRCCOLOR:  return BGFX_STATE_BLEND_INV_SRC_COLOR;
                case D3DBLEND_DESTALPHA:    return BGFX_STATE_BLEND_DST_ALPHA;
                case D3DBLEND_INVDESTALPHA: return BGFX_STATE_BLEND_INV_DST_ALPHA;
                case D3DBLEND_DESTCOLOR:    return BGFX_STATE_BLEND_DST_COLOR;
                case D3DBLEND_INVDESTCOLOR: return BGFX_STATE_BLEND_INV_DST_COLOR;
                default:                    return BGFX_STATE_BLEND_ONE;
            }
        };
        state |= BGFX_STATE_BLEND_FUNC(
            d3dBlend(s_RS[D3DRS_SRCBLEND]),
            d3dBlend(s_RS[D3DRS_DESTBLEND]));
    }

    // Depth test / depth write — ONLY for 3D draws.
    // 2D UI must never depth-test or it will be rejected by terrain's depth buffer.
    if (s_RS && s_RS[D3DRS_ZENABLE] && !s_viewIsIdentity) {
        state |= BGFX_STATE_DEPTH_TEST_LESS;
        if (s_RS[D3DRS_ZWRITEENABLE]) state |= BGFX_STATE_WRITE_Z;
    }

    if (strip) state |= BGFX_STATE_PT_TRISTRIP;
    bgfx::setState(state);

    // Submit to view 1 (2D overlay) when view is identity, view 0 (3D scene) otherwise.
    // BGFX renders views in order, so view 1 always draws on top of view 0.
    bgfx::ViewId viewId = s_viewIsIdentity ? 1 : 0;

    // Upload WVP = proj * view * world as a row-major mat4 uniform.
    // The VS uses this to transform model-space positions to clip-space on GPU,
    // avoiding the CPU perspective-divide sign-flip for behind-camera vertices.
    if (bgfx::isValid(s_uWVP)) {
        float vw[16], wvp[16];
        MatMul4x4(view, world, vw);
        MatMul4x4(proj, vw, wvp);
        bgfx::setUniform(s_uWVP, wvp);
    }

    // Select program from registry. UI draws always use the default mesh program so
    // that the terrain program (with its extra varyings) doesn't affect 2D rendering.
    int progIdx = s_viewIsIdentity ? BGFXWrapper::SP_MESH_DEFAULT : s_activeProgram;
    bgfx::ProgramHandle prog = s_programs[progIdx];
    if (!bgfx::isValid(prog)) prog = s_meshProgram; // fallback guard
    bgfx::submit(viewId, prog);
}

void BGFXWrapper::Draw_Sorting_IB_VB(
    unsigned primitive_type, unsigned short start_index,
    unsigned short polygon_count, unsigned short min_vertex_index, unsigned short vertex_count)
{
    if (!s_bgfxReallyInitted || !bgfx::isValid(s_meshProgram)) return;
    if (polygon_count == 0 || vertex_count == 0) return;

    const VertexFormatXYZNDUV2* src_verts =
        static_cast<SortingVertexBufferClass*>(render_state.vertex_buffers[0])->VertexBuffer
        + render_state.vba_offset + render_state.index_base_offset + min_vertex_index;

    const uint16_t* src_idx =
        static_cast<SortingIndexBufferClass*>(render_state.index_buffer)->index_buffer
        + render_state.iba_offset + start_index;

    unsigned i_count = (primitive_type == D3DPT_TRIANGLELIST) ? polygon_count * 3u : polygon_count + 2u;

    const float* world = reinterpret_cast<const float*>(&DX8Transforms[D3DTS_WORLD]);
    const float* view  = reinterpret_cast<const float*>(&DX8Transforms[D3DTS_VIEW]);
    const float* proj  = reinterpret_cast<const float*>(&DX8Transforms[D3DTS_PROJECTION]);

    // Filter: skip if any vertex has non-finite positions (same as SubmitDraw)
    bool all_finite = true;
    for (unsigned k = 0; k < vertex_count; k++) {
        if (!std::isfinite(src_verts[k].x) || !std::isfinite(src_verts[k].y)) {
            all_finite = false;
            break;
        }
    }
    // Track sorting path separately
    static int s_sortTotal = 0; static int s_sortFinite = 0; static int s_sortInf = 0;
    ++s_sortTotal;
    if (!all_finite) {
        ++s_sortInf;
        if (s_sortTotal % 1000 == 0) {
            Trace("SORT", "stats total=%d finite=%d inf=%d", s_sortTotal, s_sortFinite, s_sortInf);
        }
        return;
    }
    ++s_sortFinite;
    static int s_sortValidLog = 0;
    if (s_sortValidLog < 3) {
        Trace("SORT", "valid #%d v=%u src=(%.3f,%.3f,%.3f)",
            s_sortValidLog, vertex_count, src_verts[0].x, src_verts[0].y, src_verts[0].z);
        ++s_sortValidLog;
    }

    if (bgfx::getAvailTransientVertexBuffer(vertex_count, s_layoutXYZNDUV2) < vertex_count) return;
    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(&tvb, vertex_count, s_layoutXYZNDUV2);

    VertexFormatXYZNDUV2* dst_verts = (VertexFormatXYZNDUV2*)tvb.data;
    for (unsigned k = 0; k < vertex_count; k++) {
        dst_verts[k] = src_verts[k];
        // Same SELECTARG1 identity fix as SubmitDraw.
        {
            unsigned colorOp   = s_TSS ? s_TSS[0][D3DTSS_COLOROP]   : D3DTOP_MODULATE;
            unsigned colorArg1 = s_TSS ? s_TSS[0][D3DTSS_COLORARG1] : D3DTA_TEXTURE;
            unsigned alphaOp   = s_TSS ? s_TSS[0][D3DTSS_ALPHAOP]   : D3DTOP_SELECTARG1;
            unsigned alphaArg1 = s_TSS ? s_TSS[0][D3DTSS_ALPHAARG1] : D3DTA_TEXTURE;
            bool selectTexRGB  = (colorOp == D3DTOP_SELECTARG1 && (colorArg1 & 0x0F) == D3DTA_TEXTURE);
            bool selectTexA    = (alphaOp == D3DTOP_SELECTARG1 && (alphaArg1 & 0x0F) == D3DTA_TEXTURE);
            if (selectTexRGB)                            dst_verts[k].diffuse |= 0x00FFFFFF;
            if (selectTexA || (dst_verts[k].diffuse >> 24) == 0) {
                if (selectTexA)                          dst_verts[k].diffuse |= 0xFF000000;
            }
        }
    }

    if (bgfx::getAvailTransientIndexBuffer(i_count) < i_count) return;
    bgfx::TransientIndexBuffer tib;
    bgfx::allocTransientIndexBuffer(&tib, i_count);
    uint16_t* idst = reinterpret_cast<uint16_t*>(tib.data);
    for (unsigned k = 0; k < i_count; ++k)
        idst[k] = (uint16_t)(src_idx[k] - min_vertex_index);

    bgfx::setVertexBuffer(0, &tvb);
    bgfx::setIndexBuffer(&tib);

    // Bind texture (same logic as SubmitDraw — upload if dirty, then bind)
    bgfx::TextureHandle bindTex = s_defaultTexture;
    if (Textures[0]) {
        BGFXTexture8* tex = static_cast<BGFXTexture8*>(Textures[0]);
        BGFXSurface8* surf = tex->m_surface;
        if (surf && surf->m_data && (tex->m_bgfxDirty || surf->m_dirty))
            UploadSurface(tex);
        if (tex->m_bgfxIdx != 0xFFFF) { bindTex.idx = tex->m_bgfxIdx; }
    }
    if (bgfx::isValid(s_texSampler)) bgfx::setTexture(0, s_texSampler, bindTex);

    // Terrain blend texture (stage 1) — same as SubmitDraw path.
    if (s_activeProgram == BGFXWrapper::SP_TERRAIN_BASE && bgfx::isValid(s_texSamplerB)) {
        bgfx::TextureHandle bindTex1 = s_defaultTexture;
        if (Textures[1]) {
            BGFXTexture8* tex1 = static_cast<BGFXTexture8*>(Textures[1]);
            BGFXSurface8* surf1 = tex1->m_surface;
            if (surf1 && surf1->m_data && (tex1->m_bgfxDirty || surf1->m_dirty))
                UploadSurface(tex1);
            if (tex1->m_bgfxIdx != 0xFFFF) bindTex1.idx = tex1->m_bgfxIdx;
        }
        bgfx::setTexture(1, s_texSamplerB, bindTex1);
    }

    // Upload WVP uniform (same GPU-side transform as SubmitDraw).
    if (bgfx::isValid(s_uWVP)) {
        float vw[16], wvp[16];
        MatMul4x4(view, world, vw);
        MatMul4x4(proj, vw, wvp);
        bgfx::setUniform(s_uWVP, wvp);
    }

    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;
    if (RenderStates[D3DRS_ALPHABLENDENABLE]) {
        auto d3dBlend = [](unsigned d) -> uint64_t {
            switch (d) {
                case D3DBLEND_ZERO:         return BGFX_STATE_BLEND_ZERO;
                case D3DBLEND_ONE:          return BGFX_STATE_BLEND_ONE;
                case D3DBLEND_SRCALPHA:     return BGFX_STATE_BLEND_SRC_ALPHA;
                case D3DBLEND_INVSRCALPHA:  return BGFX_STATE_BLEND_INV_SRC_ALPHA;
                case D3DBLEND_SRCCOLOR:     return BGFX_STATE_BLEND_SRC_COLOR;
                case D3DBLEND_INVSRCCOLOR:  return BGFX_STATE_BLEND_INV_SRC_COLOR;
                case D3DBLEND_DESTALPHA:    return BGFX_STATE_BLEND_DST_ALPHA;
                case D3DBLEND_INVDESTALPHA: return BGFX_STATE_BLEND_INV_DST_ALPHA;
                case D3DBLEND_DESTCOLOR:    return BGFX_STATE_BLEND_DST_COLOR;
                case D3DBLEND_INVDESTCOLOR: return BGFX_STATE_BLEND_INV_DST_COLOR;
                default:                    return BGFX_STATE_BLEND_ONE;
            }
        };
        state |= BGFX_STATE_BLEND_FUNC(
            d3dBlend(RenderStates[D3DRS_SRCBLEND]),
            d3dBlend(RenderStates[D3DRS_DESTBLEND]));
    }
    // Depth test only for 3D draws — 2D UI must never depth-test.
    if (RenderStates[D3DRS_ZENABLE] && !s_viewIsIdentity) {
        state |= BGFX_STATE_DEPTH_TEST_LESS;
        if (RenderStates[D3DRS_ZWRITEENABLE]) state |= BGFX_STATE_WRITE_Z;
    }
    if (primitive_type != D3DPT_TRIANGLELIST) state |= BGFX_STATE_PT_TRISTRIP;
    bgfx::setState(state);
    bgfx::ViewId viewId = s_viewIsIdentity ? 1 : 0;
    bgfx::submit(viewId, s_meshProgram);
}

void BGFXWrapper::Draw(
    unsigned primitive_type, unsigned short start_index,
    unsigned short polygon_count, unsigned short min_vertex_index, unsigned short vertex_count)
{
    if (!s_bgfxReallyInitted || !bgfx::isValid(s_meshProgram)) return;
    if (!_EnableTriangleDraw || polygon_count == 0) return;

    VertexBufferClass* vb = render_state.vertex_buffers[0];
    IndexBufferClass*  ib = render_state.index_buffer;
    if (!vb || !ib) return;

    unsigned vb_type = render_state.vertex_buffer_types[0];
    unsigned ib_type = render_state.index_buffer_type;

    // Sorting buffer path
    if (vb_type == BUFFER_TYPE_SORTING || vb_type == BUFFER_TYPE_DYNAMIC_SORTING) {
        Draw_Sorting_IB_VB(primitive_type, start_index, polygon_count, min_vertex_index, vertex_count);
        return;
    }

    if ((vb_type != BUFFER_TYPE_DX8 && vb_type != BUFFER_TYPE_DYNAMIC_DX8) ||
        (ib_type != BUFFER_TYPE_DX8 && ib_type != BUFFER_TYPE_DYNAMIC_DX8)) return;

    // Get stride and FVF info for vertex format detection
    const FVFInfoClass& fvfInfo = static_cast<DX8VertexBufferClass*>(vb)->FVF_Info();
    unsigned stride = fvfInfo.Get_FVF_Size();
    unsigned fvfFlags = fvfInfo.Get_FVF();

    // Log all vertex formats we encounter (first 50, deduplicated by stride)
    {
        static unsigned s_seenStrides[16] = {0};
        static int s_seenCount = 0;
        bool seen = false;
        for (int si = 0; si < s_seenCount && si < 16; si++) {
            if (s_seenStrides[si] == stride) { seen = true; break; }
        }
        if (!seen && s_seenCount < 16) {
            s_seenStrides[s_seenCount++] = stride;
            Trace("DRAW", "NewVertexFormat stride=%u fvf=0x%X vb_type=%u polys=%u",
                stride, fvfFlags, vb_type, (unsigned)polygon_count);
        }
    }

    // Fix up degenerate vertex_count (matches original DX8Wrapper::Draw logic).
    //
    // The original subtraction is performed on unsigned values and WRAPS if the
    // offsets exceed the buffer's vertex count, producing a bogus count close to
    // 65535. Every reader below then walks off the end of the vertex buffer, which
    // is an out-of-bounds read rather than a bad picture. Compute it in a wide
    // signed type and reject the draw instead.
    const unsigned vb_vertex_count = vb->Get_Vertex_Count();
    if (vertex_count < 3) {
        min_vertex_index = 0;
        if (vb_type == BUFFER_TYPE_DX8) {
            const long long avail = (long long)vb_vertex_count
                                  - (long long)render_state.index_base_offset
                                  - (long long)render_state.vba_offset;
            if (avail <= 0) {
                static int s_badCountLog = 0;
                if (s_badCountLog < 20) {
                    ++s_badCountLog;
                    Trace("DRAW", "REJECT degenerate count: vb=%u base=%u vba=%u avail=%lld",
                        vb_vertex_count, render_state.index_base_offset,
                        render_state.vba_offset, avail);
                }
                return;
            }
            vertex_count = (unsigned short)avail;
        } else {
            vertex_count = render_state.vba_count;
        }
    }

    // Lock vertex data (BGFXVertexBuffer8::Lock just returns m_data + offset)
    BYTE* vdata = nullptr;
    static_cast<DX8VertexBufferClass*>(vb)->Get_DX8_Vertex_Buffer()->Lock(0, 0, &vdata, 0);
    if (!vdata) return;

    // Absolute starting vertex in the VB.
    unsigned vb_start = min_vertex_index +
        (vb_type == BUFFER_TYPE_DYNAMIC_DX8 ? render_state.vba_offset : 0u) +
        render_state.index_base_offset;

    // Everything downstream reads vertex_count vertices starting at vb_start,
    // either as 44-byte XYZNDUV2 (fast path) or at `stride` bytes (conversion
    // path). Neither reader bounds-checks, so verify the window lies inside the
    // buffer before any of them touch it. This is the guard that turns a silent
    // out-of-bounds read into a logged, skipped draw.
    if ((unsigned long long)vb_start + vertex_count > (unsigned long long)vb_vertex_count) {
        static int s_oobLog = 0;
        if (s_oobLog < 20) {
            ++s_oobLog;
            Trace("DRAW", "REJECT vertex range out of bounds: start=%u count=%u vb=%u stride=%u fvf=0x%X",
                vb_start, vertex_count, vb_vertex_count, stride, fvfFlags);
        }
        static_cast<DX8VertexBufferClass*>(vb)->Get_DX8_Vertex_Buffer()->Unlock();
        return;
    }

    // Get index data
    BGFXIndexBuffer8* bib = static_cast<BGFXIndexBuffer8*>(
        static_cast<DX8IndexBufferClass*>(ib)->Get_DX8_Index_Buffer());
    unsigned i_count   = (primitive_type == D3DPT_TRIANGLELIST) ? polygon_count * 3u : polygon_count + 2u;
    unsigned idx_start = start_index + render_state.iba_offset;
    const uint16_t* src_idx = reinterpret_cast<const uint16_t*>(bib->m_data) + idx_start;

    bool strip = (primitive_type != D3DPT_TRIANGLELIST);
    const float* world = reinterpret_cast<const float*>(&DX8Transforms[D3DTS_WORLD]);
    const float* view  = reinterpret_cast<const float*>(&DX8Transforms[D3DTS_VIEW]);
    const float* proj  = reinterpret_cast<const float*>(&DX8Transforms[D3DTS_PROJECTION]);

    // Diagnose: log first few DX8-path 3D draws to see if viewIsIdentity is wrong
    {
        static int s_dx8DrawLog = 0;
        if (s_dx8DrawLog < 10) {
            const VertexFormatXYZNDUV2* sv = reinterpret_cast<const VertexFormatXYZNDUV2*>(vdata + vb_start * stride);
            Trace("DX8DRAW", "#%d v=%u 2Dmode=%d pos0=(%.2f,%.2f,%.2f) view_row0=(%.3f,%.3f,%.3f,%.3f)",
                s_dx8DrawLog, vertex_count, (int)s_viewIsIdentity,
                sv[0].x, sv[0].y, sv[0].z,
                view[0], view[1], view[2], view[3]);
            ++s_dx8DrawLog;
        }
    }

    if (stride == (unsigned)sizeof(VertexFormatXYZNDUV2)) {
        // Fast path: native format, no conversion needed
        SubmitDraw(vdata, vb_start, vertex_count, src_idx, i_count, min_vertex_index, strip, world, view, proj);
    } else {
        // Convert any FVF vertex format to XYZNDUV2 for SubmitDraw
        bool hasNormal  = (fvfFlags & D3DFVF_NORMAL) != 0;
        bool hasDiffuse = (fvfFlags & D3DFVF_DIFFUSE) != 0;
        unsigned numTex = (fvfFlags >> 8) & 0xF; // D3DFVF_TEXn encoded in bits 8-11

        // Use FVFInfoClass offsets to locate each component
        unsigned locOff  = fvfInfo.Get_Location_Offset();
        unsigned normOff = hasNormal  ? fvfInfo.Get_Normal_Offset()  : 0;
        unsigned diffOff = hasDiffuse ? fvfInfo.Get_Diffuse_Offset() : 0;
        unsigned uv0Off  = numTex >= 1 ? fvfInfo.Get_Tex_Offset(0)  : 0;
        unsigned uv1Off  = numTex >= 2 ? fvfInfo.Get_Tex_Offset(1)  : 0;

        // Allocate conversion buffer for vertex_count vertices
        static std::vector<VertexFormatXYZNDUV2> s_convertBuf;
        if (s_convertBuf.size() < vertex_count)
            s_convertBuf.resize(vertex_count);

        const BYTE* srcBase = vdata + vb_start * stride;
        for (unsigned k = 0; k < vertex_count; k++) {
            const BYTE* v = srcBase + k * stride;
            VertexFormatXYZNDUV2& dst = s_convertBuf[k];

            // Position (always present at locOff)
            const float* pos = (const float*)(v + locOff);
            dst.x = pos[0]; dst.y = pos[1]; dst.z = pos[2];

            // Normal
            if (hasNormal) {
                const float* n = (const float*)(v + normOff);
                dst.nx = n[0]; dst.ny = n[1]; dst.nz = n[2];
            } else {
                dst.nx = 0.0f; dst.ny = 1.0f; dst.nz = 0.0f;
            }

            // Diffuse color
            dst.diffuse = hasDiffuse ? *(const unsigned*)(v + diffOff) : 0xFFFFFFFF;

            // UV coordinates
            if (numTex >= 1) {
                const float* uv = (const float*)(v + uv0Off);
                dst.u1 = uv[0]; dst.v1 = uv[1];
            } else { dst.u1 = 0.0f; dst.v1 = 0.0f; }

            if (numTex >= 2) {
                const float* uv = (const float*)(v + uv1Off);
                dst.u2 = uv[0]; dst.v2 = uv[1];
            } else { dst.u2 = 0.0f; dst.v2 = 0.0f; }
        }

        // Submit with converted buffer (vb_start=0 since we already offset srcBase)
        SubmitDraw((const BYTE*)s_convertBuf.data(), 0, vertex_count,
                   src_idx, i_count, min_vertex_index, strip, world, view, proj);
    }

    static_cast<DX8VertexBufferClass*>(vb)->Get_DX8_Vertex_Buffer()->Unlock();
}

void BGFXWrapper::Draw_Triangles(
    unsigned buffer_type, unsigned short start_index,
    unsigned short polygon_count, unsigned short min_vertex_index, unsigned short vertex_count)
{
    if (buffer_type == BUFFER_TYPE_SORTING || buffer_type == BUFFER_TYPE_DYNAMIC_SORTING)
        SortingRendererClass::Insert_Triangles(start_index, polygon_count, min_vertex_index, vertex_count);
    else
        Draw(D3DPT_TRIANGLELIST, start_index, polygon_count, min_vertex_index, vertex_count);
}

void BGFXWrapper::Draw_Triangles(
    unsigned short start_index, unsigned short polygon_count,
    unsigned short min_vertex_index, unsigned short vertex_count)
{
    Draw(D3DPT_TRIANGLELIST, start_index, polygon_count, min_vertex_index, vertex_count);
}

void BGFXWrapper::Draw_Strip(
    unsigned short start_index, unsigned short polygon_count,
    unsigned short min_vertex_index, unsigned short vertex_count)
{
    Draw(D3DPT_TRIANGLESTRIP, start_index, polygon_count, min_vertex_index, vertex_count);
}

// ============================================================================
// Resource Creation (stubs)
// ============================================================================
IDirect3DTexture8* BGFXWrapper::_Create_DX8_Texture(unsigned int width, unsigned int height, WW3DFormat format, MipCountType mip_level_count, D3DPOOL pool, bool rendertarget)
{
    (void)mip_level_count; (void)pool; (void)rendertarget;
    D3DFORMAT d3dfmt = WW3DFormat_To_D3DFormat(format);
    return new BGFXTexture8(width, height, d3dfmt);
}
IDirect3DTexture8* BGFXWrapper::_Create_DX8_Texture(const char* filename, MipCountType mip_level_count) { (void)filename; (void)mip_level_count; return nullptr; }
IDirect3DTexture8* BGFXWrapper::_Create_DX8_Texture(IDirect3DSurface8* surface, MipCountType mip_level_count)
{
    (void)mip_level_count;
    // Wrap an existing surface into a texture object
    if (!surface) return nullptr;
    D3DSURFACE_DESC desc;
    surface->GetDesc(&desc);
    return new BGFXTexture8(desc.Width, desc.Height, desc.Format);
}
IDirect3DCubeTexture8* BGFXWrapper::_Create_DX8_Cube_Texture(unsigned int width, unsigned int height, WW3DFormat format, MipCountType mip_level_count, D3DPOOL pool, bool rendertarget) { (void)width; (void)height; (void)format; (void)mip_level_count; (void)pool; (void)rendertarget; return nullptr; }
IDirect3DVolumeTexture8* BGFXWrapper::_Create_DX8_Volume_Texture(unsigned int width, unsigned int height, unsigned int depth, WW3DFormat format, MipCountType mip_level_count, D3DPOOL pool) { (void)width; (void)height; (void)depth; (void)format; (void)mip_level_count; (void)pool; return nullptr; }
IDirect3DTexture8* BGFXWrapper::_Create_DX8_ZTexture(unsigned int width, unsigned int height, WW3DZFormat zformat, MipCountType mip_level_count, D3DPOOL pool) { (void)width; (void)height; (void)zformat; (void)mip_level_count; (void)pool; return nullptr; }
IDirect3DSurface8* BGFXWrapper::_Create_DX8_Surface(unsigned int width, unsigned int height, WW3DFormat format)
{
    D3DFORMAT d3dfmt = WW3DFormat_To_D3DFormat(format);
    return new BGFXSurface8(width, height, d3dfmt);
}
IDirect3DSurface8* BGFXWrapper::_Create_DX8_Surface(const char* filename) { (void)filename; return nullptr; }
IDirect3DSurface8* BGFXWrapper::_Get_DX8_Front_Buffer() { return nullptr; }
SurfaceClass* BGFXWrapper::_Get_DX8_Back_Buffer(unsigned int num) {
    (void)num;
    // In BGFX the back buffer is the default framebuffer. Return a SurfaceClass
    // wrapping a CPU-side descriptor that records the actual render resolution
    // and pixel format — callers use it to query dimensions / format only.
    BGFXSurface8* raw = new BGFXSurface8(
        (UINT)ResolutionWidth, (UINT)ResolutionHeight, DisplayFormat);
    SurfaceClass* sc = NEW_REF(SurfaceClass, (raw));
    raw->Release(); // SurfaceClass Attach() AddRef'd it; balance the new BGFXSurface8
    return sc;
}
void BGFXWrapper::_Copy_DX8_Rects(IDirect3DSurface8* pSourceSurface, CONST RECT* pSourceRectsArray, UINT cRects, IDirect3DSurface8* pDestinationSurface, CONST POINT* pDestPointsArray)
{
    (void)pSourceRectsArray; (void)cRects; (void)pDestPointsArray;
    BGFXSurface8* src = static_cast<BGFXSurface8*>(pSourceSurface);
    BGFXSurface8* dst = static_cast<BGFXSurface8*>(pDestinationSurface);
    if (!src || !dst || !src->m_data || !dst->m_data) return;
    uint32_t dstSz = (uint32_t)dst->m_pitch * dst->m_height;
    uint32_t srcSz = (uint32_t)src->m_pitch * src->m_height;
    uint32_t copySz = dstSz < srcSz ? dstSz : srcSz;
    if (copySz > 0) memcpy(dst->m_data, src->m_data, copySz);
    dst->m_dirty = true;
}
void BGFXWrapper::_Update_Texture(TextureClass* system, TextureClass* video) {}
void BGFXWrapper::Flush_DX8_Resource_Manager(unsigned int bytes) {}
unsigned int BGFXWrapper::Get_Free_Texture_RAM() { return 256 * 1024 * 1024; }

// ============================================================================
// Render Targets (stubs)
// ============================================================================
TextureClass* BGFXWrapper::Create_Render_Target(int width, int height, WW3DFormat format) { return nullptr; }
void BGFXWrapper::Create_Render_Target(int width, int height, WW3DFormat format, WW3DZFormat zformat, TextureClass** target, ZTextureClass** depth_buffer) { if (target) *target = nullptr; if (depth_buffer) *depth_buffer = nullptr; }
void BGFXWrapper::Set_Render_Target(IDirect3DSurface8* render_target, bool use_default_depth_buffer) { CurrentRenderTarget = render_target; }
void BGFXWrapper::Set_Render_Target(IDirect3DSurface8* render_target, IDirect3DSurface8* depth_buffer) { CurrentRenderTarget = render_target; CurrentDepthBuffer = depth_buffer; }
void BGFXWrapper::Set_Render_Target(IDirect3DSwapChain8* swap_chain) {}
void BGFXWrapper::Set_Render_Target_With_Z(TextureClass* texture, ZTextureClass* ztexture) {}
IDirect3DSwapChain8* BGFXWrapper::Create_Additional_Swap_Chain(HWND render_window) { return nullptr; }

// ============================================================================
// Statistics & Registry
// ============================================================================
void BGFXWrapper::Begin_Statistics() {}
void BGFXWrapper::End_Statistics() {}
void BGFXWrapper::Reset_Statistics() {}
bool BGFXWrapper::Registry_Save_Render_Device(const char* sub_key) { return true; }
bool BGFXWrapper::Registry_Load_Render_Device(const char* sub_key, bool resize_window) { return true; }
bool BGFXWrapper::Registry_Save_Render_Device(const char* sub_key, int device, int width, int height, int depth, bool windowed, int texture_depth) { return true; }
bool BGFXWrapper::Registry_Load_Render_Device(const char* sub_key, char* device, int device_len, int& width, int& height, int& depth, int& windowed, int& texture_depth) { return false; }

// ============================================================================
// Misc
// ============================================================================
void BGFXWrapper::Set_Swap_Interval(int swap) {}
int BGFXWrapper::Get_Swap_Interval(void) { return 0; }
void BGFXWrapper::Set_Polygon_Mode(int mode) {}
bool BGFXWrapper::Create_Device(void) { return true; }
void BGFXWrapper::Release_Device(void) {}
void BGFXWrapper::Enumerate_Devices() {}
void BGFXWrapper::Set_Default_Global_Render_States(void) {}
const RenderDeviceDescClass& BGFXWrapper::Get_Render_Device_Desc(int deviceidx) { static RenderDeviceDescClass desc; return desc; }

// ============================================================================
// Debug State Names
// ============================================================================
const char* BGFXWrapper::Get_DX8_Render_State_Name(D3DRENDERSTATETYPE state) { return ""; }
const char* BGFXWrapper::Get_DX8_Texture_Stage_State_Name(D3DTEXTURESTAGESTATETYPE state) { return ""; }
void BGFXWrapper::Get_DX8_Texture_Stage_State_Value_Name(StringClass& name, D3DTEXTURESTAGESTATETYPE state, unsigned value) {}
void BGFXWrapper::Get_DX8_Render_State_Value_Name(StringClass& name, D3DRENDERSTATETYPE state, unsigned value) {}
const char* BGFXWrapper::Get_DX8_Texture_Address_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Texture_Filter_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Texture_Arg_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Texture_Op_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Texture_Transform_Flag_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_ZBuffer_Type_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Fill_Mode_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Shade_Mode_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Blend_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Cull_Mode_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Cmp_Func_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Fog_Mode_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Stencil_Op_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Material_Source_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Vertex_Blend_Flag_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Patch_Edge_Style_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Debug_Monitor_Token_Name(unsigned value) { return ""; }
const char* BGFXWrapper::Get_DX8_Blend_Op_Name(unsigned value) { return ""; }

// ============================================================================
// RenderStateStruct
// ============================================================================
RenderStateStruct::RenderStateStruct()
{
    memset(this, 0, sizeof(*this));
}

RenderStateStruct::~RenderStateStruct() {}

RenderStateStruct& RenderStateStruct::operator=(const RenderStateStruct& src)
{
    if (this != &src) memcpy(this, &src, sizeof(*this));
    return *this;
}
