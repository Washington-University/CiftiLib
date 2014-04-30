#ifndef __COMPACT_3D_LOOKUP_H__
#define __COMPACT_3D_LOOKUP_H__

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

#include "CompactLookup.h"

namespace cifti
{
    
    template <typename T>
    class Compact3DLookup
    {
        CompactLookup<CompactLookup<CompactLookup<T> > > m_lookup;//the whole point of this class is to deal with this ugliness
    public:
        ///creates the element if it didn't exist, and returns a reference to it
        T& at(const int64_t& index1, const int64_t& index2, const int64_t& index3);
        ///creates the element if it didn't exist, and returns a reference to it
        T& at(const int64_t index[3]) { return at(index[0], index[1], index[2]); }
        ///add or overwrite an element in the lookup
        void insert(const int64_t& index1, const int64_t& index2, const int64_t& index3, const T& value)
        { at(index1, index2, index3) = value; }
        ///add or overwrite an element in the lookup
        void insert(const int64_t index[3], const T& value)
        { at(index) = value; }
        ///returns a pointer to the desired element, or NULL if no such element is found
        T* find(const int64_t& index1, const int64_t& index2, const int64_t& index3);
        ///returns a pointer to the desired element, or NULL if no such element is found
        T* find(const int64_t index[3]) { return find(index[0], index[1], index[2]); }
        ///returns a pointer to the desired element, or NULL if no such element is found
        const T* find(const int64_t& index1, const int64_t& index2, const int64_t& index3) const;
        ///returns a pointer to the desired element, or NULL if no such element is found
        const T* find(const int64_t index[3]) const { return find(index[0], index[1], index[2]); }
        ///empties the lookup
        void clear();
    };
    
    template<typename T>
    T& Compact3DLookup<T>::at(const int64_t& index1, const int64_t& index2, const int64_t& index3)
    {
        return m_lookup[index3][index2][index1];//a lot of complexity is hidden in those operator[]s
    }

    template<typename T>
    T* Compact3DLookup<T>::find(const int64_t& index1, const int64_t& index2, const int64_t& index3)
    {
        typename CompactLookup<CompactLookup<CompactLookup<T> > >::iterator iter1 = m_lookup.find(index3);//oh the humanity
        if (iter1 == m_lookup.end()) return NULL;
        typename CompactLookup<CompactLookup<T> >::iterator iter2 = iter1->find(index2);
        if (iter2 == iter1->end()) return NULL;
        typename CompactLookup<T>::iterator iter3 = iter2->find(index1);
        if (iter3 == iter2->end()) return NULL;
        return &(*iter3);
    }

    template <typename T>
    const T* Compact3DLookup<T>::find(const int64_t& index1, const int64_t& index2, const int64_t& index3) const
    {
        typename CompactLookup<CompactLookup<CompactLookup<T> > >::const_iterator iter1 = m_lookup.find(index3);
        if (iter1 == m_lookup.end()) return NULL;
        typename CompactLookup<CompactLookup<T> >::const_iterator iter2 = iter1->find(index2);
        if (iter2 == iter1->end()) return NULL;
        typename CompactLookup<T>::const_iterator iter3 = iter2->find(index1);
        if (iter3 == iter2->end()) return NULL;
        return &(*iter3);
    }
    
    template <typename T>
    void Compact3DLookup<T>::clear()
    {
        m_lookup.clear();
    }

}

#endif //__COMPACT_3D_LOOKUP_H__
