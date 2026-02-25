// BGFXCaps.cpp
// BGFX-backed capabilities implementation.
// Replaces the 1162-line dx8caps.cpp that queried D3D8 device capabilities.
// Modern GPUs support everything the 2003-era engine needs.

#include "always.h"
#include "BGFXCaps.h"
#include <bgfx/bgfx.h>
#include <string.h>

BGFXCaps::BGFXCaps(WW3DFormat display_format)
	: VendorId(VENDOR_UNKNOWN),
	  DeviceId(0),
	  SupportDXTC(true),
	  MaxTexturesPerPass(8)
{
	memset(&Caps, 0, sizeof(Caps));
	Init_From_BGFX(display_format);
}

void BGFXCaps::Shutdown(void)
{
	// Nothing to clean up
}

bool BGFXCaps::Support_Texture_Format(WW3DFormat format) const
{
	// Modern GPUs support all texture formats the engine uses
	if (format == WW3D_FORMAT_UNKNOWN) return false;
	return true;
}

void BGFXCaps::Init_From_BGFX(WW3DFormat display_format)
{
	const bgfx::Caps* caps = bgfx::getCaps();
	if (!caps) return;

	// Map BGFX vendor ID to our legacy vendor enum
	switch (caps->vendorId) {
	case BGFX_PCI_ID_NVIDIA:  VendorId = VENDOR_NVIDIA; break;
	case BGFX_PCI_ID_AMD:     VendorId = VENDOR_ATI; break;
	case BGFX_PCI_ID_INTEL:   VendorId = VENDOR_INTEL; break;
	default:                   VendorId = VENDOR_UNKNOWN; break;
	}

	DeviceId = caps->deviceId;

	// Fill in D3DCAPS8 with modern defaults for any code that reads it directly
	Caps.MaxSimultaneousTextures = 8;
	Caps.MaxTextureWidth = caps->limits.maxTextureSize;
	Caps.MaxTextureHeight = caps->limits.maxTextureSize;
	Caps.MaxPointSize = 256.0f;
	Caps.MaxTextureBlendStages = 8;
	Caps.MaxActiveLights = 8;
	Caps.DevCaps = 0x10000;  // D3DDEVCAPS_HWTRANSFORMANDLIGHT
	Caps.TextureOpCaps = 0xFFFFFFFF;  // Support everything
	Caps.TextureCaps = 0xFFFFFFFF;    // Support everything
	Caps.TextureFilterCaps = 0xFFFFFFFF;
	Caps.RasterCaps = 0xFFFFFFFF;
	Caps.PrimitiveMiscCaps = 0xFFFFFFFF;
	Caps.VertexShaderVersion = 0xFFFE0300;  // VS 3.0
	Caps.PixelShaderVersion = 0xFFFF0300;   // PS 3.0

	MaxTexturesPerPass = 8;

	// Check DXT/BC compressed texture support via BGFX
	SupportDXTC = bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::BC1, 0) ||
	              bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::BC3, 0);

	CapsLog.Format("BGFX Renderer: %s (Vendor: 0x%04X, Device: 0x%04X)\r\n",
		bgfx::getRendererName(bgfx::getRendererType()),
		caps->vendorId, caps->deviceId);
}
