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

#include "CiftiFile.h"

#include "Common/CiftiAssert.h"
#include "Common/MultiDimArray.h"
#include "NiftiIO.h"

#ifdef CIFTILIB_USE_QT
    #include <QFileInfo>
#else
    //use boost filesystem, because cross-platform filesystem support with POSIX is absurd
    #define BOOST_FILESYSTEM_VERSION 3
    #include "boost/filesystem.hpp"
#endif

#include <iostream>

using namespace std;
using namespace boost;
using namespace cifti;

//private implementation classes, helpers
namespace
{
    class CiftiOnDiskImpl : public CiftiFile::WriteImplInterface
    {
        mutable NiftiIO m_nifti;//because file objects aren't stateless (current position), so reading "changes" them
        CiftiXML m_xml;//because we need to parse it to set up the dimensions anyway
    public:
        CiftiOnDiskImpl(const AString& filename);//read-only
        CiftiOnDiskImpl(const AString& filename, const CiftiXML& xml, const CiftiVersion& version, const bool& swapEndian,
                        const int16_t& datatype, const bool& rescale, const double& minval, const double& maxval);//make new empty file with read/write
        void getRow(float* dataOut, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead) const;
        void getColumn(float* dataOut, const int64_t& index) const;
        const CiftiXML& getCiftiXML() const { return m_xml; }
        AString getFilename() const { return m_nifti.getFilename(); }
        bool isSwapped() const { return m_nifti.getHeader().isSwapped(); }
        void setRow(const float* dataIn, const std::vector<int64_t>& indexSelect);
        void setColumn(const float* dataIn, const int64_t& index);
        void close();
    };
    
    class CiftiMemoryImpl : public CiftiFile::WriteImplInterface
    {
        MultiDimArray<float> m_array;
    public:
        CiftiMemoryImpl(const CiftiXML& xml);
        void getRow(float* dataOut, const std::vector<int64_t>& indexSelect, const bool& tolerateShortRead) const;
        void getColumn(float* dataOut, const int64_t& index) const;
        bool isInMemory() const { return true; }
        void setRow(const float* dataIn, const std::vector<int64_t>& indexSelect);
        void setColumn(const float* dataIn, const int64_t& index);
    };
    
    bool shouldSwap(const CiftiFile::ENDIAN& endian)
    {
        if (ByteSwapping::isBigEndian())
        {
            if (endian == CiftiFile::LITTLE) return true;
        } else {
            if (endian == CiftiFile::BIG) return true;
        }
        return false;//default for all other enum values is to write native endian
    }
    
    bool dontRewrite(const CiftiFile::ENDIAN& endian)
    {
        return (endian == CiftiFile::ANY);
    }
    
    AString pathToAbsolute(const AString& mypath)
    {
#ifdef CIFTILIB_USE_QT
        return QFileInfo(mypath).absoluteFilePath();
#else
#ifdef CIFTILIB_BOOST_NO_FSV3
	return filesystem::complete(AString_to_std_string(mypath)).file_string();
#else
        return filesystem::absolute(AString_to_std_string(mypath)).native();
#endif
#endif
    }
    
    AString pathToCanonical(const AString& mypath)
    {
#ifdef CIFTILIB_USE_QT
        return QFileInfo(mypath).canonicalFilePath();
#else
#ifdef CIFTILIB_BOOST_NO_FSV3
        return filesystem::complete(AString_to_std_string(mypath)).file_string();
#else
#ifdef CIFTILIB_BOOST_NO_CANONICAL
        filesystem::path temp = AString_to_std_string(mypath);
        if (!filesystem::exists(temp)) return "";
        return absolute(temp).normalize().native();
#else
        string temp = AString_to_std_string(mypath);
        if (!filesystem::exists(temp)) return "";
        return filesystem::canonical(temp).native();
#endif
#endif
#endif
    }
}

CiftiFile::ReadImplInterface::~ReadImplInterface()
{
}

CiftiFile::WriteImplInterface::~WriteImplInterface()
{
}

CiftiFile::CiftiFile()
{
    m_endianPref = NATIVE;
    setWritingDataTypeNoScaling();//default argument is float32
}

CiftiFile::CiftiFile(const AString& fileName)
{
    m_endianPref = NATIVE;
    setWritingDataTypeNoScaling();//default argument is float32
    openFile(fileName);
}

void CiftiFile::openFile(const AString& fileName)
{
    close();//to make sure it closes everything first, even if the open throws
    boost::shared_ptr<CiftiOnDiskImpl> newRead(new CiftiOnDiskImpl(pathToAbsolute(fileName)));//this constructor opens existing file read-only
    m_readingImpl = newRead;//it should be noted that if the constructor throws (if the file isn't readable), new guarantees the memory allocated for the object will be freed
    m_xml = newRead->getCiftiXML();
    m_dims = m_xml.getDimensions();
    m_onDiskVersion = m_xml.getParsedVersion();
}

void CiftiFile::setWritingFile(const AString& fileName, const CiftiVersion& writingVersion, const ENDIAN& endian)
{
    m_writingFile = pathToAbsolute(fileName);//always resolve paths as soon as they enter CiftiFile, in case some clown changes directory before writing data
    m_writingImpl.reset();//prevent writing to previous writing implementation, let the next set...() set up for writing
    m_onDiskVersion = writingVersion;
    m_endianPref = endian;
}

void CiftiFile::setWritingDataTypeNoScaling(const int16_t& type)
{
    m_writingDataType = type;//could do some validation here
    m_doWriteScaling = false;
    m_minScalingVal = -1.0;//these scaling values should never be used, but don't leave them uninitialized
    m_maxScalingVal = 1.0;
    m_writingImpl.reset();//prevent writing to previous writing implementation, let the next set...() set up for writing
}

void CiftiFile::setWritingDataTypeAndScaling(const int16_t& type, const double& minval, const double& maxval)
{
    m_writingDataType = type;//could do some validation here
    m_doWriteScaling = true;
    m_minScalingVal = minval;
    m_maxScalingVal = maxval;
    m_writingImpl.reset();//prevent writing to previous writing implementation, let the next set...() set up for writing
}

void CiftiFile::writeFile(const AString& fileName, const CiftiVersion& writingVersion, const ENDIAN& endian)
{
    if (m_readingImpl == NULL || m_dims.empty()) throw CiftiException("writeFile called on uninitialized CiftiFile");
    bool writeSwapped = shouldSwap(endian);
    AString canonicalFilename = pathToCanonical(fileName);//NOTE: returns EMPTY STRING for nonexistent file
    const CiftiOnDiskImpl* testImpl = dynamic_cast<CiftiOnDiskImpl*>(m_readingImpl.get());
    bool collision = false, hadWriter = (m_writingImpl != NULL);
    if (testImpl != NULL && canonicalFilename != "" && pathToCanonical(testImpl->getFilename()) == canonicalFilename)
    {//empty string test is so that we don't say collision if both are nonexistent - could happen if file is removed/unlinked while reading on some filesystems
        if (m_onDiskVersion == writingVersion && (dontRewrite(endian) || writeSwapped == testImpl->isSwapped())) return;//don't need to copy to itself
        collision = true;//we need to copy to memory temporarily
        boost::shared_ptr<WriteImplInterface> tempMemory(new CiftiMemoryImpl(m_xml));//because tempRead is a ReadImpl, can't be used to copy to
        copyImplData(m_readingImpl.get(), tempMemory.get(), m_dims);
        m_readingImpl = tempMemory;//we are about to make the old reading impl very unhappy, replace it so that if we get an error while writing, we hang onto the memory version
        m_writingImpl.reset();//and make it re-magic the writing implementation again if it tries to write again
    }
    boost::shared_ptr<WriteImplInterface> tempWrite(new CiftiOnDiskImpl(pathToAbsolute(fileName), m_xml, writingVersion, writeSwapped,
                                                                        m_writingDataType, m_doWriteScaling, m_minScalingVal, m_maxScalingVal));
    copyImplData(m_readingImpl.get(), tempWrite.get(), m_dims);
    if (collision)//if we rewrote the file, we need the handle to the new file, and to dump the temporary in-memory version
    {
        m_onDiskVersion = writingVersion;//also record the current version number
        m_readingImpl = tempWrite;//replace the temporary memory version
        if (hadWriter)//if it was in read-write mode
        {
            m_writingImpl = tempWrite;//set the writer too
        }
    }
}

void CiftiFile::close()
{
    if (m_writingImpl != NULL)
    {
        m_writingImpl->close();//only writing implementations should ever throw errors on close, and specifically only on-disk
    }
    m_writingImpl.reset();
    m_readingImpl.reset();
    m_dims.clear();
    m_xml = CiftiXML();
    m_writingFile = "";
    m_onDiskVersion = CiftiVersion();//for completeness, it gets reset on open anyway
    m_endianPref = NATIVE;//reset things to defaults
    setWritingDataTypeNoScaling();//default argument is float32
}

void CiftiFile::convertToInMemory()
{
    if (isInMemory()) return;
    if (m_readingImpl == NULL || m_dims.empty())//not set up yet
    {
        m_writingFile = "";//make sure it doesn't do on-disk when set...() is called
        return;
    }
    boost::shared_ptr<WriteImplInterface> tempWrite(new CiftiMemoryImpl(m_xml));//if we get an error while reading, free the memory immediately, and don't leave m_readingImpl and m_writingImpl pointing to different things
    copyImplData(m_readingImpl.get(), tempWrite.get(), m_dims);
    m_writingImpl = tempWrite;
    m_readingImpl = tempWrite;
}

bool CiftiFile::isInMemory() const
{
    if (m_readingImpl == NULL)
    {
        return (m_writingFile == "");//return what it would be if verifyWriteImpl() was called
    } else {
        return m_readingImpl->isInMemory();
    }
}

void CiftiFile::getRow(float* dataOut, const vector<int64_t>& indexSelect, const bool& tolerateShortRead) const
{
    if (m_dims.empty()) throw CiftiException("getRow called on uninitialized CiftiFile");
    if (m_readingImpl == NULL) return;//NOT an error because we are pretending to have a matrix already, while we are waiting for setRow to actually start writing the file
    m_readingImpl->getRow(dataOut, indexSelect, tolerateShortRead);
}

void CiftiFile::getColumn(float* dataOut, const int64_t& index) const
{
    if (m_dims.empty()) throw CiftiException("getColumn called on uninitialized CiftiFile");
    if (m_dims.size() != 2) throw CiftiException("getColumn called on non-2D CiftiFile");
    if (m_readingImpl == NULL) return;//NOT an error because we are pretending to have a matrix already, while we are waiting for setRow to actually start writing the file
    m_readingImpl->getColumn(dataOut, index);
}

void CiftiFile::setCiftiXML(const CiftiXML& xml, const bool useOldMetadata)
{
    if (xml.getNumberOfDimensions() == 0) throw CiftiException("setCiftiXML called with 0-dimensional CiftiXML");
    vector<int64_t> xmlDims = xml.getDimensions();
    for (size_t i = 0; i < xmlDims.size(); ++i)
    {
        if (xmlDims[i] < 1) throw CiftiException("cifti xml dimensions must be greater than zero");
    }
    m_readingImpl.reset();//drop old matrix/file, as it is now invalid due to XML (and therefore matrix size) change
    m_writingImpl.reset();
    if (useOldMetadata)
    {
        MetaData newmd = m_xml.getFileMetaData();//make a copy
        m_xml = xml;//because this will overwrite the metadata
        m_xml.setFileMetaData(newmd);
    } else {
        m_xml = xml;
    }
    m_dims = xmlDims;
}

void CiftiFile::setRow(const float* dataIn, const vector<int64_t>& indexSelect)
{
    verifyWriteImpl();
    m_writingImpl->setRow(dataIn, indexSelect);
}

void CiftiFile::setColumn(const float* dataIn, const int64_t& index)
{
    verifyWriteImpl();
    if (m_dims.size() != 2) throw CiftiException("setColumn called on non-2D CiftiFile");
    m_writingImpl->setColumn(dataIn, index);
}

//single-index functions
void CiftiFile::getRow(float* dataOut, const int64_t& index, const bool& tolerateShortRead) const
{
    if (m_dims.empty()) throw CiftiException("getRow called on uninitialized CiftiFile");
    if (m_dims.size() != 2) throw CiftiException("getRow with single index called on non-2D CiftiFile");
    if (m_readingImpl == NULL) return;//NOT an error because we are pretending to have a matrix already, while we are waiting for setRow to actually start writing the file
    vector<int64_t> tempvec(1, index);//could use a member if we need more speed
    m_readingImpl->getRow(dataOut, tempvec, tolerateShortRead);
}

void CiftiFile::setRow(const float* dataIn, const int64_t& index)
{
    verifyWriteImpl();
    if (m_dims.size() != 2) throw CiftiException("setRow with single index called on non-2D CiftiFile");
    vector<int64_t> tempvec(1, index);//could use a member if we need more speed
    m_writingImpl->setRow(dataIn, tempvec);
}
//*///end single-index functions

void CiftiFile::verifyWriteImpl()
{//this is where the magic happens - we want to emulate being a simple in-memory file, but actually be reading/writing on-disk when possible
    if (m_writingImpl != NULL) return;
    CiftiAssert(!m_dims.empty());//if the xml hasn't been set, then we can't do anything meaningful
    if (m_dims.empty()) throw CiftiException("setRow or setColumn attempted on uninitialized CiftiFile");
    if (m_writingFile == "")
    {
        if (m_readingImpl != NULL)
        {
            convertToInMemory();
        } else {
            m_writingImpl = boost::shared_ptr<CiftiMemoryImpl>(new CiftiMemoryImpl(m_xml));
        }
    } else {//NOTE: m_onDiskVersion gets set in setWritingFile
        if (m_readingImpl != NULL)
        {
            CiftiOnDiskImpl* testImpl = dynamic_cast<CiftiOnDiskImpl*>(m_readingImpl.get());
            if (testImpl != NULL)
            {
                AString canonicalCurrent = pathToCanonical(testImpl->getFilename());//returns "" if nonexistent, if unlinked while open
                if (canonicalCurrent != "" && canonicalCurrent == pathToCanonical(m_writingFile))//these were already absolute
                {
                    convertToInMemory();//save existing data in memory before we clobber file
                }
            }
        }
        m_writingImpl = boost::shared_ptr<CiftiOnDiskImpl>(new CiftiOnDiskImpl(m_writingFile, m_xml, m_onDiskVersion, shouldSwap(m_endianPref),
                                                                               m_writingDataType, m_doWriteScaling, m_minScalingVal, m_maxScalingVal));//this constructor makes new file for writing
        if (m_readingImpl != NULL)
        {
            copyImplData(m_readingImpl.get(), m_writingImpl.get(), m_dims);
        }
    }
    m_readingImpl = m_writingImpl;//read-only implementations are set up in specialized functions
}

void CiftiFile::copyImplData(const ReadImplInterface* from, WriteImplInterface* to, const vector<int64_t>& dims)
{
    vector<int64_t> iterateDims(dims.begin() + 1, dims.end());
    vector<float> scratchRow(dims[0]);
    for (MultiDimIterator<int64_t> iter(iterateDims); !iter.atEnd(); ++iter)
    {
        from->getRow(scratchRow.data(), *iter, false);
        to->setRow(scratchRow.data(), *iter);
    }
}

CiftiMemoryImpl::CiftiMemoryImpl(const CiftiXML& xml)
{
    CiftiAssert(xml.getNumberOfDimensions() != 0);
    m_array.resize(xml.getDimensions());
}

void CiftiMemoryImpl::getRow(float* dataOut, const vector<int64_t>& indexSelect, const bool&) const
{
    const float* ref = m_array.get(1, indexSelect);
    int64_t rowSize = m_array.getDimensions()[0];//we don't accept 0-D CiftiXML, so this will always work
    for (int64_t i = 0; i < rowSize; ++i)
    {
        dataOut[i] = ref[i];
    }
}

void CiftiMemoryImpl::getColumn(float* dataOut, const int64_t& index) const
{
    CiftiAssert(m_array.getDimensions().size() == 2);//otherwise, CiftiFile shouldn't have called this
    const float* ref = m_array.get(2, vector<int64_t>());//empty vector is intentional, only 2 dimensions exist, so no more to select from
    int64_t rowSize = m_array.getDimensions()[0];
    int64_t colSize = m_array.getDimensions()[1];
    CiftiAssert(index >= 0 && index < rowSize);//because we are doing the indexing math manually for speed
    for (int64_t i = 0; i < colSize; ++i)
    {
        dataOut[i] = ref[index + rowSize * i];
    }
}

void CiftiMemoryImpl::setRow(const float* dataIn, const vector<int64_t>& indexSelect)
{
    float* ref = m_array.get(1, indexSelect);
    int64_t rowSize = m_array.getDimensions()[0];//we don't accept 0-D CiftiXML, so this will always work
    for (int64_t i = 0; i < rowSize; ++i)
    {
        ref[i] = dataIn[i];
    }
}

void CiftiMemoryImpl::setColumn(const float* dataIn, const int64_t& index)
{
    CiftiAssert(m_array.getDimensions().size() == 2);//otherwise, CiftiFile shouldn't have called this
    float* ref = m_array.get(2, vector<int64_t>());//empty vector is intentional, only 2 dimensions exist, so no more to select from
    int64_t rowSize = m_array.getDimensions()[0];
    int64_t colSize = m_array.getDimensions()[1];
    CiftiAssert(index >= 0 && index < rowSize);//because we are doing the indexing math manually for speed
    for (int64_t i = 0; i < colSize; ++i)
    {
        ref[index + rowSize * i] = dataIn[i];
    }
}

CiftiOnDiskImpl::CiftiOnDiskImpl(const AString& filename)
{//opens existing file for reading
    m_nifti.openRead(filename);//read-only, so we don't need write permission to read a cifti file
    if (m_nifti.getNumComponents() != 1) throw CiftiException("complex or rgb datatype found in file '" + filename + "', these are not supported in cifti");
    const NiftiHeader& myHeader = m_nifti.getHeader();
    int numExts = (int)myHeader.m_extensions.size(), whichExt = -1;
    for (int i = 0; i < numExts; ++i)
    {
        if (myHeader.m_extensions[i]->m_ecode == NIFTI_ECODE_CIFTI)
        {
            whichExt = i;
            break;
        }
    }
    if (whichExt == -1) throw CiftiException("no cifti extension found in file '" + filename + "'");
    m_xml.readXML(myHeader.m_extensions[whichExt]->m_bytes);
    vector<int64_t> dimCheck = m_nifti.getDimensions();
    if (dimCheck.size() < 5) throw CiftiException("invalid dimensions in cifti file '" + filename + "'");
    for (int i = 0; i < 4; ++i)
    {
        if (dimCheck[i] != 1) throw CiftiException("non-singular dimension #" + AString_number(i + 1) + " in cifti file '" + filename + "'");
    }
    if (m_xml.getParsedVersion().hasReversedFirstDims())
    {
        while (dimCheck.size() < 6) dimCheck.push_back(1);//just in case
        int64_t temp = dimCheck[4];//note: nifti dim[5] is the 5th dimension, index 4 in this vector
        dimCheck[4] = dimCheck[5];
        dimCheck[5] = temp;
        m_nifti.overrideDimensions(dimCheck);
    }
    if (m_xml.getNumberOfDimensions() + 4 != (int)dimCheck.size()) throw CiftiException("XML does not match number of nifti dimensions in file " + filename + "'");
    for (int i = 4; i < (int)dimCheck.size(); ++i)
    {
        if (m_xml.getDimensionLength(i - 4) < 0)//CiftiXML will only let this happen with cifti-1
        {
            m_xml.getSeriesMap(i - 4).setLength(dimCheck[i]);//and only in a series map
        } else {
            if (m_xml.getDimensionLength(i - 4) != dimCheck[i])
            {
                throw CiftiException("xml and nifti header disagree on matrix dimensions");
            }
        }
    }
}

namespace
{
    void warnForBadExtension(const AString& filename, const CiftiXML& myXML)
    {
        char junk[16];
        int32_t intent_code = myXML.getIntentInfo(CiftiVersion(), junk);//use default writing version to check file extension, older version is missing some intent codes
        switch (intent_code)
        {
            default:
                cerr << "warning: unhandled cifti type in extension warning check, tell the developers what you just tried to do" << endl;
                CiftiAssert(0);//yes, let it fall through to "unknown" in release so that it at least looks for .nii
                //-fallthrough
            case 3000://unknown
                if (!AString_endsWith(filename, ".nii"))
                {
                    cerr << "warning: cifti file of nonstandard mapping combination '" << AString_to_std_string(filename) << "' should be saved ending in .<something>.nii" << endl;
                }
                if (AString_endsWith(filename, ".dconn.nii") ||
                    AString_endsWith(filename, ".dtseries.nii") ||
                    AString_endsWith(filename, ".pconn.nii") ||
                    AString_endsWith(filename, ".ptseries.nii") ||
                    AString_endsWith(filename, ".dscalar.nii") ||
                    AString_endsWith(filename, ".dfan.nii") ||
                    AString_endsWith(filename, ".fiberTemp.nii") ||
                    AString_endsWith(filename, ".dlabel.nii") ||
                    AString_endsWith(filename, ".pscalar.nii") ||
                    AString_endsWith(filename, ".pdconn.nii") ||
                    AString_endsWith(filename, ".dpconn.nii") ||
                    AString_endsWith(filename, ".pconnseries.nii") ||
                    AString_endsWith(filename, ".pconnscalar.nii"))
                {
                    cerr << "warning: cifti file of nonstandard mapping combination '" << AString_to_std_string(filename) << "' should NOT be saved using an already-used cifti extension, "
                            << "please choose a different, reasonable cifti extension of the form .<something>.nii" << endl;
                }
                break;
            case 3001:
                if (!AString_endsWith(filename, ".dconn.nii"))
                {
                    cerr << "warning: dense by dense cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .dconn.nii" << endl;
                }
                break;
            case 3002:
                if (!AString_endsWith(filename, ".dtseries.nii"))
                {
                    cerr << "warning: series by dense cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .dtseries.nii" << endl;
                }
                break;
            case 3003:
                if (!AString_endsWith(filename, ".pconn.nii"))
                {
                    cerr << "warning: parcels by parcels cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .pconn.nii" << endl;
                }
                break;
            case 3004:
                if (!AString_endsWith(filename, ".ptseries.nii"))
                {
                    cerr << "warning: series by parcels cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .ptseries.nii" << endl;
                }
                break;
            case 3006://3005 unused in practice
                if (!(AString_endsWith(filename, ".dscalar.nii") || AString_endsWith(filename, ".dfan.nii") || AString_endsWith(filename, ".fiberTEMP.nii")))
                {//there are additional special extensions in the standard for this mapping combination (specializations of scalar maps)
                    //also include workbench's fiberTEMP special extension
                    cerr << "warning: scalars by dense cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .dscalar.nii" << endl;
                }
                break;
            case 3007:
                if (!AString_endsWith(filename, ".dlabel.nii"))
                {
                    cerr << "warning: labels by dense cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .dlabel.nii" << endl;
                }
                break;
            case 3008:
                if (!AString_endsWith(filename, ".pscalar.nii"))
                {
                    cerr << "warning: scalars by parcels cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .pscalar.nii" << endl;
                }
                break;
            case 3009:
                if (!AString_endsWith(filename, ".pdconn.nii"))
                {
                    cerr << "warning: dense by parcels cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .pdconn.nii" << endl;
                }
                break;
            case 3010:
                if (!AString_endsWith(filename, ".dpconn.nii"))
                {
                    cerr << "warning: parcels by dense cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .dpconn.nii" << endl;
                }
                break;
            case 3011:
                if (!AString_endsWith(filename, ".pconnseries.nii"))
                {
                    cerr << "warning: parcels by parcels by series cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .pconnseries.nii" << endl;
                }
                break;
            case 3012:
                if (!AString_endsWith(filename, ".pconnscalar.nii"))
                {
                    cerr << "warning: parcels by parcels by scalar cifti file '" << AString_to_std_string(filename) << "' should be saved ending in .pconnscalar.nii" << endl;
                }
                break;
        }
    }
}

CiftiOnDiskImpl::CiftiOnDiskImpl(const AString& filename, const CiftiXML& xml, const CiftiVersion& version, const bool& swapEndian,
                                 const int16_t& datatype, const bool& rescale, const double& minval, const double& maxval)
{//starts writing new file
    warnForBadExtension(filename, xml);
    NiftiHeader outHeader;
    if (rescale)
    {
        outHeader.setDataTypeAndScaleRange(datatype, minval, maxval);
    } else {
        outHeader.setDataType(datatype);
    }
    if (outHeader.getNumComponents() != 1)
    {
        throw CiftiException("cifti cannot be written with multi-component nifti datatypes (i.e., complex, RGB)");
    }
    char intentName[16];
    int32_t intentCode = xml.getIntentInfo(version, intentName);
    outHeader.setIntent(intentCode, intentName);
    boost::shared_ptr<NiftiExtension> outExtension(new NiftiExtension());
    outExtension->m_ecode = NIFTI_ECODE_CIFTI;
    outExtension->m_bytes = xml.writeXMLToVector(version);
    outHeader.m_extensions.push_back(outExtension);
    vector<int64_t> matrixDims = xml.getDimensions();
    vector<int64_t> niftiDims(4, 1);//the reserved space and time dims
    niftiDims.insert(niftiDims.end(), matrixDims.begin(), matrixDims.end());
    if (version.hasReversedFirstDims())
    {
        vector<int64_t> headerDims = niftiDims;
        while (headerDims.size() < 6) headerDims.push_back(1);//just in case
        int64_t temp = headerDims[4];
        headerDims[4] = headerDims[5];
        headerDims[5] = temp;
        outHeader.setDimensions(headerDims);//give the header the reversed dimensions
        m_nifti.writeNew(filename, outHeader, 2, true, swapEndian);
        m_nifti.overrideDimensions(niftiDims);//and then tell the nifti reader to use the correct dimensions
    } else {
        outHeader.setDimensions(niftiDims);
        m_nifti.writeNew(filename, outHeader, 2, true, swapEndian);
    }
    m_xml = xml;
}

void CiftiOnDiskImpl::close()
{
    m_nifti.close();//lets this throw when there is a writing problem
}//don't bother resetting m_xml, this instance is about to be destroyed

void CiftiOnDiskImpl::getRow(float* dataOut, const vector<int64_t>& indexSelect, const bool& tolerateShortRead) const
{
    m_nifti.readData(dataOut, 5, indexSelect, tolerateShortRead);//5 means 4 reserved (space and time) plus the first cifti dimension
}

void CiftiOnDiskImpl::getColumn(float* dataOut, const int64_t& index) const
{
    CiftiAssert(m_xml.getNumberOfDimensions() == 2);//otherwise this shouldn't be called
    CiftiAssert(index >= 0 && index < m_xml.getDimensionLength(CiftiXML::ALONG_ROW));
    vector<int64_t> indexSelect(2);
    indexSelect[0] = index;
    int64_t colLength = m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN);
    for (int64_t i = 0; i < colLength; ++i)//assume if they really want getColumn on disk, they don't want their pagecache obliterated, so read it 1 element at a time
    {
        indexSelect[1] = i;
        m_nifti.readData(dataOut + i, 4, indexSelect);//4 means just the 4 reserved dimensions, so 1 element of the matrix
    }
}

void CiftiOnDiskImpl::setRow(const float* dataIn, const vector<int64_t>& indexSelect)
{
    m_nifti.writeData(dataIn, 5, indexSelect);
}

void CiftiOnDiskImpl::setColumn(const float* dataIn, const int64_t& index)
{
    CiftiAssert(m_xml.getNumberOfDimensions() == 2);//otherwise this shouldn't be called
    CiftiAssert(index >= 0 && index < m_xml.getDimensionLength(CiftiXML::ALONG_ROW));
    vector<int64_t> indexSelect(2);
    indexSelect[0] = index;
    int64_t colLength = m_xml.getDimensionLength(CiftiXML::ALONG_COLUMN);
    for (int64_t i = 0; i < colLength; ++i)//don't do RMW, so write it 1 element at a time
    {
        indexSelect[1] = i;
        m_nifti.writeData(dataIn + i, 4, indexSelect);//4 means just the 4 reserved dimensions, so 1 element of the matrix
    }
}
