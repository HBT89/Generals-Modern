// comutil.h - Minimal COM utility stub
#pragma once
#ifndef __COMUTIL_H__
#define __COMUTIL_H__

#include <windows.h>

// _variant_t stub
class _variant_t : public VARIANT {
public:
    _variant_t() { VariantInit(this); }
    ~_variant_t() { VariantClear(this); }
};

// _bstr_t stub
class _bstr_t {
public:
    BSTR m_data;
    _bstr_t() : m_data(nullptr) {}
    _bstr_t(const char* s) : m_data(nullptr) { (void)s; }
    ~_bstr_t() { if (m_data) SysFreeString(m_data); }
    operator BSTR() { return m_data; }
    operator const char*() const { return ""; }
};

#endif // __COMUTIL_H__
