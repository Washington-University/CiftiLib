#ifndef __CIFTI_SERIES_MAP_H__
#define __CIFTI_SERIES_MAP_H__

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

#include "CiftiAssert.h"
#include "CiftiMappingType.h"

namespace cifti
{
    class CiftiSeriesMap : public CiftiMappingType
    {
    public:
        enum Unit
        {
            HERTZ,
            METER,
            RADIAN,
            SECOND
        };//should this go somewhere else?
        float getStart() const { return m_start; }//using getter/setter as style choice to match other mapping types
        float getStep() const { return m_step; }//getter for number of series points is getLength(), specified by CiftiIndexMap
        Unit getUnit() const { return m_unit; }
        
        CiftiSeriesMap()
        {
            m_start = 0.0f;
            m_step = 1.0f;
            m_unit = SECOND;
            m_length = -1;//to make it clear an improperly initialized series map object was used
        }
        CiftiSeriesMap(const int64_t& length, const float& start = 0.0f, const float& step = 1.0f, const Unit& unit = SECOND)
        {
            m_start = start;
            m_step = step;
            m_unit = unit;
            m_length = length;
        }
        void setStart(const float& start) { m_start = start; }
        void setStep(const float& step) { m_step = step; }
        void setUnit(const Unit& unit) { m_unit = unit; }
        void setLength(const int64_t& length) { CiftiAssert(length > 0); m_length = length; }
        
        CiftiMappingType* clone() const { return new CiftiSeriesMap(*this); }
        MappingType getType() const { return SERIES; }
        int64_t getLength() const { return m_length; }
        bool operator==(const CiftiMappingType& rhs) const
        {
            if (rhs.getType() != getType()) return false;
            const CiftiSeriesMap& temp = dynamic_cast<const CiftiSeriesMap&>(rhs);
            return (temp.m_length == m_length &&
                    temp.m_unit == m_unit &&
                    temp.m_start == m_start &&
                    temp.m_step == m_step);
        }
        bool approximateMatch(const CiftiMappingType& rhs) const
        {
            switch (rhs.getType())
            {
                case SCALARS://maybe?
                case SERIES:
                case LABELS://maybe?
                    return getLength() == rhs.getLength();
                default:
                    return false;
            }
        }
        void readXML1(XmlReader& xml);
        void readXML2(XmlReader& xml);
        void writeXML1(XmlWriter& xml) const;
        void writeXML2(XmlWriter& xml) const;
    private:
        int64_t m_length;
        float m_start, m_step;//exponent gets applied to these on reading
        Unit m_unit;
    };
}

#endif //__CIFTI_SERIES_MAP_H__
