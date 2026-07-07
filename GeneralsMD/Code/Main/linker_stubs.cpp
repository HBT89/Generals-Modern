// linker_stubs.cpp — Stubs for symbols missing in the BGFX port build
// These provide minimal implementations so the linker can resolve them.

#include <windows.h>
#include <objbase.h>

// AsciiString is available via forced PreRTS.h include
#include "Common/AsciiString.h"

// g_LastErrorDump — global referenced by Debug.obj
AsciiString g_LastErrorDump;

// GameSpy QR2 hosting status — declared extern "C" in source
extern "C" int getQR2HostingStatus(void) { return 0; }

// Stack dump functions — not critical for initial launch
void FillStackAddresses(void **addresses, unsigned int numAddresses, unsigned int skipFrames)
{
    for (unsigned int i = 0; i < numAddresses; ++i)
        addresses[i] = nullptr;
}

void StackDumpFromAddresses(void **addresses, unsigned int numAddresses, void (*callback)(const char *))
{
    if (callback)
        callback("(stack dump not available in this build)\n");
}

// Debug crash handler — SEH translator function
void DumpExceptionInfo(unsigned int exceptionCode, struct _EXCEPTION_POINTERS *exceptionInfo)
{
    (void)exceptionCode;
    (void)exceptionInfo;
}

// Debug function details — used in WinMain for stack traces
void GetFunctionDetails(void *addr, char *name, char *file, unsigned int *line, unsigned int *address)
{
    (void)addr;
    if (name) name[0] = '\0';
    if (file) file[0] = '\0';
    if (line) *line = 0;
    if (address) *address = 0;
}

// DX8Wrapper_IsWindowed — referenced by Debug.obj (ReleaseCrash)
bool DX8Wrapper_IsWindowed = true;


// IID_IBrowserDispatch — COM GUID for EABrowserDispatch
// {BC834510-C5BC-4B90-8C9A-0E4B1998796F}
static const GUID local_IID_IBrowserDispatch =
    { 0xBC834510, 0xC5BC, 0x4B90, { 0x8C, 0x9A, 0x0E, 0x4B, 0x19, 0x98, 0x79, 0x6F } };
extern "C" const GUID IID_IBrowserDispatch = local_IID_IBrowserDispatch;
