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

//try to force large file support from zlib, any other file reading calls
#ifndef CIFTILIB_OS_MACOSX
#define _LARGEFILE64_SOURCE
#define _LFS64_LARGEFILE 1
#define _FILE_OFFSET_BITS 64
#endif

#include "BinaryFile.h"
#include "CiftiAssert.h"
#include "CiftiException.h"

#ifdef CIFTILIB_USE_QT
    #include <QFile>
#else
    #include "stdio.h"
    #include "errno.h"
    #define BOOST_FILESYSTEM_VERSION 3
    #include "boost/filesystem.hpp"
#endif

#ifdef CIFTILIB_HAVE_ZLIB
#include "zlib.h"
#endif //CIFTILIB_HAVE_ZLIB

#include <algorithm>
#include <iostream>

using namespace cifti;
using boost::shared_ptr;
using namespace std;

//private implementation classes
namespace cifti
{
#ifdef ZLIB_VERSION
    class ZFileImpl : public BinaryFile::ImplInterface
    {
        gzFile m_zfile;
        const static int64_t CHUNK_SIZE;
    public:
        ZFileImpl() { m_zfile = NULL; }
        void open(const AString& filename, const BinaryFile::OpenMode& opmode);
        void close();
        void seek(const int64_t& position);
        int64_t pos();
        void read(void* dataOut, const int64_t& count, int64_t* numRead);
        void write(const void* dataIn, const int64_t& count);
        ~ZFileImpl();
    };

    const int64_t ZFileImpl::CHUNK_SIZE = 1<<26;//64MiB, large enough for good performance, small enough for zlib, must convert to uint32
#endif //ZLIB_VERSION

#ifdef CIFTILIB_USE_QT
    class QFileImpl : public BinaryFile::ImplInterface
    {
        QFile m_file;
        const static int64_t CHUNK_SIZE;
    public:
        void open(const AString& filename, const BinaryFile::OpenMode& opmode);
        void close();
        void seek(const int64_t& position);
        int64_t pos();
        void read(void* dataOut, const int64_t& count, int64_t* numRead);
        void write(const void* dataIn, const int64_t& count);
    };

    const int64_t QFileImpl::CHUNK_SIZE = 1<<30;//1GiB, QT4 apparently chokes at more than 2GiB via buffer.read using int32
#else
    class StrFileImpl : public BinaryFile::ImplInterface
    {
        FILE* m_file;
        int64_t m_curPos;//so we can avoid calling seek when it is to current position - QFile does this, and it makes it much faster for some cases
    public:
        StrFileImpl() { m_file = NULL; m_curPos = -1; }
        void open(const AString& filename, const BinaryFile::OpenMode& opmode);
        void close();
        void seek(const int64_t& position);
        int64_t pos();
        void read(void* dataOut, const int64_t& count, int64_t* numRead);
        void write(const void* dataIn, const int64_t& count);
        ~StrFileImpl();
    };
#endif //CIFTILIB_USE_QT
}

BinaryFile::ImplInterface::~ImplInterface()
{
}

BinaryFile::BinaryFile(const AString& filename, const OpenMode& fileMode)
{
    open(filename, fileMode);
}

void BinaryFile::close()
{
    m_curMode = NONE;
    if (m_impl == NULL) return;
    m_impl->close();
    m_impl.reset();
}

AString BinaryFile::getFilename() const
{
    if (m_impl == NULL) return "";//don't throw, its not really a problem
    return m_impl->getFilename();
}

bool BinaryFile::getOpenForRead()
{
    return (m_curMode | READ) != 0;
}

bool BinaryFile::getOpenForWrite()
{
    return (m_curMode | WRITE) != 0;
}

void BinaryFile::open(const AString& filename, const OpenMode& opmode)
{
    close();
    if (opmode == NONE) throw CiftiException("can't open file with NONE mode");
    if (AString_substr(filename, filename.size() - 3) == ".gz")
    {
#ifdef ZLIB_VERSION
        m_impl = boost::shared_ptr<ZFileImpl>(new ZFileImpl());
#else //ZLIB_VERSION
        throw CiftiException("can't open .gz file '" + filename + "', compiled without zlib support");
#endif //ZLIB_VERSION
    } else {
#ifdef CIFTILIB_USE_QT
        m_impl = boost::shared_ptr<QFileImpl>(new QFileImpl());
#else
        m_impl = boost::shared_ptr<StrFileImpl>(new StrFileImpl());
#endif
    }
    m_impl->open(filename, opmode);
    m_curMode = opmode;
}

void BinaryFile::read(void* dataOut, const int64_t& count, int64_t* numRead)
{
    if (!getOpenForRead()) throw CiftiException("file is not open for reading");
    m_impl->read(dataOut, count, numRead);
}

void BinaryFile::seek(const int64_t& position)
{
    if (m_curMode == NONE) throw CiftiException("file is not open, can't seek");
    m_impl->seek(position);
}

int64_t BinaryFile::pos()
{
    if (m_curMode == NONE) throw CiftiException("file is not open, can't report position");
    return m_impl->pos();
}

void BinaryFile::write(const void* dataIn, const int64_t& count)
{
    if (!getOpenForWrite()) throw CiftiException("file is not open for writing");
    m_impl->write(dataIn, count);
}

#ifdef ZLIB_VERSION
void ZFileImpl::open(const AString& filename, const BinaryFile::OpenMode& opmode)
{
    close();//don't need to, but just because
    m_fileName = filename;
    const char* mode = NULL;
    switch (opmode)//we only support a limited number of combinations, and the string modes are quirky
    {
        case BinaryFile::READ:
            mode = "rb";
            break;
        case BinaryFile::WRITE_TRUNCATE:
            mode = "wb";//you have to do "r+b" in order to ask it to not truncate, which zlib doesn't support anyway
            break;
        default:
            throw CiftiException("compressed file only supports READ and WRITE_TRUNCATE modes");
    }
#if !defined(CIFTILIB_OS_MACOSX) && ZLIB_VERNUM > 0x1232
    m_zfile = gzopen64(ASTRING_TO_CSTR(filename), mode);
#else
    m_zfile = gzopen(ASTRING_TO_CSTR(filename), mode);
#endif
    if (m_zfile == NULL)
    {
#ifdef CIFTILIB_USE_QT
        if (QFile::exists(filename))
#else
        if (boost::filesystem::exists(AString_to_std_string(filename)))
#endif
        {
            if (!(opmode & BinaryFile::TRUNCATE))
            {
                throw CiftiException("failed to open compressed file '" + filename + "', file does not exist, or folder permissions prevent seeing it");
            } else {//m_file.error() doesn't help identify this case, see below
                throw CiftiException("failed to open compressed file '" + filename + "', unable to create file");
            }
        }
        throw CiftiException("failed to open compressed file '" + filename + "'");
    }
}

void ZFileImpl::close()
{
    if (m_zfile == NULL) return;//happens when closed and then destroyed, error opening
    gzflush(m_zfile, Z_FULL_FLUSH);
    if (gzclose(m_zfile) != 0) throw CiftiException("error closing compressed file '" + m_fileName + "'");
    m_zfile = NULL;
}

void ZFileImpl::read(void* dataOut, const int64_t& count, int64_t* numRead)
{
    if (m_zfile == NULL) throw CiftiException("read called on unopened ZFileImpl");//shouldn't happen
    int64_t totalRead = 0;
    int readret = 0;//to preserve the info of the read that broke early
    while (totalRead < count)
    {
        int64_t iterSize = min(count - totalRead, CHUNK_SIZE);
        readret = gzread(m_zfile, ((uint8_t*)dataOut) + totalRead, iterSize);
        if (readret < 1) break;//0 or -1 indicate eof or error
        totalRead += readret;
    }
    if (numRead == NULL)
    {
        if (totalRead != count)
        {
            if (readret < 0) throw CiftiException("error while reading compressed file '" + m_fileName + "'");
            throw CiftiException("premature end of file in compressed file '" + m_fileName + "'");
        }
    } else {
        *numRead = totalRead;
    }
}

void ZFileImpl::seek(const int64_t& position)
{
    if (m_zfile == NULL) throw CiftiException("seek called on unopened ZFileImpl");//shouldn't happen
    if (pos() == position) return;//slight hack, since gzseek is slow or nonfunctional for some cases, so don't try it unless necessary
#if !defined(CIFTILIB_OS_MACOSX) && ZLIB_VERNUM > 0x1232
    int64_t ret = gzseek64(m_zfile, position, SEEK_SET);
#else
    int64_t ret = gzseek(m_zfile, position, SEEK_SET);
#endif
    if (ret != position) throw CiftiException("seek failed in compressed file '" + m_fileName + "'");
}

int64_t ZFileImpl::pos()
{
    if (m_zfile == NULL) throw CiftiException("pos called on unopened ZFileImpl");//shouldn't happen
#if !defined(CIFTILIB_OS_MACOSX) && ZLIB_VERNUM > 0x1232
    return gztell64(m_zfile);
#else
    return gztell(m_zfile);
#endif
}

void ZFileImpl::write(const void* dataIn, const int64_t& count)
{
    if (m_zfile == NULL) throw CiftiException("write called on unopened ZFileImpl");//shouldn't happen
    int64_t totalWritten = 0;
    while (totalWritten < count)
    {
        int64_t iterSize = min(count - totalWritten, CHUNK_SIZE);
        int writeret = gzwrite(m_zfile, ((uint8_t*)dataIn) + totalWritten, iterSize);
        if (writeret < 1) break;//0 or -1 indicate eof or error
        totalWritten += writeret;
    }
    if (totalWritten != count) throw CiftiException("failed to write to compressed file '" + m_fileName + "'");
}

ZFileImpl::~ZFileImpl()
{
    try//throwing from a destructor is a bad idea
    {
        close();
    } catch (const CiftiException& e) {
        cerr << AString_to_std_string(e.whatString()) << endl;
    } catch (exception& e) {
        cerr << e.what() << endl;
    } catch (...) {
        cerr << AString_to_std_string("caught unknown exception type while closing compressed file '" + m_fileName + "'") << endl;
    }
}
#endif //ZLIB_VERSION

#ifdef CIFTILIB_USE_QT

void QFileImpl::open(const AString& filename, const BinaryFile::OpenMode& opmode)
{
    close();//don't need to, but just because
    m_fileName = filename;
    QIODevice::OpenMode mode = QIODevice::NotOpen;//means 0
    if (opmode & BinaryFile::READ) mode |= QIODevice::ReadOnly;
    if (opmode & BinaryFile::WRITE) mode |= QIODevice::WriteOnly;
    if (opmode & BinaryFile::TRUNCATE) mode |= QIODevice::Truncate;//expect QFile to recognize silliness like TRUNCATE by itself
    m_file.setFileName(filename);
    if (!m_file.open(mode))
    {
        if (!m_file.exists())
        {
            if (!(opmode & BinaryFile::TRUNCATE))
            {
                throw CiftiException("failed to open file '" + filename + "', file does not exist, or folder permissions prevent seeing it");
            } else {//m_file.error() doesn't help identify this case, see below
                throw CiftiException("failed to open file '" + filename + "', unable to create file");
            }
        }
        switch (m_file.error())
        {
            case QFile::ResourceError://on linux at least, it never gives another code besides the unhelpful OpenError
                throw CiftiException("failed to open file '" + filename + "', too many open files");
            default:
                throw CiftiException("failed to open file '" + filename + "'");
        }
    }
}

void QFileImpl::close()
{
    m_file.close();
}

void QFileImpl::read(void* dataOut, const int64_t& count, int64_t* numRead)
{
    int64_t total = 0;
    int64_t readret = -1;
    while (total < count)
    {
        int64_t maxToRead = min(count - total, CHUNK_SIZE);
        readret = m_file.read(((char*)dataOut) + total, maxToRead);//QFile chokes on large reads also
        if (readret < 1) break;//0 or -1 means error or eof
        total += readret;
    }
    if (numRead == NULL)
    {
        if (total != count)
        {
            if (readret < 0) throw CiftiException("error while reading file '" + m_fileName + "'");
            throw CiftiException("premature end of file in '" + m_fileName + "'");
        }
    } else {
        *numRead = total;
    }
}

void QFileImpl::seek(const int64_t& position)
{
    if (!m_file.seek(position)) throw CiftiException("seek failed in file '" + m_fileName + "'");
}

int64_t QFileImpl::pos()
{
    return m_file.pos();
}

void QFileImpl::write(const void* dataIn, const int64_t& count)
{
    int64_t total = 0;
    int64_t writeret = -1;
    while (total < count)
    {
        int64_t maxToWrite = min(count - total, CHUNK_SIZE);
        writeret = m_file.write((const char*)dataIn, maxToWrite);//QFile probably also chokes on large writes
        if (writeret < 1) break;//0 or -1 means error or eof
        total += writeret;
    }
    if (total != count) throw CiftiException("failed to write to file '" + m_fileName + "'");
}

#else //CIFTILIB_USE_QT

void StrFileImpl::open(const AString& filename, const BinaryFile::OpenMode& opmode)
{
    close();
    m_fileName = filename;
    const char* mode = NULL;
    switch (opmode)
    {
        case BinaryFile::READ:
            mode = "rb";
            break;
        case BinaryFile::READ_WRITE:
            mode = "r+b";
            break;
        case BinaryFile::WRITE_TRUNCATE:
            mode = "wb";
            break;
        case BinaryFile::READ_WRITE_TRUNCATE:
            mode = "w+b";
            break;
        default:
            throw CiftiException("unsupported open mode in StrFileImpl");
    }
    errno = 0;
    m_file = fopen(ASTRING_TO_CSTR(filename), mode);
    int save_err = errno;
    if (m_file == NULL)
    {
        if (!boost::filesystem::exists(AString_to_std_string(filename)))
        {
            if (!(opmode & BinaryFile::TRUNCATE))
            {
                throw CiftiException("failed to open file '" + filename + "', file does not exist, or folder permissions prevent seeing it");
            } else {
                throw CiftiException("failed to open file '" + filename + "', unable to create file");
            }
        }
        switch (save_err)
        {
            case EMFILE:
            case ENFILE:
                throw CiftiException("failed to open file '" + filename + "', too many open files");
            default:
                throw CiftiException("failed to open file '" + filename + "'");
        }
    }
    m_curPos = 0;
}

void StrFileImpl::close()
{
    if (m_file == NULL) return;
    int ret = fclose(m_file);
    m_file = NULL;
    m_curPos = -1;
    if (ret != 0) throw CiftiException("error closing file '" + m_fileName + "'");
}

void StrFileImpl::read(void* dataOut, const int64_t& count, int64_t* numRead)
{
    if (m_file == NULL) throw CiftiException("read called on unopened StrFileImpl");//shouldn't happen
    int64_t readret = fread(dataOut, 1, count, m_file);//expect fread to not have read size limitations comapred to memory
    m_curPos += readret;//the item size is 1
    CiftiAssert(m_curPos == ftello(m_file));//double check it in debug, ftello is fast on linux at least
    if (numRead == NULL)
    {
        if (readret != count)
        {
            if (feof(m_file)) throw CiftiException("premature end of file in file '" + m_fileName + "'");
            throw CiftiException("error while reading file '" + m_fileName + "'");
        }
    } else {
        *numRead = readret;
    }
}

void StrFileImpl::seek(const int64_t& position)
{
    if (m_file == NULL) throw CiftiException("seek called on unopened StrFileImpl");//shouldn't happen
    if (position == m_curPos) return;//optimization: calling fseeko causes nontrivial system call time, on linux at least
    int ret = fseeko(m_file, position, SEEK_SET);
    if (ret != 0) throw CiftiException("seek failed in file '" + m_fileName + "'");
    m_curPos = position;
}

int64_t StrFileImpl::pos()
{
    if (m_file == NULL) throw CiftiException("pos called on unopened StrFileImpl");//shouldn't happen
    CiftiAssert(m_curPos == ftello(m_file));//make sure it is right in debug
    return m_curPos;//we can avoid a call here also
}

void StrFileImpl::write(const void* dataIn, const int64_t& count)
{
    if (m_file == NULL) throw CiftiException("write called on unopened StrFileImpl");//shouldn't happen
    int64_t writeret = fwrite(dataIn, 1, count, m_file);//expect fwrite to not have write size limitations compared to memory
    m_curPos += writeret;//the item size is 1
    CiftiAssert(m_curPos == ftello(m_file));//double check it in debug, ftello is fast on linux at least
    if (writeret != count) throw CiftiException("failed to write to file '" + m_fileName + "'");
}

StrFileImpl::~StrFileImpl()
{
    try//throwing from a destructor is a bad idea
    {
        close();
    } catch (const CiftiException& e) {
        cerr << AString_to_std_string(e.whatString()) << endl;
    } catch (exception& e) {
        cerr << e.what() << endl;
    } catch (...) {
        cerr << AString_to_std_string("caught unknown exception type while closing file '" + m_fileName + "'") << endl;
    }
}

#endif //CIFTILIB_USE_QT
