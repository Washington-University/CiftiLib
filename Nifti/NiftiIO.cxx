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

#include "NiftiIO.h"

#include "CiftiException.h"

using namespace std;
using namespace cifti;

void NiftiIO::openRead(const AString& filename)
{
    m_file.open(filename);
    m_header.read(m_file);
    if (m_header.getDataType() == DT_BINARY)
    {
        throw CiftiException("file uses the binary datatype, which is unsupported: " + filename);
    }
    m_dims = m_header.getDimensions();
}

void NiftiIO::writeNew(const AString& filename, const NiftiHeader& header, const int& version, const bool& withRead, const bool& swapEndian)
{
    if (header.getDataType() == DT_BINARY)
    {
        throw CiftiException("writing NIFTI with binary datatype is unsupported");
    }
    if (withRead)
    {
        m_file.open(filename, BinaryFile::READ_WRITE_TRUNCATE);//for cifti on-disk writing, replace structure with along row needs to RMW
    } else {
        m_file.open(filename, BinaryFile::WRITE_TRUNCATE);
    }
    m_header = header;
    m_header.write(m_file, version, swapEndian);//the header's getDataOffset() is not what gets written, as it doesn't reflect changes in the extensions
    m_dims = m_header.getDimensions();
}

void NiftiIO::close()
{
    m_file.close();
    m_dims.clear();
}

int NiftiIO::getNumComponents() const
{
    switch (m_header.getDataType())
    {
        case NIFTI_TYPE_RGB24:
            return 3;
            break;
        case NIFTI_TYPE_COMPLEX64:
        case NIFTI_TYPE_COMPLEX128:
        case NIFTI_TYPE_COMPLEX256:
            return 2;
            break;
        case NIFTI_TYPE_INT8:
        case NIFTI_TYPE_UINT8:
        case NIFTI_TYPE_INT16:
        case NIFTI_TYPE_UINT16:
        case NIFTI_TYPE_INT32:
        case NIFTI_TYPE_UINT32:
        case NIFTI_TYPE_FLOAT32:
        case NIFTI_TYPE_INT64:
        case NIFTI_TYPE_UINT64:
        case NIFTI_TYPE_FLOAT64:
        case NIFTI_TYPE_FLOAT128:
            return 1;
            break;
        default:
            CiftiAssert(0);
            throw CiftiException("internal error, report what you did to the developers");
    }
}

int NiftiIO::numBytesPerElem()
{
    switch (m_header.getDataType())
    {
        case NIFTI_TYPE_INT8:
        case NIFTI_TYPE_UINT8:
        case NIFTI_TYPE_RGB24:
            return 1;
            break;
        case NIFTI_TYPE_INT16:
        case NIFTI_TYPE_UINT16:
            return 2;
            break;
        case NIFTI_TYPE_INT32:
        case NIFTI_TYPE_UINT32:
        case NIFTI_TYPE_FLOAT32:
        case NIFTI_TYPE_COMPLEX64:
            return 4;
            break;
        case NIFTI_TYPE_INT64:
        case NIFTI_TYPE_UINT64:
        case NIFTI_TYPE_FLOAT64:
        case NIFTI_TYPE_COMPLEX128:
            return 8;
            break;
        case NIFTI_TYPE_FLOAT128:
        case NIFTI_TYPE_COMPLEX256:
            return 16;
            break;
        default:
            CiftiAssert(0);
            throw CiftiException("internal error, report what you did to the developers");
    }
}
