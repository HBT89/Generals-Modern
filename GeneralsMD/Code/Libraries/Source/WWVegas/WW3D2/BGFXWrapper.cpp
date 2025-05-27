// BGFXWrapper.cpp
// Implementation of BGFXWrapper for WW3D replacement
#include "BGFXWrapper.h"
#include "matrix4.h"
#include <bgfx/bgfx.h>
#include <array>

// Per-stage texture transform and state (for up to 4 stages)
namespace {
    constexpr int kMaxStages = 4;
    std::array<Matrix4x4, kMaxStages> s_texTransforms;
    std::array<int, kMaxStages> s_texCoordSources = {BGFXWrapper::CAMERA_SPACE_POSITION};
    std::array<int, kMaxStages> s_texTransformFlags = {BGFXWrapper::COUNT2};
}

// BGFX uniform handles (static members)
bgfx::UniformHandle BGFXWrapper::s_uTexTransform[4] = {BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE};
bgfx::UniformHandle BGFXWrapper::s_uTexCoordSource = BGFX_INVALID_HANDLE;
bgfx::UniformHandle BGFXWrapper::s_uTexTransformFlags = BGFX_INVALID_HANDLE;

int BGFXWrapper::s_width = 0;
int BGFXWrapper::s_height = 0;
int BGFXWrapper::s_bits = 32;
bool BGFXWrapper::s_windowed = true;
bool BGFXWrapper::s_initted = false;

bool BGFXWrapper::Init(void* hwnd, int width, int height, bool windowed) {
    if (s_initted) return true;
    bgfx::PlatformData pd = {};
    pd.nwh = hwnd;
    bgfx::setPlatformData(pd);
    bgfx::Init init;
    init.type = bgfx::RendererType::Count; // Auto-detect
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = windowed ? BGFX_RESET_NONE : BGFX_RESET_FULLSCREEN;
    s_width = width;
    s_height = height;
    s_windowed = windowed;
    s_bits = 32;
    s_initted = bgfx::init(init);
    if (s_initted) {
        // Create uniforms for up to 4 stages
        for (int i = 0; i < 4; ++i) {
            s_uTexTransform[i] = bgfx::createUniform(("u_texTransform" + std::to_string(i)).c_str(), bgfx::UniformType::Mat4);
        }
        s_uTexCoordSource = bgfx::createUniform("u_texCoordSource", bgfx::UniformType::Vec4);
        s_uTexTransformFlags = bgfx::createUniform("u_texTransformFlags", bgfx::UniformType::Vec4);
    }
    return s_initted;
}

void BGFXWrapper::Shutdown() {
    if (s_initted) {
        // Destroy uniforms
        for (int i = 0; i < 4; ++i) {
            if (bgfx::isValid(s_uTexTransform[i])) bgfx::destroy(s_uTexTransform[i]);
            s_uTexTransform[i] = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(s_uTexCoordSource)) bgfx::destroy(s_uTexCoordSource);
        s_uTexCoordSource = BGFX_INVALID_HANDLE;
        if (bgfx::isValid(s_uTexTransformFlags)) bgfx::destroy(s_uTexTransformFlags);
        s_uTexTransformFlags = BGFX_INVALID_HANDLE;
        bgfx::shutdown();
        s_initted = false;
    }
}

bool BGFXWrapper::Set_Render_Device(int width, int height, int bits, int windowed, bool resize_window) {
    if (!s_initted) return false;
    bgfx::reset(width, height, windowed ? BGFX_RESET_NONE : BGFX_RESET_FULLSCREEN);
    s_width = width;
    s_height = height;
    s_windowed = windowed;
    s_bits = bits;
    return true;
}

void BGFXWrapper::Get_Render_Target_Resolution(int& width, int& height, int& bits, bool& windowed) {
    width = s_width;
    height = s_height;
    bits = s_bits;
    windowed = s_windowed;
}

void BGFXWrapper::Get_Device_Resolution(int& width, int& height, int& bits, bool& windowed) {
    Get_Render_Target_Resolution(width, height, bits, windowed);
}

bool BGFXWrapper::Toggle_Windowed() {
    return Set_Render_Device(s_width, s_height, s_bits, !s_windowed, true);
}

int BGFXWrapper::Get_Render_Device_Count() {
    // BGFX abstracts this, so return 1
    return 1;
}

int BGFXWrapper::Get_Render_Device() {
    return 0;
}

const char* BGFXWrapper::Get_Render_Device_Name(int device_index) {
    return "BGFX";
}

void BGFXWrapper::Set_Texture_Transform(int stage, const Matrix4x4& mat) {
    if (stage >= 0 && stage < kMaxStages) {
        s_texTransforms[stage] = mat;
        // In a real implementation, upload as a uniform to the shader here
        // Example: bgfx::setUniform(u_texTransform[stage], &mat, 1);
    }
}

void BGFXWrapper::Set_Texture_Coord_Source(int stage, int source) {
    if (stage >= 0 && stage < kMaxStages) {
        s_texCoordSources[stage] = source;
        // In a real implementation, upload as a uniform or set a flag for the shader
    }
}

void BGFXWrapper::Set_Texture_Transform_Flags(int stage, int flags) {
    if (stage >= 0 && stage < kMaxStages) {
        s_texTransformFlags[stage] = flags;
        // In a real implementation, upload as a uniform or set a flag for the shader
    }
}

void BGFXWrapper::Upload_Texture_Uniforms() {
    // Upload per-stage texture transforms
    for (int i = 0; i < 4; ++i) {
        bgfx::setUniform(s_uTexTransform[i], s_texTransforms[i].m, 1);
    }
    // Pack coord sources and flags into vec4s (one per 4 stages)
    float coordSources[4] = { float(s_texCoordSources[0]), float(s_texCoordSources[1]), float(s_texCoordSources[2]), float(s_texCoordSources[3]) };
    float transformFlags[4] = { float(s_texTransformFlags[0]), float(s_texTransformFlags[1]), float(s_texTransformFlags[2]), float(s_texTransformFlags[3]) };
    bgfx::setUniform(s_uTexCoordSource, coordSources, 1);
    bgfx::setUniform(s_uTexTransformFlags, transformFlags, 1);
}
