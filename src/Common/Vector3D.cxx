/*LICENSE_START*/ 
/*
 *  Copyright (c) 2014, Washington University School of Medicine
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CiftiAssert.h"
#include "Vector3D.h"

#include <cmath>

using namespace std;
using namespace cifti;

Vector3D Vector3D::cross(const Vector3D& right) const
{
    Vector3D ret;
    ret[0] = m_vec[1] * right[2] - m_vec[2] * right[1];
    ret[1] = m_vec[2] * right[0] - m_vec[0] * right[2];
    ret[2] = m_vec[0] * right[1] - m_vec[1] * right[0];
    return ret;
}

float Vector3D::dot(const Vector3D& right) const
{
    return m_vec[0] * right[0] + m_vec[1] * right[1] + m_vec[2] * right[2];
}

float Vector3D::length() const
{
    return sqrt(lengthsquared());
}

float Vector3D::lengthsquared() const
{
    return m_vec[0] * m_vec[0] + m_vec[1] * m_vec[1] + m_vec[2] * m_vec[2];
}

Vector3D Vector3D::normal(float* origLength) const
{
    Vector3D ret = *this;
    float mylength = length();
    if (mylength != 0.0f) ret /= mylength;
    if (origLength != NULL)
    {
        *origLength = mylength;
    }
    return ret;
}

Vector3D::Vector3D()
{
    m_vec[0] = 0.0f;
    m_vec[1] = 0.0f;
    m_vec[2] = 0.0f;
}

Vector3D::Vector3D(const float& x, const float& y, const float& z)
{
    m_vec[0] = x;
    m_vec[1] = y;
    m_vec[2] = z;
}

Vector3D::Vector3D(const float* right)
{
    m_vec[0] = right[0];
    m_vec[1] = right[1];
    m_vec[2] = right[2];
}

float& Vector3D::operator[](const int64_t& index)
{
    CiftiAssert(index > -1 && index < 3);
    return m_vec[index];
}

const float& Vector3D::operator[](const int64_t& index) const
{
    CiftiAssert(index > -1 && index < 3);
    return m_vec[index];
}

float& Vector3D::operator[](const int32_t& index)
{
    CiftiAssert(index > -1 && index < 3);
    return m_vec[index];
}

const float& Vector3D::operator[](const int32_t& index) const
{
    CiftiAssert(index > -1 && index < 3);
    return m_vec[index];
}

Vector3D Vector3D::operator*(const float& right) const
{
    Vector3D ret = *this;
    ret *= right;
    return ret;
}

Vector3D& Vector3D::operator*=(const float& right)
{
    m_vec[0] *= right;
    m_vec[1] *= right;
    m_vec[2] *= right;
    return *this;
}

Vector3D cifti::operator*(const float& left, const Vector3D& right)
{
    return right * left;
}

Vector3D Vector3D::operator+(const Vector3D& right) const
{
    Vector3D ret = *this;
    ret += right;
    return ret;
}

Vector3D& Vector3D::operator+=(const Vector3D& right)
{
    m_vec[0] += right.m_vec[0];
    m_vec[1] += right.m_vec[1];
    m_vec[2] += right.m_vec[2];
    return *this;
}

Vector3D Vector3D::operator-(const Vector3D& right) const
{
    Vector3D ret = *this;
    ret -= right;
    return ret;
}

Vector3D Vector3D::operator-() const
{
    Vector3D ret;
    ret.m_vec[0] = -m_vec[0];
    ret.m_vec[1] = -m_vec[1];
    ret.m_vec[2] = -m_vec[2];
    return ret;
}

Vector3D& Vector3D::operator-=(const Vector3D& right)
{
    m_vec[0] -= right.m_vec[0];
    m_vec[1] -= right.m_vec[1];
    m_vec[2] -= right.m_vec[2];
    return *this;
}

Vector3D Vector3D::operator/(const float& right) const
{
    Vector3D ret = *this;
    ret /= right;
    return ret;
}

Vector3D& Vector3D::operator/=(const float& right)
{
    m_vec[0] /= right;
    m_vec[1] /= right;
    m_vec[2] /= right;
    return *this;
}

Vector3D& Vector3D::operator=(const float* right)
{
    m_vec[0] = right[0];
    m_vec[1] = right[1];
    m_vec[2] = right[2];
    return *this;
}
