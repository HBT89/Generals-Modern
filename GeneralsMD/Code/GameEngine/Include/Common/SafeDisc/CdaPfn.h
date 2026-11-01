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

// CdaPfn.h - stand-in for the SafeDisc "protected function" markers.
//
// The real header shipped with the SafeDisc SDK, which is binary-only and was stripped
// from the source release. Its macros mark a function so the copy-protection layer can
// verify it at run time. There is no SafeDisc here and nothing to verify, so they expand
// to nothing. WinMain.cpp (munkeeFunc) is the only user, in every build configuration.
//
// Adapted from Zero Hour Reforged's GameEngine/Include/Common/SafeDisc/CdaPfn.h
// (github.com/olcayseygan/CnCGeneralsZH-Reforged).

#pragma once

#ifndef __CDAPFN_H_
#define __CDAPFN_H_

#define CDAPFN_OVERHEAD_L1 0
#define CDAPFN_OVERHEAD_L2 0
#define CDAPFN_OVERHEAD_L3 0
#define CDAPFN_OVERHEAD_L4 0
#define CDAPFN_OVERHEAD_L5 0

#define CDAPFN_CONSTRAINT_NONE 0

// Expands to an empty declaration; the call site supplies the semicolon.
#define CDAPFN_DECLARE_GLOBAL( fn, overhead, constraint )

#define CDAPFN_ENDMARK( fn ) ((void)0)

#endif // __CDAPFN_H_
