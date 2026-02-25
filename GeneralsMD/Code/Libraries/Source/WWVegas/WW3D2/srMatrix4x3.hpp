#pragma once
#ifndef SRMATRIX4X3_HPP
#define SRMATRIX4X3_HPP

#include <cstring>

// Surrender Engine stub - proprietary library removed
// Minimal matrix implementations for compilation

class srMatrix4x3
{
public:
    float m[4][3];

    srMatrix4x3()
    {
        memset(m, 0, sizeof(m));
        // Identity-like initialization (as much as 4x3 allows)
        m[0][0] = 1.0f;
        m[1][1] = 1.0f;
        m[2][2] = 1.0f;
    }
};

class srMatrix4
{
public:
    float m[4][4];

    srMatrix4()
    {
        memset(m, 0, sizeof(m));
        m[0][0] = 1.0f;
        m[1][1] = 1.0f;
        m[2][2] = 1.0f;
        m[3][3] = 1.0f;
    }
};

class srMatrix4d
{
public:
    double m[4][4];

    srMatrix4d()
    {
        memset(m, 0, sizeof(m));
        m[0][0] = 1.0;
        m[1][1] = 1.0;
        m[2][2] = 1.0;
        m[3][3] = 1.0;
    }
};

class srMatrix3
{
public:
    float m[3][3];

    srMatrix3()
    {
        memset(m, 0, sizeof(m));
        m[0][0] = 1.0f;
        m[1][1] = 1.0f;
        m[2][2] = 1.0f;
    }
};

#endif // SRMATRIX4X3_HPP
