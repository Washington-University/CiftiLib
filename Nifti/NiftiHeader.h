#ifndef __NIFTI_HEADER_H__
#define __NIFTI_HEADER_H__

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

#include "BinaryFile.h"

#include "nifti1.h"
#include "nifti2.h"

#include "boost/shared_ptr.hpp"
#include <vector>

namespace cifti
{
    
    struct NiftiExtension
    {
        int32_t m_ecode;
        std::vector<char> m_bytes;
    };
    
    struct NiftiHeader
    {
        std::vector<boost::shared_ptr<NiftiExtension> > m_extensions;//allow direct access to the extensions
        
        NiftiHeader();
        void read(BinaryFile& inFile);
        void write(BinaryFile& outFile, const int& version = 1, const bool& swapEndian = false);//returns new vox_offset, doesn't set it internally
        bool canWriteVersion(const int& version) const;
        bool isSwapped() const { return m_isSwapped; }
        int version() const { return m_version; }
        
        std::vector<int64_t> getDimensions() const;
        std::vector<std::vector<float> > getSForm() const;
        int64_t getDataOffset() const { return m_header.vox_offset; }
        int16_t getDataType() const { return m_header.datatype; }
        int32_t getIntentCode() const { return m_header.intent_code; }
        const char* getIntentName() const { return m_header.intent_name; }//NOTE: MAY NOT HAVE A NULL TERMINATOR
        bool getDataScaling(double& mult, double& offset) const;//returns false if scaling not needed
        AString toString() const;
        
        void setDimensions(const std::vector<int64_t>& dimsIn);
        void setSForm(const std::vector<std::vector<float> > &sForm);
        void setIntent(const int32_t& code, const char name[16]);
        void setDataType(const int16_t& type);
        void clearDataScaling();
        void setDataScaling(const double& mult, const double& offset);
        ///get the FSL "scale" space
        std::vector<std::vector<float> > getFSLSpace() const;
        
        bool operator==(const NiftiHeader& rhs) const;//for testing purposes
        bool operator!=(const NiftiHeader& rhs) const { return !((*this) == rhs); }
    private:
        nifti_2_header m_header;//storage for header values regardless of version
        int m_version;
        bool m_isSwapped;
        static void swapHeaderBytes(nifti_1_header &header);
        static void swapHeaderBytes(nifti_2_header &header);
        void prepareHeader(nifti_1_header& header) const;//transform internal state into ready to write header struct
        void prepareHeader(nifti_2_header& header) const;
        void setupFrom(const nifti_1_header& header);//error check provided header, and populate members from it
        void setupFrom(const nifti_2_header& header);
        static int typeToNumBits(const int64_t& type);
        int64_t computeVoxOffset(const int& version) const;
    };
    
}

#endif //__NIFTI_HEADER_H__
