// RenderTypes.h
// Compatibility type definitions for the DX8-to-BGFX migration.
// Defines D3D8 types (enums, structs) used throughout the codebase as
// engine-native types, allowing compilation without the DirectX 8 SDK.
#pragma once

#ifndef RENDER_TYPES_H
#define RENDER_TYPES_H

// Block DX90SDK stub headers — RenderTypes.h is the single source of truth
// for all D3D placeholder types used in the BGFX port.
#define _D3D8TYPES_H_
#define _D3D8CAPS_H_
#define _D3D8_H_
#define _D3DX8MATH_H_
#define _D3DX8CORE_H_
#define _D3DX8TEX_H_

#include <windows.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <bgfx/bgfx.h>

// ============================================================================
// D3D Result Codes
// ============================================================================
#ifndef D3D_OK
#define D3D_OK 0
#endif

#define D3DERR_DEVICELOST           ((HRESULT)0x88760868L)
#define D3DERR_DEVICENOTRESET       ((HRESULT)0x88760869L)
#define D3DERR_NOTAVAILABLE         ((HRESULT)0x88760826L)
#define D3DERR_OUTOFVIDEOMEMORY     ((HRESULT)0x8876017CL)
#define D3DERR_CONFLICTINGTEXTUREFILTER   ((HRESULT)0x88760873L)
#define D3DERR_CONFLICTINGTEXTUREPALETTE  ((HRESULT)0x88760874L)
#define D3DERR_TOOMANYOPERATIONS          ((HRESULT)0x88760877L)
#define D3DERR_UNSUPPORTEDALPHAARG        ((HRESULT)0x88760878L)
#define D3DERR_UNSUPPORTEDALPHAOPERATION  ((HRESULT)0x88760879L)
#define D3DERR_UNSUPPORTEDCOLORARG        ((HRESULT)0x8876087AL)
#define D3DERR_UNSUPPORTEDCOLOROPERATION  ((HRESULT)0x8876087BL)
#define D3DERR_UNSUPPORTEDFACTORVALUE     ((HRESULT)0x8876087CL)
#define D3DERR_UNSUPPORTEDTEXTUREFILTER   ((HRESULT)0x8876087DL)
#define D3DERR_WRONGTEXTUREFORMAT         ((HRESULT)0x88760818L)

// ============================================================================
// D3DLOCK flags
// ============================================================================
#define D3DLOCK_READONLY        0x00000010
#define D3DLOCK_DISCARD         0x00002000
#define D3DLOCK_NOOVERWRITE     0x00001000
#define D3DLOCK_NOSYSLOCK       0x00000800

// ============================================================================
// D3D Backbuffer type
// ============================================================================
#define D3DBACKBUFFER_TYPE_MONO 0

// ============================================================================
// D3DCUBEMAP_FACES
// ============================================================================
typedef enum _D3DCUBEMAP_FACES {
    D3DCUBEMAP_FACE_POSITIVE_X = 0,
    D3DCUBEMAP_FACE_NEGATIVE_X = 1,
    D3DCUBEMAP_FACE_POSITIVE_Y = 2,
    D3DCUBEMAP_FACE_NEGATIVE_Y = 3,
    D3DCUBEMAP_FACE_POSITIVE_Z = 4,
    D3DCUBEMAP_FACE_NEGATIVE_Z = 5,
    D3DCUBEMAP_FACE_FORCE_DWORD = 0x7fffffff
} D3DCUBEMAP_FACES;

// ============================================================================
// D3DLOCKED_BOX (for volume textures)
// ============================================================================
typedef struct _D3DLOCKED_BOX {
    INT     RowPitch;
    INT     SlicePitch;
    void*   pBits;
} D3DLOCKED_BOX;

// D3D Texture filter capability flags (moved here so macros are available early)
// D3D Texture operation capability flags
#define D3DTEXOPCAPS_DISABLE                0x00000001
#define D3DTEXOPCAPS_SELECTARG1             0x00000002
#define D3DTEXOPCAPS_SELECTARG2             0x00000004
#define D3DTEXOPCAPS_MODULATE               0x00000008
#define D3DTEXOPCAPS_MODULATE2X             0x00000010
#define D3DTEXOPCAPS_MODULATE4X             0x00000020
#define D3DTEXOPCAPS_ADD                    0x00000040
#define D3DTEXOPCAPS_ADDSIGNED              0x00000080
#define D3DTEXOPCAPS_ADDSIGNED2X            0x00000100
#define D3DTEXOPCAPS_SUBTRACT               0x00000200
#define D3DTEXOPCAPS_ADDSMOOTH              0x00000400
#define D3DTEXOPCAPS_BLENDDIFFUSEALPHA      0x00000800
#define D3DTEXOPCAPS_BLENDTEXTUREALPHA      0x00001000
#define D3DTEXOPCAPS_BLENDFACTORALPHA       0x00002000
#define D3DTEXOPCAPS_BLENDTEXTUREALPHAPM    0x00004000
#define D3DTEXOPCAPS_BLENDCURRENTALPHA      0x00008000
#define D3DTEXOPCAPS_PREMODULATE            0x00010000
#define D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR 0x00020000
#define D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA 0x00040000
#define D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR 0x00080000
#define D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA 0x00100000
#define D3DTEXOPCAPS_BUMPENVMAP             0x00200000
#define D3DTEXOPCAPS_BUMPENVMAPLUMINANCE    0x00400000
#define D3DTEXOPCAPS_DOTPRODUCT3            0x00800000
#define D3DTEXOPCAPS_MULTIPLYADD            0x01000000
#define D3DTEXOPCAPS_LERP                   0x02000000

// D3D Usage flags
#define D3DUSAGE_SOFTWAREPROCESSING         0x00000010
#define D3DUSAGE_RENDERTARGET               0x00000001
#define D3DUSAGE_DEPTHSTENCIL               0x00000002
#define D3DUSAGE_WRITEONLY                   0x00000008
#define D3DUSAGE_DYNAMIC                    0x00000200
#define D3DUSAGE_NPATCHES                   0x00000100
#define D3DUSAGE_POINTS                     0x00000040
#define D3DUSAGE_RTPATCHES                  0x00000080

// D3D Device creation flags
#define D3DCREATE_SOFTWARE_VERTEXPROCESSING  0x00000020
#define D3DCREATE_HARDWARE_VERTEXPROCESSING  0x00000040
#define D3DCREATE_MIXED_VERTEXPROCESSING     0x00000080

// D3D Swap effect
#define D3DSWAPEFFECT_DISCARD               1
#define D3DSWAPEFFECT_FLIP                  2
#define D3DSWAPEFFECT_COPY                  3

// D3D Texture filter capability flags
#define D3DPTFILTERCAPS_MAGFLINEAR        0x01000000
#define D3DPTFILTERCAPS_MINFLINEAR        0x00000100
#define D3DPTFILTERCAPS_MAGFANISOTROPIC   0x04000000
#define D3DPTFILTERCAPS_MINFANISOTROPIC   0x00000400
#define D3DPTFILTERCAPS_MIPFLINEAR        0x00020000

// ============================================================================
// D3DCOLOR - 32-bit ARGB packed color
// ============================================================================
typedef DWORD D3DCOLOR;

#ifndef D3DCOLOR_ARGB
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#endif

#ifndef D3DCOLOR_XRGB
#define D3DCOLOR_XRGB(r,g,b) D3DCOLOR_ARGB(0xff,r,g,b)
#endif

#ifndef D3DCOLOR_COLORVALUE
#define D3DCOLOR_COLORVALUE(r,g,b,a) \
    D3DCOLOR_ARGB((DWORD)((a)*255.f),(DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f))
#endif

// ============================================================================
// D3DCOLORVALUE
// ============================================================================
typedef struct _D3DCOLORVALUE {
    float r, g, b, a;
} D3DCOLORVALUE;

// ============================================================================
// D3DMATRIX
// ============================================================================
typedef struct _D3DMATRIX {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} D3DMATRIX;

// ============================================================================
// D3DVECTOR
// ============================================================================
typedef struct _D3DVECTOR {
    float x, y, z;
} D3DVECTOR;

// ============================================================================
// D3DFORMAT - Surface/texture format enumeration
// ============================================================================
typedef enum _D3DFORMAT {
    D3DFMT_UNKNOWN              = 0,
    D3DFMT_R8G8B8               = 20,
    D3DFMT_A8R8G8B8             = 21,
    D3DFMT_X8R8G8B8             = 22,
    D3DFMT_R5G6B5               = 23,
    D3DFMT_X1R5G5B5             = 24,
    D3DFMT_A1R5G5B5             = 25,
    D3DFMT_A4R4G4B4             = 26,
    D3DFMT_R3G3B2               = 27,
    D3DFMT_A8                   = 28,
    D3DFMT_A8R3G3B2             = 29,
    D3DFMT_X4R4G4B4             = 30,
    D3DFMT_A2B10G10R10          = 31,
    D3DFMT_G16R16               = 34,
    D3DFMT_A8P8                 = 40,
    D3DFMT_P8                   = 41,
    D3DFMT_L8                   = 50,
    D3DFMT_A8L8                 = 51,
    D3DFMT_A4L4                 = 52,
    D3DFMT_V8U8                 = 60,
    D3DFMT_L6V5U5               = 61,
    D3DFMT_X8L8V8U8             = 62,
    D3DFMT_Q8W8V8U8             = 63,
    D3DFMT_V16U16               = 64,
    D3DFMT_W11V11U10            = 65,
    D3DFMT_A2W10V10U10          = 67,
    D3DFMT_UYVY                 = 0x59565955, // MAKEFOURCC('U','Y','V','Y')
    D3DFMT_YUY2                 = 0x32595559, // MAKEFOURCC('Y','U','Y','2')
    D3DFMT_DXT1                 = 0x31545844, // MAKEFOURCC('D','X','T','1')
    D3DFMT_DXT2                 = 0x32545844,
    D3DFMT_DXT3                 = 0x33545844,
    D3DFMT_DXT4                 = 0x34545844,
    D3DFMT_DXT5                 = 0x35545844,
    D3DFMT_D16_LOCKABLE         = 70,
    D3DFMT_D32                  = 71,
    D3DFMT_D15S1                = 73,
    D3DFMT_D24S8                = 75,
    D3DFMT_D16                  = 80,
    D3DFMT_D24X8                = 77,
    D3DFMT_D24X4S4              = 79,
    D3DFMT_VERTEXDATA           = 100,
    D3DFMT_INDEX16              = 101,
    D3DFMT_INDEX32              = 102,
    D3DFMT_FORCE_DWORD          = 0x7fffffff
} D3DFORMAT;

// ============================================================================
// D3DPOOL - Memory pool for resources
// ============================================================================
typedef enum _D3DPOOL {
    D3DPOOL_DEFAULT     = 0,
    D3DPOOL_MANAGED     = 1,
    D3DPOOL_SYSTEMMEM   = 2,
    D3DPOOL_SCRATCH     = 3,
    D3DPOOL_FORCE_DWORD = 0x7fffffff
} D3DPOOL;

// ============================================================================
// D3DTRANSFORMSTATETYPE
// ============================================================================
typedef enum _D3DTRANSFORMSTATETYPE {
    D3DTS_VIEW          = 2,
    D3DTS_PROJECTION    = 3,
    D3DTS_WORLD         = 256,
    D3DTS_TEXTURE0      = 16,
    D3DTS_TEXTURE1      = 17,
    D3DTS_TEXTURE2      = 18,
    D3DTS_TEXTURE3      = 19,
    D3DTS_TEXTURE4      = 20,
    D3DTS_TEXTURE5      = 21,
    D3DTS_TEXTURE6      = 22,
    D3DTS_TEXTURE7      = 23,
    D3DTS_FORCE_DWORD   = 0x7fffffff
} D3DTRANSFORMSTATETYPE;

// ============================================================================
// D3DRENDERSTATETYPE
// ============================================================================
typedef enum _D3DRENDERSTATETYPE {
    D3DRS_ZENABLE                   = 7,
    D3DRS_FILLMODE                  = 8,
    D3DRS_SHADEMODE                 = 9,
    D3DRS_LINEPATTERN               = 10,
    D3DRS_ZWRITEENABLE              = 14,
    D3DRS_ALPHATESTENABLE           = 15,
    D3DRS_LASTPIXEL                 = 16,
    D3DRS_SRCBLEND                  = 19,
    D3DRS_DESTBLEND                 = 20,
    D3DRS_CULLMODE                  = 22,
    D3DRS_ZFUNC                     = 23,
    D3DRS_ALPHAREF                  = 24,
    D3DRS_ALPHAFUNC                 = 25,
    D3DRS_DITHERENABLE              = 26,
    D3DRS_ALPHABLENDENABLE          = 27,
    D3DRS_FOGENABLE                 = 28,
    D3DRS_SPECULARENABLE            = 29,
    D3DRS_ZVISIBLE                  = 30,
    D3DRS_FOGCOLOR                  = 34,
    D3DRS_FOGTABLEMODE              = 35,
    D3DRS_FOGSTART                  = 36,
    D3DRS_FOGEND                    = 37,
    D3DRS_FOGDENSITY                = 38,
    D3DRS_EDGEANTIALIAS             = 40,
    D3DRS_ZBIAS                     = 47,
    D3DRS_RANGEFOGENABLE            = 48,
    D3DRS_STENCILENABLE             = 52,
    D3DRS_STENCILFAIL               = 53,
    D3DRS_STENCILZFAIL              = 54,
    D3DRS_STENCILPASS               = 55,
    D3DRS_STENCILFUNC               = 56,
    D3DRS_STENCILREF                = 57,
    D3DRS_STENCILMASK               = 58,
    D3DRS_STENCILWRITEMASK          = 59,
    D3DRS_TEXTUREFACTOR             = 60,
    D3DRS_WRAP0                     = 128,
    D3DRS_WRAP1                     = 129,
    D3DRS_WRAP2                     = 130,
    D3DRS_WRAP3                     = 131,
    D3DRS_WRAP4                     = 132,
    D3DRS_WRAP5                     = 133,
    D3DRS_WRAP6                     = 134,
    D3DRS_WRAP7                     = 135,
    D3DRS_CLIPPING                  = 136,
    D3DRS_LIGHTING                  = 137,
    D3DRS_AMBIENT                   = 139,
    D3DRS_FOGVERTEXMODE             = 140,
    D3DRS_COLORVERTEX               = 141,
    D3DRS_LOCALVIEWER               = 142,
    D3DRS_NORMALIZENORMALS          = 143,
    D3DRS_DIFFUSEMATERIALSOURCE     = 145,
    D3DRS_SPECULARMATERIALSOURCE    = 146,
    D3DRS_AMBIENTMATERIALSOURCE     = 147,
    D3DRS_EMISSIVEMATERIALSOURCE    = 148,
    D3DRS_VERTEXBLEND               = 151,
    D3DRS_CLIPPLANEENABLE           = 152,
    D3DRS_SOFTWAREVERTEXPROCESSING  = 153,
    D3DRS_POINTSIZE                 = 154,
    D3DRS_POINTSIZE_MIN             = 155,
    D3DRS_POINTSPRITEENABLE         = 156,
    D3DRS_POINTSCALEENABLE          = 157,
    D3DRS_POINTSCALE_A              = 158,
    D3DRS_POINTSCALE_B              = 159,
    D3DRS_POINTSCALE_C              = 160,
    D3DRS_MULTISAMPLEANTIALIAS      = 161,
    D3DRS_MULTISAMPLEMASK           = 162,
    D3DRS_PATCHEDGESTYLE            = 163,
    D3DRS_PATCHSEGMENTS             = 164,
    D3DRS_DEBUGMONITORTOKEN         = 165,
    D3DRS_POINTSIZE_MAX             = 166,
    D3DRS_INDEXEDVERTEXBLENDENABLE  = 167,
    D3DRS_COLORWRITEENABLE          = 168,
    D3DRS_TWEENFACTOR               = 170,
    D3DRS_BLENDOP                   = 171,
    D3DRS_POSITIONORDER             = 172,
    D3DRS_NORMALORDER               = 173,
    D3DRS_FORCE_DWORD               = 0x7fffffff
} D3DRENDERSTATETYPE;

// ============================================================================
// D3DTEXTURESTAGESTATETYPE
// ============================================================================
typedef enum _D3DTEXTURESTAGESTATETYPE {
    D3DTSS_COLOROP                  = 1,
    D3DTSS_COLORARG1                = 2,
    D3DTSS_COLORARG2                = 3,
    D3DTSS_ALPHAOP                  = 4,
    D3DTSS_ALPHAARG1                = 5,
    D3DTSS_ALPHAARG2                = 6,
    D3DTSS_BUMPENVMAT00             = 7,
    D3DTSS_BUMPENVMAT01             = 8,
    D3DTSS_BUMPENVMAT10             = 9,
    D3DTSS_BUMPENVMAT11             = 10,
    D3DTSS_TEXCOORDINDEX            = 11,
    D3DTSS_ADDRESSU                 = 13,
    D3DTSS_ADDRESSV                 = 14,
    D3DTSS_BORDERCOLOR              = 15,
    D3DTSS_MAGFILTER                = 16,
    D3DTSS_MINFILTER                = 17,
    D3DTSS_MIPFILTER                = 18,
    D3DTSS_MIPMAPLODBIAS            = 19,
    D3DTSS_MAXMIPLEVEL              = 20,
    D3DTSS_MAXANISOTROPY            = 21,
    D3DTSS_BUMPENVLSCALE            = 22,
    D3DTSS_BUMPENVLOFFSET           = 23,
    D3DTSS_TEXTURETRANSFORMFLAGS    = 24,
    D3DTSS_ADDRESSW                 = 25,
    D3DTSS_COLORARG0                = 26,
    D3DTSS_ALPHAARG0                = 27,
    D3DTSS_RESULTARG                = 28,
    D3DTSS_FORCE_DWORD              = 0x7fffffff
} D3DTEXTURESTAGESTATETYPE;

// ============================================================================
// Texture Stage State Values
// ============================================================================

typedef enum _D3DTEXTUREOP {
    D3DTOP_DISABLE                  = 1,
    D3DTOP_SELECTARG1               = 2,
    D3DTOP_SELECTARG2               = 3,
    D3DTOP_MODULATE                 = 4,
    D3DTOP_MODULATE2X               = 5,
    D3DTOP_MODULATE4X               = 6,
    D3DTOP_ADD                      = 7,
    D3DTOP_ADDSIGNED                = 8,
    D3DTOP_ADDSIGNED2X              = 9,
    D3DTOP_SUBTRACT                 = 10,
    D3DTOP_ADDSMOOTH                = 11,
    D3DTOP_BLENDDIFFUSEALPHA        = 12,
    D3DTOP_BLENDTEXTUREALPHA        = 13,
    D3DTOP_BLENDFACTORALPHA         = 14,
    D3DTOP_BLENDTEXTUREALPHAPM      = 15,
    D3DTOP_BLENDCURRENTALPHA        = 16,
    D3DTOP_PREMODULATE              = 17,
    D3DTOP_MODULATEALPHA_ADDCOLOR   = 18,
    D3DTOP_MODULATECOLOR_ADDALPHA   = 19,
    D3DTOP_MODULATEINVALPHA_ADDCOLOR= 20,
    D3DTOP_MODULATEINVCOLOR_ADDALPHA= 21,
    D3DTOP_BUMPENVMAP               = 22,
    D3DTOP_BUMPENVMAPLUMINANCE      = 23,
    D3DTOP_DOTPRODUCT3              = 24,
    D3DTOP_MULTIPLYADD              = 25,
    D3DTOP_LERP                     = 26,
} D3DTEXTUREOP;

// Texture arguments
#define D3DTA_DIFFUSE           0x00000000
#define D3DTA_CURRENT           0x00000001
#define D3DTA_TEXTURE           0x00000002
#define D3DTA_TFACTOR           0x00000003
#define D3DTA_SPECULAR          0x00000004
#define D3DTA_TEMP              0x00000005
#define D3DTA_COMPLEMENT        0x00000010
#define D3DTA_ALPHAREPLICATE    0x00000020

// Texture address modes
enum {
    D3DTADDRESS_WRAP        = 1,
    D3DTADDRESS_MIRROR      = 2,
    D3DTADDRESS_CLAMP       = 3,
    D3DTADDRESS_BORDER      = 4,
    D3DTADDRESS_MIRRORONCE  = 5,
};

// Texture filter types
enum {
    D3DTEXF_NONE            = 0,
    D3DTEXF_POINT           = 1,
    D3DTEXF_LINEAR          = 2,
    D3DTEXF_ANISOTROPIC     = 3,
    D3DTEXF_FLATCUBIC       = 4,
    D3DTEXF_GAUSSIANCUBIC   = 5,
};

// Texture coordinate index flags
#define D3DTSS_TCI_PASSTHRU                     0x00000000
#define D3DTSS_TCI_CAMERASPACENORMAL            0x00010000
#define D3DTSS_TCI_CAMERASPACEPOSITION          0x00020000
#define D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR  0x00030000

// Texture transform flags
enum {
    D3DTTFF_DISABLE = 0,
    D3DTTFF_COUNT1  = 1,
    D3DTTFF_COUNT2  = 2,
    D3DTTFF_COUNT3  = 3,
    D3DTTFF_COUNT4  = 4,
    D3DTTFF_PROJECTED = 256,
};

// ============================================================================
// Blend modes
// ============================================================================
typedef enum _D3DBLEND {
    D3DBLEND_ZERO               = 1,
    D3DBLEND_ONE                = 2,
    D3DBLEND_SRCCOLOR           = 3,
    D3DBLEND_INVSRCCOLOR        = 4,
    D3DBLEND_SRCALPHA           = 5,
    D3DBLEND_INVSRCALPHA        = 6,
    D3DBLEND_DESTALPHA          = 7,
    D3DBLEND_INVDESTALPHA       = 8,
    D3DBLEND_DESTCOLOR          = 9,
    D3DBLEND_INVDESTCOLOR       = 10,
    D3DBLEND_SRCALPHASAT        = 11,
    D3DBLEND_BOTHSRCALPHA       = 12,
    D3DBLEND_BOTHINVSRCALPHA    = 13,
} D3DBLEND;

// ============================================================================
// Compare functions
// ============================================================================
typedef enum _D3DCMPFUNC {
    D3DCMP_NEVER        = 1,
    D3DCMP_LESS         = 2,
    D3DCMP_EQUAL        = 3,
    D3DCMP_LESSEQUAL    = 4,
    D3DCMP_GREATER      = 5,
    D3DCMP_NOTEQUAL     = 6,
    D3DCMP_GREATEREQUAL = 7,
    D3DCMP_ALWAYS       = 8,
} D3DCMPFUNC;

// ============================================================================
// Cull modes
// ============================================================================
enum {
    D3DCULL_NONE    = 1,
    D3DCULL_CW      = 2,
    D3DCULL_CCW     = 3,
};

// ============================================================================
// Fill modes
// ============================================================================
enum {
    D3DFILL_POINT       = 1,
    D3DFILL_WIREFRAME   = 2,
    D3DFILL_SOLID       = 3,
};

// ============================================================================
// Shade modes
// ============================================================================
enum {
    D3DSHADE_FLAT       = 1,
    D3DSHADE_GOURAUD    = 2,
    D3DSHADE_PHONG      = 3,
};

// ============================================================================
// Fog modes
// ============================================================================
enum {
    D3DFOG_NONE     = 0,
    D3DFOG_EXP      = 1,
    D3DFOG_EXP2     = 2,
    D3DFOG_LINEAR   = 3,
};

// ============================================================================
// Stencil ops
// ============================================================================
enum {
    D3DSTENCILOP_KEEP       = 1,
    D3DSTENCILOP_ZERO       = 2,
    D3DSTENCILOP_REPLACE    = 3,
    D3DSTENCILOP_INCRSAT    = 4,
    D3DSTENCILOP_DECRSAT    = 5,
    D3DSTENCILOP_INVERT     = 6,
    D3DSTENCILOP_INCR       = 7,
    D3DSTENCILOP_DECR       = 8,
};

// ============================================================================
// Material color sources
// ============================================================================
enum {
    D3DMCS_MATERIAL     = 0,
    D3DMCS_COLOR1       = 1,
    D3DMCS_COLOR2       = 2,
};

// ============================================================================
// Blend ops
// ============================================================================
enum {
    D3DBLENDOP_ADD          = 1,
    D3DBLENDOP_SUBTRACT     = 2,
    D3DBLENDOP_REVSUBTRACT  = 3,
    D3DBLENDOP_MIN          = 4,
    D3DBLENDOP_MAX          = 5,
};

// ============================================================================
// Vertex blend flags
// ============================================================================
enum {
    D3DVBF_DISABLE  = 0,
    D3DVBF_1WEIGHTS = 1,
    D3DVBF_2WEIGHTS = 2,
    D3DVBF_3WEIGHTS = 3,
    D3DVBF_TWEENING = 255,
};

// ============================================================================
// Primitive types
// ============================================================================
typedef enum _D3DPRIMITIVETYPE {
    D3DPT_POINTLIST     = 1,
    D3DPT_LINELIST      = 2,
    D3DPT_LINESTRIP     = 3,
    D3DPT_TRIANGLELIST  = 4,
    D3DPT_TRIANGLESTRIP = 5,
    D3DPT_TRIANGLEFAN   = 6,
    D3DPT_FORCE_DWORD   = 0x7fffffff
} D3DPRIMITIVETYPE;

// ============================================================================
// Light types
// ============================================================================
typedef enum _D3DLIGHTTYPE {
    D3DLIGHT_POINT      = 1,
    D3DLIGHT_SPOT       = 2,
    D3DLIGHT_DIRECTIONAL= 3,
    D3DLIGHT_FORCE_DWORD= 0x7fffffff
} D3DLIGHTTYPE;

// ============================================================================
// D3DLIGHT8 structure
// ============================================================================
typedef struct _D3DLIGHT8 {
    D3DLIGHTTYPE    Type;
    D3DCOLORVALUE   Diffuse;
    D3DCOLORVALUE   Specular;
    D3DCOLORVALUE   Ambient;
    D3DVECTOR       Position;
    D3DVECTOR       Direction;
    float           Range;
    float           Falloff;
    float           Attenuation0;
    float           Attenuation1;
    float           Attenuation2;
    float           Theta;
    float           Phi;
} D3DLIGHT8;

// ============================================================================
// D3DMATERIAL8 structure
// ============================================================================
typedef struct _D3DMATERIAL8 {
    D3DCOLORVALUE   Diffuse;
    D3DCOLORVALUE   Ambient;
    D3DCOLORVALUE   Specular;
    D3DCOLORVALUE   Emissive;
    float           Power;
} D3DMATERIAL8;

// ============================================================================
// D3DVIEWPORT8 structure
// ============================================================================
typedef struct _D3DVIEWPORT8 {
    DWORD   X;
    DWORD   Y;
    DWORD   Width;
    DWORD   Height;
    float   MinZ;
    float   MaxZ;
} D3DVIEWPORT8;

// ============================================================================
// D3DDEVTYPE
// ============================================================================
typedef enum _D3DDEVTYPE {
    D3DDEVTYPE_HAL      = 1,
    D3DDEVTYPE_REF      = 2,
    D3DDEVTYPE_SW       = 3,
    D3DDEVTYPE_FORCE_DWORD = 0x7fffffff
} D3DDEVTYPE;

// ============================================================================
// D3DCAPS8 - Device capabilities (stub with commonly accessed fields)
// ============================================================================
typedef struct _D3DCAPS8 {
    D3DDEVTYPE  DeviceType;
    UINT        AdapterOrdinal;
    DWORD       Caps;
    DWORD       Caps2;
    DWORD       Caps3;
    DWORD       PresentationIntervals;
    DWORD       CursorCaps;
    DWORD       DevCaps;
    DWORD       PrimitiveMiscCaps;
    DWORD       RasterCaps;
    DWORD       ZCmpCaps;
    DWORD       SrcBlendCaps;
    DWORD       DestBlendCaps;
    DWORD       AlphaCmpCaps;
    DWORD       ShadeCaps;
    DWORD       TextureCaps;
    DWORD       TextureFilterCaps;
    DWORD       CubeTextureFilterCaps;
    DWORD       VolumeTextureFilterCaps;
    DWORD       TextureAddressCaps;
    DWORD       VolumeTextureAddressCaps;
    DWORD       LineCaps;
    DWORD       MaxTextureWidth;
    DWORD       MaxTextureHeight;
    DWORD       MaxVolumeExtent;
    DWORD       MaxTextureRepeat;
    DWORD       MaxTextureAspectRatio;
    DWORD       MaxAnisotropy;
    float       MaxVertexW;
    float       GuardBandLeft;
    float       GuardBandTop;
    float       GuardBandRight;
    float       GuardBandBottom;
    float       ExtentsAdjust;
    DWORD       StencilCaps;
    DWORD       FVFCaps;
    DWORD       TextureOpCaps;
    DWORD       MaxTextureBlendStages;
    DWORD       MaxSimultaneousTextures;
    DWORD       VertexProcessingCaps;
    DWORD       MaxActiveLights;
    DWORD       MaxUserClipPlanes;
    DWORD       MaxVertexBlendMatrices;
    DWORD       MaxVertexBlendMatrixIndex;
    float       MaxPointSize;
    DWORD       MaxPrimitiveCount;
    DWORD       MaxVertexIndex;
    DWORD       MaxStreams;
    DWORD       MaxStreamStride;
    DWORD       VertexShaderVersion;
    DWORD       MaxVertexShaderConst;
    DWORD       PixelShaderVersion;
    float       MaxPixelShaderValue;
} D3DCAPS8;

// ============================================================================
// D3DADAPTER_IDENTIFIER8
// ============================================================================
typedef struct _D3DADAPTER_IDENTIFIER8 {
    char    Driver[512];
    char    Description[512];
    LARGE_INTEGER DriverVersion;
    DWORD   VendorId;
    DWORD   DeviceId;
    DWORD   SubSysId;
    DWORD   Revision;
    GUID    DeviceIdentifier;
    DWORD   WHQLLevel;
} D3DADAPTER_IDENTIFIER8;

// ============================================================================
// D3DPRESENT_PARAMETERS
// ============================================================================
typedef struct _D3DPRESENT_PARAMETERS {
    UINT                BackBufferWidth;
    UINT                BackBufferHeight;
    D3DFORMAT           BackBufferFormat;
    UINT                BackBufferCount;
    DWORD               MultiSampleType;
    DWORD               SwapEffect;
    HWND                hDeviceWindow;
    BOOL                Windowed;
    BOOL                EnableAutoDepthStencil;
    D3DFORMAT           AutoDepthStencilFormat;
    DWORD               Flags;
    UINT                FullScreen_RefreshRateInHz;
    UINT                FullScreen_PresentationInterval;
} D3DPRESENT_PARAMETERS;

// ============================================================================
// D3DDISPLAYMODE
// ============================================================================
typedef struct _D3DDISPLAYMODE {
    UINT    Width;
    UINT    Height;
    UINT    RefreshRate;
    D3DFORMAT Format;
} D3DDISPLAYMODE;

// D3DBACKBUFFER_TYPE
typedef DWORD D3DBACKBUFFER_TYPE;
#ifndef D3DBACKBUFFER_TYPE_MONO
#define D3DBACKBUFFER_TYPE_MONO 0
#endif
#define D3DBACKBUFFER_TYPE_LEFT  1
#define D3DBACKBUFFER_TYPE_RIGHT 2

// D3DRECT - for Clear() viewport rect
typedef struct _D3DRECT {
    LONG x1, y1, x2, y2;
} D3DRECT;

// D3DGAMMARAMP
typedef struct _D3DGAMMARAMP {
    WORD red[256];
    WORD green[256];
    WORD blue[256];
} D3DGAMMARAMP;

// D3DDEVICE_CREATION_PARAMETERS
typedef struct _D3DDEVICE_CREATION_PARAMETERS {
    UINT   AdapterOrdinal;
    DWORD  DeviceType;
    HWND   hFocusWindow;
    DWORD  BehaviorFlags;
} D3DDEVICE_CREATION_PARAMETERS;

#define D3DADAPTER_DEFAULT 0

// ============================================================================
// D3DLOCKED_RECT
// ============================================================================
typedef struct _D3DLOCKED_RECT {
    INT     Pitch;
    void*   pBits;
} D3DLOCKED_RECT;

// ============================================================================
// D3DSURFACE_DESC
// ============================================================================
typedef struct _D3DSURFACE_DESC {
    D3DFORMAT   Format;
    DWORD       Type;
    DWORD       Usage;
    D3DPOOL     Pool;
    UINT        Size;
    DWORD       MultiSampleType;
    UINT        Width;
    UINT        Height;
} D3DSURFACE_DESC;

// ============================================================================
// D3DVOLUME_DESC
// ============================================================================
typedef struct _D3DVOLUME_DESC {
    D3DFORMAT   Format;
    DWORD       Type;
    DWORD       Usage;
    D3DPOOL     Pool;
    UINT        Size;
    UINT        Width;
    UINT        Height;
    UINT        Depth;
} D3DVOLUME_DESC;

// ============================================================================
// FVF (Flexible Vertex Format) constants
// ============================================================================
#define D3DFVF_XYZ              0x002
#define D3DFVF_XYZRHW           0x004
#define D3DFVF_XYZB1            0x006
#define D3DFVF_XYZB2            0x008
#define D3DFVF_XYZB3            0x00a
#define D3DFVF_XYZB4            0x00c
#define D3DFVF_XYZB5            0x00e
#define D3DFVF_NORMAL           0x010
#define D3DFVF_PSIZE            0x020
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_SPECULAR         0x080
#define D3DFVF_TEXCOUNT_MASK    0xf00
#define D3DFVF_TEXCOUNT_SHIFT   8
#define D3DFVF_TEX0             0x000
#define D3DFVF_TEX1             0x100
#define D3DFVF_TEX2             0x200
#define D3DFVF_TEX3             0x300
#define D3DFVF_TEX4             0x400
#define D3DFVF_TEX5             0x500
#define D3DFVF_TEX6             0x600
#define D3DFVF_TEX7             0x700
#define D3DFVF_TEX8             0x800
#define D3DFVF_POSITION_MASK    0x00e
#define D3DFVF_LASTBETA_UBYTE4  0x1000
#define D3DDP_MAXTEXCOORD       8

// Texture coordinate size encoding macros (used by vertex format info)
#define D3DFVF_TEXTUREFORMAT1 3
#define D3DFVF_TEXTUREFORMAT2 0
#define D3DFVF_TEXTUREFORMAT3 1
#define D3DFVF_TEXTUREFORMAT4 2
#define D3DFVF_TEXCOORDSIZE1(CoordIndex) (D3DFVF_TEXTUREFORMAT1 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE2(CoordIndex) (D3DFVF_TEXTUREFORMAT2)
#define D3DFVF_TEXCOORDSIZE3(CoordIndex) (D3DFVF_TEXTUREFORMAT3 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE4(CoordIndex) (D3DFVF_TEXTUREFORMAT4 << (CoordIndex*2 + 16))

// ============================================================================
// D3DX Math Type Compatibility (replaces d3dx8math.h)
// ============================================================================
typedef struct _D3DXVECTOR2 {
    float x, y;
    _D3DXVECTOR2() : x(0), y(0) {}
    _D3DXVECTOR2(float _x, float _y) : x(_x), y(_y) {}
} D3DXVECTOR2;

typedef struct _D3DXVECTOR3 : public D3DVECTOR {
    _D3DXVECTOR3() { x = y = z = 0; }
    _D3DXVECTOR3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
    _D3DXVECTOR3(const D3DVECTOR& v) { x = v.x; y = v.y; z = v.z; }
    _D3DXVECTOR3(const float* pf) { x = pf[0]; y = pf[1]; z = pf[2]; }
    operator float* () { return (float*)&x; }
    operator const float* () const { return (const float*)&x; }
} D3DXVECTOR3;

typedef struct _D3DXVECTOR4 {
    float x, y, z, w;
    _D3DXVECTOR4() : x(0), y(0), z(0), w(0) {}
    _D3DXVECTOR4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    _D3DXVECTOR4(const float* pf) : x(pf[0]), y(pf[1]), z(pf[2]), w(pf[3]) {}
    operator float* () { return &x; }
    operator const float* () const { return &x; }
    operator const void* () const { return this; }
} D3DXVECTOR4;

typedef struct _D3DXMATRIX : public D3DMATRIX {
    _D3DXMATRIX() { memset(this, 0, sizeof(D3DMATRIX)); _11 = _22 = _33 = _44 = 1.0f; }
} D3DXMATRIX;

// D3DX math function stubs (inline, return identity/zero where appropriate)
inline D3DXVECTOR3* D3DXVec3TransformCoord(D3DXVECTOR3* out, const D3DXVECTOR3* v, const D3DXMATRIX* m) {
    float w = v->x * m->m[0][3] + v->y * m->m[1][3] + v->z * m->m[2][3] + m->m[3][3];
    if (w == 0.0f) w = 1.0f;
    out->x = (v->x * m->m[0][0] + v->y * m->m[1][0] + v->z * m->m[2][0] + m->m[3][0]) / w;
    out->y = (v->x * m->m[0][1] + v->y * m->m[1][1] + v->z * m->m[2][1] + m->m[3][1]) / w;
    out->z = (v->x * m->m[0][2] + v->y * m->m[1][2] + v->z * m->m[2][2] + m->m[3][2]) / w;
    return out;
}

inline D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* out, const D3DXVECTOR3* v) {
    float len = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (len > 0.0f) { out->x = v->x / len; out->y = v->y / len; out->z = v->z / len; }
    else { out->x = out->y = out->z = 0.0f; }
    return out;
}

inline D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* out, const D3DXMATRIX* a, const D3DXMATRIX* b) {
    D3DXMATRIX tmp;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            tmp.m[i][j] = 0;
            for (int k = 0; k < 4; k++) tmp.m[i][j] += a->m[i][k] * b->m[k][j];
        }
    *out = tmp;
    return out;
}

inline D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX* out, float* det, const D3DXMATRIX* m) {
    // Simplified - return identity for now (full implementation needed for production)
    *out = D3DXMATRIX();
    if (det) *det = 1.0f;
    return out;
}

inline D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* out, const D3DXMATRIX* m) {
    D3DXMATRIX tmp;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) tmp.m[i][j] = m->m[j][i];
    *out = tmp;
    return out;
}

inline D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX* out) {
    *out = D3DXMATRIX();
    return out;
}

inline D3DXMATRIX* D3DXMatrixScaling(D3DXMATRIX* out, float sx, float sy, float sz) {
    *out = D3DXMATRIX(); out->m[0][0] = sx; out->m[1][1] = sy; out->m[2][2] = sz; return out;
}
inline D3DXMATRIX* D3DXMatrixTranslation(D3DXMATRIX* out, float tx, float ty, float tz) {
    *out = D3DXMATRIX(); out->m[3][0] = tx; out->m[3][1] = ty; out->m[3][2] = tz; return out;
}
inline D3DXMATRIX* D3DXMatrixRotationX(D3DXMATRIX* out, float a) {
    *out = D3DXMATRIX(); out->m[1][1] = cosf(a); out->m[1][2] = sinf(a);
    out->m[2][1] = -sinf(a); out->m[2][2] = cosf(a); return out;
}
inline D3DXMATRIX* D3DXMatrixRotationY(D3DXMATRIX* out, float a) {
    *out = D3DXMATRIX(); out->m[0][0] = cosf(a); out->m[0][2] = -sinf(a);
    out->m[2][0] = sinf(a); out->m[2][2] = cosf(a); return out;
}
inline D3DXMATRIX* D3DXMatrixRotationZ(D3DXMATRIX* out, float a) {
    *out = D3DXMATRIX(); out->m[0][0] = cosf(a); out->m[0][1] = sinf(a);
    out->m[1][0] = -sinf(a); out->m[1][1] = cosf(a); return out;
}

// D3DXMATRIX multiplication operator for chained transforms
inline D3DXMATRIX operator*(const D3DXMATRIX& a, const D3DXMATRIX& b) {
    D3DXMATRIX out;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            out.m[i][j] = 0.0f;
            for (int k = 0; k < 4; k++) out.m[i][j] += a.m[i][k] * b.m[k][j];
        }
    return out;
}

// D3DXGetFVFVertexSize - compute vertex size from FVF flags
inline UINT D3DXGetFVFVertexSize(DWORD fvf) {
    UINT size = 0;
    DWORD posBits = fvf & D3DFVF_POSITION_MASK;
    if (posBits == D3DFVF_XYZ) size += 12;
    else if (posBits == D3DFVF_XYZRHW) size += 16;
    else if (posBits >= D3DFVF_XYZB1 && posBits <= D3DFVF_XYZB5) {
        size += 12 + 4 * ((posBits - D3DFVF_XYZB1) / 2 + 1);
    }
    if (fvf & D3DFVF_NORMAL) size += 12;
    if (fvf & D3DFVF_PSIZE) size += 4;
    if (fvf & D3DFVF_DIFFUSE) size += 4;
    if (fvf & D3DFVF_SPECULAR) size += 4;
    UINT texCount = (fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    size += texCount * 8; // Default 2D tex coords
    return size;
}

// ============================================================================
// COM-like Interface Stubs (minimal for compilation)
// These are stub types - the real resources are managed by BGFX.
// Each interface has only the methods actually called in the codebase.
// ============================================================================

struct IUnknown_Stub {
    virtual ~IUnknown_Stub() {}
    virtual ULONG AddRef() { return 1; }
    virtual ULONG Release() { return 0; }
};

// Forward declarations
struct IDirect3DSurface8;

struct IDirect3DBaseTexture8 : public IUnknown_Stub {
    virtual UINT GetLevelCount() { return 1; }
    virtual DWORD GetPriority() { return 0; }
    virtual DWORD SetPriority(DWORD Priority) { (void)Priority; return 0; }
    virtual DWORD SetLOD(DWORD LOD) { (void)LOD; return 0; }
    virtual DWORD GetLOD() { return 0; }
    virtual HRESULT SetAutoGenFilterType(DWORD FilterType) { (void)FilterType; return D3D_OK; }
    virtual DWORD GetAutoGenFilterType() { return 0; }
};

struct IDirect3DTexture8 : public IDirect3DBaseTexture8 {
    virtual HRESULT GetSurfaceLevel(UINT Level, IDirect3DSurface8** ppSurface) {
        (void)Level; if (ppSurface) *ppSurface = nullptr; return D3D_OK;
    }
    virtual HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) {
        (void)Level; if (pDesc) memset(pDesc, 0, sizeof(*pDesc)); return D3D_OK;
    }
    virtual HRESULT LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) {
        (void)Level; (void)pRect; (void)Flags;
        if (pLockedRect) { pLockedRect->Pitch = 0; pLockedRect->pBits = nullptr; }
        return D3D_OK;
    }
    virtual HRESULT UnlockRect(UINT Level) { (void)Level; return D3D_OK; }
};

struct IDirect3DCubeTexture8 : public IDirect3DBaseTexture8 {
    virtual HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) {
        (void)Level; if (pDesc) memset(pDesc, 0, sizeof(*pDesc)); return D3D_OK;
    }
    virtual HRESULT LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) {
        (void)FaceType; (void)Level; (void)pRect; (void)Flags;
        if (pLockedRect) { pLockedRect->Pitch = 0; pLockedRect->pBits = nullptr; }
        return D3D_OK;
    }
    virtual HRESULT UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level) { (void)FaceType; (void)Level; return D3D_OK; }
};

struct IDirect3DVolumeTexture8 : public IDirect3DBaseTexture8 {
    virtual HRESULT GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc) {
        (void)Level; if (pDesc) memset(pDesc, 0, sizeof(*pDesc)); return D3D_OK;
    }
    virtual HRESULT LockBox(UINT Level, D3DLOCKED_BOX* pLockedBox, const void* pBox, DWORD Flags) {
        (void)Level; (void)pBox; (void)Flags;
        if (pLockedBox) { pLockedBox->RowPitch = 0; pLockedBox->SlicePitch = 0; pLockedBox->pBits = nullptr; }
        return D3D_OK;
    }
    virtual HRESULT UnlockBox(UINT Level) { (void)Level; return D3D_OK; }
};

struct IDirect3DSurface8 : public IUnknown_Stub {
    virtual HRESULT GetDesc(D3DSURFACE_DESC* pDesc) {
        if (pDesc) memset(pDesc, 0, sizeof(*pDesc));
        return D3D_OK;
    }
    virtual HRESULT LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) {
        (void)pRect; (void)Flags;
        if (pLockedRect) { pLockedRect->Pitch = 0; pLockedRect->pBits = nullptr; }
        return D3D_OK;
    }
    virtual HRESULT UnlockRect() { return D3D_OK; }
};

struct IDirect3DVertexBuffer8 : public IUnknown_Stub {
    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags) {
        (void)OffsetToLock; (void)SizeToLock; (void)Flags;
        if (ppbData) *ppbData = nullptr;
        return D3D_OK;
    }
    virtual HRESULT Unlock() { return D3D_OK; }
};

struct IDirect3DIndexBuffer8 : public IUnknown_Stub {
    virtual HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags) {
        (void)OffsetToLock; (void)SizeToLock; (void)Flags;
        if (ppbData) *ppbData = nullptr;
        return D3D_OK;
    }
    virtual HRESULT Unlock() { return D3D_OK; }
};

struct IDirect3DSwapChain8 : public IUnknown_Stub {
    virtual HRESULT GetBackBuffer(UINT iBackBuffer, DWORD Type, IDirect3DSurface8** ppBackBuffer) {
        (void)iBackBuffer; (void)Type;
        if (ppBackBuffer) *ppBackBuffer = nullptr;
        return D3D_OK;
    }
};

// ============================================================================
// Helper: bytes per pixel for a D3DFORMAT value
// Used by concrete BGFX resource implementations below.
// ============================================================================
inline UINT D3DFormatBytesPerPixel(D3DFORMAT fmt) {
    switch (fmt) {
    case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: return 4;
    case D3DFMT_R8G8B8:   return 3;
    case D3DFMT_R5G6B5:   case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4: case D3DFMT_A8R3G3B2: case D3DFMT_X4R4G4B4:
    case D3DFMT_A8P8:     case D3DFMT_A8L8:     case D3DFMT_V8U8:
    case D3DFMT_L6V5U5:   case D3DFMT_G16R16:   return 2;
    case D3DFMT_R3G3B2:   case D3DFMT_A8:       case D3DFMT_P8:
    case D3DFMT_L8:       case D3DFMT_A4L4:     return 1;
    default: return 4;
    }
}

// ============================================================================
// Concrete BGFX-backed implementations of the DX8 resource interfaces.
// These provide real CPU memory so Lock/Unlock actually work.
// The objects are ref-counted and self-delete in Release().
// ============================================================================

// BGFXVertexBuffer8 — concrete vertex buffer backed by CPU memory.
// DX8VertexBufferClass calls Lock(0,0,...) to get a write pointer,
// writes vertices, then Unlock(). No GPU upload needed for the sorting
// renderer path; for the DX8 path we keep data in CPU memory for now.
struct BGFXVertexBuffer8 : public IDirect3DVertexBuffer8 {
    uint8_t* m_data;
    UINT     m_size;
    LONG     m_refs;

    BGFXVertexBuffer8(UINT size)
        : m_data(size ? static_cast<uint8_t*>(malloc(size)) : nullptr)
        , m_size(size)
        , m_refs(1)
    {
        if (m_data) memset(m_data, 0, size);
    }
    // Note: destructor is called via delete this inside Release(), where this
    // has static type BGFXVertexBuffer8*, so the right dtor runs without needing
    // a virtual dtor on the base.
    ~BGFXVertexBuffer8() { free(m_data); m_data = nullptr; }

    ULONG AddRef()  override { return ++m_refs; }
    ULONG Release() override { ULONG r = --m_refs; if (!r) delete this; return r; }

    HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags) override {
        (void)SizeToLock; (void)Flags;
        if (ppbData) *ppbData = m_data ? m_data + OffsetToLock : nullptr;
        return D3D_OK;
    }
    HRESULT Unlock() override { return D3D_OK; }
};

// BGFXIndexBuffer8 — concrete index buffer backed by CPU memory.
struct BGFXIndexBuffer8 : public IDirect3DIndexBuffer8 {
    uint8_t* m_data;
    UINT     m_size;
    LONG     m_refs;

    BGFXIndexBuffer8(UINT size)
        : m_data(size ? static_cast<uint8_t*>(malloc(size)) : nullptr)
        , m_size(size)
        , m_refs(1)
    {
        if (m_data) memset(m_data, 0, size);
    }
    ~BGFXIndexBuffer8() { free(m_data); m_data = nullptr; }

    ULONG AddRef()  override { return ++m_refs; }
    ULONG Release() override { ULONG r = --m_refs; if (!r) delete this; return r; }

    HRESULT Lock(UINT OffsetToLock, UINT SizeToLock, BYTE** ppbData, DWORD Flags) override {
        (void)SizeToLock; (void)Flags;
        if (ppbData) *ppbData = m_data ? m_data + OffsetToLock : nullptr;
        return D3D_OK;
    }
    HRESULT Unlock() override { return D3D_OK; }
};

// BGFXSurface8 — concrete surface backed by CPU memory.
// Used for font rendering: FontCharsClass blits GDI chars into a locked
// surface via LockRect/UnlockRect. The CPU buffer is the ground truth;
// texture upload happens when the texture is actually drawn.
struct BGFXSurface8 : public IDirect3DSurface8 {
    uint8_t*  m_data;
    UINT      m_width;
    UINT      m_height;
    INT       m_pitch;   // bytes per row
    D3DFORMAT m_format;
    LONG      m_refs;
    bool      m_dirty;  // pixel data written since last GPU upload

    BGFXSurface8(UINT w, UINT h, D3DFORMAT fmt)
        : m_width(w), m_height(h), m_format(fmt), m_refs(1), m_dirty(false)
    {
        UINT bpp = D3DFormatBytesPerPixel(fmt);
        m_pitch  = static_cast<INT>(w * bpp);
        UINT sz  = static_cast<UINT>(m_pitch) * h;
        m_data   = sz ? static_cast<uint8_t*>(malloc(sz)) : nullptr;
        if (m_data) memset(m_data, 0, sz);
    }
    ~BGFXSurface8() { free(m_data); m_data = nullptr; }

    ULONG AddRef()  override { return ++m_refs; }
    ULONG Release() override { ULONG r = --m_refs; if (!r) delete this; return r; }

    HRESULT GetDesc(D3DSURFACE_DESC* pDesc) override {
        if (pDesc) {
            memset(pDesc, 0, sizeof(*pDesc));
            pDesc->Format = m_format;
            pDesc->Width  = m_width;
            pDesc->Height = m_height;
        }
        return D3D_OK;
    }
    HRESULT LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) override {
        (void)pRect; (void)Flags;
        if (pLockedRect) {
            pLockedRect->Pitch = m_pitch;
            pLockedRect->pBits = m_data;
        }
        return D3D_OK;
    }
    // Mark dirty so BGFXWrapper uploads pixel data to GPU on next draw.
    HRESULT UnlockRect() override { m_dirty = true; return D3D_OK; }
};

// ---------------------------------------------------------------------------
// Texture handle cache — prevents bgfx handle recycling between subsystems.
// When a BGFXTexture8 is destroyed, its GPU handle goes here instead of back
// to bgfx's free pool.  When a new texture of the same size is created, we
// pull a cached handle and just updateTexture2D with new content.
// This stops font glyph handles (64x64) from being recycled by terrain.
// ---------------------------------------------------------------------------
// TexHandleCache was REMOVED. Textures now own their bgfx handle for their whole
// lifetime -- see the note in UploadSurface (BGFXWrapper.cpp) and ~BGFXTexture8
// below. It pooled freed handles keyed on {width, height} with NO format field,
// so a freed BC3 handle could be reissued to a BGRA8 texture and then written
// with BGRA8 bytes; and once its 64-entry pool filled it destroyed the incoming
// handle instead of evicting the oldest, so the pool froze with the first 64
// handles ever freed. Do not reintroduce a shared handle pool without making
// format part of the key.

// BGFXTexture8 — concrete texture backed by a BGFXSurface8 level 0.
// The game writes pixels via LockRect/UnlockRect on the mip level 0.
// GPU upload is handled by SubmitDraw in BGFXWrapper when m_bgfxDirty is true.
struct BGFXTexture8 : public IDirect3DTexture8 {
    BGFXSurface8*  m_surface;   // level 0 CPU pixel data
    unsigned short m_bgfxIdx;   // bgfx::TextureHandle.idx (0xFFFF = not yet uploaded)
    bool           m_bgfxDirty; // pixel data written since last GPU upload
    LONG           m_refs;

    BGFXTexture8(UINT w, UINT h, D3DFORMAT fmt)
        : m_surface(new BGFXSurface8(w, h, fmt)), m_bgfxIdx(0xFFFF), m_bgfxDirty(false), m_refs(1) {}
    ~BGFXTexture8() {
        // This texture owns its GPU handle: destroy it here, unconditionally.
        // It is never handed to another texture, so no other object can be
        // holding it. (Previously this returned the handle to TexHandleCache,
        // which could reissue it to a texture of a DIFFERENT format.)
        if (m_bgfxIdx != 0xFFFF) {
            bgfx::TextureHandle th; th.idx = m_bgfxIdx;
            bgfx::destroy(th);
            m_bgfxIdx = 0xFFFF;
        }
        if (m_surface) { m_surface->Release(); m_surface = nullptr; }
    }

    ULONG AddRef()  override { return ++m_refs; }
    ULONG Release() override { ULONG r = --m_refs; if (!r) delete this; return r; }

    UINT GetLevelCount() override { return 1; }

    HRESULT GetSurfaceLevel(UINT Level, IDirect3DSurface8** ppSurface) override {
        if (ppSurface) {
            if (Level == 0 && m_surface) {
                m_surface->AddRef();
                *ppSurface = m_surface;
            } else if (Level > 0 && m_surface) {
                // Allocate a correctly-sized dummy surface for higher mip levels.
                // The textureloader writes mip data into it; we discard it (only
                // level 0 is uploaded to GPU). Without this, the loop in
                // textureloader.cpp crashes dereferencing a null pointer.
                UINT mipW = m_surface->m_width  >> Level; if (mipW < 1) mipW = 1;
                UINT mipH = m_surface->m_height >> Level; if (mipH < 1) mipH = 1;
                *ppSurface = new BGFXSurface8(mipW, mipH, m_surface->m_format);
            } else {
                *ppSurface = nullptr;
            }
        }
        return D3D_OK;
    }
    HRESULT GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) override {
        if (Level == 0 && m_surface) return m_surface->GetDesc(pDesc);
        if (pDesc) memset(pDesc, 0, sizeof(*pDesc));
        return D3D_OK;
    }
    HRESULT LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) override {
        if (Level == 0 && m_surface) return m_surface->LockRect(pLockedRect, pRect, Flags);
        if (pLockedRect) { pLockedRect->Pitch = 0; pLockedRect->pBits = nullptr; }
        return D3D_OK;
    }
    HRESULT UnlockRect(UINT Level) override {
        if (Level == 0 && m_surface) {
            m_bgfxDirty = true; // signal BGFXWrapper to re-upload pixel data
            return m_surface->UnlockRect();
        }
        return D3D_OK;
    }
};

struct IDirect3DDevice8 : public IUnknown_Stub {
    virtual HRESULT TestCooperativeLevel() { return D3D_OK; }
    virtual HRESULT CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage,
        D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture8** ppTexture) {
        (void)Levels; (void)Usage; (void)Pool;
        if (ppTexture) *ppTexture = new BGFXTexture8(Width, Height, Format);
        return D3D_OK;
    }
    virtual HRESULT CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format,
        D3DPOOL Pool, IDirect3DCubeTexture8** ppCubeTexture) {
        (void)EdgeLength; (void)Levels; (void)Usage; (void)Format; (void)Pool;
        if (ppCubeTexture) *ppCubeTexture = nullptr;
        return D3D_OK;
    }
    virtual HRESULT CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels,
        DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, void** ppVolumeTexture) {
        (void)Width; (void)Height; (void)Depth; (void)Levels; (void)Usage; (void)Format; (void)Pool;
        if (ppVolumeTexture) *ppVolumeTexture = nullptr;
        return D3D_OK;
    }
    virtual HRESULT CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool,
        IDirect3DVertexBuffer8** ppVertexBuffer) {
        (void)Usage; (void)FVF; (void)Pool;
        if (ppVertexBuffer) *ppVertexBuffer = new BGFXVertexBuffer8(Length);
        return D3D_OK;
    }
    virtual HRESULT CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
        IDirect3DIndexBuffer8** ppIndexBuffer) {
        (void)Usage; (void)Format; (void)Pool;
        if (ppIndexBuffer) *ppIndexBuffer = new BGFXIndexBuffer8(Length);
        return D3D_OK;
    }
    virtual UINT GetAvailableTextureMem() { return 256 * 1024 * 1024; }
    virtual HRESULT GetDeviceCaps(D3DCAPS8* pCaps) {
        if (pCaps) memset(pCaps, 0, sizeof(*pCaps)); return D3D_OK;
    }
    virtual HRESULT ResourceManagerDiscardBytes(DWORD Bytes) {
        (void)Bytes; return D3D_OK;
    }
    virtual HRESULT CreateImageSurface(UINT Width, UINT Height, D3DFORMAT Format,
        IDirect3DSurface8** ppSurface) {
        if (ppSurface) *ppSurface = new BGFXSurface8(Width, Height, Format);
        return D3D_OK;
    }
    virtual HRESULT CopyRects(IDirect3DSurface8* pSrc, const RECT* pSrcRects, UINT cRects,
        IDirect3DSurface8* pDst, const POINT* pDstPoints) {
        (void)pSrcRects; (void)cRects; (void)pDstPoints;
        // Copy CPU pixel data from source surface to destination surface and mark dirty
        // so BGFXWrapper will upload the pixels to GPU on the next draw call.
        BGFXSurface8* src = static_cast<BGFXSurface8*>(pSrc);
        BGFXSurface8* dst = static_cast<BGFXSurface8*>(pDst);
        if (src && dst && src->m_data && dst->m_data) {
            uint32_t dstSz = (uint32_t)dst->m_pitch * dst->m_height;
            uint32_t srcSz = (uint32_t)src->m_pitch * src->m_height;
            uint32_t copySz = dstSz < srcSz ? dstSz : srcSz;
            if (copySz > 0) memcpy(dst->m_data, src->m_data, copySz);
            dst->m_dirty = true;
        }
        return D3D_OK;
    }
    virtual HRESULT CreateVertexShader(const DWORD* pDeclaration, const DWORD* pFunction,
        DWORD* pHandle, DWORD Usage) {
        (void)pDeclaration; (void)pFunction; (void)Usage;
        if (pHandle) *pHandle = 0;
        return D3D_OK;
    }
    virtual HRESULT SetVertexShader(DWORD Handle) { (void)Handle; return D3D_OK; }
    virtual HRESULT DeleteVertexShader(DWORD Handle) { (void)Handle; return D3D_OK; }
    virtual HRESULT CreatePixelShader(const DWORD* pFunction, DWORD* pHandle) {
        (void)pFunction;
        if (pHandle) *pHandle = 0;
        return D3D_OK;
    }
    virtual HRESULT SetPixelShader(DWORD Handle) { (void)Handle; return D3D_OK; }
    virtual HRESULT DeletePixelShader(DWORD Handle) { (void)Handle; return D3D_OK; }
    // Drawing methods (stubs - actual rendering via BGFX)
    virtual HRESULT DrawPrimitive(D3DPRIMITIVETYPE, UINT, UINT) { return D3D_OK; }
    virtual HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE, UINT, UINT, UINT, UINT) { return D3D_OK; }
    virtual HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE, UINT, const void*, UINT) { return D3D_OK; }
    virtual HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE, UINT, UINT, UINT, const void*, D3DFORMAT, const void*, UINT) { return D3D_OK; }
    // State query methods (stubs)
    virtual HRESULT GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) { (void)State; if (pValue) *pValue = 0; return D3D_OK; }
    virtual HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) { (void)State; (void)Value; return D3D_OK; }
    virtual HRESULT GetRenderTarget(IDirect3DSurface8** ppRenderTarget) { if (ppRenderTarget) *ppRenderTarget = nullptr; return D3D_OK; }
    virtual HRESULT SetRenderTarget(IDirect3DSurface8*, IDirect3DSurface8*) { return D3D_OK; }
    virtual HRESULT GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) { (void)State; if (pMatrix) memset(pMatrix, 0, sizeof(*pMatrix)); return D3D_OK; }
    virtual HRESULT SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) { (void)State; (void)pMatrix; return D3D_OK; }
    virtual HRESULT GetDepthStencilSurface(IDirect3DSurface8** ppZStencilSurface) { if (ppZStencilSurface) *ppZStencilSurface = nullptr; return D3D_OK; }
    virtual HRESULT SetStreamSource(UINT, IDirect3DVertexBuffer8*, UINT) { return D3D_OK; }
    virtual HRESULT GetStreamSource(UINT, IDirect3DVertexBuffer8** ppVB, UINT* pStr) { if (ppVB) *ppVB = nullptr; if (pStr) *pStr = 0; return D3D_OK; }
    virtual HRESULT SetIndices(IDirect3DIndexBuffer8*, UINT) { return D3D_OK; }
    virtual HRESULT GetIndices(IDirect3DIndexBuffer8** ppIB, UINT* pBaseIdx) { if (ppIB) *ppIB = nullptr; if (pBaseIdx) *pBaseIdx = 0; return D3D_OK; }
    virtual HRESULT SetTexture(DWORD, IDirect3DBaseTexture8*) { return D3D_OK; }
    virtual HRESULT GetTexture(DWORD, IDirect3DBaseTexture8** ppTex) { if (ppTex) *ppTex = nullptr; return D3D_OK; }
    virtual HRESULT SetTextureStageState(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD) { return D3D_OK; }
    virtual HRESULT GetTextureStageState(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD* pVal) { if (pVal) *pVal = 0; return D3D_OK; }
    virtual HRESULT SetLight(DWORD, const D3DLIGHT8*) { return D3D_OK; }
    virtual HRESULT LightEnable(DWORD, BOOL) { return D3D_OK; }
    virtual HRESULT SetViewport(const D3DVIEWPORT8*) { return D3D_OK; }
    virtual HRESULT GetViewport(D3DVIEWPORT8* pVP) { if (pVP) memset(pVP, 0, sizeof(*pVP)); return D3D_OK; }
    virtual HRESULT Clear(DWORD, const D3DRECT*, DWORD, D3DCOLOR, float, DWORD) { return D3D_OK; }
    virtual HRESULT BeginScene() { return D3D_OK; }
    virtual HRESULT EndScene() { return D3D_OK; }
    virtual HRESULT Present(const RECT*, const RECT*, HWND, const RGNDATA*) { return D3D_OK; }
    virtual HRESULT GetBackBuffer(UINT, D3DBACKBUFFER_TYPE, IDirect3DSurface8** ppSurf) { if (ppSurf) *ppSurf = nullptr; return D3D_OK; }
    virtual HRESULT Reset(D3DPRESENT_PARAMETERS*) { return D3D_OK; }
    virtual HRESULT ValidateDevice(DWORD* pPass) { if (pPass) *pPass = 0; return D3D_OK; }
    virtual HRESULT SetMaterial(const D3DMATERIAL8*) { return D3D_OK; }
    virtual HRESULT GetMaterial(D3DMATERIAL8* pMat) { if (pMat) memset(pMat, 0, sizeof(*pMat)); return D3D_OK; }
    virtual HRESULT SetVertexShaderConstant(DWORD, const void*, DWORD) { return D3D_OK; }
    virtual HRESULT SetPixelShaderConstant(DWORD, const void*, DWORD) { return D3D_OK; }
    virtual HRESULT GetVertexShader(DWORD* pH) { if (pH) *pH = 0; return D3D_OK; }
    virtual HRESULT GetPixelShader(DWORD* pH) { if (pH) *pH = 0; return D3D_OK; }
    virtual HRESULT SetClipPlane(DWORD, const float*) { return D3D_OK; }
    virtual HRESULT BeginStateBlock() { return D3D_OK; }
    virtual HRESULT EndStateBlock(DWORD* pH) { if (pH) *pH = 0; return D3D_OK; }
    virtual HRESULT ApplyStateBlock(DWORD) { return D3D_OK; }
    virtual HRESULT DeleteStateBlock(DWORD) { return D3D_OK; }
    virtual void SetGammaRamp(DWORD, const D3DGAMMARAMP*) {}
    virtual void SetCursorPosition(int, int, DWORD) {}
    virtual BOOL ShowCursor(BOOL) { return FALSE; }
    virtual HRESULT SetCursorProperties(UINT, UINT, IDirect3DSurface8*) { return D3D_OK; }
    virtual HRESULT GetDisplayMode(D3DDISPLAYMODE* pMode) { if (pMode) memset(pMode, 0, sizeof(*pMode)); return D3D_OK; }
    virtual HRESULT GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pP) { if (pP) memset(pP, 0, sizeof(*pP)); return D3D_OK; }
    virtual HRESULT ProcessVertices(UINT, UINT, UINT, IDirect3DVertexBuffer8*, DWORD) { return D3D_OK; }
    virtual HRESULT GetFrontBuffer(IDirect3DSurface8*) { return D3D_OK; }
    virtual HRESULT UpdateTexture(IDirect3DBaseTexture8*, IDirect3DBaseTexture8*) { return D3D_OK; }
};

// D3DENUM flags
#define D3DENUM_NO_WHQL_LEVEL  0x00000002

struct IDirect3D8 : public IUnknown_Stub {
    virtual HRESULT RegisterSoftwareDevice(void* pInitializeFunction) { (void)pInitializeFunction; return D3D_OK; }
    virtual UINT GetAdapterCount() { return 1; }
    virtual HRESULT GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER8* pIdentifier) {
        (void)Adapter; (void)Flags;
        if (pIdentifier) memset(pIdentifier, 0, sizeof(D3DADAPTER_IDENTIFIER8));
        return D3D_OK;
    }
    virtual UINT GetAdapterModeCount(UINT Adapter) { (void)Adapter; return 1; }
    virtual HRESULT EnumAdapterModes(UINT Adapter, UINT Mode, D3DDISPLAYMODE* pMode) {
        (void)Adapter; (void)Mode;
        if (pMode) memset(pMode, 0, sizeof(D3DDISPLAYMODE));
        return D3D_OK;
    }
    virtual HRESULT GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) {
        (void)Adapter;
        if (pMode) { pMode->Width = 1920; pMode->Height = 1080; pMode->RefreshRate = 60; pMode->Format = D3DFMT_X8R8G8B8; }
        return D3D_OK;
    }
    virtual HRESULT CheckDeviceType(UINT, DWORD, D3DFORMAT, D3DFORMAT, BOOL) { return D3D_OK; }
    virtual HRESULT CheckDeviceFormat(UINT, DWORD, D3DFORMAT, DWORD, DWORD, D3DFORMAT) { return D3D_OK; }
    virtual HRESULT CheckDeviceMultiSampleType(UINT, DWORD, D3DFORMAT, BOOL, DWORD) { return D3D_OK; }
    virtual HRESULT CheckDepthStencilMatch(UINT, DWORD, D3DFORMAT, D3DFORMAT, D3DFORMAT) { return D3D_OK; }
    virtual HRESULT GetDeviceCaps(UINT Adapter, DWORD DeviceType, D3DCAPS8* pCaps) {
        (void)Adapter; (void)DeviceType;
        if (pCaps) memset(pCaps, 0, sizeof(D3DCAPS8));
        return D3D_OK;
    }
    virtual HMONITOR GetAdapterMonitor(UINT Adapter) { (void)Adapter; return nullptr; }
    virtual HRESULT CreateDevice(UINT Adapter, DWORD DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
        D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface) {
        (void)Adapter; (void)DeviceType; (void)hFocusWindow; (void)BehaviorFlags; (void)pPresentationParameters;
        if (ppReturnedDeviceInterface) *ppReturnedDeviceInterface = nullptr;
        return D3D_OK;
    }
};

typedef IDirect3DSurface8* LPDIRECT3DSURFACE8;
typedef IDirect3DTexture8* LPDIRECT3DTEXTURE8;
typedef IDirect3DBaseTexture8* LPDIRECT3DBASETEXTURE8;
typedef IDirect3DDevice8* LPDIRECT3DDEVICE8;
typedef IDirect3DVertexBuffer8* LPDIRECT3DVERTEXBUFFER8;
typedef IDirect3DIndexBuffer8* LPDIRECT3DINDEXBUFFER8;
typedef IDirect3DCubeTexture8* LPDIRECT3DCUBETEXTURE8;
typedef IDirect3DSwapChain8* LPDIRECT3DSWAPCHAIN8;

// D3D render state flag constants missing from earlier headers
#define D3DCOLORWRITEENABLE_RED   0x00000001
#define D3DCOLORWRITEENABLE_GREEN 0x00000002
#define D3DCOLORWRITEENABLE_BLUE  0x00000004
#define D3DCOLORWRITEENABLE_ALPHA 0x00000008

#define D3DWRAP_U  0x00000001
#define D3DWRAP_V  0x00000002
#define D3DWRAP_W  0x00000004

#define D3DPMISCCAPS_COLORWRITEENABLE 0x00002000
#define D3DPMISCCAPS_BLENDOP          0x00004000

#define D3DCURSOR_IMMEDIATE_UPDATE 0x00000001

#define D3DSTATUS_CLIPUNIONALL       0x0000FFFF
#define D3DSTATUS_CLIPINTERSECTALL   0xFFFF0000

// D3D Texture lock flags
#define D3DLOCK_NO_DIRTY_UPDATE  0x00008000

// D3DX surface/texture operations (stubs)
#define D3DX_FILTER_NONE            0x00000001
#define D3DX_FILTER_POINT           0x00000002
#define D3DX_FILTER_LINEAR          0x00000003
#define D3DX_FILTER_TRIANGLE        0x00000004
#define D3DX_FILTER_BOX             0x00000005
#define D3DX_DEFAULT                0xFFFFFFFF

typedef struct _D3DXIMAGE_INFO {
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    D3DFORMAT Format;
    DWORD ResourceType;
    DWORD ImageFileFormat;
} D3DXIMAGE_INFO;

// D3DX function stubs
inline HRESULT D3DXLoadSurfaceFromSurface(IDirect3DSurface8* pDst, void*, void*, IDirect3DSurface8* pSrc, void*, void*, DWORD, D3DCOLOR) {
    // Simple memcpy between BGFXSurface8 objects (same as _Copy_DX8_Rects)
    if (!pDst || !pSrc) return D3D_OK;
    BGFXSurface8* src = static_cast<BGFXSurface8*>(pSrc);
    BGFXSurface8* dst = static_cast<BGFXSurface8*>(pDst);
    if (!src->m_data || !dst->m_data) return D3D_OK;
    uint32_t dstSz = (uint32_t)dst->m_pitch * dst->m_height;
    uint32_t srcSz = (uint32_t)src->m_pitch * src->m_height;
    uint32_t copySz = dstSz < srcSz ? dstSz : srcSz;
    if (copySz > 0) memcpy(dst->m_data, src->m_data, copySz);
    dst->m_dirty = true;
    return D3D_OK;
}
inline HRESULT D3DXLoadSurfaceFromMemory(IDirect3DSurface8* pDst, void*, void*, const void* pSrcMem, D3DFORMAT, UINT SrcPitch, void*, void*, DWORD, D3DCOLOR) {
    // Copy raw pixel data into BGFXSurface8
    if (!pDst || !pSrcMem) return D3D_OK;
    BGFXSurface8* dst = static_cast<BGFXSurface8*>(pDst);
    if (!dst->m_data) return D3D_OK;
    uint32_t dstSz = (uint32_t)dst->m_pitch * dst->m_height;
    uint32_t srcSz = SrcPitch * dst->m_height;
    uint32_t copySz = dstSz < srcSz ? dstSz : srcSz;
    if (copySz > 0) memcpy(dst->m_data, pSrcMem, copySz);
    dst->m_dirty = true;
    return D3D_OK;
}
inline HRESULT D3DXCreateTextureFromFileInMemory(IDirect3DDevice8*, const void*, UINT, IDirect3DTexture8**) { return D3D_OK; }
inline HRESULT D3DXCreateTextureFromFileInMemoryEx(IDirect3DDevice8*, const void*, UINT, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, DWORD, DWORD, D3DCOLOR, D3DXIMAGE_INFO*, void*, IDirect3DTexture8**) { return D3D_OK; }
inline HRESULT D3DXSaveTextureToFileA(const char*, DWORD, IDirect3DBaseTexture8*, void*) { return D3D_OK; }
inline HRESULT D3DXFilterTexture(IDirect3DBaseTexture8*, void*, UINT, DWORD) { return D3D_OK; }
inline HRESULT D3DXGetImageInfoFromFile(const char*, D3DXIMAGE_INFO*) { return D3D_OK; }

// D3DX font/buffer stubs
typedef IUnknown_Stub ID3DXFont;
struct ID3DXBuffer_Impl : public IUnknown_Stub {
    virtual void* GetBufferPointer() { return nullptr; }
    virtual DWORD GetBufferSize() { return 0; }
};
typedef ID3DXBuffer_Impl ID3DXBuffer;
typedef ID3DXBuffer* LPD3DXBUFFER;
inline HRESULT D3DXCreateFont(IDirect3DDevice8*, HFONT, ID3DXFont**) { return D3D_OK; }
inline HRESULT D3DXCreateTextureFromFile(IDirect3DDevice8*, const char*, IDirect3DTexture8**) { return D3D_OK; }

// ============================================================================
// D3DX math types (placeholders for BGFX migration)
// Only defined when the DX90SDK stub headers haven't already provided them
// ============================================================================
#ifndef _D3DX8MATH_H_
#ifndef _D3D8TYPES_HAVE_D3DVECTOR_
#define _D3D8TYPES_HAVE_D3DVECTOR_
struct D3DVECTOR { float x, y, z; };
#endif
struct D3DXVECTOR3 : public D3DVECTOR {
    D3DXVECTOR3() { x = y = z = 0; }
    D3DXVECTOR3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
};
struct D3DXVECTOR4 {
    float x, y, z, w;
    D3DXVECTOR4() : x(0), y(0), z(0), w(0) {}
    D3DXVECTOR4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    operator const void*() const { return this; }
};
struct D3DXMATRIX {
    float m[4][4];
    D3DXMATRIX() { memset(m, 0, sizeof(m)); }
    float& operator()(int r, int c) { return m[r][c]; }
    float operator()(int r, int c) const { return m[r][c]; }
    D3DXMATRIX& operator*=(const D3DXMATRIX& rhs) {
        D3DXMATRIX tmp;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                tmp.m[i][j] = 0.0f;
                for (int k = 0; k < 4; k++)
                    tmp.m[i][j] += m[i][k] * rhs.m[k][j];
            }
        *this = tmp;
        return *this;
    }
};
typedef D3DXMATRIX* LPD3DXMATRIX;
#endif // _D3DX8MATH_H_

// ============================================================================
// D3D8 Vertex Shader Declaration macros (placeholders for BGFX migration)
// ============================================================================
#define D3DVSD_STREAM(StreamNumber)     ((DWORD)(1 << 28) | ((StreamNumber) << 0))
#define D3DVSD_REG(reg, type)           ((DWORD)(2 << 28) | ((reg) << 0) | ((type) << 16))
#define D3DVSD_END()                    ((DWORD)0xFFFFFFFF)
#define D3DVSDT_FLOAT1      0x00
#define D3DVSDT_FLOAT2      0x01
#define D3DVSDT_FLOAT3      0x02
#define D3DVSDT_FLOAT4      0x03
#define D3DVSDT_D3DCOLOR    0x04
#define D3DVSDT_UBYTE4      0x05
#define D3DVSDT_SHORT2      0x06
#define D3DVSDT_SHORT4      0x07
#define D3DVSDE_POSITION    0
#define D3DVSDE_NORMAL      3
#define D3DVSDE_DIFFUSE     5
#define D3DVSDE_TEXCOORD0   7
#define D3DVSDE_TEXCOORD1   8
#define D3DVSDE_TEXCOORD2   9

// D3DX shader compilation stubs — return E_NOTIMPL so callers skip the
// compiled-shader usage path (avoids uninitialized-pointer crashes).
inline HRESULT D3DXAssembleShader(const void*, UINT, DWORD, LPD3DXBUFFER*, LPD3DXBUFFER*, LPD3DXBUFFER*) { return E_NOTIMPL; }
inline HRESULT D3DXAssembleShaderFromFile(const char*, DWORD, LPD3DXBUFFER*, LPD3DXBUFFER*, LPD3DXBUFFER*) { return E_NOTIMPL; }

// ============================================================================
// DirectDraw Compatibility (for DDS file loading)
// ============================================================================

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
    ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | \
    ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24))
#endif

typedef struct _DDPIXELFORMAT {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    union {
        DWORD dwRGBBitCount;
        DWORD dwYUVBitCount;
        DWORD dwZBufferBitDepth;
        DWORD dwAlphaBitDepth;
        DWORD dwLuminanceBitCount;
        DWORD dwBumpBitCount;
    };
    union {
        DWORD dwRBitMask;
        DWORD dwYBitMask;
        DWORD dwStencilBitDepth;
        DWORD dwLuminanceBitMask;
        DWORD dwBumpDuBitMask;
    };
    union {
        DWORD dwGBitMask;
        DWORD dwUBitMask;
        DWORD dwZBitMask;
        DWORD dwBumpDvBitMask;
    };
    union {
        DWORD dwBBitMask;
        DWORD dwVBitMask;
        DWORD dwStencilBitMask;
        DWORD dwBumpLuminanceBitMask;
    };
    union {
        DWORD dwRGBAlphaBitMask;
        DWORD dwYUVAlphaBitMask;
        DWORD dwLuminanceAlphaBitMask;
        DWORD dwRGBZBitMask;
        DWORD dwYUVZBitMask;
    };
} DDPIXELFORMAT;

typedef struct _DDSCAPS2 {
    DWORD dwCaps;
    DWORD dwCaps2;
    DWORD dwCaps3;
    DWORD dwCaps4;
} DDSCAPS2;

typedef struct _DDSURFACEDESC2 {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    union {
        LONG  lPitch;
        DWORD dwLinearSize;
    };
    DWORD dwBackBufferCount;
    union {
        DWORD dwMipMapCount;
        DWORD dwRefreshRate;
    };
    DWORD dwAlphaBitDepth;
    DWORD dwReserved;
    void* lpSurface;
    DDPIXELFORMAT ddpfPixelFormat;
    DDSCAPS2 ddsCaps;
    DWORD dwTextureStage;
} DDSURFACEDESC2;

// DDSD flags
#define DDSD_CAPS               0x00000001
#define DDSD_HEIGHT             0x00000002
#define DDSD_WIDTH              0x00000004
#define DDSD_PITCH              0x00000008
#define DDSD_PIXELFORMAT        0x00001000
#define DDSD_MIPMAPCOUNT        0x00020000
#define DDSD_LINEARSIZE         0x00080000
#define DDSD_DEPTH              0x00800000

// DDPF flags
#define DDPF_ALPHAPIXELS        0x00000001
#define DDPF_ALPHA              0x00000002
#define DDPF_FOURCC             0x00000004
#define DDPF_RGB                0x00000040
#define DDPF_LUMINANCE          0x00020000
#define DDPF_BUMPDUDV           0x00080000

// DDSCAPS
#define DDSCAPS_COMPLEX         0x00000008
#define DDSCAPS_TEXTURE         0x00001000
#define DDSCAPS_MIPMAP          0x00400000
#define DDSCAPS2_CUBEMAP        0x00000200
#define DDSCAPS2_VOLUME         0x00200000

// ============================================================================
// Legacy DirectDraw types (DDraw1/DDraw2 era, used by WWLib dsurface/ddraw)
// ============================================================================

// DDSCAPS (DDraw1 version - single dwCaps)
typedef struct _DDSCAPS {
    DWORD dwCaps;
} DDSCAPS;

// Additional DDSCAPS flags used by legacy code
#define DDSCAPS_OFFSCREENPLAIN  0x00002000
#define DDSCAPS_PRIMARYSURFACE  0x00000200
#define DDSCAPS_FLIP            0x00000010
#define DDSCAPS_BACKBUFFER      0x00000004
#define DDSCAPS_SYSTEMMEMORY    0x00000800
#define DDSCAPS_VIDEOMEMORY     0x00004000

// DDSURFACEDESC (DDraw1 version)
typedef struct _DDSURFACEDESC {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    union {
        LONG  lPitch;
        DWORD dwLinearSize;
    };
    DWORD dwBackBufferCount;
    union {
        DWORD dwMipMapCount;
        DWORD dwRefreshRate;
        DWORD dwZBufferBitDepth;
    };
    DWORD dwAlphaBitDepth;
    DWORD dwReserved;
    void* lpSurface;
    DDPIXELFORMAT ddpfPixelFormat;
    DDSCAPS ddsCaps;
    DWORD dwTextureStage;
} DDSURFACEDESC;

// Additional DDSD flags
#define DDSD_BACKBUFFERCOUNT    0x00000020

// DDBLTFX structure
typedef struct _DDBLTFX {
    DWORD dwSize;
    DWORD dwDDFX;
    DWORD dwROP;
    DWORD dwDDROP;
    DWORD dwRotationAngle;
    DWORD dwZBufferOpCode;
    DWORD dwZBufferLow;
    DWORD dwZBufferHigh;
    DWORD dwZBufferBaseDest;
    DWORD dwZDestConstBitDepth;
    union {
        DWORD dwZDestConst;
        void* lpDDSZBufferDest;
    };
    DWORD dwZSrcConstBitDepth;
    union {
        DWORD dwZSrcConst;
        void* lpDDSZBufferSrc;
    };
    DWORD dwAlphaEdgeBlendBitDepth;
    DWORD dwAlphaEdgeBlend;
    DWORD dwReserved;
    DWORD dwAlphaDestConstBitDepth;
    union {
        DWORD dwAlphaDestConst;
        void* lpDDSAlphaDest;
    };
    DWORD dwAlphaSrcConstBitDepth;
    union {
        DWORD dwAlphaSrcConst;
        void* lpDDSAlphaSrc;
    };
    union {
        DWORD dwFillColor;
        DWORD dwFillDepth;
        DWORD dwFillPixel;
        void* lpDDSPattern;
    };
    DWORD ddckDestColorkey_dwColorSpaceLowValue;
    DWORD ddckDestColorkey_dwColorSpaceHighValue;
    DWORD ddckSrcColorkey_dwColorSpaceLowValue;
    DWORD ddckSrcColorkey_dwColorSpaceHighValue;
} DDBLTFX;

// Blt flags
#define DDBLT_WAIT              0x01000000
#define DDBLT_COLORFILL         0x00000400

// Lock flags
#define DDLOCK_SURFACEMEMORYPTR  0x00000000
#define DDLOCK_WAIT             0x00000001

// Cooperative level flags
#define DDSCL_NORMAL            0x00000008
#define DDSCL_EXCLUSIVE         0x00000010
#define DDSCL_FULLSCREEN        0x00000001

// DDCAPS structure (simplified)
typedef struct _DDCAPS {
    DWORD dwSize;
    DWORD dwCaps;
    DWORD dwCaps2;
    DWORD dwCKeyCaps;
    DWORD dwFXCaps;
    DWORD dwFXAlphaCaps;
    DWORD dwPalCaps;
    DWORD dwSVCaps;
    DWORD dwAlphaBltConstBitDepths;
    DWORD dwAlphaBltPixelBitDepths;
    DWORD dwAlphaBltSurfaceBitDepths;
    DWORD dwAlphaOverlayConstBitDepths;
    DWORD dwAlphaOverlayPixelBitDepths;
    DWORD dwAlphaOverlaySurfaceBitDepths;
    DWORD dwZBufferBitDepths;
    DWORD dwVidMemTotal;
    DWORD dwVidMemFree;
    DWORD dwMaxVisibleOverlays;
    DWORD dwCurrVisibleOverlays;
    DWORD dwNumFourCCCodes;
    DWORD dwAlignBoundarySrc;
    DWORD dwAlignSizeSrc;
    DWORD dwAlignBoundaryDest;
    DWORD dwAlignSizeDest;
    DWORD dwAlignStrideAlign;
    DWORD dwRops[8];
    DWORD dwReservedCaps;
    DWORD dwMinOverlayStretch;
    DWORD dwMaxOverlayStretch;
    DWORD dwMinLiveVideoStretch;
    DWORD dwMaxLiveVideoStretch;
    DWORD dwMinHwCodecStretch;
    DWORD dwMaxHwCodecStretch;
    DWORD dwReserved1;
    DWORD dwReserved2;
    DWORD dwReserved3;
    DWORD dwSVBCaps;
    DWORD dwSVBCKeyCaps;
    DWORD dwSVBFXCaps;
    DWORD dwVSBCaps;
    DWORD dwVSBCKeyCaps;
    DWORD dwVSBFXCaps;
    DWORD dwSSBCaps;
    DWORD dwSSBCKeyCaps;
    DWORD dwSSBFXCaps;
    DWORD dwMaxVideoPorts;
    DWORD dwCurrVideoPorts;
    DWORD dwSVBCaps2;
} DDCAPS;

// DDCAPS capability flags
#define DDCAPS_BLT              0x00000040
#define DDCAPS_BLTQUEUE         0x00000080
#define DDCAPS_CANBLTSYSMEM     0x08000000
#define DDCAPS_PALETTEVSYNC     0x01000000
#define DDCAPS_BANKSWITCHED     0x04000000
#define DDCAPS_BLTCOLORFILL     0x02000000
#define DDCAPS_NOHARDWARE       0x00000200

// ============================================================================
// Stub COM interfaces for DirectDraw
// ============================================================================

// Forward declarations
struct IDirectDrawSurface;
struct IDirectDrawClipper;
struct IDirectDrawPalette;
struct IDirectDraw;
struct IDirectDraw2;

typedef IDirectDrawSurface* LPDIRECTDRAWSURFACE;
typedef IDirectDrawClipper* LPDIRECTDRAWCLIPPER;
typedef IDirectDrawPalette* LPDIRECTDRAWPALETTE;
typedef IDirectDraw* LPDIRECTDRAW;
typedef IDirectDraw2* LPDIRECTDRAW2;

// Minimal IDirectDrawSurface stub
struct IDirectDrawSurface {
    virtual ULONG AddRef() { return 1; }
    virtual ULONG Release() { return 0; }
    virtual HRESULT Lock(RECT* r, DDSURFACEDESC* desc, DWORD flags, HANDLE h) { (void)r; (void)desc; (void)flags; (void)h; return E_NOTIMPL; }
    virtual HRESULT Unlock(void* p) { (void)p; return S_OK; }
    virtual HRESULT Blt(RECT* dst, IDirectDrawSurface* src, RECT* srcr, DWORD flags, DDBLTFX* fx) { (void)dst; (void)src; (void)srcr; (void)flags; (void)fx; return E_NOTIMPL; }
    virtual HRESULT Flip(IDirectDrawSurface* s, DWORD f) { (void)s; (void)f; return E_NOTIMPL; }
    virtual HRESULT GetAttachedSurface(DDSCAPS* caps, IDirectDrawSurface** surf) { (void)caps; (void)surf; return E_NOTIMPL; }
    virtual HRESULT GetDC(HDC* hdc) { (void)hdc; return E_NOTIMPL; }
    virtual HRESULT ReleaseDC(HDC hdc) { (void)hdc; return S_OK; }
    virtual HRESULT GetSurfaceDesc(DDSURFACEDESC* desc) { (void)desc; return E_NOTIMPL; }
    virtual HRESULT SetClipper(IDirectDrawClipper* clip) { (void)clip; return S_OK; }
    virtual HRESULT BltFast(DWORD x, DWORD y, IDirectDrawSurface* src, RECT* srcr, DWORD flags) { (void)x; (void)y; (void)src; (void)srcr; (void)flags; return E_NOTIMPL; }
    virtual HRESULT SetPalette(IDirectDrawPalette* pal) { (void)pal; return S_OK; }
};

// Minimal IDirectDrawClipper stub
struct IDirectDrawClipper {
    virtual ULONG AddRef() { return 1; }
    virtual ULONG Release() { return 0; }
    virtual HRESULT SetHWnd(DWORD flags, HWND hwnd) { (void)flags; (void)hwnd; return S_OK; }
};

// Minimal IDirectDrawPalette stub
struct IDirectDrawPalette {
    virtual ULONG AddRef() { return 1; }
    virtual ULONG Release() { return 0; }
    virtual HRESULT SetEntries(DWORD flags, DWORD start, DWORD count, void* entries) { (void)flags; (void)start; (void)count; (void)entries; return S_OK; }
};

// Minimal IDirectDraw stub
struct IDirectDraw {
    virtual ULONG AddRef() { return 1; }
    virtual ULONG Release() { return 0; }
    virtual HRESULT QueryInterface(REFIID riid, void** ppv) { (void)riid; (void)ppv; return E_NOTIMPL; }
    virtual HRESULT SetCooperativeLevel(HWND hwnd, DWORD flags) { (void)hwnd; (void)flags; return S_OK; }
    virtual HRESULT SetDisplayMode(DWORD w, DWORD h, DWORD bpp) { (void)w; (void)h; (void)bpp; return S_OK; }
    virtual HRESULT RestoreDisplayMode() { return S_OK; }
    virtual HRESULT CreateSurface(DDSURFACEDESC* desc, IDirectDrawSurface** surf, void* unk) { (void)desc; (void)surf; (void)unk; return E_NOTIMPL; }
    virtual HRESULT CreateClipper(DWORD flags, IDirectDrawClipper** clip, void* unk) { (void)flags; (void)clip; (void)unk; return E_NOTIMPL; }
    virtual HRESULT CreatePalette(DWORD flags, void* entries, IDirectDrawPalette** pal, void* unk) { (void)flags; (void)entries; (void)pal; (void)unk; return E_NOTIMPL; }
    virtual HRESULT GetCaps(DDCAPS* driverCaps, DDCAPS* helCaps) { (void)driverCaps; (void)helCaps; return E_NOTIMPL; }
    virtual HRESULT GetAvailableVidMem(DDSCAPS* caps, DWORD* total, DWORD* free_mem) { (void)caps; (void)total; (void)free_mem; return E_NOTIMPL; }
};

// Minimal IDirectDraw2 stub
struct IDirectDraw2 : public IDirectDraw {
};

// IID_IDirectDraw2 (stub GUID)
static const GUID IID_IDirectDraw2 = { 0xB3A6F3E0, 0x2B43, 0x11CF, { 0xA2, 0xDE, 0x00, 0xAA, 0x00, 0xB9, 0x33, 0x56 } };

// DirectDraw function stub
inline HRESULT DirectDrawCreate(void*, void**, void*) { return E_NOTIMPL; }

#endif // RENDER_TYPES_H
