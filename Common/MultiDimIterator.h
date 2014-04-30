#ifndef __MULTI_DIM_ITERATOR_H__
#define __MULTI_DIM_ITERATOR_H__

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
#include <vector>

namespace cifti
{
    
    template<typename T>
    class MultiDimIterator
    {
        std::vector<T> m_dims, m_pos;
        bool m_atEnd;
        void gotoBegin();
        void gotoLast();
    public:
        explicit MultiDimIterator(const std::vector<T>& dimensions);
        void operator++();
        void operator++(int);
        void operator--();
        void operator--(int);
        const std::vector<T>& operator*() const { return m_pos; }
        bool atEnd() const { return m_atEnd; }
    };
    
    template<typename T>
    MultiDimIterator<T>::MultiDimIterator(const std::vector<T>& dimensions)
    {
        m_dims = dimensions;
        gotoBegin();
    }
    
    template<typename T>
    void MultiDimIterator<T>::gotoBegin()
    {
        m_pos = std::vector<T>(m_dims.size(), 0);
        m_atEnd = false;
        size_t numDims = m_dims.size();
        for (size_t i = 0; i < numDims; ++i)
        {
            if (m_dims[i] < 1)
            {
                m_atEnd = true;
                break;
            }
        }
    }
    
    template<typename T>
    void MultiDimIterator<T>::gotoLast()
    {
        m_pos = std::vector<T>(m_dims.size());
        m_atEnd = false;
        size_t numDims = m_dims.size();
        for (size_t i = 0; i < numDims; ++i)
        {
            m_pos[i] = m_dims[i] - 1;
            if (m_dims[i] < 1)
            {
                m_atEnd = true;
            }
        }
    }

    template<typename T>
    void MultiDimIterator<T>::operator++()
    {
        if (atEnd())//wrap around
        {
            gotoBegin();
            return;
        }
        if (m_dims.size() == 0)
        {
            m_atEnd = true;//special case: no dimensions works the same as 1 dimension of length 1
            return;
        }
        size_t numDims = m_dims.size();
        for (size_t i = 0; i < numDims; ++i)
        {
            ++m_pos[i];
            if (m_pos[i] < m_dims[i]) return;
            m_pos[i] = 0;
        }
        m_atEnd = true;//if we didn't return already, all of them wrapped, so we are at the end
    }
    
    template<typename T>
    void MultiDimIterator<T>::operator++(int)
    {
        ++(*this);
    }
    
    template<typename T>
    void MultiDimIterator<T>::operator--()
    {
        if (atEnd())//wrap around
        {
            gotoLast();
            return;
        }
        if (m_dims.size() == 0)
        {
            m_atEnd = true;//special case: no dimensions works the same as 1 dimension of length 1
            return;
        }
        size_t numDims = m_dims.size();
        for (size_t i = 0; i < numDims; ++i)
        {
            if (m_pos[i] > 0)
            {
                --m_pos[i];
                return;
            } else {
                m_pos[i] = m_dims[i] - 1;
            }
        }
        m_atEnd = true;//if we didn't return already, all of them wrapped, so we are at the end
    }
    
    template<typename T>
    void MultiDimIterator<T>::operator--(int)
    {
        --(*this);
    }
    
}

#endif //__MULTI_DIM_ITERATOR_H__
