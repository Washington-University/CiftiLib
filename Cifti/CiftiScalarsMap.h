#ifndef __CIFTI_SCALARS_MAP_H__
#define __CIFTI_SCALARS_MAP_H__

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

#include "MetaData.h"

#include <map>
#include <vector>

namespace cifti
{
    class CiftiScalarsMap : public CiftiMappingType
    {
    public:
        const MetaData& getMapMetadata(const int64_t& index) const;
        const QString& getMapName(const int64_t& index) const;
        
        void setMapMetadata(const int64_t& index, const MetaData& mdIn);
        void setMapName(const int64_t& index, const QString& mapName);
        void setLength(const int64_t& length);
        void clear();
        
        CiftiMappingType* clone() const { return new CiftiScalarsMap(*this); }
        MappingType getType() const { return SCALARS; }
        int64_t getLength() const { return m_maps.size(); }
        bool operator==(const CiftiMappingType& rhs) const;
        bool approximateMatch(const CiftiMappingType& rhs) const;
        void readXML1(QXmlStreamReader& xml);
        void readXML2(QXmlStreamReader& xml);
        void writeXML1(QXmlStreamWriter& xml) const;
        void writeXML2(QXmlStreamWriter& xml) const;
    private:
        struct ScalarMap
        {
            QString m_name;//we need a better way to change metadata in an in-memory file
            MetaData m_metaData;//ditto
            bool operator==(const ScalarMap& rhs) const;
            void readXML1(QXmlStreamReader& xml);
            void readXML2(QXmlStreamReader& xml);
        };
        std::vector<ScalarMap> m_maps;
    };
}

#endif //__CIFTI_SCALARS_MAP_H__
