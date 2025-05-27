/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : WW3D                                                         *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/ww3d2/formconv.cpp                           $*
 *                                                                                             *
 *              Original Author:: Nathaniel Hoffman                                            *
 *                                                                                             *
 *                       Author : Kenny Mitchell                                               * 
 *                                                                                             * 
 *                     $Modtime:: 06/27/02 1:27p                                              $*
 *                                                                                             *
 *                    $Revision:: 3                                                           $*
 *                                                                                             *
 * 06/27/02 KM Z Format support																						*
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#include <cstdint>
#include <bgfx/bgfx.h>
#include "BGFXWrapper.h"
#include "ww3dformat.h"

// Conversion tables: map WW3DFormat to bgfx::TextureFormat::Enum
static const bgfx::TextureFormat::Enum WW3DFormatToBGFXFormatConversionArray[WW3D_FORMAT_COUNT] = {
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_UNKNOWN
    bgfx::TextureFormat::RGB8,    // WW3D_FORMAT_R8G8B8
    bgfx::TextureFormat::RGBA8,   // WW3D_FORMAT_A8R8G8B8
    bgfx::TextureFormat::BGRA8,   // WW3D_FORMAT_X8R8G8B8 (approx)
    bgfx::TextureFormat::RGB565,  // WW3D_FORMAT_R5G6B5
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_X1R5G5B5
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_A1R5G5B5
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_A4R4G4B4
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_R3G3B2
    bgfx::TextureFormat::R8,      // WW3D_FORMAT_A8
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_A8R3G3B2
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_X4R4G4B4
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_A8P8
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_P8
    bgfx::TextureFormat::R8,      // WW3D_FORMAT_L8
    bgfx::TextureFormat::RG8,     // WW3D_FORMAT_A8L8
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_A4L4
    bgfx::TextureFormat::RG8,     // WW3D_FORMAT_V8U8 (bumpmap)
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_L6V5U5
    bgfx::TextureFormat::Unknown, // WW3D_FORMAT_X8L8V8U8
    bgfx::TextureFormat::BC1,     // WW3D_FORMAT_DXT1
    bgfx::TextureFormat::BC2,     // WW3D_FORMAT_DXT2
    bgfx::TextureFormat::BC2,     // WW3D_FORMAT_DXT3
    bgfx::TextureFormat::BC3,     // WW3D_FORMAT_DXT4
    bgfx::TextureFormat::BC3      // WW3D_FORMAT_DXT5
};

bgfx::TextureFormat::Enum WW3DFormat_To_BGFXFormat(WW3DFormat ww3d_format) {
    if (ww3d_format >= WW3D_FORMAT_COUNT) {
        return bgfx::TextureFormat::Unknown;
    } else {
        return WW3DFormatToBGFXFormatConversionArray[(unsigned int)ww3d_format];
    }
}

// TODO: Add Z/depth format conversion if needed for BGFX

void Init_BGFX_To_WW3_Conversion() {
    // No-op for BGFX, but keep for interface compatibility
}
