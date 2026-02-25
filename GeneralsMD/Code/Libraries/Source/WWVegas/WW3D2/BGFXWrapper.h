// BGFXWrapper.h
// Drop-in replacement for dx8wrapper.h - provides the same DX8Wrapper interface
// but implemented using BGFX instead of Direct3D 8.
//
// All existing code that calls DX8Wrapper::Method() will work via the
// #define DX8Wrapper BGFXWrapper alias at the bottom of this file.

#pragma once
#ifndef BGFX_WRAPPER_H
#define BGFX_WRAPPER_H

#include "always.h"
#include "RenderTypes.h"
#include "texturefilter.h"

#include "matrix4.h"
#include "matrix3d.h"
#include "vector3.h"
#include "vector4.h"
#include "statistics.h"
#include "wwstring.h"
#include "lightenvironment.h"
#include "shader.h"
#include "cpudetect.h"
// Forward-declare to break circular include: texture.h -> texturefilter.h -> dx8wrapper.h -> BGFXWrapper.h
class TextureBaseClass;
class VertexMaterialClass;

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "BGFXCaps.h"

// ============================================================================
// Constants
// ============================================================================
const unsigned MAX_TEXTURE_STAGES = 8;
const unsigned MAX_VERTEX_STREAMS = 2;
const unsigned MAX_VERTEX_SHADER_CONSTANTS = 96;
const unsigned MAX_PIXEL_SHADER_CONSTANTS = 8;
const unsigned MAX_SHADOW_MAPS = 1;

// Buffer type constants
enum {
    BUFFER_TYPE_DX8,
    BUFFER_TYPE_SORTING,
    BUFFER_TYPE_DYNAMIC_DX8,
    BUFFER_TYPE_DYNAMIC_SORTING,
    BUFFER_TYPE_INVALID
};

// Obfuscation compatibility macros (from original code)
#define prevVer
#define nextVer
#define __volatile unsigned
#define D3D2_BASE_VEC unsigned

// ============================================================================
// Forward Declarations
// ============================================================================
class VertexMaterialClass;
class CameraClass;
class LightEnvironmentClass;
class RenderDeviceDescClass;
class VertexBufferClass;
class DynamicVBAccessClass;
class IndexBufferClass;
class DynamicIBAccessClass;
class TextureClass;
class ZTextureClass;
class LightClass;
class SurfaceClass;
class RenderInfoClass;

// ============================================================================
// Statistics tracking macros (no-ops for now, can be implemented later)
// ============================================================================
#define DX8_RECORD_MATRIX_CHANGE()
#define DX8_RECORD_MATERIAL_CHANGE()
#define DX8_RECORD_VERTEX_BUFFER_CHANGE()
#define DX8_RECORD_INDEX_BUFFER_CHANGE()
#define DX8_RECORD_LIGHT_CHANGE()
#define DX8_RECORD_TEXTURE_CHANGE()
#define DX8_RECORD_RENDER_STATE_CHANGE()
#define DX8_RECORD_TEXTURE_STAGE_STATE_CHANGE()
#define DX8_RECORD_DRAW_CALLS()

extern unsigned number_of_DX8_calls;
extern bool _DX8SingleThreaded;

inline void DX8_Assert() {}
inline void Log_DX8_ErrorCode(unsigned res) {}
inline void DX8_ErrorCode(unsigned res) {}

// DX8CALL macros - no longer call D3D, they do nothing (BGFX calls are explicit)
#define DX8CALL_HRES(x, res) res = D3D_OK;
#define DX8CALL(x)
#define DX8CALL_D3D(x)
#define DX8_THREAD_ASSERT()

// Snapshot macros (debug logging)
#ifndef SNAPSHOT_SAY
#define SNAPSHOT_SAY(x)
#endif

// ============================================================================
// Registry value names
// ============================================================================
#define VALUE_NAME_RENDER_DEVICE_NAME          "RenderDeviceName"
#define VALUE_NAME_RENDER_DEVICE_WIDTH         "RenderDeviceWidth"
#define VALUE_NAME_RENDER_DEVICE_HEIGHT        "RenderDeviceHeight"
#define VALUE_NAME_RENDER_DEVICE_DEPTH         "RenderDeviceDepth"
#define VALUE_NAME_RENDER_DEVICE_WINDOWED      "RenderDeviceWindowed"
#define VALUE_NAME_RENDER_DEVICE_TEXTURE_DEPTH "RenderDeviceTextureDepth"

// ============================================================================
// Cleanup Hook Interface
// ============================================================================
class DX8_CleanupHook
{
public:
    virtual void ReleaseResources(void) = 0;
    virtual void ReAcquireResources(void) = 0;
};

// ============================================================================
// Render State Structure
// ============================================================================
struct RenderStateStruct
{
    ShaderClass shader;
    VertexMaterialClass* material;
    TextureBaseClass* Textures[MAX_TEXTURE_STAGES];
    D3DLIGHT8 Lights[4];
    bool LightEnable[4];
    Matrix4x4 world;
    Matrix4x4 view;
    unsigned vertex_buffer_types[MAX_VERTEX_STREAMS];
    unsigned index_buffer_type;
    unsigned short vba_offset;
    unsigned short vba_count;
    unsigned short iba_offset;
    VertexBufferClass* vertex_buffers[MAX_VERTEX_STREAMS];
    IndexBufferClass* index_buffer;
    unsigned short index_base_offset;

    RenderStateStruct();
    ~RenderStateStruct();
    RenderStateStruct& operator=(const RenderStateStruct& src);
};

// ============================================================================
// BGFXWrapper - The main rendering interface class
// Provides the same public API as the original DX8Wrapper.
// ============================================================================
class BGFXWrapper
{
    enum ChangedStates {
        WORLD_CHANGED       = 1 << 0,
        VIEW_CHANGED        = 1 << 1,
        LIGHT0_CHANGED      = 1 << 2,
        LIGHT1_CHANGED      = 1 << 3,
        LIGHT2_CHANGED      = 1 << 4,
        LIGHT3_CHANGED      = 1 << 5,
        TEXTURE0_CHANGED    = 1 << 6,
        TEXTURE1_CHANGED    = 1 << 7,
        TEXTURE2_CHANGED    = 1 << 8,
        TEXTURE3_CHANGED    = 1 << 9,
        MATERIAL_CHANGED    = 1 << 14,
        SHADER_CHANGED      = 1 << 15,
        VERTEX_BUFFER_CHANGED = 1 << 16,
        INDEX_BUFFER_CHANGED = 1 << 17,
        WORLD_IDENTITY      = 1 << 18,
        VIEW_IDENTITY        = 1 << 19,

        TEXTURES_CHANGED = TEXTURE0_CHANGED | TEXTURE1_CHANGED | TEXTURE2_CHANGED | TEXTURE3_CHANGED,
        LIGHTS_CHANGED = LIGHT0_CHANGED | LIGHT1_CHANGED | LIGHT2_CHANGED | LIGHT3_CHANGED,
    };

    static void Draw_Sorting_IB_VB(unsigned primitive_type, unsigned short start_index,
        unsigned short polygon_count, unsigned short min_vertex_index, unsigned short vertex_count);
    static void Draw(unsigned primitive_type, unsigned short start_index,
        unsigned short polygon_count, unsigned short min_vertex_index = 0, unsigned short vertex_count = 0);

public:
    // ---- Texture coordinate source constants ----
    enum {
        CAMERA_SPACE_POSITION = 0,
        CAMERA_SPACE_NORMAL = 1,
        CAMERA_SPACE_REFLECTION = 2,
    };

    // ---- Texture transform flag constants ----
    enum {
        COUNT2 = 2,
        COUNT3 = 3,
        COUNT4 = 4,
        PROJECTED = 256,
    };

    // ---- Initialization & Device Management ----
    static bool Init(void* hwnd, bool lite = false);
    static void Shutdown(void);
    static void SetCleanupHook(DX8_CleanupHook* pCleanupHook) { m_pCleanupHook = pCleanupHook; }
    static void Do_Onetime_Device_Dependent_Inits(void);
    static void Do_Onetime_Device_Dependent_Shutdowns(void);
    static bool Is_Device_Lost() { return IsDeviceLost; }
    static bool Is_Initted(void) { return IsInitted; }
    static bool Has_Stencil(void);
    static void Get_Format_Name(unsigned int format, StringClass* tex_format);

    // ---- Rendering ----
    static void Begin_Scene(void);
    static void End_Scene(bool flip_frame = true);
    static void Flip_To_Primary(void);
    static void Clear(bool clear_color, bool clear_z_stencil, const Vector3& color,
        float dest_alpha = 0.0f, float z = 1.0f, unsigned int stencil = 0);
    static void Set_Viewport(CONST D3DVIEWPORT8* pViewport);

    // ---- Buffers ----
    static void Set_Vertex_Buffer(const VertexBufferClass* vb, unsigned stream = 0);
    static void Set_Vertex_Buffer(const DynamicVBAccessClass& vba);
    static void Set_Index_Buffer(const IndexBufferClass* ib, unsigned short index_base_offset);
    static void Set_Index_Buffer(const DynamicIBAccessClass& iba, unsigned short index_base_offset);
    static void Set_Index_Buffer_Index_Offset(unsigned offset);
    static void Get_Render_State(RenderStateStruct& state);
    static void Set_Render_State(const RenderStateStruct& state);
    static void Release_Render_State();

    // ---- Materials & Shaders ----
    static void Set_DX8_Material(const D3DMATERIAL8* mat);
    static void Set_Gamma(float gamma, float bright, float contrast, bool calibrate = true, bool uselimit = true);

    // ---- Transforms ----
    static void Set_DX8_ZBias(int zbias);
    static void Set_Projection_Transform_With_Z_Bias(const Matrix4x4& matrix, float znear, float zfar);
    static void Set_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix4x4& m);
    static void Set_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix3D& m);
    static void Get_Transform(D3DTRANSFORMSTATETYPE transform, Matrix4x4& m);
    static void Set_World_Identity();
    static void Set_View_Identity();
    static bool Is_World_Identity();
    static bool Is_View_Identity();
    static void _Set_DX8_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix4x4& m);
    static void _Set_DX8_Transform(D3DTRANSFORMSTATETYPE transform, const Matrix3D& m);
    static void _Get_DX8_Transform(D3DTRANSFORMSTATETYPE transform, Matrix4x4& m);

    // ---- Lighting ----
    static void Set_DX8_Light(int index, D3DLIGHT8* light);
    static void Set_Light_Environment(LightEnvironmentClass* light_env);
    static LightEnvironmentClass* Get_Light_Environment() { return Light_Environment; }
    static void Set_Fog(bool enable, const Vector3& color, float start, float end);
    static const D3DLIGHT8& Peek_Light(unsigned index);
    static bool Is_Light_Enabled(unsigned index);

    // ---- Render State ----
    static void Set_DX8_Render_State(D3DRENDERSTATETYPE state, unsigned value);
    static void Set_DX8_Clip_Plane(DWORD Index, CONST float* pPlane);
    static void Set_DX8_Texture_Stage_State(unsigned stage, D3DTEXTURESTAGESTATETYPE state, unsigned value);
    static void Set_DX8_Texture(unsigned int stage, IDirect3DBaseTexture8* texture);
    static bool Validate_Device(void);
    static void Set_Shader(const ShaderClass& shader);
    static void Get_Shader(ShaderClass& shader);
    static void Set_Texture(unsigned stage, TextureBaseClass* texture);
    static void Set_Material(const VertexMaterialClass* material);
    static void Set_Light(unsigned index, const D3DLIGHT8* light);
    static void Set_Light(unsigned index, const LightClass& light);
    static void Apply_Render_State_Changes();
    static void Apply_Default_State();
    static void Invalidate_Cached_Render_States(void);
    static unsigned Get_DX8_Render_State(D3DRENDERSTATETYPE state) { return RenderStates[state]; }

    // ---- Drawing ----
    static void Draw_Triangles(unsigned buffer_type, unsigned short start_index,
        unsigned short polygon_count, unsigned short min_vertex_index, unsigned short vertex_count);
    static void Draw_Triangles(unsigned short start_index, unsigned short polygon_count,
        unsigned short min_vertex_index, unsigned short vertex_count);
    static void Draw_Strip(unsigned short start_index, unsigned short index_count,
        unsigned short min_vertex_index, unsigned short vertex_count);

    // ---- Resource Creation ----
    static IDirect3DTexture8* _Create_DX8_Texture(unsigned int width, unsigned int height,
        WW3DFormat format, MipCountType mip_level_count, D3DPOOL pool = D3DPOOL_MANAGED, bool rendertarget = false);
    static IDirect3DTexture8* _Create_DX8_Texture(const char* filename, MipCountType mip_level_count);
    static IDirect3DTexture8* _Create_DX8_Texture(IDirect3DSurface8* surface, MipCountType mip_level_count);
    static IDirect3DCubeTexture8* _Create_DX8_Cube_Texture(unsigned int width, unsigned int height,
        WW3DFormat format, MipCountType mip_level_count, D3DPOOL pool = D3DPOOL_MANAGED, bool rendertarget = false);
    static IDirect3DVolumeTexture8* _Create_DX8_Volume_Texture(unsigned int width, unsigned int height,
        unsigned int depth, WW3DFormat format, MipCountType mip_level_count, D3DPOOL pool = D3DPOOL_MANAGED);
    static IDirect3DTexture8* _Create_DX8_ZTexture(unsigned int width, unsigned int height,
        WW3DZFormat zformat, MipCountType mip_level_count, D3DPOOL pool = D3DPOOL_MANAGED);
    static IDirect3DSurface8* _Create_DX8_Surface(unsigned int width, unsigned int height, WW3DFormat format);
    static IDirect3DSurface8* _Create_DX8_Surface(const char* filename);
    static IDirect3DSurface8* _Get_DX8_Front_Buffer();
    static SurfaceClass* _Get_DX8_Back_Buffer(unsigned int num = 0);
    static void _Copy_DX8_Rects(IDirect3DSurface8* pSourceSurface, CONST RECT* pSourceRectsArray,
        UINT cRects, IDirect3DSurface8* pDestinationSurface, CONST POINT* pDestPointsArray);
    static void _Update_Texture(TextureClass* system, TextureClass* video);
    static void Flush_DX8_Resource_Manager(unsigned int bytes = 0);
    static unsigned int Get_Free_Texture_RAM();

    // ---- Device Access ----
    static unsigned _Get_Main_Thread_ID() { return _MainThreadID; }
    static const D3DADAPTER_IDENTIFIER8& Get_Current_Adapter_Identifier() { return CurrentAdapterIdentifier; }
    static IDirect3DDevice8* _Get_D3D_Device8() { return &s_stubDevice; }
    static IDirect3D8* _Get_D3D8() { return &s_stubD3D; }
    static WW3DFormat getBackBufferFormat(void);
    static bool Reset_Device(bool reload_assets = true);
    static const DX8Caps* Get_Current_Caps() { return CurrentCaps; }

    // ---- Statistics ----
    static void Begin_Statistics();
    static void End_Statistics();
    static unsigned Get_Last_Frame_Matrix_Changes() { return 0; }
    static unsigned Get_Last_Frame_Material_Changes() { return 0; }
    static unsigned Get_Last_Frame_Vertex_Buffer_Changes() { return 0; }
    static unsigned Get_Last_Frame_Index_Buffer_Changes() { return 0; }
    static unsigned Get_Last_Frame_Light_Changes() { return 0; }
    static unsigned Get_Last_Frame_Texture_Changes() { return 0; }
    static unsigned Get_Last_Frame_Render_State_Changes() { return 0; }
    static unsigned Get_Last_Frame_Texture_Stage_State_Changes() { return 0; }
    static unsigned Get_Last_Frame_DX8_Calls() { return 0; }
    static unsigned Get_Last_Frame_Draw_Calls() { return 0; }
    static unsigned long Get_FrameCount(void) { return FrameCount; }

    // ---- Fog ----
    static bool Get_Fog_Enable() { return FogEnable; }
    static D3DCOLOR Get_Fog_Color() { return FogColor; }

    // ---- Color Utilities ----
    static Vector4 Convert_Color(unsigned color);
    static unsigned int Convert_Color(const Vector4& color);
    static unsigned int Convert_Color(const Vector3& color, const float alpha);
    static void Clamp_Color(Vector4& color);
    static unsigned int Convert_Color_Clamp(const Vector4& color);
    static void Set_Alpha(const float alpha, unsigned int& color);

    // ---- Triangle Draw Control ----
    static void _Enable_Triangle_Draw(bool enable) { _EnableTriangleDraw = enable; }
    static bool _Is_Triangle_Draw_Enabled() { return _EnableTriangleDraw; }
    static void Set_Draw_Polygon_Low_Bound_Limit(unsigned n) { DrawPolygonLowBoundLimit = n; }

    // ---- Render Targets ----
    static TextureClass* Create_Render_Target(int width, int height, WW3DFormat format = WW3D_FORMAT_UNKNOWN);
    static void Create_Render_Target(int width, int height, WW3DFormat format, WW3DZFormat zformat,
        TextureClass** target, ZTextureClass** depth_buffer);
    static void Set_Render_Target(IDirect3DSurface8* render_target, bool use_default_depth_buffer = false);
    static void Set_Render_Target(IDirect3DSurface8* render_target, IDirect3DSurface8* depth_buffer);
    static void Set_Render_Target(IDirect3DSwapChain8* swap_chain);
    static void Set_Render_Target_With_Z(TextureClass* texture, ZTextureClass* ztexture = NULL);
    static bool Is_Render_To_Texture(void) { return IsRenderToTexture; }
    static IDirect3DSwapChain8* Create_Additional_Swap_Chain(HWND render_window);

    // ---- Shadow Maps ----
    static void Set_Shadow_Map(int idx, ZTextureClass* ztex) { Shadow_Map[idx] = ztex; }
    static ZTextureClass* Get_Shadow_Map(int idx) { return Shadow_Map[idx]; }

    // ---- Vertex/Pixel Shaders ----
    static void Set_Vertex_Shader(DWORD vertex_shader);
    static void Set_Pixel_Shader(DWORD pixel_shader);
    static void Set_Vertex_Shader_Constant(int reg, const void* data, int count);
    static void Set_Pixel_Shader_Constant(int reg, const void* data, int count);
    static DWORD Get_Vertex_Processing_Behavior() { return Vertex_Processing_Behavior; }

    // ---- Ambient ----
    static void Set_Ambient(const Vector3& color);
    static const Vector3& Get_Ambient() { return Ambient_Color; }

    // ---- Registry ----
    static bool Registry_Save_Render_Device(const char* sub_key);
    static bool Registry_Load_Render_Device(const char* sub_key, bool resize_window);

    // ---- Debug State Names ----
    static const char* Get_DX8_Render_State_Name(D3DRENDERSTATETYPE state);
    static const char* Get_DX8_Texture_Stage_State_Name(D3DTEXTURESTAGESTATETYPE state);
    static void Get_DX8_Texture_Stage_State_Value_Name(StringClass& name, D3DTEXTURESTAGESTATETYPE state, unsigned value);
    static void Get_DX8_Render_State_Value_Name(StringClass& name, D3DRENDERSTATETYPE state, unsigned value);
    static const char* Get_DX8_Texture_Address_Name(unsigned value);
    static const char* Get_DX8_Texture_Filter_Name(unsigned value);
    static const char* Get_DX8_Texture_Arg_Name(unsigned value);
    static const char* Get_DX8_Texture_Op_Name(unsigned value);
    static const char* Get_DX8_Texture_Transform_Flag_Name(unsigned value);
    static const char* Get_DX8_ZBuffer_Type_Name(unsigned value);
    static const char* Get_DX8_Fill_Mode_Name(unsigned value);
    static const char* Get_DX8_Shade_Mode_Name(unsigned value);
    static const char* Get_DX8_Blend_Name(unsigned value);
    static const char* Get_DX8_Cull_Mode_Name(unsigned value);
    static const char* Get_DX8_Cmp_Func_Name(unsigned value);
    static const char* Get_DX8_Fog_Mode_Name(unsigned value);
    static const char* Get_DX8_Stencil_Op_Name(unsigned value);
    static const char* Get_DX8_Material_Source_Name(unsigned value);
    static const char* Get_DX8_Vertex_Blend_Flag_Name(unsigned value);
    static const char* Get_DX8_Patch_Edge_Style_Name(unsigned value);
    static const char* Get_DX8_Debug_Monitor_Token_Name(unsigned value);
    static const char* Get_DX8_Blend_Op_Name(unsigned value);

    // ---- BGFX-specific Texture Transform API ----
    static void Set_Texture_Transform(int stage, const Matrix4x4& mat);
    static void Set_Texture_Coord_Source(int stage, int source);
    static void Set_Texture_Transform_Flags(int stage, int flags);
    static void Upload_Texture_Uniforms();

protected:
    // Device selection (accessed via WW3D friend class)
    static bool Set_Any_Render_Device(void);
    static bool Set_Render_Device(const char* dev_name, int width = -1, int height = -1,
        int bits = -1, int windowed = -1, bool resize_window = false);
    static bool Set_Render_Device(int dev = -1, int resx = -1, int resy = -1, int bits = -1,
        int windowed = -1, bool resize_window = false, bool reset_device = false, bool restore_assets = true);
    static bool Set_Next_Render_Device(void);
    static bool Toggle_Windowed(void);
    static int  Get_Render_Device_Count(void);
    static int  Get_Render_Device(void);
    static const RenderDeviceDescClass& Get_Render_Device_Desc(int deviceidx);
    static const char* Get_Render_Device_Name(int device_index);
    static bool Set_Device_Resolution(int width = -1, int height = -1, int bits = -1,
        int windowed = -1, bool resize_window = false);
    static void Get_Device_Resolution(int& set_w, int& set_h, int& set_bits, bool& set_windowed);
    static void Get_Render_Target_Resolution(int& set_w, int& set_h, int& set_bits, bool& set_windowed);
    static int  Get_Device_Resolution_Width(void) { return ResolutionWidth; }
    static int  Get_Device_Resolution_Height(void) { return ResolutionHeight; }
    static bool Is_Windowed(void) { return IsWindowed; }

    static bool Registry_Save_Render_Device(const char* sub_key, int device, int width, int height,
        int depth, bool windowed, int texture_depth);
    static bool Registry_Load_Render_Device(const char* sub_key, char* device, int device_len,
        int& width, int& height, int& depth, int& windowed, int& texture_depth);

    static void Set_Texture_Bitdepth(int depth) { TextureBitDepth = depth; }
    static int  Get_Texture_Bitdepth(void) { return TextureBitDepth; }
    static void Set_Swap_Interval(int swap);
    static int  Get_Swap_Interval(void);
    static void Set_Polygon_Mode(int mode);

    static bool Create_Device(void);
    static void Release_Device(void);
    static void Reset_Statistics();
    static void Enumerate_Devices();
    static void Set_Default_Global_Render_States(void);

    // ---- Member Variables ----
    static DX8_CleanupHook* m_pCleanupHook;
    static RenderStateStruct render_state;
    static unsigned render_state_changed;
    static Matrix4x4 DX8Transforms[D3DTS_WORLD + 1];

    static bool IsInitted;
    static bool IsDeviceLost;
    static void* Hwnd;
    static unsigned _MainThreadID;
    static bool _EnableTriangleDraw;

    static int CurRenderDevice;
    static int ResolutionWidth;
    static int ResolutionHeight;
    static int BitDepth;
    static int TextureBitDepth;
    static bool IsWindowed;
    static D3DFORMAT DisplayFormat;

    static D3DMATRIX old_world;
    static D3DMATRIX old_view;
    static D3DMATRIX old_prj;

    static DWORD Vertex_Shader;
    static DWORD Pixel_Shader;
    static Vector4 Vertex_Shader_Constants[MAX_VERTEX_SHADER_CONSTANTS];
    static Vector4 Pixel_Shader_Constants[MAX_PIXEL_SHADER_CONSTANTS];

    static LightEnvironmentClass* Light_Environment;
    static RenderInfoClass* Render_Info;
    static DWORD Vertex_Processing_Behavior;
    static ZTextureClass* Shadow_Map[MAX_SHADOW_MAPS];
    static Vector3 Ambient_Color;

    static bool world_identity;
    static unsigned RenderStates[256];
    static unsigned TextureStageStates[MAX_TEXTURE_STAGES][32];
    static IDirect3DBaseTexture8* Textures[MAX_TEXTURE_STAGES];

    static bool FogEnable;
    static D3DCOLOR FogColor;

    static unsigned long FrameCount;
    static DX8Caps* CurrentCaps;
    static D3DADAPTER_IDENTIFIER8 CurrentAdapterIdentifier;

    // Stub device objects (returned by _Get_D3D_Device8 / _Get_D3D8 for compatibility)
    static IDirect3DDevice8 s_stubDevice;
    static IDirect3D8 s_stubD3D;

    static IDirect3DSurface8* CurrentRenderTarget;
    static IDirect3DSurface8* CurrentDepthBuffer;
    static IDirect3DSurface8* DefaultRenderTarget;
    static IDirect3DSurface8* DefaultDepthBuffer;

    static unsigned DrawPolygonLowBoundLimit;
    static bool IsRenderToTexture;

    static int ZBias;
    static float ZNear;
    static float ZFar;
    static Matrix4x4 ProjectionMatrix;

    static bool CurrentDX8LightEnables[4];

    // BGFX-specific state
    static bgfx::UniformHandle s_uTexTransform[4];
    static bgfx::UniformHandle s_uTexCoordSource;
    static bgfx::UniformHandle s_uTexTransformFlags;

    friend void DX8_Assert();
    friend class WW3D;
    friend class DX8IndexBufferClass;
    friend class DX8VertexBufferClass;
};

// ============================================================================
// Compatibility alias - makes all existing DX8Wrapper:: calls work
// ============================================================================
#define DX8Wrapper BGFXWrapper

// ============================================================================
// Inline Implementations
// ============================================================================

WWINLINE Vector4 BGFXWrapper::Convert_Color(unsigned color)
{
    Vector4 col;
    col[3] = ((color & 0xff000000) >> 24) / 255.0f;
    col[0] = ((color & 0xff0000) >> 16) / 255.0f;
    col[1] = ((color & 0xff00) >> 8) / 255.0f;
    col[2] = ((color & 0xff) >> 0) / 255.0f;
    return col;
}

WWINLINE unsigned int BGFXWrapper::Convert_Color(const Vector3& color, float alpha)
{
    // Portable C++ version (replaces x86 __asm version from original)
    unsigned int a = (unsigned int)(alpha * 255.0f);
    unsigned int r = (unsigned int)(color.X * 255.0f);
    unsigned int g = (unsigned int)(color.Y * 255.0f);
    unsigned int b = (unsigned int)(color.Z * 255.0f);
    return (a << 24) | (r << 16) | (g << 8) | b;
}

WWINLINE unsigned int BGFXWrapper::Convert_Color(const Vector4& color)
{
    return Convert_Color(reinterpret_cast<const Vector3&>(color), color[3]);
}

WWINLINE void BGFXWrapper::Clamp_Color(Vector4& color)
{
    for (int i = 0; i < 4; ++i) {
        if (color[i] < 0.0f) color[i] = 0.0f;
        if (color[i] > 1.0f) color[i] = 1.0f;
    }
}

WWINLINE unsigned int BGFXWrapper::Convert_Color_Clamp(const Vector4& color)
{
    Vector4 clamped_color = color;
    BGFXWrapper::Clamp_Color(clamped_color);
    return Convert_Color(reinterpret_cast<const Vector3&>(clamped_color), clamped_color[3]);
}

WWINLINE void BGFXWrapper::Set_Alpha(const float alpha, unsigned int& color)
{
    unsigned char* component = (unsigned char*)&color;
    component[3] = (unsigned char)(255.0f * alpha);
}

WWINLINE void BGFXWrapper::Set_Index_Buffer_Index_Offset(unsigned offset)
{
    if (render_state.index_base_offset == offset) return;
    render_state.index_base_offset = (unsigned short)offset;
    render_state_changed |= INDEX_BUFFER_CHANGED;
}

WWINLINE const D3DLIGHT8& BGFXWrapper::Peek_Light(unsigned index)
{
    return render_state.Lights[index];
}

WWINLINE bool BGFXWrapper::Is_Light_Enabled(unsigned index)
{
    return render_state.LightEnable[index];
}

WWINLINE void BGFXWrapper::Set_Vertex_Shader(DWORD vertex_shader)
{
    Vertex_Shader = vertex_shader;
}

WWINLINE void BGFXWrapper::Set_Pixel_Shader(DWORD pixel_shader)
{
    if (Pixel_Shader == pixel_shader) return;
    Pixel_Shader = pixel_shader;
}

WWINLINE void BGFXWrapper::Set_Vertex_Shader_Constant(int reg, const void* data, int count)
{
    int memsize = sizeof(Vector4) * count;
    if (memcmp(data, &Vertex_Shader_Constants[reg], memsize) == 0) return;
    memcpy(&Vertex_Shader_Constants[reg], data, memsize);
}

WWINLINE void BGFXWrapper::Set_Pixel_Shader_Constant(int reg, const void* data, int count)
{
    int memsize = sizeof(Vector4) * count;
    if (memcmp(data, &Pixel_Shader_Constants[reg], memsize) == 0) return;
    memcpy(&Pixel_Shader_Constants[reg], data, memsize);
}

#endif // BGFX_WRAPPER_H
