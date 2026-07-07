// dx8WebBrowser.h - Stub for DX8 web browser integration
#pragma once
#ifndef _DX8WEBBROWSER_H_
#define _DX8WEBBROWSER_H_

#include <windows.h>

#define BROWSEROPTION_SCROLLBARS  0x0001
#define BROWSEROPTION_3DBORDER    0x0002

typedef IDispatch* LPDISPATCH;

class DX8WebBrowser {
public:
    static void* CreateBrowser(const char*, const char*, int, int, int, int, int, unsigned int, LPDISPATCH) { return nullptr; }
    static void DestroyBrowser(const char*) {}
    static bool Initialize(void) { return true; }
    static void Shutdown(void) {}
};

#endif
