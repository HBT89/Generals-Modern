// BGFXWrapper.cpp
// Implementation of BGFXWrapper for WW3D replacement
#include "BGFXWrapper.h"
#include "matrix4.h"
#include "matrix3d.h"
#include "dx8caps.h"
#include "rddesc.h"
#include <bgfx/bgfx.h>
#include <string>

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
bgfx::UniformHandle BGFXWrapper::s_uTexCoordSource = BGFX_INVALID_HANDLE;
bgfx::UniformHandle BGFXWrapper::s_uTexTransformFlags = BGFX_INVALID_HANDLE;

// Per-stage texture transform state (internal)
namespace {
    constexpr int kMaxStages = 4;
    Matrix4x4 s_texTransforms[kMaxStages];
    int s_texCoordSources[kMaxStages] = {};
    int s_texTransformFlags[kMaxStages] = {};
}

// ============================================================================
// Initialization
// ============================================================================
bool BGFXWrapper::Init(void* hwnd, bool lite)
{
    if (IsInitted) return true;
    Hwnd = hwnd;

    bgfx::PlatformData pd = {};
    pd.nwh = hwnd;
    bgfx::setPlatformData(pd);

    bgfx::Init init;
    init.type = bgfx::RendererType::Count; // Auto-detect
    init.resolution.width = ResolutionWidth;
    init.resolution.height = ResolutionHeight;
    init.resolution.reset = IsWindowed ? BGFX_RESET_NONE : BGFX_RESET_FULLSCREEN;

    IsInitted = bgfx::init(init);
    if (IsInitted) {
        for (int i = 0; i < 4; ++i) {
            s_uTexTransform[i] = bgfx::createUniform(
                (std::string("u_texTransform") + std::to_string(i)).c_str(),
                bgfx::UniformType::Mat4);
        }
        s_uTexCoordSource = bgfx::createUniform("u_texCoordSource", bgfx::UniformType::Vec4);
        s_uTexTransformFlags = bgfx::createUniform("u_texTransformFlags", bgfx::UniformType::Vec4);
    }
    return IsInitted;
}

void BGFXWrapper::Shutdown()
{
    if (IsInitted) {
        for (int i = 0; i < 4; ++i) {
            if (bgfx::isValid(s_uTexTransform[i])) bgfx::destroy(s_uTexTransform[i]);
            s_uTexTransform[i] = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(s_uTexCoordSource)) bgfx::destroy(s_uTexCoordSource);
        s_uTexCoordSource = BGFX_INVALID_HANDLE;
        if (bgfx::isValid(s_uTexTransformFlags)) bgfx::destroy(s_uTexTransformFlags);
        s_uTexTransformFlags = BGFX_INVALID_HANDLE;
        bgfx::shutdown();
        IsInitted = false;
    }
}

void BGFXWrapper::Do_Onetime_Device_Dependent_Inits(void) {}
void BGFXWrapper::Do_Onetime_Device_Dependent_Shutdowns(void) {}

// ============================================================================
// Scene Management
// ============================================================================
void BGFXWrapper::Begin_Scene(void)
{
    bgfx::touch(0);
}

void BGFXWrapper::End_Scene(bool flip_frame)
{
    if (flip_frame) {
        bgfx::frame();
        FrameCount++;
    }
}

void BGFXWrapper::Flip_To_Primary(void)
{
    bgfx::frame();
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
    bgfx::setViewClear(0, flags, rgba, z, stencil);
}

void BGFXWrapper::Set_Viewport(CONST D3DVIEWPORT8* pViewport)
{
    if (pViewport) {
        bgfx::setViewRect(0, pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height);
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
    if (windowed >= 0) IsWindowed = (windowed != 0);
    if (IsInitted) {
        bgfx::reset(ResolutionWidth, ResolutionHeight, IsWindowed ? BGFX_RESET_NONE : BGFX_RESET_FULLSCREEN);
    }
    return true;
}
bool BGFXWrapper::Set_Next_Render_Device(void) { return true; }
bool BGFXWrapper::Toggle_Windowed(void)
{
    IsWindowed = !IsWindowed;
    if (IsInitted) bgfx::reset(ResolutionWidth, ResolutionHeight, IsWindowed ? BGFX_RESET_NONE : BGFX_RESET_FULLSCREEN);
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
    if (transform == D3DTS_VIEW) render_state_changed |= VIEW_CHANGED;
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
void BGFXWrapper::Set_World_Identity() { world_identity = true; render_state_changed |= WORLD_IDENTITY; }
void BGFXWrapper::Set_View_Identity() { render_state_changed |= VIEW_IDENTITY; }
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

void BGFXWrapper::Upload_Texture_Uniforms()
{
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
void BGFXWrapper::Set_DX8_Texture_Stage_State(unsigned stage, D3DTEXTURESTAGESTATETYPE state, unsigned value) { if (stage < MAX_TEXTURE_STAGES) TextureStageStates[stage][state] = value; }
void BGFXWrapper::Set_DX8_Texture(unsigned int stage, IDirect3DBaseTexture8* texture) { if (stage < MAX_TEXTURE_STAGES) { Textures[stage] = texture; render_state_changed |= (TEXTURE0_CHANGED << stage); } }
void BGFXWrapper::Set_DX8_Material(const D3DMATERIAL8* mat) { (void)mat; render_state_changed |= MATERIAL_CHANGED; }
void BGFXWrapper::Set_DX8_ZBias(int zbias) { ZBias = zbias; }
void BGFXWrapper::Set_Projection_Transform_With_Z_Bias(const Matrix4x4& matrix, float znear, float zfar) { ZNear = znear; ZFar = zfar; ProjectionMatrix = matrix; Set_Transform(D3DTS_PROJECTION, matrix); }
void BGFXWrapper::Set_Gamma(float gamma, float bright, float contrast, bool calibrate, bool uselimit) {}
bool BGFXWrapper::Validate_Device(void) { return true; }

void BGFXWrapper::Set_Shader(const ShaderClass& shader) {}
void BGFXWrapper::Get_Shader(ShaderClass& shader) {}
void BGFXWrapper::Set_Texture(unsigned stage, TextureBaseClass* texture) {}
void BGFXWrapper::Set_Material(const VertexMaterialClass* material) {}
void BGFXWrapper::Set_Light(unsigned index, const D3DLIGHT8* light) { if (light) Set_DX8_Light(index, const_cast<D3DLIGHT8*>(light)); }
void BGFXWrapper::Set_Light(unsigned index, const LightClass& light) {}
void BGFXWrapper::Apply_Render_State_Changes() { render_state_changed = 0; }
void BGFXWrapper::Apply_Default_State() {}
void BGFXWrapper::Invalidate_Cached_Render_States(void) {}

// ============================================================================
// Buffers
// ============================================================================
void BGFXWrapper::Set_Vertex_Buffer(const VertexBufferClass* vb, unsigned stream) { render_state.vertex_buffers[stream] = const_cast<VertexBufferClass*>(vb); render_state_changed |= VERTEX_BUFFER_CHANGED; }
void BGFXWrapper::Set_Vertex_Buffer(const DynamicVBAccessClass& vba) {}
void BGFXWrapper::Set_Index_Buffer(const IndexBufferClass* ib, unsigned short index_base_offset) { render_state.index_buffer = const_cast<IndexBufferClass*>(ib); render_state.index_base_offset = index_base_offset; render_state_changed |= INDEX_BUFFER_CHANGED; }
void BGFXWrapper::Set_Index_Buffer(const DynamicIBAccessClass& iba, unsigned short index_base_offset) {}
void BGFXWrapper::Get_Render_State(RenderStateStruct& state) { state = render_state; }
void BGFXWrapper::Set_Render_State(const RenderStateStruct& state) { render_state = state; }
void BGFXWrapper::Release_Render_State() {}

// ============================================================================
// Drawing
// ============================================================================
void BGFXWrapper::Draw_Sorting_IB_VB(unsigned primitive_type, unsigned short start_index, unsigned short polygon_count, unsigned short min_vertex_index, unsigned short vertex_count) {}
void BGFXWrapper::Draw(unsigned primitive_type, unsigned short start_index, unsigned short polygon_count, unsigned short min_vertex_index, unsigned short vertex_count) {}
void BGFXWrapper::Draw_Triangles(unsigned buffer_type, unsigned short start_index, unsigned short polygon_count, unsigned short min_vertex_index, unsigned short vertex_count) {}
void BGFXWrapper::Draw_Triangles(unsigned short start_index, unsigned short polygon_count, unsigned short min_vertex_index, unsigned short vertex_count) {}
void BGFXWrapper::Draw_Strip(unsigned short start_index, unsigned short index_count, unsigned short min_vertex_index, unsigned short vertex_count) {}

// ============================================================================
// Resource Creation (stubs)
// ============================================================================
IDirect3DTexture8* BGFXWrapper::_Create_DX8_Texture(unsigned int width, unsigned int height, WW3DFormat format, MipCountType mip_level_count, D3DPOOL pool, bool rendertarget) { return nullptr; }
IDirect3DTexture8* BGFXWrapper::_Create_DX8_Texture(const char* filename, MipCountType mip_level_count) { return nullptr; }
IDirect3DTexture8* BGFXWrapper::_Create_DX8_Texture(IDirect3DSurface8* surface, MipCountType mip_level_count) { return nullptr; }
IDirect3DCubeTexture8* BGFXWrapper::_Create_DX8_Cube_Texture(unsigned int width, unsigned int height, WW3DFormat format, MipCountType mip_level_count, D3DPOOL pool, bool rendertarget) { return nullptr; }
IDirect3DVolumeTexture8* BGFXWrapper::_Create_DX8_Volume_Texture(unsigned int width, unsigned int height, unsigned int depth, WW3DFormat format, MipCountType mip_level_count, D3DPOOL pool) { return nullptr; }
IDirect3DTexture8* BGFXWrapper::_Create_DX8_ZTexture(unsigned int width, unsigned int height, WW3DZFormat zformat, MipCountType mip_level_count, D3DPOOL pool) { return nullptr; }
IDirect3DSurface8* BGFXWrapper::_Create_DX8_Surface(unsigned int width, unsigned int height, WW3DFormat format) { return nullptr; }
IDirect3DSurface8* BGFXWrapper::_Create_DX8_Surface(const char* filename) { return nullptr; }
IDirect3DSurface8* BGFXWrapper::_Get_DX8_Front_Buffer() { return nullptr; }
SurfaceClass* BGFXWrapper::_Get_DX8_Back_Buffer(unsigned int num) { return nullptr; }
void BGFXWrapper::_Copy_DX8_Rects(IDirect3DSurface8* pSourceSurface, CONST RECT* pSourceRectsArray, UINT cRects, IDirect3DSurface8* pDestinationSurface, CONST POINT* pDestPointsArray) {}
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
