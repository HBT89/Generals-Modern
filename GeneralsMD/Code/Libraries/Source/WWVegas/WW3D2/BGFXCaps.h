// BGFXCaps.h
// BGFX-backed capabilities class replacing the old DX8Caps.
// Queries bgfx::getCaps() instead of D3D8 device capabilities.
// All modern GPUs support everything the 2003 engine needs.

#pragma once
#ifndef BGFX_CAPS_H
#define BGFX_CAPS_H

#include "always.h"
#include "RenderTypes.h"
#include "wwstring.h"
#include "ww3dformat.h"
#include <bgfx/bgfx.h>

// Adapter identifier — wraps the D3D8 adapter struct for compat
typedef D3DADAPTER_IDENTIFIER8 BGFXAdapterIdentifier;

class BGFXCaps
{
public:
	// Legacy vendor/device enums (kept for code compatibility, largely unused with BGFX)
	enum VendorIdType {
		VENDOR_UNKNOWN = 0,
		VENDOR_NVIDIA,
		VENDOR_ATI,
		VENDOR_INTEL,
		VENDOR_S3,
		VENDOR_POWERVR,
		VENDOR_MATROX,
		VENDOR_3DFX,
		VENDOR_3DLABS,
		VENDOR_CIRRUSLOGIC,
		VENDOR_RENDITION
	};

	enum DeviceTypeNVidia {
		DEVICE_NVIDIA_UNKNOWN = 0,
		DEVICE_NVIDIA_GEFORCE3, DEVICE_NVIDIA_QUADRO2_PRO, DEVICE_NVIDIA_GEFORCE2_GO,
		DEVICE_NVIDIA_GEFORCE2_ULTRA, DEVICE_NVIDIA_GEFORCE2_GTS, DEVICE_NVIDIA_QUADRO,
		DEVICE_NVIDIA_GEFORCE_DDR, DEVICE_NVIDIA_GEFORCE_256, DEVICE_NVIDIA_TNT2_ALADDIN,
		DEVICE_NVIDIA_TNT2, DEVICE_NVIDIA_TNT2_ULTRA, DEVICE_NVIDIA_TNT2_VANTA,
		DEVICE_NVIDIA_TNT2_M64, DEVICE_NVIDIA_TNT, DEVICE_NVIDIA_RIVA_128,
		DEVICE_NVIDIA_TNT_VANTA, DEVICE_NVIDIA_NV1, DEVICE_NVIDIA_GEFORCE2_MX,
		DEVICE_NVIDIA_GEFORCE4_TI_4600, DEVICE_NVIDIA_GEFORCE4_TI_4400,
		DEVICE_NVIDIA_GEFORCE4_TI, DEVICE_NVIDIA_GEFORCE4_TI_4200,
		DEVICE_NVIDIA_GEFORCE4_MX_460, DEVICE_NVIDIA_GEFORCE4_MX_440,
		DEVICE_NVIDIA_GEFORCE4_MX_420, DEVICE_NVIDIA_GEFORCE4,
		DEVICE_NVIDIA_GEFORCE4_GO_440, DEVICE_NVIDIA_GEFORCE4_GO_420,
		DEVICE_NVIDIA_GEFORCE4_GO_420_32M, DEVICE_NVIDIA_GEFORCE4_GO_440_64M,
		DEVICE_NVIDIA_GEFORCE4_GO, DEVICE_NVIDIA_GEFORCE3_TI_500,
		DEVICE_NVIDIA_GEFORCE3_TI_200, DEVICE_NVIDIA_GEFORCE2_INTEGRATED,
		DEVICE_NVIDIA_GEFORCE2_TI, DEVICE_NVIDIA_QUADRO2_MXR_EX_GO,
		DEVICE_NVIDIA_GEFORCE2_MX_100_200, DEVICE_NVIDIA_GEFORCE2_MX_400,
		DEVICE_NVIDIA_QUADRO_DCC
	};

	enum DeviceTypeATI {
		DEVICE_ATI_UNKNOWN = 0,
		DEVICE_ATI_RAGE_II, DEVICE_ATI_RAGE_II_PLUS, DEVICE_ATI_RAGE_IIC_PCI,
		DEVICE_ATI_RAGE_IIC_AGP, DEVICE_ATI_RAGE_128_MOBILITY,
		DEVICE_ATI_RAGE_128_MOBILITY_M3, DEVICE_ATI_RAGE_128_MOBILITY_M4,
		DEVICE_ATI_RAGE_128_PRO_ULTRA, DEVICE_ATI_RAGE_128_4X,
		DEVICE_ATI_RAGE_128_PRO_GL, DEVICE_ATI_RAGE_128_PRO_VR,
		DEVICE_ATI_RAGE_128_GL, DEVICE_ATI_RAGE_128_VR,
		DEVICE_ATI_RAGE_PRO, DEVICE_ATI_RAGE_PRO_MOBILITY,
		DEVICE_ATI_MOBILITY_RADEON, DEVICE_ATI_MOBILITY_RADEON_VE_M6,
		DEVICE_ATI_RADEON_VE, DEVICE_ATI_RADEON_DDR, DEVICE_ATI_RADEON,
		DEVICE_ATI_MOBILITY_R7500, DEVICE_ATI_R7500, DEVICE_ATI_R8500
	};

	enum DeviceType3DLabs {
		DEVICE_3DLABS_UNKNOWN = 0,
		DEVICE_3DLABS_PERMEDIA, DEVICE_3DLABS_300SX, DEVICE_3DLABS_500TX,
		DEVICE_3DLABS_DELTA, DEVICE_3DLABS_MX, DEVICE_3DLABS_GAMMA,
		DEVICE_3DLABS_PERMEDIA2S_ST, DEVICE_3DLABS_PERMEDIA3, DEVICE_3DLABS_R3,
		DEVICE_3DLABS_PERMEDIA4, DEVICE_3DLABS_R4, DEVICE_3DLABS_G2,
		DEVICE_3DLABS_OXYGEN_VX1, DEVICE_3DLABS_TI_P1, DEVICE_3DLABS_PERMEDIA2
	};

	enum DeviceType3Dfx {
		DEVICE_3DFX_UNKNOWN = 0,
		DEVICE_3DFX_VOODOO_5500_AGP, DEVICE_3DFX_VOODOO_3, DEVICE_3DFX_BANSHEE,
		DEVICE_3DFX_VOODOO_2, DEVICE_3DFX_VOODOO_GRAPHICS, DEVICE_3DFX_VOODOO_RUSH
	};

	enum DeviceTypeMatrox {
		DEVICE_MATROX_UNKNOWN = 0,
		DEVICE_MATROX_G550, DEVICE_MATROX_G400, DEVICE_MATROX_G200_AGP,
		DEVICE_MATROX_G200_PCI, DEVICE_MATROX_G100_PCI, DEVICE_MATROX_G100_AGP,
		DEVICE_MATROX_MILLENNIUM_II_AGP, DEVICE_MATROX_MILLENNIUM_II_PCI,
		DEVICE_MATROX_MYSTIQUE, DEVICE_MATROX_MILLENNIUM,
		DEVICE_MATROX_PARHELIA, DEVICE_MATROX_PARHELIA_AGP8X
	};

	enum DeviceTypePowerVR {
		DEVICE_POWERVR_UNKNOWN = 0,
		DEVICE_POWERVR_KYRO
	};

	enum DeviceTypeS3 {
		DEVICE_S3_UNKNOWN = 0,
		DEVICE_S3_SAVAGE_MX, DEVICE_S3_SAVAGE_4, DEVICE_S3_SAVAGE_200
	};

	enum DeviceTypeIntel {
		DEVICE_INTEL_UNKNOWN = 0,
		DEVICE_INTEL_810, DEVICE_INTEL_810E, DEVICE_INTEL_815
	};

	enum DriverVersionStatusType {
		DRIVER_STATUS_UNKNOWN = 0,
		DRIVER_STATUS_OK,
		DRIVER_STATUS_GOOD,
		DRIVER_STATUS_BAD
	};

	// Construction — BGFX version just queries bgfx::getCaps()
	BGFXCaps(WW3DFormat display_format = WW3D_FORMAT_UNKNOWN);

	static void Shutdown(void);

	// Capability queries — modern GPUs support everything
	bool Support_TnL() const { return true; }
	bool Support_DXTC() const { return SupportDXTC; }
	bool Support_NPatches() const { return false; }
	bool Support_ZBias() const { return true; }
	bool Support_Gamma() const { return true; }
	bool Support_ModAlphaAddClr() const { return true; }
	bool Support_Dot3() const { return true; }
	bool Support_Cubemaps() const { return true; }
	bool Support_Anisotropic_Filtering() const { return true; }
	bool Support_BumpEnvmap() const { return true; }
	bool Support_Bump_Envmap() const { return true; }
	bool Support_BumpEnvmapLuminance() const { return true; }
	bool Support_Bump_Envmap_Luminance() const { return true; }
	bool Support_PointSprites() const { return true; }
	bool Support_Texture_Format(WW3DFormat format) const;
	bool Support_Render_To_Texture_Format(WW3DFormat format) const { return true; }
	bool Support_Depth_Stencil_Format(WW3DZFormat format) const { return true; }
	bool Can_Do_Multi_Pass() const { return true; }
	bool Is_Fog_Allowed() const { return true; }
	bool Is_Valid_Display_Format(int width, int height, WW3DFormat format) const { return true; }

	unsigned Get_Max_Textures_Per_Pass() const { return MaxTexturesPerPass; }
	unsigned Get_Max_Simultaneous_Textures() const { return MaxTexturesPerPass; }

	unsigned Get_Pixel_Shader_Major_Version() const { return 3; }
	unsigned Get_Pixel_Shader_Minor_Version() const { return 0; }
	unsigned Get_Vertex_Shader_Major_Version() const { return 3; }
	unsigned Get_Vertex_Shader_Minor_Version() const { return 0; }

	VendorIdType Get_Vendor() const { return VendorId; }
	unsigned Get_Device() const { return DeviceId; }

	const D3DCAPS8& Get_DX8_Caps() const { return Caps; }

	const StringClass& Get_Caps_Log() const { return CapsLog; }

private:
	void Init_From_BGFX(WW3DFormat display_format);

	D3DCAPS8 Caps;
	VendorIdType VendorId;
	unsigned DeviceId;
	bool SupportDXTC;
	unsigned MaxTexturesPerPass;
	StringClass CapsLog;
};

// Compatibility alias
#define DX8Caps BGFXCaps

#endif // BGFX_CAPS_H
