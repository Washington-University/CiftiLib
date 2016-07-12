#ifndef __COMPACT_LOOKUP_H__
#define __COMPACT_LOOKUP_H__

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
    
    template <typename T>
    class CompactLookup
    {
        struct Chunk
        {
            int64_t start;
            std::vector<T> elements;
        };
        std::vector<Chunk> m_chunks;
    public:
        class iterator
        {
            CompactLookup<T>& m_container;
            std::size_t m_chunk;
            int64_t m_elem;
            iterator(CompactLookup<T>& container, std::size_t chunk, int64_t elem) : m_container(container), m_chunk(chunk), m_elem(elem) { }
        public:
            bool operator==(const iterator& rhs) const
            {
                if (&m_container != &(rhs.m_container)) return false;
                if (m_chunk != rhs.m_chunk) return false;
                if (m_elem != rhs.m_elem) return false;
                return true;
            }
            T& operator*()
            {
                CiftiAssert(m_chunk < m_container.m_chunks.size());
                CiftiAssert(m_elem >= 0 && m_elem < (int64_t)m_container.m_chunks[m_chunk].elements.size());
                return m_container.m_chunks[m_chunk].elements[m_elem];
            }
            T* operator->()
            {
                CiftiAssert(m_chunk < m_container.m_chunks.size());
                CiftiAssert(m_elem >= 0 && m_elem < (int64_t)m_container.m_chunks[m_chunk].elements.size());
                return &(m_container.m_chunks[m_chunk].elements[m_elem]);
            }
            friend class CompactLookup<T>;
        };
        class const_iterator
        {
            const CompactLookup<T>& m_container;
            std::size_t m_chunk;
            int64_t m_elem;
            const_iterator(const CompactLookup<T>& container, std::size_t chunk, std::size_t elem) : m_container(container), m_chunk(chunk), m_elem(elem) { }
        public:
            bool operator==(const const_iterator& rhs) const
            {
                if (&m_container != &(rhs.m_container)) return false;
                if (m_chunk != rhs.m_chunk) return false;
                if (m_elem != rhs.m_elem) return false;
                return true;
            }
            const T& operator*()
            {
                CiftiAssert(m_chunk < m_container.m_chunks.size());
                CiftiAssert(m_elem >= 0 && m_elem < (int64_t)m_container.m_chunks[m_chunk].elements.size());
                return m_container.m_chunks[m_chunk].elements[m_elem];
            }
            const T* operator->()
            {
                CiftiAssert(m_chunk < m_container.m_chunks.size());
                CiftiAssert(m_elem >= 0 && m_elem < (int64_t)m_container.m_chunks[m_chunk].elements.size());
                return &(m_container.m_chunks[m_chunk].elements[m_elem]);
            }
            friend class CompactLookup<T>;
        };
        ///creates the element if it didn't exist, and returns a reference to it
        T& operator[](const int64_t& index);
        ///returns an iterator pointing to the desired element, or one equal to end() if no such element is found
        iterator find(const int64_t& index);
        ///returns a const_iterator pointing to the desired element, or one equal to end() if no such element is found
        const_iterator find(const int64_t& index) const;
        iterator end();
        const_iterator end() const;
        ///empties the lookup
        void clear();
    };
    
    template <typename T>
    T& CompactLookup<T>::operator[](const int64_t& index)
    {
        std::size_t numChunks = m_chunks.size();
        std::size_t low = 0, high = numChunks, guess;//NOTE: low is 0 because size_t is unsigned, really means -1
        bool attach_low = false, attach_high = false;
        while (low < high)//bisection search for the chunk with the largest start value not greater than
        {
            guess = (low + high - 1) / 2;//which is why we subtract 1 here
            CiftiAssert(guess < m_chunks.size());
            if (m_chunks[guess].start > index)
            {
                high = guess;
            } else {
                low = guess + 1;
            }
        }//NOTE: low == high after loop ends
        if (high > 0 && m_chunks[high - 1].start + (int64_t)(m_chunks[high - 1].elements.size()) > index)//element exists, return it
        {
            CiftiAssertVectorIndex(m_chunks[high -1].elements, index - m_chunks[high - 1].start);
            return m_chunks[high - 1].elements[index - m_chunks[high - 1].start];
        }
        if (high > 0 && m_chunks[high - 1].start + (int64_t)(m_chunks[high - 1].elements.size()) == index) attach_low = true;//index is 1 beyond the range
        if (high < numChunks && m_chunks[high].start == index + 1) attach_high = true;//index is 1 before the next range
        if (attach_low)
        {
            std::vector<T>& lowvec = m_chunks[high - 1].elements;
            std::size_t retIndex = lowvec.size();
            lowvec.push_back(T());
            if (attach_high)
            {
                std::vector<T>& highvec = m_chunks[high].elements;
                lowvec.insert(lowvec.end(), highvec.begin(), highvec.end());
                m_chunks.erase(m_chunks.begin() + high);
            }
            return lowvec[retIndex];
        } else {
            if (attach_high)
            {
                std::vector<T>& highvec = m_chunks[high].elements;
                highvec.insert(highvec.begin(), T());//add a new element to the start of the vector (yes, this could be slow)
                m_chunks[high].start = index;//and change the start value of the chunk
                return highvec[0];
            } else {
                m_chunks.insert(m_chunks.begin() + high, Chunk());
                m_chunks[high].start = index;
                m_chunks[high].elements.push_back(T());
                return m_chunks[high].elements[0];
            }
        }
    }

    template <typename T>
    typename CompactLookup<T>::iterator CompactLookup<T>::find(const int64_t& index)
    {
        std::size_t numChunks = m_chunks.size();
        std::size_t low = 0, high = numChunks, guess;//NOTE: low is 0 because size_t is unsigned, really means -1
        while (low < high)//bisection search for the chunk with the largest start value not greater than
        {
            guess = (low + high - 1) / 2;//which is why we subtract 1 here
            CiftiAssert(guess < m_chunks.size());
            if (m_chunks[guess].start > index)
            {
                high = guess;
            } else {
                low = guess + 1;
            }
        }//NOTE: low == high after loop ends
        if (high > 0 && m_chunks[high - 1].start + (int64_t)(m_chunks[high - 1].elements.size()) > index)//element exists, return it
        {
            std::size_t outIndex = index - m_chunks[high - 1].start;
            CiftiAssert(outIndex < m_chunks[high - 1].elements.size());
            return iterator(*this, high - 1, outIndex);
        }
        return end();
    }

    template <typename T>
    typename CompactLookup<T>::const_iterator CompactLookup<T>::find(const int64_t& index) const
    {
        std::size_t numChunks = m_chunks.size();
        std::size_t low = 0, high = numChunks, guess;//NOTE: low is 0 because size_t is unsigned, really means -1
        while (low < high)//bisection search for the chunk with the largest start value not greater than
        {
            guess = (low + high - 1) / 2;//which is why we subtract 1 here
            CiftiAssert(guess < m_chunks.size());
            if (m_chunks[guess].start > index)
            {
                high = guess;
            } else {
                low = guess + 1;
            }
        }//NOTE: low == high after loop ends
        if (high > 0 && m_chunks[high - 1].start + (int64_t)(m_chunks[high - 1].elements.size()) > index)//element exists, return it
        {
            std::size_t outIndex = index - m_chunks[high - 1].start;
            CiftiAssert(outIndex < m_chunks[high - 1].elements.size());
            return const_iterator(*this, high - 1, outIndex);
        }
        return end();
    }
    
    template <typename T>
    typename CompactLookup<T>::iterator CompactLookup<T>::end()
    {
        return iterator(*this, 0, -1);
    }

    template <typename T>
    typename CompactLookup<T>::const_iterator CompactLookup<T>::end() const
    {
        return const_iterator(*this, 0, -1);
    }

    template <typename T>
    void CompactLookup<T>::clear()
    {
        m_chunks.clear();
    }
}

#endif //__COMPACT_LOOKUP_H__
