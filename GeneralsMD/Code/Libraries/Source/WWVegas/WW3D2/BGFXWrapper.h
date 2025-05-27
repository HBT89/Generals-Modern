// BGFXWrapper.h
// Wrapper class to abstract BGFX initialization and device management for WW3D replacement
#pragma once

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <string>

class BGFXWrapper {
public:
    static bool Init(void* hwnd, int width, int height, bool windowed);
    static void Shutdown();
    static bool Set_Render_Device(int width, int height, int bits, int windowed, bool resize_window);
    static void Get_Render_Target_Resolution(int& width, int& height, int& bits, bool& windowed);
    static void Get_Device_Resolution(int& width, int& height, int& bits, bool& windowed);
    static bool Toggle_Windowed();
    static int  Get_Render_Device_Count();
    static int  Get_Render_Device();
    static const char* Get_Render_Device_Name(int device_index);
    // ... add more as needed to match DX8Wrapper interface ...

    // BGFX texture transform/state API for matrixmapper migration
    enum TexCoordSource {
        CAMERA_SPACE_POSITION = 0,
        CAMERA_SPACE_NORMAL = 1
    };
    enum TexTransformFlags {
        COUNT2 = 0x01,
        COUNT3 = 0x02,
        PROJECTED = 0x04
    };
    static void Set_Texture_Transform(int stage, const Matrix4x4& mat);
    static void Set_Texture_Coord_Source(int stage, int source);
    static void Set_Texture_Transform_Flags(int stage, int flags);
    static void Upload_Texture_Uniforms(); // Uploads texture transform/state to GPU

private:
    static int s_width;
    static int s_height;
    static int s_bits;
    static bool s_windowed;
    static bool s_initted;

    // BGFX uniform handles for texture transforms and state (up to 4 stages)
    static bgfx::UniformHandle s_uTexTransform[4];
    static bgfx::UniformHandle s_uTexCoordSource;
    static bgfx::UniformHandle s_uTexTransformFlags;
};
