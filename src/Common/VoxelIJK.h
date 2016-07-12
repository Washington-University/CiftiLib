#ifndef __VOXEL_IJK_H__
#define __VOXEL_IJK_H__

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

#include "stdint.h"

namespace cifti {
    
    struct VoxelIJK
    {
        int64_t m_ijk[3];
        VoxelIJK() { }
        VoxelIJK(int64_t i, int64_t j, int64_t k) { m_ijk[0] = i; m_ijk[1] = j; m_ijk[2] = k; }
        template<typename T>
        VoxelIJK(const T ijk[3]) {
            m_ijk[0] = ijk[0];
            m_ijk[1] = ijk[1];
            m_ijk[2] = ijk[2];
        }
        bool operator<(const VoxelIJK& rhs) const//so it kan be the key of a map
        {
            if (m_ijk[2] < rhs.m_ijk[2]) return true;//compare such that when sorted, m_ijk[0] moves fastest
            if (m_ijk[2] > rhs.m_ijk[2]) return false;
            if (m_ijk[1] < rhs.m_ijk[1]) return true;
            if (m_ijk[1] > rhs.m_ijk[1]) return false;
            return (m_ijk[0] < rhs.m_ijk[0]);
        }
        bool operator==(const VoxelIJK& rhs) const
        {
            return (m_ijk[0] == rhs.m_ijk[0] &&
                    m_ijk[1] == rhs.m_ijk[1] &&
                    m_ijk[2] == rhs.m_ijk[2]);
        }
        bool operator!=(const VoxelIJK& rhs) const { return !((*this) == rhs); }
    };
    
}

#endif //__VOXEL_IJK_H__
