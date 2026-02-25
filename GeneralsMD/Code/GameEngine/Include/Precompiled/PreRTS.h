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

////////////////////////////////////////////////////////////////////////////////
//																																						//
//  (c) 2001-2003 Electronic Arts Inc.																				//
//																																						//
////////////////////////////////////////////////////////////////////////////////

// This file contains all the header files that shouldn't change frequently.
// Be careful what you stick in here, because putting files that change often in here will
// tend to cheese people's goats.

#ifndef __PRERTS_H__
#define __PRERTS_H__

//-----------------------------------------------------------------------------
// STLport compatibility - the original code used STLport 4.5.3 as its STL
// implementation. Modern MSVC has a conforming STL, so we disable STLport macros.
#ifdef _STLP_USE_NEWALLOC
#undef _STLP_USE_NEWALLOC
#endif
// Original STLport allocator configuration (no longer needed):
// #define _STLP_USE_NEWALLOC 1
// #define _STLP_USE_CUSTOM_NEWALLOC STLSpecialAlloc
class STLSpecialAlloc;


// We actually don't use Windows for much other than timeGetTime, but it was included in 40
// different .cpp files, so I bit the bullet and included it here.
// PLEASE DO NOT ABUSE WINDOWS OR IT WILL BE REMOVED ENTIRELY. :-)
//--------------------------------------------------------------------------------- System Includes
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

// Redirect Win32 AnimateWindow() to avoid name collision with game's AnimateWindow class.
// CRITICAL: This #define MUST be set BEFORE any header that pulls in <windows.h>,
// including <atlbase.h> and <comutil.h> which transitively include it.
// The #undef is placed after ALL system includes, just before game code includes.
#define AnimateWindow _Win32_AnimateWindow

// ATL is only needed for CComBSTR and CComVariant in a few places.
// On modern VS, atlbase.h requires the ATL component to be installed.
#if defined(_ATL_VER) || __has_include(<atlbase.h>)
#include <atlbase.h>
#else
// Minimal ATL stub for compilation without ATL installed
#include <comutil.h>
#endif

#include <windows.h>

#include <assert.h>
#include <ctype.h>
#include <direct.h>
#include <excpt.h>
#include <float.h>
#include <fstream>        // was <fstream.h> - modernized for C++ standard compliance
#include <imagehlp.h>
#include <io.h>
#include <limits.h>
#include <lmcons.h>
// #include <mapicode.h>  // Removed: not used by game code, requires MAPI SDK
#include <math.h>
#include <memory.h>
#include <mmsystem.h>
#include <objbase.h>
#include <ocidl.h>
#include <process.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlguid.h>
// #include <snmp.h>  // Removed: not used by game code, may not be available
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <tchar.h>
#include <time.h>
#include <vfw.h>
#include <winerror.h>
#include <wininet.h>
#include <winreg.h>

#ifndef DIRECTINPUT_VERSION
#	define DIRECTINPUT_VERSION	0x0800
#endif

#include <dinput.h>

//------------------------------------------------------------------------------------ STL Includes
// srj sez: no, include STLTypesdefs below, instead, thanks
//#include <algorithm>
//#include <bitset>
//#include <hash_map>
//#include <list>
//#include <map>
//#include <queue>
//#include <set>
//#include <stack>
//#include <string>
//#include <vector>

// Now safe to undefine the AnimateWindow redirect — all system headers processed.
#undef AnimateWindow

//------------------------------------------------------------------------------------ RTS Includes
// Icky. These have to be in this order.
#include "Lib/Basetype.h"
#include "Common/STLTypedefs.h"
#include "Common/Errors.h"
#include "Common/Debug.h"
#include "Common/AsciiString.h"
#include "Common/SubsystemInterface.h"

#include "Common/GameCommon.h"
#include "Common/GameMemory.h"
#include "Common/GameType.h"
#include "Common/GlobalData.h"

// You might not want Kindof in here because it seems like it changes frequently, but the problem
// is that Kindof is included EVERYWHERE, so it might as well be precompiled.
#include "Common/INI.h"
#include "Common/KindOf.h"
#include "Common/DisabledTypes.h"
#include "Common/NameKeyGenerator.h"
#include "GameClient/ClientRandomValue.h"
#include "GameLogic/LogicRandomValue.h"
#include "Common/ObjectStatusTypes.h"

#include "Common/Thing.h"
#include "Common/UnicodeString.h"

#endif /* __PRERTS_H__ */
