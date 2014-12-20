#ifndef __CIFTI_LABELS_MAP_H__
#define __CIFTI_LABELS_MAP_H__

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

#include "AString.h"
#include "MetaData.h"
#include "LabelTable.h"

#include <map>
#include <vector>

namespace cifti
{
    class CiftiLabelsMap : public CiftiMappingType
    {
    public:
        const MetaData& getMapMetadata(const int64_t& index) const;
        const LabelTable& getMapLabelTable(const int64_t& index) const;
        const AString& getMapName(const int64_t& index) const;
        
        void setMapMetadata(const int64_t& index, const MetaData& md);
        void setMapLabelTable(const int64_t& index, const LabelTable& lt);
        void setMapName(const int64_t& index, const AString& mapName);
        void setLength(const int64_t& length);
        void clear();
        
        CiftiMappingType* clone() const { return new CiftiLabelsMap(*this); }
        MappingType getType() const { return LABELS; }
        int64_t getLength() const { return m_maps.size(); }
        bool operator==(const CiftiMappingType& rhs) const;
        bool approximateMatch(const CiftiMappingType& rhs) const;
        void readXML1(XmlReader& xml);
        void readXML2(XmlReader& xml);
        void writeXML1(XmlWriter& xml) const;
        void writeXML2(XmlWriter& xml) const;
    private:
        struct LabelMap
        {
            AString m_name;
            MetaData m_metaData;
            LabelTable m_labelTable;
            bool operator==(const LabelMap& rhs) const;
            void readXML1(XmlReader& xml);
            void readXML2(XmlReader& xml);
        };
        std::vector<LabelMap> m_maps;
    };
}

#endif //__CIFTI_LABELS_MAP_H__
