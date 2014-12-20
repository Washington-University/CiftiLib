#ifndef __CIFTI_XML_H__
#define __CIFTI_XML_H__

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

#include "CiftiMappingType.h"
#include "CiftiVersion.h"
#include "MetaData.h"

#include "CiftiBrainModelsMap.h"
#include "CiftiLabelsMap.h"
#include "CiftiParcelsMap.h"
#include "CiftiScalarsMap.h"
#include "CiftiSeriesMap.h"

#include "boost/shared_ptr.hpp"

#include <vector>

namespace cifti
{
    class CiftiXML
    {
    public:
        enum
        {
            ALONG_ROW = 0,
            ALONG_COLUMN = 1,
            ALONG_STACK = 2//better name for this?
        };
        int getNumberOfDimensions() const { return m_indexMaps.size(); }
        const CiftiVersion& getParsedVersion() const { return m_parsedVersion; }
        const CiftiMappingType* getMap(const int& direction) const;//can return null in unfilled XML object
        CiftiMappingType* getMap(const int& direction);//can return null in unfilled XML object
        const MetaData& getFileMetaData() const;
        
        CiftiMappingType::MappingType getMappingType(const int& direction) const;//convenience functions
        const CiftiBrainModelsMap& getBrainModelsMap(const int& direction) const;
        CiftiBrainModelsMap& getBrainModelsMap(const int& direction);
        const CiftiLabelsMap& getLabelsMap(const int& direction) const;
        CiftiLabelsMap& getLabelsMap(const int& direction);
        const CiftiParcelsMap& getParcelsMap(const int& direction) const;
        CiftiParcelsMap& getParcelsMap(const int& direction);
        const CiftiScalarsMap& getScalarsMap(const int& direction) const;
        CiftiScalarsMap& getScalarsMap(const int& direction);
        const CiftiSeriesMap& getSeriesMap(const int& direction) const;
        CiftiSeriesMap& getSeriesMap(const int& direction);
        int64_t getDimensionLength(const int& direction) const;
        std::vector<int64_t> getDimensions() const;
        
        void setNumberOfDimensions(const int& num);
        void setMap(const int& direction, const CiftiMappingType& mapIn);
        void setFileMetaData(const MetaData& mdIn) { m_fileMetaData = mdIn; }
        void clear();
        
        void readXML(XmlReader& xml);
        void readXML(const std::vector<char>& text);
        
        std::vector<char> writeXMLToVector(const CiftiVersion& writingVersion = CiftiVersion()) const;
        void writeXML(XmlWriter& xml, const CiftiVersion& writingVersion = CiftiVersion()) const;
        
        ///uses the mapping types to figure out what the intent info should be
        int32_t getIntentInfo(const CiftiVersion& writingVersion, char intentNameOut[16]) const;
        
        CiftiXML() { }
        CiftiXML(const CiftiXML& rhs);
        CiftiXML& operator=(const CiftiXML& rhs);
        bool operator==(const CiftiXML& rhs) const;
        bool operator!=(const CiftiXML& rhs) const { return !((*this) == rhs); }
        bool approximateMatch(const CiftiXML& rhs) const;
    private:
        std::vector<boost::shared_ptr<CiftiMappingType> > m_indexMaps;
        CiftiVersion m_parsedVersion;
        MetaData m_fileMetaData;
        
        void copyHelper(const CiftiXML& rhs);
        //parsing functions
        void parseCIFTI1(XmlReader& xml);
        void parseMatrix1(XmlReader& xml);
        void parseCIFTI2(XmlReader& xml);
        void parseMatrix2(XmlReader& xml);
        void parseMatrixIndicesMap1(XmlReader& xml);
        void parseMatrixIndicesMap2(XmlReader& xml);
        //writing functions
        void writeMatrix1(XmlWriter& xml) const;
        void writeMatrix2(XmlWriter& xml) const;
    };
}

#endif //__CIFTI_XML_H__
