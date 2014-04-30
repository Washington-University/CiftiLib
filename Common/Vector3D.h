#ifndef __VECTOR_3D_H__
#define __VECTOR_3D_H__

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

#include <cstddef>
#include "stdint.h"

namespace cifti {
    
    class Vector3D
    {
        float m_vec[3];
    public:
        //vector functions
        float dot(const Vector3D& right) const;
        Vector3D cross(const Vector3D& right) const;
        Vector3D normal(float* origLength = NULL) const;
        float length() const;
        float lengthsquared() const;
        //constructors
        Vector3D();
        Vector3D(const float& x, const float& y, const float& z);
        Vector3D(const float* right);
        //compatibility operators
        float& operator[](const int64_t& index);
        const float& operator[](const int64_t& index) const;
        float& operator[](const int32_t& index);
        const float& operator[](const int32_t& index) const;
        Vector3D& operator=(const float* right);
        //numerical operators
        Vector3D& operator+=(const Vector3D& right);
        Vector3D& operator-=(const Vector3D& right);
        Vector3D& operator*=(const float& right);
        Vector3D& operator/=(const float& right);
        Vector3D operator+(const Vector3D& right) const;
        Vector3D operator-(const Vector3D& right) const;
        Vector3D operator-() const;
        Vector3D operator*(const float& right) const;
        Vector3D operator/(const float& right) const;//NOTE: doesn't really make sense to have the other division, unlike multiplication
        inline operator float*() { return m_vec; }
    };
    
    Vector3D operator*(const float& left, const Vector3D& right);

}
#endif //__VECTOR_3D_H__
