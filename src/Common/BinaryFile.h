#ifndef __BINARY_FILE_H__
#define __BINARY_FILE_H__

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

#include "boost/shared_ptr.hpp"

#include "AString.h"

#include <stdint.h>

namespace cifti {
    
    //class to hide difference between compressed and standard binary file reading, and to automate error checking (throws if problem)
    class BinaryFile
    {
    public:
        enum OpenMode
        {
            NONE = 0,
            READ = 1,
            WRITE = 2,
            READ_WRITE = 3,//for convenience
            TRUNCATE = 4,
            WRITE_TRUNCATE = 6,//ditto
            READ_WRITE_TRUNCATE = 7//ditto
        };
        BinaryFile() { }
        ///constructor that opens file
        BinaryFile(const AString& filename, const OpenMode& fileMode = READ);
        void open(const AString& filename, const OpenMode& opmode = READ);
        void close();
        AString getFilename() const;//not a reference because when no file is open, m_impl is NULL
        bool getOpenForRead();
        bool getOpenForWrite();
        void seek(const int64_t& position);
        int64_t pos();
        void read(void* dataOut, const int64_t& count, int64_t* numRead = NULL);//throw if numRead is NULL and (error or end of file reached early)
        void write(const void* dataIn, const int64_t& count);//failure to complete write is always an exception
        class ImplInterface
        {
        protected:
            AString m_fileName;//filename is tracked here so error messages can be implementation-specific
        public:
            virtual void open(const AString& filename, const OpenMode& opmode) = 0;
            virtual void close() = 0;
            const AString& getFilename() const { return m_fileName; }
            virtual void seek(const int64_t& position) = 0;
            virtual int64_t pos() = 0;
            virtual void read(void* dataOut, const int64_t& count, int64_t* numRead) = 0;
            virtual void write(const void* dataIn, const int64_t& count) = 0;
            virtual ~ImplInterface();
        };
    private:
        boost::shared_ptr<ImplInterface> m_impl;
        OpenMode m_curMode;//so implementation classes don't have to track it
    };
} //namespace cifti

#endif //__BINARY_FILE_H__
