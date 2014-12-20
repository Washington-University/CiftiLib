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

#include "CiftiSeriesMap.h"

#include "CiftiException.h"

#include <cmath>

using namespace cifti;
using namespace std;

void CiftiSeriesMap::readXML1(XmlReader& xml)
{
    vector<AString> mandAttrs(2), optAttrs(1, "TimeStart");
    mandAttrs[0] = "TimeStep";
    mandAttrs[1] = "TimeStepUnits";
    XmlAttributesResult myAttrs = XmlReader_parseAttributes(xml, mandAttrs, optAttrs);
    float newStart = 0.0f, newStep = -1.0f, mult = 0.0f;
    bool ok = false;
    if (myAttrs.mandatoryVals[1] == "NIFTI_UNITS_SEC")
    {
        mult = 1.0f;
    } else if (myAttrs.mandatoryVals[1] == "NIFTI_UNITS_MSEC") {
        mult = 0.001f;
    } else if (myAttrs.mandatoryVals[1] == "NIFTI_UNITS_USEC") {
        mult = 0.000001f;
    } else {
        throw CiftiException("unrecognized value for TimeStepUnits: " + myAttrs.mandatoryVals[1]);
    }
    if (myAttrs.optionalVals[0].present)//optional and nonstandard
    {
        newStart = mult * AString_toFloat(myAttrs.optionalVals[0].value, ok);
        if (!ok)
        {
            throw CiftiException("unrecognized value for TimeStart: " + myAttrs.optionalVals[0].value);
        }
    }
    newStep = mult * AString_toFloat(myAttrs.mandatoryVals[0], ok);
    if (!ok)
    {
        throw CiftiException("unrecognized value for TimeStep: " + myAttrs.mandatoryVals[0]);
    }
#ifdef CIFTILIB_USE_QT
    if (xml.readNextStartElement())
    {
        throw CiftiException("unexpected element in timepoints map: " + xml.name().toString());
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
                throw CiftiException("unexpected element in timepoints map: " + name);
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
    CiftiAssert(XmlReader_checkEndElement(xml, "MatrixIndicesMap"));
    m_length = -1;//cifti-1 doesn't know length in xml, must be set by checking the matrix
    m_start = newStart;
    m_step = newStep;
    m_unit = SECOND;
}

void CiftiSeriesMap::readXML2(XmlReader& xml)
{
    vector<AString> mandAttrs(5);
    mandAttrs[0] = "SeriesStep";
    mandAttrs[1] = "SeriesUnit";
    mandAttrs[2] = "SeriesExponent";
    mandAttrs[3] = "SeriesStart";
    mandAttrs[4] = "NumberOfSeriesPoints";
    XmlAttributesResult myAttrs = XmlReader_parseAttributes(xml, mandAttrs);
    float newStart = 0.0f, newStep = -1.0f, mult = 0.0f;
    int64_t newLength = -1;
    Unit newUnit;
    bool ok = false;
    if (myAttrs.mandatoryVals[1] == "HERTZ")
    {
        newUnit = HERTZ;
    } else if (myAttrs.mandatoryVals[1] == "METER") {
        newUnit = METER;
    } else if (myAttrs.mandatoryVals[1] == "RADIAN") {
        newUnit = RADIAN;
    } else if (myAttrs.mandatoryVals[1] == "SECOND") {
        newUnit = SECOND;
    } else {
        throw CiftiException("unrecognized value for SeriesUnit: " + myAttrs.mandatoryVals[1]);
    }
    int exponent = AString_toInt(myAttrs.mandatoryVals[2], ok);
    if (!ok)
    {
        throw CiftiException("unrecognized value for SeriesExponent: " + myAttrs.mandatoryVals[2]);
    }
    mult = pow(10.0f, exponent);
    newStart = mult * AString_toFloat(myAttrs.mandatoryVals[3], ok);
    if (!ok)
    {
        throw CiftiException("unrecognized value for SeriesStart: " + myAttrs.mandatoryVals[3]);
    }
    newStep = mult * AString_toFloat(myAttrs.mandatoryVals[0], ok);
    if (!ok)
    {
        throw CiftiException("unrecognized value for SeriesStep: " + myAttrs.mandatoryVals[0]);
    }
    newLength = AString_toInt(myAttrs.mandatoryVals[4], ok);
    if (!ok)
    {
        throw CiftiException("unrecognized value for NumberOfSeriesPoints: " + myAttrs.mandatoryVals[4]);
    }
    if (newLength < 1)
    {
        throw CiftiException("NumberOfSeriesPoints must be positive");
    }
#ifdef CIFTILIB_USE_QT
    if (xml.readNextStartElement())
    {
        throw CiftiException("unexpected element in series map: " + xml.name().toString());
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
                throw CiftiException("unexpected element in series map: " + name);
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
    CiftiAssert(XmlReader_checkEndElement(xml, "MatrixIndicesMap"));
    m_length = newLength;
    m_start = newStart;
    m_step = newStep;
    m_unit = newUnit;
}

void CiftiSeriesMap::writeXML1(XmlWriter& xml) const
{
    CiftiAssert(m_length != -1);
    if (m_unit != SECOND)
    {
        throw CiftiException("cifti-1 does not support writing series with non-time units");
    }
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_TIME_POINTS");
    float mult = 1.0f;
    AString unitString = "NIFTI_UNITS_SEC";
    float test = m_step;
    if (test == 0.0f) test = m_start;
    if (test != 0.0f)
    {
        if (abs(test) < 0.00005f)
        {
            mult = 1000000.0f;
            unitString = "NIFTI_UNITS_USEC";
        } else if (abs(test) < 0.05f) {
            mult = 1000.0f;
            unitString = "NIFTI_UNITS_MSEC";
        }
    }
    xml.writeAttribute("TimeStepUnits", unitString);
    xml.writeAttribute("TimeStart", AString_number_fixed(mult * m_start, 7));//even though it is nonstandard, write it, always
    xml.writeAttribute("TimeStep", AString_number_fixed(mult * m_step, 7));
}

void CiftiSeriesMap::writeXML2(XmlWriter& xml) const
{
    CiftiAssert(m_length != -1);
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_SERIES");
    int exponent = 0;
    float test = m_step;
    if (test == 0.0f) test = m_start;
    if (test != 0.0f)
    {
        exponent = 3 * (int)floor((log10(test) - log10(0.05f)) / 3.0f);//some magic to get the exponent that is a multiple of 3 that puts the test value in [0.05, 50]
    }
    float mult = pow(10.0f, -exponent);
    AString unitString;
    switch (m_unit)
    {
        case HERTZ:
            unitString = "HERTZ";
            break;
        case METER:
            unitString = "METER";
            break;
        case RADIAN:
            unitString = "RADIAN";
            break;
        case SECOND:
            unitString = "SECOND";
            break;
    }
    xml.writeAttribute("NumberOfSeriesPoints", AString_number(m_length));
    xml.writeAttribute("SeriesExponent", AString_number(exponent));
    xml.writeAttribute("SeriesStart", AString_number_fixed(mult * m_start, 7));
    xml.writeAttribute("SeriesStep", AString_number_fixed(mult * m_step, 7));
    xml.writeAttribute("SeriesUnit", unitString);
}
