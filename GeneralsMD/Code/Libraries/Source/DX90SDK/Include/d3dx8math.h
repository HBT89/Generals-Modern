/**
 * D3DX8 Math - Stub header for compilation only
 */
#pragma once
#ifndef _D3DX8MATH_H_
#define _D3DX8MATH_H_

#include <math.h>
#include "d3d8.h"

#ifndef D3DX_PI
#define D3DX_PI    ((float)3.14159265358979323846)
#endif
#ifndef D3DX_1BYPI
#define D3DX_1BYPI ((float)0.318309886183790671538)
#endif

#define D3DXToRadian(degree) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree(radian) ((radian) * (180.0f / D3DX_PI))

#ifdef __cplusplus

/* ------------------------------------------------------------------ */
/*  D3DXVECTOR2                                                        */
/* ------------------------------------------------------------------ */

typedef struct D3DXVECTOR2 {
    float x, y;

    D3DXVECTOR2() {}
    D3DXVECTOR2(float _x, float _y) : x(_x), y(_y) {}
    D3DXVECTOR2(const float* pf) : x(pf[0]), y(pf[1]) {}

    operator float* () { return (float*)&x; }
    operator const float* () const { return (const float*)&x; }

    D3DXVECTOR2& operator += (const D3DXVECTOR2& v) { x += v.x; y += v.y; return *this; }
    D3DXVECTOR2& operator -= (const D3DXVECTOR2& v) { x -= v.x; y -= v.y; return *this; }
    D3DXVECTOR2& operator *= (float f) { x *= f; y *= f; return *this; }
    D3DXVECTOR2& operator /= (float f) { float inv = 1.0f / f; x *= inv; y *= inv; return *this; }

    D3DXVECTOR2 operator + (const D3DXVECTOR2& v) const { return D3DXVECTOR2(x + v.x, y + v.y); }
    D3DXVECTOR2 operator - (const D3DXVECTOR2& v) const { return D3DXVECTOR2(x - v.x, y - v.y); }
    D3DXVECTOR2 operator * (float f) const { return D3DXVECTOR2(x * f, y * f); }
    D3DXVECTOR2 operator / (float f) const { float inv = 1.0f / f; return D3DXVECTOR2(x * inv, y * inv); }
    D3DXVECTOR2 operator - () const { return D3DXVECTOR2(-x, -y); }

    BOOL operator == (const D3DXVECTOR2& v) const { return x == v.x && y == v.y; }
    BOOL operator != (const D3DXVECTOR2& v) const { return x != v.x || y != v.y; }
} D3DXVECTOR2, *LPD3DXVECTOR2;

/* ------------------------------------------------------------------ */
/*  D3DXVECTOR3                                                        */
/* ------------------------------------------------------------------ */

typedef struct D3DXVECTOR3 : public D3DVECTOR {
    D3DXVECTOR3() {}
    D3DXVECTOR3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
    D3DXVECTOR3(const float* pf) { x = pf[0]; y = pf[1]; z = pf[2]; }
    D3DXVECTOR3(const D3DVECTOR& v) { x = v.x; y = v.y; z = v.z; }

    operator float* () { return (float*)&x; }
    operator const float* () const { return (const float*)&x; }

    D3DXVECTOR3& operator += (const D3DXVECTOR3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    D3DXVECTOR3& operator -= (const D3DXVECTOR3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    D3DXVECTOR3& operator *= (float f) { x *= f; y *= f; z *= f; return *this; }
    D3DXVECTOR3& operator /= (float f) { float inv = 1.0f / f; x *= inv; y *= inv; z *= inv; return *this; }

    D3DXVECTOR3 operator + (const D3DXVECTOR3& v) const { return D3DXVECTOR3(x + v.x, y + v.y, z + v.z); }
    D3DXVECTOR3 operator - (const D3DXVECTOR3& v) const { return D3DXVECTOR3(x - v.x, y - v.y, z - v.z); }
    D3DXVECTOR3 operator * (float f) const { return D3DXVECTOR3(x * f, y * f, z * f); }
    D3DXVECTOR3 operator / (float f) const { float inv = 1.0f / f; return D3DXVECTOR3(x * inv, y * inv, z * inv); }
    D3DXVECTOR3 operator - () const { return D3DXVECTOR3(-x, -y, -z); }

    BOOL operator == (const D3DXVECTOR3& v) const { return x == v.x && y == v.y && z == v.z; }
    BOOL operator != (const D3DXVECTOR3& v) const { return x != v.x || y != v.y || z != v.z; }
} D3DXVECTOR3, *LPD3DXVECTOR3;

/* ------------------------------------------------------------------ */
/*  D3DXVECTOR4                                                        */
/* ------------------------------------------------------------------ */

typedef struct D3DXVECTOR4 {
    float x, y, z, w;

    D3DXVECTOR4() {}
    D3DXVECTOR4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    D3DXVECTOR4(const float* pf) : x(pf[0]), y(pf[1]), z(pf[2]), w(pf[3]) {}

    operator float* () { return (float*)&x; }
    operator const float* () const { return (const float*)&x; }
    operator const void* () const { return this; }

    D3DXVECTOR4& operator += (const D3DXVECTOR4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    D3DXVECTOR4& operator -= (const D3DXVECTOR4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    D3DXVECTOR4& operator *= (float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
    D3DXVECTOR4& operator /= (float f) { float inv = 1.0f / f; x *= inv; y *= inv; z *= inv; w *= inv; return *this; }

    D3DXVECTOR4 operator + (const D3DXVECTOR4& v) const { return D3DXVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w); }
    D3DXVECTOR4 operator - (const D3DXVECTOR4& v) const { return D3DXVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w); }
    D3DXVECTOR4 operator * (float f) const { return D3DXVECTOR4(x * f, y * f, z * f, w * f); }
    D3DXVECTOR4 operator / (float f) const { float inv = 1.0f / f; return D3DXVECTOR4(x * inv, y * inv, z * inv, w * inv); }
    D3DXVECTOR4 operator - () const { return D3DXVECTOR4(-x, -y, -z, -w); }

    BOOL operator == (const D3DXVECTOR4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    BOOL operator != (const D3DXVECTOR4& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }
} D3DXVECTOR4, *LPD3DXVECTOR4;

/* ------------------------------------------------------------------ */
/*  D3DXMATRIX                                                         */
/* ------------------------------------------------------------------ */

typedef struct D3DXMATRIX : public D3DMATRIX {
    D3DXMATRIX() {}
    D3DXMATRIX(const float* pf) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = pf[i * 4 + j];
    }
    D3DXMATRIX(const D3DMATRIX& mat) { *((D3DMATRIX*)this) = mat; }
    D3DXMATRIX(float f11, float f12, float f13, float f14,
               float f21, float f22, float f23, float f24,
               float f31, float f32, float f33, float f34,
               float f41, float f42, float f43, float f44) {
        _11 = f11; _12 = f12; _13 = f13; _14 = f14;
        _21 = f21; _22 = f22; _23 = f23; _24 = f24;
        _31 = f31; _32 = f32; _33 = f33; _34 = f34;
        _41 = f41; _42 = f42; _43 = f43; _44 = f44;
    }

    float& operator () (UINT Row, UINT Col) { return m[Row][Col]; }
    float  operator () (UINT Row, UINT Col) const { return m[Row][Col]; }

    operator float* () { return (float*)&_11; }
    operator const float* () const { return (const float*)&_11; }

    D3DXMATRIX operator * (const D3DXMATRIX& mat) const {
        D3DXMATRIX result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; k++)
                    result.m[i][j] += m[i][k] * mat.m[k][j];
            }
        return result;
    }

    D3DXMATRIX& operator *= (const D3DXMATRIX& mat) {
        *this = *this * mat;
        return *this;
    }

    D3DXMATRIX operator + (const D3DXMATRIX& mat) const {
        D3DXMATRIX result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                result.m[i][j] = m[i][j] + mat.m[i][j];
        return result;
    }

    D3DXMATRIX operator - (const D3DXMATRIX& mat) const {
        D3DXMATRIX result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                result.m[i][j] = m[i][j] - mat.m[i][j];
        return result;
    }

    D3DXMATRIX operator * (float f) const {
        D3DXMATRIX result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                result.m[i][j] = m[i][j] * f;
        return result;
    }

    D3DXMATRIX operator - () const {
        D3DXMATRIX result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                result.m[i][j] = -m[i][j];
        return result;
    }

    BOOL operator == (const D3DXMATRIX& mat) const {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                if (m[i][j] != mat.m[i][j]) return FALSE;
        return TRUE;
    }
    BOOL operator != (const D3DXMATRIX& mat) const { return !(*this == mat); }
} D3DXMATRIX, *LPD3DXMATRIX;

/* ------------------------------------------------------------------ */
/*  D3DXQUATERNION                                                     */
/* ------------------------------------------------------------------ */

typedef struct D3DXQUATERNION {
    float x, y, z, w;

    D3DXQUATERNION() {}
    D3DXQUATERNION(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    D3DXQUATERNION(const float* pf) : x(pf[0]), y(pf[1]), z(pf[2]), w(pf[3]) {}

    operator float* () { return (float*)&x; }
    operator const float* () const { return (const float*)&x; }

    D3DXQUATERNION operator + (const D3DXQUATERNION& q) const { return D3DXQUATERNION(x + q.x, y + q.y, z + q.z, w + q.w); }
    D3DXQUATERNION operator - (const D3DXQUATERNION& q) const { return D3DXQUATERNION(x - q.x, y - q.y, z - q.z, w - q.w); }
    D3DXQUATERNION operator * (float f) const { return D3DXQUATERNION(x * f, y * f, z * f, w * f); }
    D3DXQUATERNION operator - () const { return D3DXQUATERNION(-x, -y, -z, -w); }

    BOOL operator == (const D3DXQUATERNION& q) const { return x == q.x && y == q.y && z == q.z && w == q.w; }
    BOOL operator != (const D3DXQUATERNION& q) const { return !(*this == q); }
} D3DXQUATERNION, *LPD3DXQUATERNION;

/* ------------------------------------------------------------------ */
/*  D3DXPLANE                                                          */
/* ------------------------------------------------------------------ */

typedef struct D3DXPLANE {
    float a, b, c, d;

    D3DXPLANE() {}
    D3DXPLANE(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) {}
    D3DXPLANE(const float* pf) : a(pf[0]), b(pf[1]), c(pf[2]), d(pf[3]) {}

    operator float* () { return (float*)&a; }
    operator const float* () const { return (const float*)&a; }

    D3DXPLANE operator - () const { return D3DXPLANE(-a, -b, -c, -d); }

    BOOL operator == (const D3DXPLANE& p) const { return a == p.a && b == p.b && c == p.c && d == p.d; }
    BOOL operator != (const D3DXPLANE& p) const { return !(*this == p); }
} D3DXPLANE, *LPD3DXPLANE;

/* ------------------------------------------------------------------ */
/*  D3DXCOLOR                                                          */
/* ------------------------------------------------------------------ */

typedef struct D3DXCOLOR {
    float r, g, b, a;

    D3DXCOLOR() {}
    D3DXCOLOR(DWORD argb) {
        a = ((argb >> 24) & 0xff) / 255.0f;
        r = ((argb >> 16) & 0xff) / 255.0f;
        g = ((argb >>  8) & 0xff) / 255.0f;
        b = ((argb >>  0) & 0xff) / 255.0f;
    }
    D3DXCOLOR(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
    D3DXCOLOR(const float* pf) : r(pf[0]), g(pf[1]), b(pf[2]), a(pf[3]) {}
    D3DXCOLOR(const D3DCOLORVALUE& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}

    operator float* () { return (float*)&r; }
    operator const float* () const { return (const float*)&r; }
    operator DWORD () const {
        DWORD _r = (DWORD)(r * 255.0f);
        DWORD _g = (DWORD)(g * 255.0f);
        DWORD _b = (DWORD)(b * 255.0f);
        DWORD _a = (DWORD)(a * 255.0f);
        return (_a << 24) | (_r << 16) | (_g << 8) | _b;
    }
    operator D3DCOLORVALUE () const { D3DCOLORVALUE c = {r,g,b,a}; return c; }

    D3DXCOLOR operator + (const D3DXCOLOR& c) const { return D3DXCOLOR(r + c.r, g + c.g, b + c.b, a + c.a); }
    D3DXCOLOR operator - (const D3DXCOLOR& c) const { return D3DXCOLOR(r - c.r, g - c.g, b - c.b, a - c.a); }
    D3DXCOLOR operator * (float f) const { return D3DXCOLOR(r * f, g * f, b * f, a * f); }
    D3DXCOLOR operator / (float f) const { float inv = 1.0f / f; return D3DXCOLOR(r * inv, g * inv, b * inv, a * inv); }
    D3DXCOLOR operator - () const { return D3DXCOLOR(-r, -g, -b, -a); }

    BOOL operator == (const D3DXCOLOR& c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
    BOOL operator != (const D3DXCOLOR& c) const { return !(*this == c); }
} D3DXCOLOR, *LPD3DXCOLOR;

/* ------------------------------------------------------------------ */
/*  Inline Vector functions                                            */
/* ------------------------------------------------------------------ */

inline float D3DXVec2Length(const D3DXVECTOR2* pV) {
    return sqrtf(pV->x * pV->x + pV->y * pV->y);
}

inline float D3DXVec2LengthSq(const D3DXVECTOR2* pV) {
    return pV->x * pV->x + pV->y * pV->y;
}

inline float D3DXVec2Dot(const D3DXVECTOR2* pV1, const D3DXVECTOR2* pV2) {
    return pV1->x * pV2->x + pV1->y * pV2->y;
}

inline D3DXVECTOR2* D3DXVec2Normalize(D3DXVECTOR2* pOut, const D3DXVECTOR2* pV) {
    float len = D3DXVec2Length(pV);
    if (len > 0.0f) { pOut->x = pV->x / len; pOut->y = pV->y / len; }
    else { pOut->x = 0.0f; pOut->y = 0.0f; }
    return pOut;
}

inline D3DXVECTOR2* D3DXVec2Lerp(D3DXVECTOR2* pOut, const D3DXVECTOR2* pV1, const D3DXVECTOR2* pV2, float s) {
    pOut->x = pV1->x + s * (pV2->x - pV1->x);
    pOut->y = pV1->y + s * (pV2->y - pV1->y);
    return pOut;
}

inline float D3DXVec3Length(const D3DXVECTOR3* pV) {
    return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
}

inline float D3DXVec3LengthSq(const D3DXVECTOR3* pV) {
    return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z;
}

inline float D3DXVec3Dot(const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2) {
    return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
}

inline D3DXVECTOR3* D3DXVec3Cross(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2) {
    pOut->x = pV1->y * pV2->z - pV1->z * pV2->y;
    pOut->y = pV1->z * pV2->x - pV1->x * pV2->z;
    pOut->z = pV1->x * pV2->y - pV1->y * pV2->x;
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV) {
    float len = D3DXVec3Length(pV);
    if (len > 0.0f) { pOut->x = pV->x / len; pOut->y = pV->y / len; pOut->z = pV->z / len; }
    else { pOut->x = 0.0f; pOut->y = 0.0f; pOut->z = 0.0f; }
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Lerp(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2, float s) {
    pOut->x = pV1->x + s * (pV2->x - pV1->x);
    pOut->y = pV1->y + s * (pV2->y - pV1->y);
    pOut->z = pV1->z + s * (pV2->z - pV1->z);
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Scale(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, float s) {
    pOut->x = pV->x * s;
    pOut->y = pV->y * s;
    pOut->z = pV->z * s;
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Add(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2) {
    pOut->x = pV1->x + pV2->x;
    pOut->y = pV1->y + pV2->y;
    pOut->z = pV1->z + pV2->z;
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Subtract(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2) {
    pOut->x = pV1->x - pV2->x;
    pOut->y = pV1->y - pV2->y;
    pOut->z = pV1->z - pV2->z;
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Minimize(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2) {
    pOut->x = pV1->x < pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y < pV2->y ? pV1->y : pV2->y;
    pOut->z = pV1->z < pV2->z ? pV1->z : pV2->z;
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Maximize(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2) {
    pOut->x = pV1->x > pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y > pV2->y ? pV1->y : pV2->y;
    pOut->z = pV1->z > pV2->z ? pV1->z : pV2->z;
    return pOut;
}

inline float D3DXVec4Length(const D3DXVECTOR4* pV) {
    return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z + pV->w * pV->w);
}

inline float D3DXVec4LengthSq(const D3DXVECTOR4* pV) {
    return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z + pV->w * pV->w;
}

inline float D3DXVec4Dot(const D3DXVECTOR4* pV1, const D3DXVECTOR4* pV2) {
    return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z + pV1->w * pV2->w;
}

/* ------------------------------------------------------------------ */
/*  Inline Vector Transform functions                                  */
/* ------------------------------------------------------------------ */

inline D3DXVECTOR3* D3DXVec3TransformCoord(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM) {
    float x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
    float y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
    float z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;
    float w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + pM->_44;
    if (w != 0.0f) { pOut->x = x / w; pOut->y = y / w; pOut->z = z / w; }
    else { pOut->x = x; pOut->y = y; pOut->z = z; }
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3TransformNormal(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM) {
    float x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31;
    float y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32;
    float z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33;
    pOut->x = x; pOut->y = y; pOut->z = z;
    return pOut;
}

inline D3DXVECTOR4* D3DXVec3Transform(D3DXVECTOR4* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM) {
    pOut->x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
    pOut->y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
    pOut->z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;
    pOut->w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + pM->_44;
    return pOut;
}

inline D3DXVECTOR4* D3DXVec4Transform(D3DXVECTOR4* pOut, const D3DXVECTOR4* pV, const D3DXMATRIX* pM) {
    pOut->x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pV->w * pM->_41;
    pOut->y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pV->w * pM->_42;
    pOut->z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pV->w * pM->_43;
    pOut->w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + pV->w * pM->_44;
    return pOut;
}

/* ------------------------------------------------------------------ */
/*  Inline Matrix functions                                            */
/* ------------------------------------------------------------------ */

inline D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX* pOut) {
    pOut->_11 = 1.0f; pOut->_12 = 0.0f; pOut->_13 = 0.0f; pOut->_14 = 0.0f;
    pOut->_21 = 0.0f; pOut->_22 = 1.0f; pOut->_23 = 0.0f; pOut->_24 = 0.0f;
    pOut->_31 = 0.0f; pOut->_32 = 0.0f; pOut->_33 = 1.0f; pOut->_34 = 0.0f;
    pOut->_41 = 0.0f; pOut->_42 = 0.0f; pOut->_43 = 0.0f; pOut->_44 = 1.0f;
    return pOut;
}

inline BOOL D3DXMatrixIsIdentity(const D3DXMATRIX* pM) {
    return pM->_11 == 1.0f && pM->_22 == 1.0f && pM->_33 == 1.0f && pM->_44 == 1.0f &&
           pM->_12 == 0.0f && pM->_13 == 0.0f && pM->_14 == 0.0f &&
           pM->_21 == 0.0f && pM->_23 == 0.0f && pM->_24 == 0.0f &&
           pM->_31 == 0.0f && pM->_32 == 0.0f && pM->_34 == 0.0f &&
           pM->_41 == 0.0f && pM->_42 == 0.0f && pM->_43 == 0.0f;
}

inline D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* pOut, const D3DXMATRIX* pM1, const D3DXMATRIX* pM2) {
    D3DXMATRIX temp;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            temp.m[i][j] = 0.0f;
            for (int k = 0; k < 4; k++)
                temp.m[i][j] += pM1->m[i][k] * pM2->m[k][j];
        }
    *pOut = temp;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* pOut, const D3DXMATRIX* pM) {
    D3DXMATRIX temp;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp.m[i][j] = pM->m[j][i];
    *pOut = temp;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixScaling(D3DXMATRIX* pOut, float sx, float sy, float sz) {
    D3DXMatrixIdentity(pOut);
    pOut->_11 = sx; pOut->_22 = sy; pOut->_33 = sz;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixTranslation(D3DXMATRIX* pOut, float x, float y, float z) {
    D3DXMatrixIdentity(pOut);
    pOut->_41 = x; pOut->_42 = y; pOut->_43 = z;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationX(D3DXMATRIX* pOut, float Angle) {
    float s = sinf(Angle), c = cosf(Angle);
    D3DXMatrixIdentity(pOut);
    pOut->_22 = c; pOut->_23 = s;
    pOut->_32 = -s; pOut->_33 = c;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationY(D3DXMATRIX* pOut, float Angle) {
    float s = sinf(Angle), c = cosf(Angle);
    D3DXMatrixIdentity(pOut);
    pOut->_11 = c; pOut->_13 = -s;
    pOut->_31 = s; pOut->_33 = c;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationZ(D3DXMATRIX* pOut, float Angle) {
    float s = sinf(Angle), c = cosf(Angle);
    D3DXMatrixIdentity(pOut);
    pOut->_11 = c; pOut->_12 = s;
    pOut->_21 = -s; pOut->_22 = c;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationYawPitchRoll(D3DXMATRIX* pOut, float Yaw, float Pitch, float Roll) {
    D3DXMATRIX my, mx, mz, temp;
    D3DXMatrixRotationY(&my, Yaw);
    D3DXMatrixRotationX(&mx, Pitch);
    D3DXMatrixRotationZ(&mz, Roll);
    D3DXMatrixMultiply(&temp, &mz, &mx);
    D3DXMatrixMultiply(pOut, &temp, &my);
    return pOut;
}

inline float _D3DXMatrixDeterminant(const D3DXMATRIX* pM) {
    return pM->_11 * (pM->_22 * (pM->_33 * pM->_44 - pM->_34 * pM->_43) -
                      pM->_23 * (pM->_32 * pM->_44 - pM->_34 * pM->_42) +
                      pM->_24 * (pM->_32 * pM->_43 - pM->_33 * pM->_42)) -
           pM->_12 * (pM->_21 * (pM->_33 * pM->_44 - pM->_34 * pM->_43) -
                      pM->_23 * (pM->_31 * pM->_44 - pM->_34 * pM->_41) +
                      pM->_24 * (pM->_31 * pM->_43 - pM->_33 * pM->_41)) +
           pM->_13 * (pM->_21 * (pM->_32 * pM->_44 - pM->_34 * pM->_42) -
                      pM->_22 * (pM->_31 * pM->_44 - pM->_34 * pM->_41) +
                      pM->_24 * (pM->_31 * pM->_42 - pM->_32 * pM->_41)) -
           pM->_14 * (pM->_21 * (pM->_32 * pM->_43 - pM->_33 * pM->_42) -
                      pM->_22 * (pM->_31 * pM->_43 - pM->_33 * pM->_41) +
                      pM->_23 * (pM->_31 * pM->_42 - pM->_32 * pM->_41));
}

inline float D3DXMatrixfDeterminant(const D3DXMATRIX* pM) {
    return _D3DXMatrixDeterminant(pM);
}

inline D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX* pOut, float* pDeterminant, const D3DXMATRIX* pM) {
    float det = _D3DXMatrixDeterminant(pM);
    if (pDeterminant) *pDeterminant = det;
    if (det == 0.0f) return NULL;
    float invDet = 1.0f / det;

    D3DXMATRIX result;
    result._11 =  invDet * (pM->_22 * (pM->_33 * pM->_44 - pM->_34 * pM->_43) - pM->_23 * (pM->_32 * pM->_44 - pM->_34 * pM->_42) + pM->_24 * (pM->_32 * pM->_43 - pM->_33 * pM->_42));
    result._12 = -invDet * (pM->_12 * (pM->_33 * pM->_44 - pM->_34 * pM->_43) - pM->_13 * (pM->_32 * pM->_44 - pM->_34 * pM->_42) + pM->_14 * (pM->_32 * pM->_43 - pM->_33 * pM->_42));
    result._13 =  invDet * (pM->_12 * (pM->_23 * pM->_44 - pM->_24 * pM->_43) - pM->_13 * (pM->_22 * pM->_44 - pM->_24 * pM->_42) + pM->_14 * (pM->_22 * pM->_43 - pM->_23 * pM->_42));
    result._14 = -invDet * (pM->_12 * (pM->_23 * pM->_34 - pM->_24 * pM->_33) - pM->_13 * (pM->_22 * pM->_34 - pM->_24 * pM->_32) + pM->_14 * (pM->_22 * pM->_33 - pM->_23 * pM->_32));
    result._21 = -invDet * (pM->_21 * (pM->_33 * pM->_44 - pM->_34 * pM->_43) - pM->_23 * (pM->_31 * pM->_44 - pM->_34 * pM->_41) + pM->_24 * (pM->_31 * pM->_43 - pM->_33 * pM->_41));
    result._22 =  invDet * (pM->_11 * (pM->_33 * pM->_44 - pM->_34 * pM->_43) - pM->_13 * (pM->_31 * pM->_44 - pM->_34 * pM->_41) + pM->_14 * (pM->_31 * pM->_43 - pM->_33 * pM->_41));
    result._23 = -invDet * (pM->_11 * (pM->_23 * pM->_44 - pM->_24 * pM->_43) - pM->_13 * (pM->_21 * pM->_44 - pM->_24 * pM->_41) + pM->_14 * (pM->_21 * pM->_43 - pM->_23 * pM->_41));
    result._24 =  invDet * (pM->_11 * (pM->_23 * pM->_34 - pM->_24 * pM->_33) - pM->_13 * (pM->_21 * pM->_34 - pM->_24 * pM->_31) + pM->_14 * (pM->_21 * pM->_33 - pM->_23 * pM->_31));
    result._31 =  invDet * (pM->_21 * (pM->_32 * pM->_44 - pM->_34 * pM->_42) - pM->_22 * (pM->_31 * pM->_44 - pM->_34 * pM->_41) + pM->_24 * (pM->_31 * pM->_42 - pM->_32 * pM->_41));
    result._32 = -invDet * (pM->_11 * (pM->_32 * pM->_44 - pM->_34 * pM->_42) - pM->_12 * (pM->_31 * pM->_44 - pM->_34 * pM->_41) + pM->_14 * (pM->_31 * pM->_42 - pM->_32 * pM->_41));
    result._33 =  invDet * (pM->_11 * (pM->_22 * pM->_44 - pM->_24 * pM->_42) - pM->_12 * (pM->_21 * pM->_44 - pM->_24 * pM->_41) + pM->_14 * (pM->_21 * pM->_42 - pM->_22 * pM->_41));
    result._34 = -invDet * (pM->_11 * (pM->_22 * pM->_34 - pM->_24 * pM->_32) - pM->_12 * (pM->_21 * pM->_34 - pM->_24 * pM->_31) + pM->_14 * (pM->_21 * pM->_32 - pM->_22 * pM->_31));
    result._41 = -invDet * (pM->_21 * (pM->_32 * pM->_43 - pM->_33 * pM->_42) - pM->_22 * (pM->_31 * pM->_43 - pM->_33 * pM->_41) + pM->_23 * (pM->_31 * pM->_42 - pM->_32 * pM->_41));
    result._42 =  invDet * (pM->_11 * (pM->_32 * pM->_43 - pM->_33 * pM->_42) - pM->_12 * (pM->_31 * pM->_43 - pM->_33 * pM->_41) + pM->_13 * (pM->_31 * pM->_42 - pM->_32 * pM->_41));
    result._43 = -invDet * (pM->_11 * (pM->_22 * pM->_43 - pM->_23 * pM->_42) - pM->_12 * (pM->_21 * pM->_43 - pM->_23 * pM->_41) + pM->_13 * (pM->_21 * pM->_42 - pM->_22 * pM->_41));
    result._44 =  invDet * (pM->_11 * (pM->_22 * pM->_33 - pM->_23 * pM->_32) - pM->_12 * (pM->_21 * pM->_33 - pM->_23 * pM->_31) + pM->_13 * (pM->_21 * pM->_32 - pM->_22 * pM->_31));

    *pOut = result;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixPerspectiveFovLH(D3DXMATRIX* pOut, float fovy, float Aspect, float zn, float zf) {
    float yScale = 1.0f / tanf(fovy / 2.0f);
    float xScale = yScale / Aspect;
    D3DXMatrixIdentity(pOut);
    pOut->_11 = xScale;
    pOut->_22 = yScale;
    pOut->_33 = zf / (zf - zn);
    pOut->_34 = 1.0f;
    pOut->_43 = -zn * zf / (zf - zn);
    pOut->_44 = 0.0f;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixPerspectiveFovRH(D3DXMATRIX* pOut, float fovy, float Aspect, float zn, float zf) {
    float yScale = 1.0f / tanf(fovy / 2.0f);
    float xScale = yScale / Aspect;
    D3DXMatrixIdentity(pOut);
    pOut->_11 = xScale;
    pOut->_22 = yScale;
    pOut->_33 = zf / (zn - zf);
    pOut->_34 = -1.0f;
    pOut->_43 = zn * zf / (zn - zf);
    pOut->_44 = 0.0f;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixPerspectiveLH(D3DXMATRIX* pOut, float w, float h, float zn, float zf) {
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f * zn / w;
    pOut->_22 = 2.0f * zn / h;
    pOut->_33 = zf / (zf - zn);
    pOut->_34 = 1.0f;
    pOut->_43 = -zn * zf / (zf - zn);
    pOut->_44 = 0.0f;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixOrthoLH(D3DXMATRIX* pOut, float w, float h, float zn, float zf) {
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f / w;
    pOut->_22 = 2.0f / h;
    pOut->_33 = 1.0f / (zf - zn);
    pOut->_43 = zn / (zn - zf);
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixOrthoRH(D3DXMATRIX* pOut, float w, float h, float zn, float zf) {
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f / w;
    pOut->_22 = 2.0f / h;
    pOut->_33 = 1.0f / (zn - zf);
    pOut->_43 = zn / (zn - zf);
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixOrthoOffCenterLH(D3DXMATRIX* pOut, float l, float r, float b, float t, float zn, float zf) {
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 2.0f / (r - l);
    pOut->_22 = 2.0f / (t - b);
    pOut->_33 = 1.0f / (zf - zn);
    pOut->_41 = (l + r) / (l - r);
    pOut->_42 = (t + b) / (b - t);
    pOut->_43 = zn / (zn - zf);
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixLookAtLH(D3DXMATRIX* pOut, const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt, const D3DXVECTOR3* pUp) {
    D3DXVECTOR3 zaxis, xaxis, yaxis;
    D3DXVec3Subtract(&zaxis, pAt, pEye);
    D3DXVec3Normalize(&zaxis, &zaxis);
    D3DXVec3Cross(&xaxis, pUp, &zaxis);
    D3DXVec3Normalize(&xaxis, &xaxis);
    D3DXVec3Cross(&yaxis, &zaxis, &xaxis);

    pOut->_11 = xaxis.x; pOut->_12 = yaxis.x; pOut->_13 = zaxis.x; pOut->_14 = 0.0f;
    pOut->_21 = xaxis.y; pOut->_22 = yaxis.y; pOut->_23 = zaxis.y; pOut->_24 = 0.0f;
    pOut->_31 = xaxis.z; pOut->_32 = yaxis.z; pOut->_33 = zaxis.z; pOut->_34 = 0.0f;
    pOut->_41 = -D3DXVec3Dot(&xaxis, pEye);
    pOut->_42 = -D3DXVec3Dot(&yaxis, pEye);
    pOut->_43 = -D3DXVec3Dot(&zaxis, pEye);
    pOut->_44 = 1.0f;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixLookAtRH(D3DXMATRIX* pOut, const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt, const D3DXVECTOR3* pUp) {
    D3DXVECTOR3 zaxis, xaxis, yaxis;
    D3DXVec3Subtract(&zaxis, pEye, pAt);
    D3DXVec3Normalize(&zaxis, &zaxis);
    D3DXVec3Cross(&xaxis, pUp, &zaxis);
    D3DXVec3Normalize(&xaxis, &xaxis);
    D3DXVec3Cross(&yaxis, &zaxis, &xaxis);

    pOut->_11 = xaxis.x; pOut->_12 = yaxis.x; pOut->_13 = zaxis.x; pOut->_14 = 0.0f;
    pOut->_21 = xaxis.y; pOut->_22 = yaxis.y; pOut->_23 = zaxis.y; pOut->_24 = 0.0f;
    pOut->_31 = xaxis.z; pOut->_32 = yaxis.z; pOut->_33 = zaxis.z; pOut->_34 = 0.0f;
    pOut->_41 = -D3DXVec3Dot(&xaxis, pEye);
    pOut->_42 = -D3DXVec3Dot(&yaxis, pEye);
    pOut->_43 = -D3DXVec3Dot(&zaxis, pEye);
    pOut->_44 = 1.0f;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationAxis(D3DXMATRIX* pOut, const D3DXVECTOR3* pV, float Angle) {
    D3DXVECTOR3 n;
    D3DXVec3Normalize(&n, pV);
    float s = sinf(Angle), c = cosf(Angle), t = 1.0f - c;
    D3DXMatrixIdentity(pOut);
    pOut->_11 = t * n.x * n.x + c;
    pOut->_12 = t * n.x * n.y + s * n.z;
    pOut->_13 = t * n.x * n.z - s * n.y;
    pOut->_21 = t * n.x * n.y - s * n.z;
    pOut->_22 = t * n.y * n.y + c;
    pOut->_23 = t * n.y * n.z + s * n.x;
    pOut->_31 = t * n.x * n.z + s * n.y;
    pOut->_32 = t * n.y * n.z - s * n.x;
    pOut->_33 = t * n.z * n.z + c;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixReflect(D3DXMATRIX* pOut, const D3DXPLANE* pPlane) {
    float a = pPlane->a, b = pPlane->b, c = pPlane->c, d = pPlane->d;
    float len = sqrtf(a*a + b*b + c*c);
    a /= len; b /= len; c /= len; d /= len;
    D3DXMatrixIdentity(pOut);
    pOut->_11 = 1.0f - 2.0f*a*a; pOut->_12 = -2.0f*a*b;       pOut->_13 = -2.0f*a*c;
    pOut->_21 = -2.0f*a*b;       pOut->_22 = 1.0f - 2.0f*b*b; pOut->_23 = -2.0f*b*c;
    pOut->_31 = -2.0f*a*c;       pOut->_32 = -2.0f*b*c;       pOut->_33 = 1.0f - 2.0f*c*c;
    pOut->_41 = -2.0f*a*d;       pOut->_42 = -2.0f*b*d;       pOut->_43 = -2.0f*c*d;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixShadow(D3DXMATRIX* pOut, const D3DXVECTOR4* pLight, const D3DXPLANE* pPlane) {
    float dot = pPlane->a * pLight->x + pPlane->b * pLight->y + pPlane->c * pLight->z + pPlane->d * pLight->w;
    pOut->_11 = dot - pLight->x * pPlane->a; pOut->_12 = -pLight->x * pPlane->b;      pOut->_13 = -pLight->x * pPlane->c;      pOut->_14 = -pLight->x * pPlane->d;
    pOut->_21 = -pLight->y * pPlane->a;      pOut->_22 = dot - pLight->y * pPlane->b; pOut->_23 = -pLight->y * pPlane->c;      pOut->_24 = -pLight->y * pPlane->d;
    pOut->_31 = -pLight->z * pPlane->a;      pOut->_32 = -pLight->z * pPlane->b;      pOut->_33 = dot - pLight->z * pPlane->c; pOut->_34 = -pLight->z * pPlane->d;
    pOut->_41 = -pLight->w * pPlane->a;      pOut->_42 = -pLight->w * pPlane->b;      pOut->_43 = -pLight->w * pPlane->c;      pOut->_44 = dot - pLight->w * pPlane->d;
    return pOut;
}

/* ------------------------------------------------------------------ */
/*  Quaternion functions                                                */
/* ------------------------------------------------------------------ */

inline D3DXQUATERNION* D3DXQuaternionIdentity(D3DXQUATERNION* pOut) {
    pOut->x = 0.0f; pOut->y = 0.0f; pOut->z = 0.0f; pOut->w = 1.0f;
    return pOut;
}

inline float D3DXQuaternionLength(const D3DXQUATERNION* pQ) {
    return sqrtf(pQ->x * pQ->x + pQ->y * pQ->y + pQ->z * pQ->z + pQ->w * pQ->w);
}

inline float D3DXQuaternionDot(const D3DXQUATERNION* pQ1, const D3DXQUATERNION* pQ2) {
    return pQ1->x * pQ2->x + pQ1->y * pQ2->y + pQ1->z * pQ2->z + pQ1->w * pQ2->w;
}

inline D3DXQUATERNION* D3DXQuaternionNormalize(D3DXQUATERNION* pOut, const D3DXQUATERNION* pQ) {
    float len = D3DXQuaternionLength(pQ);
    if (len > 0.0f) { float inv = 1.0f / len; pOut->x = pQ->x * inv; pOut->y = pQ->y * inv; pOut->z = pQ->z * inv; pOut->w = pQ->w * inv; }
    else { pOut->x = 0; pOut->y = 0; pOut->z = 0; pOut->w = 0; }
    return pOut;
}

inline D3DXQUATERNION* D3DXQuaternionConjugate(D3DXQUATERNION* pOut, const D3DXQUATERNION* pQ) {
    pOut->x = -pQ->x; pOut->y = -pQ->y; pOut->z = -pQ->z; pOut->w = pQ->w;
    return pOut;
}

inline D3DXQUATERNION* D3DXQuaternionRotationMatrix(D3DXQUATERNION* pOut, const D3DXMATRIX* pM) {
    float trace = pM->_11 + pM->_22 + pM->_33;
    if (trace > 0.0f) {
        float s = 0.5f / sqrtf(trace + 1.0f);
        pOut->w = 0.25f / s;
        pOut->x = (pM->_23 - pM->_32) * s;
        pOut->y = (pM->_31 - pM->_13) * s;
        pOut->z = (pM->_12 - pM->_21) * s;
    } else if (pM->_11 > pM->_22 && pM->_11 > pM->_33) {
        float s = 2.0f * sqrtf(1.0f + pM->_11 - pM->_22 - pM->_33);
        pOut->w = (pM->_23 - pM->_32) / s;
        pOut->x = 0.25f * s;
        pOut->y = (pM->_12 + pM->_21) / s;
        pOut->z = (pM->_13 + pM->_31) / s;
    } else if (pM->_22 > pM->_33) {
        float s = 2.0f * sqrtf(1.0f + pM->_22 - pM->_11 - pM->_33);
        pOut->w = (pM->_31 - pM->_13) / s;
        pOut->x = (pM->_12 + pM->_21) / s;
        pOut->y = 0.25f * s;
        pOut->z = (pM->_23 + pM->_32) / s;
    } else {
        float s = 2.0f * sqrtf(1.0f + pM->_33 - pM->_11 - pM->_22);
        pOut->w = (pM->_12 - pM->_21) / s;
        pOut->x = (pM->_13 + pM->_31) / s;
        pOut->y = (pM->_23 + pM->_32) / s;
        pOut->z = 0.25f * s;
    }
    return pOut;
}

inline D3DXQUATERNION* D3DXQuaternionRotationYawPitchRoll(D3DXQUATERNION* pOut, float Yaw, float Pitch, float Roll) {
    float cy = cosf(Yaw * 0.5f), sy = sinf(Yaw * 0.5f);
    float cp = cosf(Pitch * 0.5f), sp = sinf(Pitch * 0.5f);
    float cr = cosf(Roll * 0.5f), sr = sinf(Roll * 0.5f);
    pOut->x = sr * cp * cy - cr * sp * sy;
    pOut->y = cr * sp * cy + sr * cp * sy;
    pOut->z = cr * cp * sy - sr * sp * cy;
    pOut->w = cr * cp * cy + sr * sp * sy;
    return pOut;
}

inline D3DXQUATERNION* D3DXQuaternionSlerp(D3DXQUATERNION* pOut, const D3DXQUATERNION* pQ1, const D3DXQUATERNION* pQ2, float t) {
    float dot = D3DXQuaternionDot(pQ1, pQ2);
    D3DXQUATERNION q2 = *pQ2;
    if (dot < 0.0f) { dot = -dot; q2.x = -q2.x; q2.y = -q2.y; q2.z = -q2.z; q2.w = -q2.w; }
    if (dot > 0.9995f) {
        pOut->x = pQ1->x + t * (q2.x - pQ1->x);
        pOut->y = pQ1->y + t * (q2.y - pQ1->y);
        pOut->z = pQ1->z + t * (q2.z - pQ1->z);
        pOut->w = pQ1->w + t * (q2.w - pQ1->w);
    } else {
        float theta = acosf(dot);
        float sinTheta = sinf(theta);
        float w1 = sinf((1.0f - t) * theta) / sinTheta;
        float w2 = sinf(t * theta) / sinTheta;
        pOut->x = w1 * pQ1->x + w2 * q2.x;
        pOut->y = w1 * pQ1->y + w2 * q2.y;
        pOut->z = w1 * pQ1->z + w2 * q2.z;
        pOut->w = w1 * pQ1->w + w2 * q2.w;
    }
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationQuaternion(D3DXMATRIX* pOut, const D3DXQUATERNION* pQ) {
    D3DXMatrixIdentity(pOut);
    float xx = pQ->x * pQ->x, yy = pQ->y * pQ->y, zz = pQ->z * pQ->z;
    float xy = pQ->x * pQ->y, xz = pQ->x * pQ->z, yz = pQ->y * pQ->z;
    float wx = pQ->w * pQ->x, wy = pQ->w * pQ->y, wz = pQ->w * pQ->z;
    pOut->_11 = 1.0f - 2.0f * (yy + zz);
    pOut->_12 = 2.0f * (xy + wz);
    pOut->_13 = 2.0f * (xz - wy);
    pOut->_21 = 2.0f * (xy - wz);
    pOut->_22 = 1.0f - 2.0f * (xx + zz);
    pOut->_23 = 2.0f * (yz + wx);
    pOut->_31 = 2.0f * (xz + wy);
    pOut->_32 = 2.0f * (yz - wx);
    pOut->_33 = 1.0f - 2.0f * (xx + yy);
    return pOut;
}

inline D3DXQUATERNION* D3DXQuaternionRotationAxis(D3DXQUATERNION* pOut, const D3DXVECTOR3* pV, float Angle) {
    D3DXVECTOR3 n;
    D3DXVec3Normalize(&n, pV);
    float halfAngle = Angle * 0.5f;
    float s = sinf(halfAngle);
    pOut->x = n.x * s;
    pOut->y = n.y * s;
    pOut->z = n.z * s;
    pOut->w = cosf(halfAngle);
    return pOut;
}

/* ------------------------------------------------------------------ */
/*  Plane functions                                                    */
/* ------------------------------------------------------------------ */

inline float D3DXPlaneDotCoord(const D3DXPLANE* pP, const D3DXVECTOR3* pV) {
    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d;
}

inline float D3DXPlaneDotNormal(const D3DXPLANE* pP, const D3DXVECTOR3* pV) {
    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z;
}

inline D3DXPLANE* D3DXPlaneNormalize(D3DXPLANE* pOut, const D3DXPLANE* pP) {
    float len = sqrtf(pP->a * pP->a + pP->b * pP->b + pP->c * pP->c);
    if (len > 0.0f) { float inv = 1.0f / len; pOut->a = pP->a * inv; pOut->b = pP->b * inv; pOut->c = pP->c * inv; pOut->d = pP->d * inv; }
    else { pOut->a = 0; pOut->b = 0; pOut->c = 0; pOut->d = 0; }
    return pOut;
}

inline D3DXPLANE* D3DXPlaneFromPointNormal(D3DXPLANE* pOut, const D3DXVECTOR3* pPoint, const D3DXVECTOR3* pNormal) {
    pOut->a = pNormal->x;
    pOut->b = pNormal->y;
    pOut->c = pNormal->z;
    pOut->d = -(pNormal->x * pPoint->x + pNormal->y * pPoint->y + pNormal->z * pPoint->z);
    return pOut;
}

inline D3DXPLANE* D3DXPlaneFromPoints(D3DXPLANE* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2, const D3DXVECTOR3* pV3) {
    D3DXVECTOR3 e1, e2, n;
    D3DXVec3Subtract(&e1, pV2, pV1);
    D3DXVec3Subtract(&e2, pV3, pV1);
    D3DXVec3Cross(&n, &e1, &e2);
    D3DXVec3Normalize(&n, &n);
    return D3DXPlaneFromPointNormal(pOut, pV1, &n);
}

/* ------------------------------------------------------------------ */
/*  Color functions                                                    */
/* ------------------------------------------------------------------ */

inline D3DXCOLOR* D3DXColorLerp(D3DXCOLOR* pOut, const D3DXCOLOR* pC1, const D3DXCOLOR* pC2, float s) {
    pOut->r = pC1->r + s * (pC2->r - pC1->r);
    pOut->g = pC1->g + s * (pC2->g - pC1->g);
    pOut->b = pC1->b + s * (pC2->b - pC1->b);
    pOut->a = pC1->a + s * (pC2->a - pC1->a);
    return pOut;
}

#endif /* __cplusplus */

#include "d3dx8math.inl"

#endif /* _D3DX8MATH_H_ */
