// linker_stubs.cpp — Stubs for symbols missing in the BGFX port build
// These provide minimal implementations so the linker can resolve them.

#include <windows.h>
#include <objbase.h>

// AsciiString is available via forced PreRTS.h include
#include "Common/AsciiString.h"

// GameSpy QR2 hosting status — declared extern "C" in source
extern "C" int getQR2HostingStatus(void) { return 0; }

// ---------------------------------------------------------------------------
// The crash-reporting stubs that used to live here have been REMOVED.
//
// This file previously supplied no-op replacements for FillStackAddresses,
// StackDumpFromAddresses, GetFunctionDetails and DumpExceptionInfo — plus the
// g_LastErrorDump global — because GameEngine/Source/Common/System/StackDump.cpp
// compiles its real implementations only under
//   #if defined(_DEBUG) || defined(_INTERNAL) || defined(IG_DEBUG_STACKTRACE)
// and none of those were defined in this Release build.
//
// The consequence was severe and quiet: DumpExceptionInfo was an empty function
// and FillStackAddresses nulled every frame, so an access violation produced no
// stack trace whatsoever. Multiple faults this week — including a reproducible
// level-load crash — had to be diagnosed by reading trace logs and guessing,
// which produced two wrong root causes.
//
// CMakeLists.txt now defines IG_DEBUG_STACKTRACE, so StackDump.cpp provides the
// real versions and these stubs would be duplicate symbols. It also defines
// g_LastErrorDump (StackDump.cpp:453), so that is gone from here too.
//
// Do not re-add these. If a link error for one of them appears, the correct fix
// is to work out why StackDump.cpp is no longer being compiled.
// ---------------------------------------------------------------------------

// DX8Wrapper_IsWindowed — referenced by Debug.obj (ReleaseCrash)
bool DX8Wrapper_IsWindowed = true;


// IID_IBrowserDispatch — COM GUID for EABrowserDispatch
// {BC834510-C5BC-4B90-8C9A-0E4B1998796F}
static const GUID local_IID_IBrowserDispatch =
    { 0xBC834510, 0xC5BC, 0x4B90, { 0x8C, 0x9A, 0x0E, 0x4B, 0x19, 0x98, 0x79, 0x6F } };
extern "C" const GUID IID_IBrowserDispatch = local_IID_IBrowserDispatch;
