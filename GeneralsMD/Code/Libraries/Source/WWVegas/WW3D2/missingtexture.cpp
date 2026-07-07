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

// missingtexture.cpp — BGFX port stub
// Original D3D8 implementation created a purple "missing texture" placeholder.
// The source file was truncated (pixel data array incomplete).
// For BGFX, we stub these functions; a proper BGFX missing texture can be
// added later if needed.

#include "missingtexture.h"

static IDirect3DTexture8 * _MissingTexture = NULL;

IDirect3DTexture8* MissingTexture::_Get_Missing_Texture()
{
	return _MissingTexture;
}

IDirect3DSurface8* MissingTexture::_Create_Missing_Surface()
{
	return NULL;
}

void MissingTexture::_Init()
{
	// TODO: Create a BGFX missing texture (solid magenta) when renderer is active
}

void MissingTexture::_Deinit()
{
	if (_MissingTexture) {
		_MissingTexture->Release();
		_MissingTexture = NULL;
	}
}
