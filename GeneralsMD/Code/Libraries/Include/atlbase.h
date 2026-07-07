// atlbase.h - Minimal ATL stub for compilation without full ATL
// Provides only what Generals/ZH code needs.
#pragma once
#ifndef __ATLBASE_H__
#define __ATLBASE_H__

#include <windows.h>
#include <objbase.h>

// CComModule stub - needed by FEBDispatch.h via atlcom.h
struct _ATL_OBJMAP_ENTRY { void* pfnGetClassObject; };
class CComModule {
public:
    HRESULT Init(_ATL_OBJMAP_ENTRY*, HINSTANCE, const GUID* = nullptr) { return S_OK; }
    void Term() {}
    HINSTANCE GetModuleInstance() { return nullptr; }
    HRESULT RegisterServer(BOOL = FALSE) { return S_OK; }
    HRESULT UnregisterServer() { return S_OK; }
};
extern CComModule _Module;

// CComObject<T> - creates a concrete COM object from an interface class
// Stub: just derives from T, no real COM refcounting
template<class T>
class CComObject : public T {
public:
    CComObject() {}
    virtual ~CComObject() {}
    virtual ULONG STDMETHODCALLTYPE AddRef() { return 1; }
    virtual ULONG STDMETHODCALLTYPE Release() { return 0; }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) {
        (void)riid; if (ppv) *ppv = static_cast<IUnknown*>(this); return S_OK;
    }
};

// CComQIPtr<T> - smart pointer for COM interface query, no UUID needed
template<class T>
class CComQIPtr {
public:
    T* p;
    CComQIPtr() : p(nullptr) {}
    CComQIPtr(IUnknown* lp) : p(nullptr) {
        (void)lp;  // stub - just store null
    }
    CComQIPtr(T* lp) : p(lp) { if (p) p->AddRef(); }
    ~CComQIPtr() { if (p) p->Release(); }
    T* operator->() { return p; }
    operator T*() { return p; }
    bool operator!() const { return p == nullptr; }
    operator bool() const { return p != nullptr; }
};

// Specialization without IID default param (used in atlbase.h original with __uuidof)
// Left as CComQIPtr<T> above - caller can pass IID explicitly if needed

// CComBSTR - BSTR wrapper stub
class CComBSTR {
public:
    BSTR m_str;
    CComBSTR() : m_str(nullptr) {}
    CComBSTR(const char* s) : m_str(nullptr) { (void)s; }
    ~CComBSTR() { if (m_str) SysFreeString(m_str); }
    operator BSTR() { return m_str; }
};

// CComPtr<T> - simple COM smart pointer
template<class T>
class CComPtr {
public:
    T* p;
    CComPtr() : p(nullptr) {}
    ~CComPtr() { if (p) p->Release(); }
    T* operator->() { return p; }
    operator T*() { return p; }
    T** operator&() { return &p; }
    bool operator!() const { return p == nullptr; }
};

#endif // __ATLBASE_H__
