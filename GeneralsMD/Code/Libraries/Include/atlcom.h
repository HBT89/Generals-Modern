// atlcom.h - Minimal ATL COM stub for compilation without full ATL
// Provides CComObjectRootEx, CComCoClass, COM_MAP macros for FEBDispatch.h
#pragma once
#ifndef __ATLCOM_H__
#define __ATLCOM_H__

#include <windows.h>

// Thread models
struct CComSingleThreadModel {};
struct CComMultiThreadModel {};
struct CComGlobalsThreadModel {};

// CComObjectRootBase - base for COM object root (does NOT inherit from IUnknown)
class CComObjectRootBase {
public:
    LONG m_dwRef;
    CComObjectRootBase() : m_dwRef(0) {}
    void SetVoid(void*) {}
    HRESULT _AtlFinalConstruct() { return S_OK; }
    void FinalRelease() {}
};

// CComObjectRootEx<ThreadModel> - ref counting helpers
// Does NOT declare AddRef/Release/QueryInterface - those come from the COM interface base class
template <class ThreadModel = CComSingleThreadModel>
class CComObjectRootEx : public CComObjectRootBase {
public:
    ULONG InternalAddRef() { return (ULONG)InterlockedIncrement(&m_dwRef); }
    ULONG InternalRelease() { return (ULONG)InterlockedDecrement(&m_dwRef); }
};

// CComCoClass<T> - base class for COM coclass; stub provides nothing
template <class T, const CLSID* pclsid = nullptr>
class CComCoClass {};

// COM_MAP macros - define QueryInterface in the derived class
// BEGIN_COM_MAP opens a QueryInterface function that returns this as IUnknown
// COM_INTERFACE_ENTRY adds no logic in stub (all QI calls succeed with this)
// END_COM_MAP closes the function

#define BEGIN_COM_MAP(x) \
    public: \
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void** ppv) { \
        if (!ppv) return E_POINTER; \
        *ppv = (void*)this;

#define COM_INTERFACE_ENTRY(x)       /* stub - handled above */
#define COM_INTERFACE_ENTRY_AGGREGATE(r, p)  /* stub */
#define COM_INTERFACE_ENTRY2(x, x2)  /* stub */

#define END_COM_MAP() \
        return S_OK; \
    }

// ATL_NO_VTABLE - removes vtable pointer to save memory; stub is no-op
#define ATL_NO_VTABLE

// DECLARE_PROTECT_FINAL_CONSTRUCT - stub
#define DECLARE_PROTECT_FINAL_CONSTRUCT()

// DECLARE_NOT_AGGREGATABLE - stub
#define DECLARE_NOT_AGGREGATABLE(x)
#define DECLARE_AGGREGATABLE(x)
#define DECLARE_ONLY_AGGREGATABLE(x)

// ATL object map macros - stubs
#define BEGIN_OBJECT_MAP(x)
#define OBJECT_ENTRY(clsid, class)
#define END_OBJECT_MAP()

#endif // __ATLCOM_H__
