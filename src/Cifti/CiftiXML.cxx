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

#include "CiftiXML.h"

#include "Common/CiftiException.h"
#include "Common/XmlAdapter.h"

#include <cassert>
#include <iostream>
#include <set>

using namespace std;
using namespace boost;
using namespace cifti;

CiftiXML::CiftiXML(const CiftiXML& rhs)
{
    copyHelper(rhs);
}

CiftiXML& CiftiXML::operator=(const CiftiXML& rhs)
{
    if (this != &rhs) copyHelper(rhs);
    return *this;
}

void CiftiXML::copyHelper(const CiftiXML& rhs)
{
    int numDims = (int)rhs.m_indexMaps.size();
    m_indexMaps.resize(numDims);
    for (int i = 0; i < numDims; ++i)
    {
        m_indexMaps[i] = boost::shared_ptr<CiftiMappingType>(rhs.m_indexMaps[i]->clone());
    }
    m_parsedVersion = rhs.m_parsedVersion;
    m_fileMetaData = rhs.m_fileMetaData;
}

bool CiftiXML::operator==(const CiftiXML& rhs) const
{
    int numDims = getNumberOfDimensions();
    if (rhs.getNumberOfDimensions() != numDims) return false;
    if (m_fileMetaData != rhs.m_fileMetaData) return false;
    for (int i = 0; i < numDims; ++i)
    {
        const CiftiMappingType* left = getMap(i), *right = rhs.getMap(i);
        if (left == NULL && right == NULL) continue;
        if (left == NULL || right == NULL) return false;//only one NULL, due to above test
        if ((*left) != (*right)) return false;//finally can dereference them
    }
    return true;
}

bool CiftiXML::approximateMatch(const CiftiXML& rhs) const
{
    int numDims = getNumberOfDimensions();
    if (rhs.getNumberOfDimensions() != numDims) return false;
    for (int i = 0; i < numDims; ++i)
    {
        const CiftiMappingType* left = getMap(i), *right = rhs.getMap(i);
        if (left == NULL && right == NULL) continue;
        if (left == NULL || right == NULL) return false;//only one NULL, due to above test
        if (!left->approximateMatch(*right)) return false;//finally can dereference them
    }
    return true;
}

const CiftiMappingType* CiftiXML::getMap(const int& direction) const
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    return m_indexMaps[direction].get();
}

CiftiMappingType* CiftiXML::getMap(const int& direction)
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    return m_indexMaps[direction].get();
}

const MetaData& CiftiXML::getFileMetaData() const
{
    return m_fileMetaData;
}

const CiftiBrainModelsMap& CiftiXML::getBrainModelsMap(const int& direction) const
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(getMappingType(direction) == CiftiMappingType::BRAIN_MODELS);//assert so we catch it in debug
    return dynamic_cast<const CiftiBrainModelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

CiftiBrainModelsMap& CiftiXML::getBrainModelsMap(const int& direction)
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(getMappingType(direction) == CiftiMappingType::BRAIN_MODELS);//assert so we catch it in debug
    return dynamic_cast<CiftiBrainModelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiLabelsMap& CiftiXML::getLabelsMap(const int& direction) const
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(getMappingType(direction) == CiftiMappingType::LABELS);//assert so we catch it in debug
    return dynamic_cast<const CiftiLabelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

CiftiLabelsMap& CiftiXML::getLabelsMap(const int& direction)
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(getMappingType(direction) == CiftiMappingType::LABELS);//assert so we catch it in debug
    return dynamic_cast<CiftiLabelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiParcelsMap& CiftiXML::getParcelsMap(const int& direction) const
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(getMappingType(direction) == CiftiMappingType::PARCELS);//assert so we catch it in debug
    return dynamic_cast<const CiftiParcelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

CiftiParcelsMap& CiftiXML::getParcelsMap(const int& direction)
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(getMappingType(direction) == CiftiMappingType::PARCELS);//assert so we catch it in debug
    return dynamic_cast<CiftiParcelsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiScalarsMap& CiftiXML::getScalarsMap(const int& direction) const
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(getMappingType(direction) == CiftiMappingType::SCALARS);//assert so we catch it in debug
    return dynamic_cast<const CiftiScalarsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

CiftiScalarsMap& CiftiXML::getScalarsMap(const int& direction)
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(getMappingType(direction) == CiftiMappingType::SCALARS);//assert so we catch it in debug
    return dynamic_cast<CiftiScalarsMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

const CiftiSeriesMap& CiftiXML::getSeriesMap(const int& direction) const
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(getMappingType(direction) == CiftiMappingType::SERIES);//assert so we catch it in debug
    return dynamic_cast<const CiftiSeriesMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

CiftiSeriesMap& CiftiXML::getSeriesMap(const int& direction)
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(getMappingType(direction) == CiftiMappingType::SERIES);//assert so we catch it in debug
    return dynamic_cast<CiftiSeriesMap&>(*getMap(direction));//let release throw bad_cast or segfault
}

int64_t CiftiXML::getDimensionLength(const int& direction) const
{
    const CiftiMappingType* tempMap = getMap(direction);
    CiftiAssert(tempMap != NULL);
    return tempMap->getLength();
}

vector<int64_t> CiftiXML::getDimensions() const
{
    vector<int64_t> ret(getNumberOfDimensions());
    for (int i = 0; i < (int)ret.size(); ++i)
    {
        ret[i] = getDimensionLength(i);
    }
    return ret;
}

CiftiMappingType::MappingType CiftiXML::getMappingType(const int& direction) const
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    CiftiAssert(m_indexMaps[direction] != NULL);
    return m_indexMaps[direction]->getType();
}

void CiftiXML::setMap(const int& direction, const CiftiMappingType& mapIn)
{
    CiftiAssertVectorIndex(m_indexMaps, direction);
    if (mapIn.getType() == CiftiMappingType::LABELS)
    {
        for (int i = 0; i < getNumberOfDimensions(); ++i)
        {
            if (i != direction && m_indexMaps[i] != NULL && m_indexMaps[i]->getType() == CiftiMappingType::LABELS)
            {
                throw CiftiException("Cifti XML cannot contain more than one label mapping");
            }
        }
    }
    m_indexMaps[direction] = boost::shared_ptr<CiftiMappingType>(mapIn.clone());
}

void CiftiXML::setNumberOfDimensions(const int& num)
{
    m_indexMaps.resize(num);
}

void CiftiXML::clear()
{
    setNumberOfDimensions(0);
    m_fileMetaData.clear();
    m_parsedVersion = CiftiVersion();
}

void CiftiXML::readXML(const vector<char>& text)
{
#ifdef CIFTILIB_USE_QT
    vector<char> text2 = text;
    text2.push_back('\0');//make sure it has a null terminator
    XmlReader xml(text2.data());//so it works as a C string
#else
#ifdef CIFTILIB_USE_XMLPP
    vector<char>::const_iterator end = find(text.begin(), text.end(), '\0');//find the null terminator, if it exists, to prevent "extra content at end of document" errors
    XmlReader xml((unsigned char*)text.data(), end - text.begin());//get the number of bytes
#else
#error "not implemented"
#endif
#endif
    readXML(xml);
}

int32_t CiftiXML::getIntentInfo(const CiftiVersion& writingVersion, char intentNameOut[16]) const
{
    int32_t ret;
    const char* name = NULL;
    if (writingVersion == CiftiVersion(1, 0))//cifti-1: unknown didn't exist, and "ConnDense" was default in workbench
    {
        ret = 3001;//default
        name = "ConnDense";
        if (getNumberOfDimensions() > 0 && getMappingType(0) == CiftiMappingType::SERIES) { ret = 3002; name = "ConnDenseTime"; }//same logic as was used in workbench
        if (getNumberOfDimensions() > 1 && getMappingType(1) == CiftiMappingType::SERIES) { ret = 3002; name = "ConnDenseTime"; }//NOTE: name for this code is different than cifti-2
    } else if (writingVersion == CiftiVersion(1, 1) || writingVersion == CiftiVersion(2, 0)) {//cifti-2
        ret = 3000;//default
        name = "ConnUnknown";
        switch (getNumberOfDimensions())
        {
            case 2:
            {
                CiftiMappingType::MappingType first = getMappingType(0), second = getMappingType(1);
                if (first == CiftiMappingType::BRAIN_MODELS && second == CiftiMappingType::BRAIN_MODELS) { ret = 3001; name = "ConnDense"; }
                if (first == CiftiMappingType::SERIES && second == CiftiMappingType::BRAIN_MODELS) { ret = 3002; name = "ConnDenseSeries"; }
                if (first == CiftiMappingType::PARCELS && second == CiftiMappingType::PARCELS) { ret = 3003; name = "ConnParcels"; }
                if (first == CiftiMappingType::SERIES && second == CiftiMappingType::PARCELS) { ret = 3004; name = "ConnParcelSries"; }//NOTE: 3005 is reserved but not used
                if (first == CiftiMappingType::SCALARS && second == CiftiMappingType::BRAIN_MODELS) { ret = 3006; name = "ConnDenseScalar"; }
                if (first == CiftiMappingType::LABELS && second == CiftiMappingType::BRAIN_MODELS) { ret = 3007; name = "ConnDenseLabel"; }
                if (first == CiftiMappingType::SCALARS && second == CiftiMappingType::PARCELS) { ret = 3008; name = "ConnParcelScalr"; }
                if (first == CiftiMappingType::BRAIN_MODELS && second == CiftiMappingType::PARCELS) { ret = 3009; name = "ConnParcelDense"; }
                if (first == CiftiMappingType::PARCELS && second == CiftiMappingType::BRAIN_MODELS) { ret = 3010; name = "ConnDenseParcel"; }
                break;
            }
            case 3:
            {
                CiftiMappingType::MappingType first = getMappingType(0), second = getMappingType(1), third = getMappingType(2);
                if (first == CiftiMappingType::PARCELS && second == CiftiMappingType::PARCELS && third == CiftiMappingType::SERIES) { ret = 3011; name = "ConnPPSr"; }
                if (first == CiftiMappingType::PARCELS && second == CiftiMappingType::PARCELS && third == CiftiMappingType::SCALARS) { ret = 3012; name = "ConnPPSc"; }
                break;
            }
            default:
                break;
        }
    } else {
        throw CiftiException("unknown cifti version: " + writingVersion.toString());
    }
    int i;
    for (i = 0; i < 16 && name[i] != '\0'; ++i) intentNameOut[i] = name[i];
    for (; i < 16; ++i) intentNameOut[i] = '\0';
    return ret;
}

void CiftiXML::readXML(XmlReader& xml)
{
    clear();
    bool haveCifti = false;
    try
    {
#ifdef CIFTILIB_USE_QT
        for (; !xml.atEnd(); xml.readNext())
        {
            if (xml.isStartElement())
            {
                QStringRef name = xml.name();
                if (name == "CIFTI")
                {
                    if (haveCifti)
                    {
                        throw CiftiException("CIFTI element may only be specified once");
                    }
                    QXmlStreamAttributes attributes = xml.attributes();
                    if(attributes.hasAttribute("Version"))
                    {
                        m_parsedVersion = CiftiVersion(attributes.value("Version").toString());
                    } else {
                        throw CiftiException("Cifti XML missing Version attribute.");
                    }
                    if (m_parsedVersion == CiftiVersion(1, 0))//switch/case on major/minor would be much harder to read
                    {
                        parseCIFTI1(xml);
                        if (xml.hasError()) break;
                    } else if (m_parsedVersion == CiftiVersion(2, 0)) {
                        parseCIFTI2(xml);
                        if (xml.hasError()) break;
                    } else {
                        throw CiftiException("unknown Cifti Version: '" + m_parsedVersion.toString());
                    }
                    haveCifti = true;
                } else {
                    throw CiftiException("unexpected root element in Cifti XML: " + name.toString());
                }
            }
        }
        if (!xml.hasError() && !haveCifti)
        {
            throw CiftiException("CIFTI element not found");
        }
#else
#ifdef CIFTILIB_USE_XMLPP
        while (true)//don't want to skip the first element, but there is no obvious function for "at end"
        {
            switch (xml.get_node_type())
            {
                case XmlReader::Element:
                {
                    AString name = xml.get_local_name();
                    if (name == "CIFTI")
                    {
                        if (haveCifti)
                        {
                            throw CiftiException("CIFTI element may only be specified once");
                        }
                        vector<AString> mandAttrs(1, "Version");
                        XmlAttributesResult attrVals = XmlReader_parseAttributes(xml, mandAttrs);
                        m_parsedVersion = CiftiVersion(attrVals.mandatoryVals[0]);
                        if (m_parsedVersion == CiftiVersion(1, 0))//switch/case on major/minor would be much harder to read
                        {
                            parseCIFTI1(xml);
                        } else if (m_parsedVersion == CiftiVersion(2, 0)) {
                            parseCIFTI2(xml);
                        } else {
                            throw CiftiException("unknown Cifti Version: '" + m_parsedVersion.toString());
                        }
                        haveCifti = true;
                    } else {
                        throw CiftiException("unexpected root element in Cifti XML: " + name);
                    }
                }
                default:
                    break;
            }
            if (!xml.read()) break;
        }
#else
#error "not implemented"
#endif
#endif
    } catch (CiftiException& e) {
        throw CiftiException("Cifti XML error: " + e.whatString());//so we can throw on error instead of doing a bunch of dancing with xml.raiseError and xml.hasError
    } catch (std::exception& e) {//libxml++ throws things that inherit from std::exception, catch them too
        throw CiftiException("Cifti XML error: " + AString(e.what()));
    }
#ifdef CIFTILIB_USE_QT
    if(xml.hasError())
    {
        throw CiftiException("Cifti XML error: " + xml.errorString());
    }
#endif
}

void CiftiXML::parseCIFTI1(XmlReader& xml)
{
    bool haveMatrix = false;
#ifdef CIFTILIB_USE_QT
    QXmlStreamAttributes attributes = xml.attributes();
    if (attributes.hasAttribute("NumberOfMatrices"))
    {
        if (attributes.value("NumberOfMatrices") != "1")
        {
            throw CiftiException("attribute NumberOfMatrices in CIFTI is required to be 1 for CIFTI-1");
        }
    } else {
        throw CiftiException("missing attribute NumberOfMatrices in CIFTI");
    }
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "Matrix")
            {
                if (haveMatrix)
                {
                    throw CiftiException("Matrix element may only be specified once");
                }
                parseMatrix1(xml);
                if (xml.hasError()) return;
                haveMatrix = true;
            } else {
                throw CiftiException("unexpected element in CIFTI: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
    if (xml.hasError()) return;
#else
#ifdef CIFTILIB_USE_XMLPP
    vector<AString> mandAttrs(1, "NumberOfMatrices");
    XmlAttributesResult attrVals = XmlReader_parseAttributes(xml, mandAttrs);
    if (attrVals.mandatoryVals[0] != "1")
    {
        throw CiftiException("attribute NumberOfMatrices in CIFTI is required to be 1 for CIFTI-1");
    }
    bool done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
    while(!done && xml.read())
    {
        switch (xml.get_node_type())
        {
            case XmlReader::Element:
            {
                AString name = xml.get_local_name();
                if (name == "Matrix")
                {
                    if (haveMatrix)
                    {
                        throw CiftiException("Matrix element may only be specified once");
                    }
                    parseMatrix1(xml);
                    haveMatrix = true;
                } else {
                    throw CiftiException("unexpected element in CIFTI: " + name);
                }
                break;
            }
            case XmlReader::EndElement:
                done = true;
                break;
            default:
                break;
        }
    }
#else
#error "not implemented"
#endif
#endif
    CiftiAssert(XmlReader_checkEndElement(xml, "CIFTI"));
    if (!haveMatrix)
    {
        throw CiftiException("Matrix element not found in CIFTI");
    }
}

void CiftiXML::parseCIFTI2(XmlReader& xml)//yes, these will often have largely similar code, but it seems cleaner than having only some functions split, or constantly rechecking the version
{//also, helps keep changes to cifti-2 away from code that parses cifti-1
    bool haveMatrix = false;
#ifdef CIFTILIB_USE_QT
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.hasError()) return;
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "Matrix")
            {
                if (haveMatrix)
                {
                    throw CiftiException("Matrix element may only be specified once");
                }
                parseMatrix2(xml);
                if (xml.hasError()) return;
                haveMatrix = true;
            } else {
                throw CiftiException("unexpected element in CIFTI: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
#else
#ifdef CIFTILIB_USE_XMLPP
    bool done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
    while(!done && xml.read())
    {
        switch (xml.get_node_type())
        {
            case XmlReader::Element:
            {
                AString name = xml.get_local_name();
                if (name == "Matrix")
                {
                    if (haveMatrix)
                    {
                        throw CiftiException("Matrix element may only be specified once");
                    }
                    parseMatrix2(xml);
                    haveMatrix = true;
                } else {
                    throw CiftiException("unexpected element in CIFTI: " + name);
                }
                break;
            }
            case XmlReader::EndElement:
                done = true;
                break;
            default:
                break;
        }
    }
#else
#error "not implemented"
#endif
#endif
    CiftiAssert(XmlReader_checkEndElement(xml, "CIFTI"));
    if (!haveMatrix)
    {
        throw CiftiException("Matrix element not found in CIFTI");
    }
}

void CiftiXML::parseMatrix1(XmlReader& xml)
{
    VolumeSpace fileVolSpace;
    bool haveVolSpace = false, haveMetadata = false;
#ifdef CIFTILIB_USE_QT
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.hasError()) return;
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "MetaData")
            {
                if (haveMetadata)
                {
                    throw CiftiException("MetaData may only be specified once in Matrix");
                }
                m_fileMetaData.readCiftiXML1(xml);
                if (xml.hasError()) return;
                haveMetadata = true;
            } else if (name == "MatrixIndicesMap") {
                parseMatrixIndicesMap1(xml);
                if (xml.hasError()) return;
            } else if (name == "Volume") {
                if (haveVolSpace)
                {
                    throw CiftiException("Volume may only be specified once in Matrix");
                }
                fileVolSpace.readCiftiXML1(xml);
                if (xml.hasError()) return;
                haveVolSpace = true;
            } else if (name == "LabelTable") {
                xml.readElementText(XmlReader::SkipChildElements);
            } else {
                throw CiftiException("unexpected element in Matrix: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
#else
#ifdef CIFTILIB_USE_XMLPP
    bool skipread = false, done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
    while(!done)
    {
        if (!skipread)
        {
            if (!xml.read()) break;
        } else {
            skipread = false;
        }
        switch (xml.get_node_type())
        {
            case XmlReader::Element:
            {
                AString name = xml.get_local_name();
                if (name == "MetaData")
                {
                    if (haveMetadata)
                    {
                        throw CiftiException("MetaData may only be specified once in Matrix");
                    }
                    m_fileMetaData.readCiftiXML1(xml);
                    haveMetadata = true;
                } else if (name == "MatrixIndicesMap") {
                    parseMatrixIndicesMap1(xml);
                } else if (name == "Volume") {
                    if (haveVolSpace)
                    {
                        throw CiftiException("Volume may only be specified once in Matrix");
                    }
                    fileVolSpace.readCiftiXML1(xml);
                    haveVolSpace = true;
                } else if (name == "LabelTable") {
                    xml.next();//TODO: test if this does this actually does what we want
                    skipread = true;
                } else {
                    throw CiftiException("unexpected element in Matrix: " + name);
                }
                break;
            }
            case XmlReader::EndElement:
                done = true;
                break;
            default:
                break;
        }
    }
#else
#error "not implemented"
#endif
#endif
    CiftiAssert(XmlReader_checkEndElement(xml, "Matrix"));
    for (int i = 0; i < (int)m_indexMaps.size(); ++i)
    {
        if (m_indexMaps[i] == NULL)
        {
            int displaynum = i;
            if (displaynum < 2) displaynum = 1 - displaynum;//re-invert so that it shows the same number as the XML is missing
            throw CiftiException("missing mapping for dimension '" + AString_number(displaynum) + "'");
        }
        switch (m_indexMaps[i]->getType())
        {
            case CiftiMappingType::BRAIN_MODELS:
            {
                CiftiBrainModelsMap& myMap = dynamic_cast<CiftiBrainModelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolSpace)
                    {
                        myMap.setVolumeSpace(fileVolSpace);//also does the needed checking of voxel indices
                    } else {
                        throw CiftiException("BrainModels map uses voxels, but no Volume element exists");
                    }
                }
                break;
            }
            case CiftiMappingType::PARCELS:
            {
                CiftiParcelsMap& myMap = dynamic_cast<CiftiParcelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolSpace)
                    {
                        myMap.setVolumeSpace(fileVolSpace);//ditto
                    } else {
                        throw CiftiException("Parcels map uses voxels, but no Volume element exists");
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

void CiftiXML::parseMatrix2(XmlReader& xml)
{
    bool haveMetadata = false;
#ifdef CIFTILIB_USE_QT
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.hasError()) return;
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "MetaData")
            {
                if (haveMetadata)
                {
                    throw CiftiException("MetaData may only be specified once in Matrix");
                }
                m_fileMetaData.readCiftiXML2(xml);
                if (xml.hasError()) return;
                haveMetadata = true;
            } else if (name == "MatrixIndicesMap") {
                parseMatrixIndicesMap2(xml);
                if (xml.hasError()) return;
            } else {
                throw CiftiException("unexpected element in Matrix: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
#else
#ifdef CIFTILIB_USE_XMLPP
    bool done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
    while(!done && xml.read())
    {
        switch (xml.get_node_type())
        {
            case XmlReader::Element:
            {
                AString name = xml.get_local_name();
                if (name == "MetaData")
                {
                    if (haveMetadata)
                    {
                        throw CiftiException("MetaData may only be specified once in Matrix");
                    }
                    m_fileMetaData.readCiftiXML2(xml);
                    haveMetadata = true;
                } else if (name == "MatrixIndicesMap") {
                    parseMatrixIndicesMap2(xml);
                } else {
                    throw CiftiException("unexpected element in Matrix: " + name);
                }
                break;
            }
            case XmlReader::EndElement:
                done = true;
                break;
            default:
                break;
        }
    }
#else
#error "not implemented"
#endif
#endif
    CiftiAssert(XmlReader_checkEndElement(xml, "Matrix"));
    for (int i = 0; i < (int)m_indexMaps.size(); ++i)
    {
        if (m_indexMaps[i] == NULL)
        {
            throw CiftiException("missing mapping for dimension '" + AString_number(i) + "'");
        }
    }
}

void CiftiXML::parseMatrixIndicesMap1(XmlReader& xml)
{
    vector<AString> mandAttrs(2);
    mandAttrs[0] = "AppliesToMatrixDimension";
    mandAttrs[1] = "IndicesMapToDataType";
    XmlAttributesResult myAttrs = XmlReader_parseAttributes(xml, mandAttrs);
    vector<AString> values = AString_split(myAttrs.mandatoryVals[0], ',');
    bool ok = false;
    set<int> used;
    for(int i = 0; i < (int)values.size(); i++)
    {
        int parsed = AString_toInt(values[i], ok);
        if (!ok || parsed < 0)
        {
            throw CiftiException("bad value in AppliesToMatrixDimension list: " + values[i]);
        }
        if (parsed < 2) parsed = 1 - parsed;//in other words, 0 becomes 1 and 1 becomes 0, since cifti-1 had them reversed
        if (used.find(parsed) != used.end())
        {
            throw CiftiException("AppliesToMatrixDimension contains repeated value: " + values[i]);
        }
        used.insert(parsed);
    }
    boost::shared_ptr<CiftiMappingType> toRead;
    AString type = myAttrs.mandatoryVals[1];
    if (type == "CIFTI_INDEX_TYPE_BRAIN_MODELS")
    {
        toRead = boost::shared_ptr<CiftiBrainModelsMap>(new CiftiBrainModelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_TIME_POINTS") {
        toRead = boost::shared_ptr<CiftiSeriesMap>(new CiftiSeriesMap());
    } else if (type == "CIFTI_INDEX_TYPE_LABELS") {//this and below are nonstandard
        toRead = boost::shared_ptr<CiftiLabelsMap>(new CiftiLabelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_PARCELS") {
        toRead = boost::shared_ptr<CiftiParcelsMap>(new CiftiParcelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_SCALARS") {
        toRead = boost::shared_ptr<CiftiScalarsMap>(new CiftiScalarsMap());
    } else {
        throw CiftiException("invalid value for IndicesMapToDataType in CIFTI-1: " + type);
    }
    toRead->readXML1(xml);//this will warn if it is nonstandard
#ifdef CIFTILIB_USE_QT
    if (xml.hasError()) return;
#endif
    if (toRead->getLength() < 1 && !(type == "CIFTI_INDEX_TYPE_TIME_POINTS" && toRead->getLength() == -1)) throw CiftiException("cifti mapping type with zero length found");
    bool first = true;//NOTE: series maps didn't encode length in cifti-1, -1 is used as a stand-in, CiftiFile fills in the length from the matrix
    for (set<int>::iterator iter = used.begin(); iter != used.end(); ++iter)
    {
        if (*iter >= (int)m_indexMaps.size()) m_indexMaps.resize(*iter + 1);
        if (first)
        {
            m_indexMaps[*iter] = toRead;
            first = false;
        } else {
            m_indexMaps[*iter] = boost::shared_ptr<CiftiMappingType>(toRead->clone());//make in-memory information independent per-dimension, rather than dealing with deduplication everywhere
        }
    }
    CiftiAssert(XmlReader_checkEndElement(xml, "MatrixIndicesMap"));
}

void CiftiXML::parseMatrixIndicesMap2(XmlReader& xml)
{
    vector<AString> mandAttrs(2);
    mandAttrs[0] = "AppliesToMatrixDimension";
    mandAttrs[1] = "IndicesMapToDataType";
    XmlAttributesResult myAttrs = XmlReader_parseAttributes(xml, mandAttrs);
    vector<AString> values = AString_split(myAttrs.mandatoryVals[0], ',');
    bool ok = false;
    set<int> used;
    for(int i = 0; i < (int)values.size(); i++)
    {
        int parsed = AString_toInt(values[i], ok);
        if (!ok || parsed < 0)
        {
            throw CiftiException("bad value in AppliesToMatrixDimension list: " + values[i]);
        }
        if (used.find(parsed) != used.end())
        {
            throw CiftiException("AppliesToMatrixDimension contains repeated value: " + values[i]);
        }
        used.insert(parsed);
    }
    boost::shared_ptr<CiftiMappingType> toRead;
    AString type = myAttrs.mandatoryVals[1];
    if (type == "CIFTI_INDEX_TYPE_BRAIN_MODELS")
    {
        toRead = boost::shared_ptr<CiftiBrainModelsMap>(new CiftiBrainModelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_LABELS") {
        toRead = boost::shared_ptr<CiftiLabelsMap>(new CiftiLabelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_PARCELS") {
        toRead = boost::shared_ptr<CiftiParcelsMap>(new CiftiParcelsMap());
    } else if (type == "CIFTI_INDEX_TYPE_SCALARS") {
        toRead = boost::shared_ptr<CiftiScalarsMap>(new CiftiScalarsMap());
    } else if (type == "CIFTI_INDEX_TYPE_SERIES") {
        toRead = boost::shared_ptr<CiftiSeriesMap>(new CiftiSeriesMap());
    } else {
        throw CiftiException("invalid value for IndicesMapToDataType in CIFTI-1: " + type);
    }
    toRead->readXML2(xml);
#ifdef CIFTILIB_USE_QT
    if (xml.hasError()) return;
#endif
    if (toRead->getLength() < 1) throw CiftiException("cifti mapping type with zero length found");
    bool first = true;
    for (set<int>::iterator iter = used.begin(); iter != used.end(); ++iter)
    {
        if (*iter >= (int)m_indexMaps.size()) m_indexMaps.resize(*iter + 1);
        if (first)
        {
            m_indexMaps[*iter] = toRead;
            first = false;
        } else {
            m_indexMaps[*iter] = boost::shared_ptr<CiftiMappingType>(toRead->clone());//make in-memory information independent per-dimension, rather than dealing with deduplication everywhere
        }
    }
    CiftiAssert(XmlReader_checkEndElement(xml, "MatrixIndicesMap"));
}

vector<char> CiftiXML::writeXMLToVector(const CiftiVersion& writingVersion) const
{
#ifdef CIFTILIB_USE_QT
    QByteArray tempArray;
    XmlWriter xml(&tempArray);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    writeXML(xml, writingVersion);
    xml.writeEndDocument();
    int numBytes = tempArray.size();//QByteArray is limited to 2GB
    vector<char> ret(numBytes + 1);//include room for null terminator
    for (int i = 0; i < numBytes; ++i)
    {
        ret[i] = tempArray[i];
    }
    ret[numBytes] = '\0';
    return ret;
#else
#ifdef CIFTILIB_USE_XMLPP
    XmlWriter xml;
    xml.writeStartDocument();
    writeXML(xml, writingVersion);
    xml.writeEndDocument();
    return xml.getXmlData();
#else
#error "not implemented"
#endif
#endif
}

void CiftiXML::writeXML(XmlWriter& xml, const CiftiVersion& writingVersion) const
{
    xml.writeStartElement("CIFTI");
    xml.writeAttribute("Version", writingVersion.toString());
    if (writingVersion == CiftiVersion(1, 0))//switch/case on major/minor would be much harder to read
    {
        xml.writeAttribute("NumberOfMatrices", "1");
        writeMatrix1(xml);
    } else if (writingVersion == CiftiVersion(2, 0)) {
        writeMatrix2(xml);
    } else {
        throw CiftiException("unknown Cifti writing version: '" + writingVersion.toString() + "'");
    }
    xml.writeEndElement();
}

void CiftiXML::writeMatrix1(XmlWriter& xml) const
{
    int numDims = (int)m_indexMaps.size();
    bool haveVolData = false;
    VolumeSpace volSpace;
    for (int i = 0; i < numDims; ++i)
    {
        if (m_indexMaps[i] == NULL) throw CiftiException("dimension " + AString_number(i) + " was not given a mapping");
        switch (m_indexMaps[i]->getType())
        {
            case CiftiMappingType::BRAIN_MODELS:
            {
                const CiftiBrainModelsMap& myMap = dynamic_cast<const CiftiBrainModelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolData)
                    {
                        if (myMap.getVolumeSpace() != volSpace)
                        {
                            throw CiftiException("cannot write different volume spaces for different dimensions in CIFTI-1");
                        }
                    } else {
                        haveVolData = true;
                        volSpace = myMap.getVolumeSpace();
                    }
                }
                break;
            }
            case CiftiMappingType::PARCELS:
            {
                const CiftiParcelsMap& myMap = dynamic_cast<const CiftiParcelsMap&>(*(m_indexMaps[i]));
                if (myMap.hasVolumeData())
                {
                    if (haveVolData)
                    {
                        if (myMap.getVolumeSpace() != volSpace)
                        {
                            throw CiftiException("cannot write different volume spaces for different dimensions in CIFTI-1");
                        }
                    } else {
                        haveVolData = true;
                        volSpace = myMap.getVolumeSpace();
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    xml.writeStartElement("Matrix");
    m_fileMetaData.writeCiftiXML1(xml);
    if (haveVolData)
    {
        volSpace.writeCiftiXML1(xml);
    }
    vector<bool> used(numDims, false);
    for (int i = 0; i < numDims; ++i)
    {
        if (!used[i])
        {
            used[i] = true;
            int outputNum = i;
            if (outputNum < 2) outputNum = 1 - outputNum;//ie, swap 0 and 1
            AString appliesTo = AString_number(outputNum);//initialize containing just the current dimension
            for (int j = i + 1; j < numDims; ++j)//compare to all later unused dimensions for deduplication
            {//technically, shouldn't need to check for previously used as long as equality is exact, but means maybe fewer comparisons, and to prevent a bug in == from getting stranger behavior
                if (!used[j])
                {
                    if ((*m_indexMaps[i]) == (*m_indexMaps[j]))
                    {
                        outputNum = j;
                        if (outputNum < 2) outputNum = 1 - outputNum;
                        appliesTo += "," + AString_number(outputNum);
                        used[j] = true;
                    }
                }
            }
            xml.writeStartElement("MatrixIndicesMap");//should the CiftiIndexMap do this instead, and we pass appliesTo to it as string?  probably not important, we won't use them in any other xml
            xml.writeAttribute("AppliesToMatrixDimension", appliesTo);
            m_indexMaps[i]->writeXML1(xml);
            xml.writeEndElement();
        }
    }
    xml.writeEndElement();
}

void CiftiXML::writeMatrix2(XmlWriter& xml) const
{
    int numDims = (int)m_indexMaps.size();
    for (int i = 0; i < numDims; ++i)
    {
        if (m_indexMaps[i] == NULL) throw CiftiException("dimension " + AString_number(i) + " was not given a mapping");
    }
    xml.writeStartElement("Matrix");
    m_fileMetaData.writeCiftiXML2(xml);
    vector<bool> used(numDims, false);
    for (int i = 0; i < numDims; ++i)
    {
        if (!used[i])
        {
            used[i] = true;
            AString appliesTo = AString_number(i);//initialize containing just the current dimension
            for (int j = i + 1; j < numDims; ++j)//compare to all later unused dimensions for deduplication
            {//technically, shouldn't need to check for previously used as long as equality is exact, but means maybe fewer comparisons, and to prevent a bug in == from getting stranger behavior
                if (!used[j])
                {
                    if ((*m_indexMaps[i]) == (*m_indexMaps[j]))
                    {
                        appliesTo += "," + AString_number(j);
                        used[j] = true;
                    }
                }
            }
            xml.writeStartElement("MatrixIndicesMap");//should the CiftiIndexMap do this instead, and we pass appliesTo to it as string?  probably not important, we won't use them in any other xml
            xml.writeAttribute("AppliesToMatrixDimension", appliesTo);
            m_indexMaps[i]->writeXML2(xml);
            xml.writeEndElement();
        }
    }
    xml.writeEndElement();
}
