#ifndef __BYTE_SWAPPING_H__
#define __BYTE_SWAPPING_H__

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

#include <stdint.h>

/**
 * This class contains static methods for byte swapping data, typically used
 * when reading binary data files.
 */

namespace cifti {

    class ByteSwapping {
    public:
        template<typename T>
        static void swap(T& toSwap);

        template<typename T>
        static void swapArray(T* toSwap, const uint64_t& count);

    };

    template<typename T>
    void ByteSwapping::swap(T& toSwap)
    {
        if (sizeof(T) == 1) return;//we could specialize 1-byte types, but this should optimize out
        T temp = toSwap;
        char* from = (char*)&temp;
        char* to = (char*)&toSwap;
        for (int i = 0; i < (int)sizeof(T); ++i)
        {
            to[i] = from[sizeof(T) - i - 1];
        }
    }

    template<typename T>
    void ByteSwapping::swapArray(T* toSwap, const uint64_t& count)
    {
        if (sizeof(T) == 1) return;//ditto
        for (uint64_t i = 0; i < count; ++i)
        {
            swap(toSwap[i]);
        }
    }

}

#endif  // __BYTE_SWAPPING_H__

