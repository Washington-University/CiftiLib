#ifndef __MULTI_DIM_ARRAY_H__
#define __MULTI_DIM_ARRAY_H__

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

#include "stdint.h"
#include <vector>

namespace cifti
{
    
    template<typename T>
    class MultiDimArray
    {
        std::vector<int64_t> m_dims, m_skip;//always use int64_t for indexes internally
        std::vector<T> m_data;
        template<typename I>
        int64_t index(const int& fullDims, const std::vector<I>& indexSelect) const;//assume we never need over 2 billion dimensions
    public:
        const std::vector<int64_t>& getDimensions() const { return m_dims; }
        template<typename I>
        void resize(const std::vector<I>& dims);//destructive resize
        template<typename I>
        T& at(const std::vector<I>& pos);
        template<typename I>
        const T& at(const std::vector<I>& pos) const;
        template<typename I>
        T* get(const int& fullDims, const std::vector<I>& indexSelect);//subarray reference selection
        template<typename I>
        const T* get(const int& fullDims, const std::vector<I>& indexSelect) const;
    };
    
    template<typename T>
    template<typename I>
    void MultiDimArray<T>::resize(const std::vector<I>& dims)
    {
        m_dims = std::vector<int64_t>(dims.begin(), dims.end());
        m_skip.resize(m_dims.size());
        if (dims.size() == 0)
        {
            m_data.clear();
            return;
        }
        int64_t numElems = 1;
        for (int i = 0; i < (int)m_dims.size(); ++i)
        {
            CiftiAssert(m_dims[i] > 0);
            m_skip[i] = numElems;
            numElems *= m_dims[i];
        }
        m_data.resize(numElems);
    }
    
    template<typename T>
    template<typename I>
    int64_t MultiDimArray<T>::index(const int& fullDims, const std::vector<I>& indexSelect) const
    {
        CiftiAssert(fullDims + indexSelect.size() == m_dims.size());
        int64_t ret = 0;
        for (int i = fullDims; i < (int)m_dims.size(); ++i)
        {
            CiftiAssert(indexSelect[i - fullDims] >= 0 && indexSelect[i - fullDims] < m_dims[i]);
            ret += m_skip[i] * indexSelect[i - fullDims];
        }
        return ret;
    }
    
    template<typename T>
    template<typename I>
    T& MultiDimArray<T>::at(const std::vector<I>& pos)
    {
        return m_data[index(0, pos)];
    }
    
    template<typename T>
    template<typename I>
    const T& MultiDimArray<T>::at(const std::vector<I>& pos) const
    {
        return m_data[index(0, pos)];
    }
    
    template<typename T>
    template<typename I>
    T* MultiDimArray<T>::get(const int& fullDims, const std::vector<I>& indexSelect)
    {
        return m_data.data() + index(fullDims, indexSelect);
    }
    
    template<typename T>
    template<typename I>
    const T* MultiDimArray<T>::get(const int& fullDims, const std::vector<I>& indexSelect) const
    {
        return m_data.data() + index(fullDims, indexSelect);
    }
}

#endif //__MULTI_DIM_ARRAY_H__
