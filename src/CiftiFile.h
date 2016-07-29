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

#include "Common/AString.h"
#include "Common/CiftiException.h"
#include "Common/MultiDimIterator.h"
#include "Cifti/CiftiXML.h"

#include "boost/shared_ptr.hpp"

#include <vector>

///namespace for all CiftiLib functionality
namespace cifti
{
    ///class for reading and writing cifti files
    class CiftiFile
    {
    public:
        
        enum ENDIAN
        {
            ANY,//so that writeFile() with default endian argument can do nothing after setWritingFile with any endian argument - uses native if there is no rewrite to avoid
            NATIVE,//as long as there are more than two options anyway, provide a convenience option so people don't need to figure out the machine endianness for a common case
            LITTLE,
            BIG
        };
        
        CiftiFile() { m_endianPref = NATIVE; }
        
        ///starts on-disk reading
        explicit CiftiFile(const AString &fileName);
        
        ///starts on-disk reading
        void openFile(const AString& fileName);
        
        ///starts on-disk writing
        void setWritingFile(const AString& fileName, const CiftiVersion& writingVersion = CiftiVersion(), const ENDIAN& endian = NATIVE);
        
        ///does nothing if filename, version, and effective endianness match file currently open, otherwise writes complete file
        void writeFile(const AString& fileName, const CiftiVersion& writingVersion = CiftiVersion(), const ENDIAN& endian = ANY);
        
        ///reads file into memory, closes file
        void convertToInMemory();
        
        const CiftiXML& getCiftiXML() const { return m_xml; }
        bool isInMemory() const;
        
        ///the tolerateShortRead parameter is useful for on-disk writing when it is easiest to do RMW multiple times on a new file
        void getRow(float* dataOut, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead = false) const;
        const std::vector<int64_t>& getDimensions() const { return m_dims; }
        
        ///convenience function for iterating over arbitrary numbers of dimensions
        MultiDimIterator<int64_t> getIteratorOverRows() const
        {
            return MultiDimIterator<int64_t>(std::vector<int64_t>(getDimensions().begin() + 1, getDimensions().end()));
        }
        
        ///for 2D only, will be slow if on disk!
        void getColumn(float* dataOut, const int64_t& index) const;
        
        void setCiftiXML(const CiftiXML& xml, const bool useOldMetadata = true);
        void setRow(const float* dataIn, const std::vector<int64_t>& indexSelect);
        
        ///for 2D only, will be slow if on disk!
        void setColumn(const float* dataIn, const int64_t& index);
        
        ///for 2D only, if you don't want to pass a vector for indexing
        void getRow(float* dataOut, const int64_t& index, const bool& tolerateShortRead = false) const;
        
        ///for 2D only, if you don't want to pass a vector for indexing
        void setRow(const float* dataIn, const int64_t& index);

        //implementation details from here down
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
    private:
        std::vector<int64_t> m_dims;
        boost::shared_ptr<WriteImplInterface> m_writingImpl;//this will be equal to m_readingImpl when non-null
        boost::shared_ptr<ReadImplInterface> m_readingImpl;
        AString m_writingFile;
        CiftiXML m_xml;
        CiftiVersion m_onDiskVersion;
        ENDIAN m_endianPref;
        
        void verifyWriteImpl();
        static void copyImplData(const ReadImplInterface* from, WriteImplInterface* to, const std::vector<int64_t>& dims);
    };
    
}

#endif //__CIFTI_FILE_H__
