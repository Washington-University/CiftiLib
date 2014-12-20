#ifndef __CIFTI_FILE_H__
#define __CIFTI_FILE_H__

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

#include "AString.h"
#include "CiftiXML.h"

#include "boost/shared_ptr.hpp"

#include <vector>

namespace cifti
{
    
    class CiftiFile
    {
    public:
        class ReadImplInterface
        {
        public:
            virtual void getRow(float* dataOut, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead) const = 0;
            virtual void getColumn(float* dataOut, const int64_t& index) const = 0;
            virtual bool isInMemory() const { return false; }
            virtual ~ReadImplInterface();
        };
        //assume if you can write to it, you can also read from it
        class WriteImplInterface : public ReadImplInterface
        {
        public:
            virtual void setRow(const float* dataIn, const std::vector<int64_t>& indexSelect) = 0;
            virtual void setColumn(const float* dataIn, const int64_t& index) = 0;
            virtual ~WriteImplInterface();
        };
        CiftiFile() { }
        explicit CiftiFile(const AString &fileName);//calls openFile
        void openFile(const AString& fileName);//starts on-disk reading
        void setWritingFile(const AString& fileName, const CiftiVersion& writingVersion = CiftiVersion());//starts on-disk writing
        void writeFile(const AString& fileName, const CiftiVersion& writingVersion = CiftiVersion());//leaves current state as-is, rewrites if already writing to that filename and version mismatch
        void convertToInMemory();
        
        const CiftiXML& getCiftiXML() const { return m_xml; }
        bool isInMemory() const;
        void getRow(float* dataOut, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead = false) const;//tolerateShortRead is useful for on-disk writing when it is easiest to do RMW multiple times on a new file
        const std::vector<int64_t>& getDimensions() const { return m_dims; }
        void getColumn(float* dataOut, const int64_t& index) const;//for 2D only, will be slow if on disk!
        
        void setCiftiXML(const CiftiXML& xml, const bool useOldMetadata = true);
        void setRow(const float* dataIn, const std::vector<int64_t>& indexSelect);
        void setColumn(const float* dataIn, const int64_t& index);//for 2D only, will be slow if on disk!
        
        void getRow(float* dataOut, const int64_t& index, const bool& tolerateShortRead) const;//for 2D only, if you don't want to pass a vector
        void getRow(float* dataOut, const int64_t& index) const;
        
        void setRow(const float* dataIn, const int64_t& index);//for 2D only, if you don't want to pass a vector
    private:
        std::vector<int64_t> m_dims;
        boost::shared_ptr<WriteImplInterface> m_writingImpl;//this will be equal to m_readingImpl when non-null
        boost::shared_ptr<ReadImplInterface> m_readingImpl;
        AString m_writingFile;
        CiftiXML m_xml;
        CiftiVersion m_onDiskVersion;
        void verifyWriteImpl();
        static void copyImplData(const ReadImplInterface* from, WriteImplInterface* to, const std::vector<int64_t>& dims);
    };
    
}

#endif //__CIFTI_FILE_H__
