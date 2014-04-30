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

void CiftiSeriesMap::readXML1(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attrs = xml.attributes();
    float newStart = 0.0f, newStep = -1.0f, mult = 0.0f;
    bool ok = false;
    if (!attrs.hasAttribute("TimeStepUnits"))
    {
        throw CiftiException("timepoints mapping is missing requred attribute TimeStepUnits");
    }
    QStringRef unitString = attrs.value("TimeStepUnits");
    if (unitString == "NIFTI_UNITS_SEC")
    {
        mult = 1.0f;
    } else if (unitString == "NIFTI_UNITS_MSEC") {
        mult = 0.001f;
    } else if (unitString == "NIFTI_UNITS_USEC") {
        mult = 0.000001f;
    } else {
        throw CiftiException("unrecognized value for TimeStepUnits: " + unitString.toString());
    }
    if (attrs.hasAttribute("TimeStart"))//optional and nonstandard
    {
        newStart = mult * attrs.value("TimeStart").toString().toFloat(&ok);
        if (!ok)
        {
            throw CiftiException("unrecognized value for TimeStart: " + attrs.value("TimeStart").toString());
        }
    }
    if (!attrs.hasAttribute("TimeStep"))
    {
        throw CiftiException("timepoints mapping is missing requred attribute TimeStep");
    }
    newStep = mult * attrs.value("TimeStep").toString().toFloat(&ok);
    if (!ok)
    {
        throw CiftiException("unrecognized value for TimeStep: " + attrs.value("TimeStep").toString());
    }
    if (xml.readNextStartElement())
    {
        throw CiftiException("unexpected element in timepoints mapping: " + xml.name().toString());
    }
    m_length = -1;//cifti-1 doesn't know length in xml, must be set by checking the matrix
    m_start = newStart;
    m_step = newStep;
    m_unit = SECOND;
    CaretAssert(xml.isEndElement() && xml.name() == "MatrixIndicesMap");
}

void CiftiSeriesMap::readXML2(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attrs = xml.attributes();
    float newStart = 0.0f, newStep = -1.0f, mult = 0.0f;
    int64_t newLength = -1;
    Unit newUnit;
    bool ok = false;
    if (!attrs.hasAttribute("SeriesUnit"))
    {
        throw CiftiException("series mapping is missing requred attribute SeriesUnit");
    }
    QStringRef unitString = attrs.value("SeriesUnit");
    if (unitString == "HERTZ")
    {
        newUnit = HERTZ;
    } else if (unitString == "METER") {
        newUnit = METER;
    } else if (unitString == "RADIAN") {
        newUnit = RADIAN;
    } else if (unitString == "SECOND") {
        newUnit = SECOND;
    } else {
        throw CiftiException("unrecognized value for TimeStepUnits: " + unitString.toString());
    }
    if (!attrs.hasAttribute("SeriesExponent"))
    {
        throw CiftiException("series mapping is missing requred attribute SeriesExponent");
    }
    int exponent = attrs.value("SeriesExponent").toString().toInt(&ok);
    if (!ok)
    {
        throw CiftiException("unrecognized value for SeriesExponent: " + attrs.value("SeriesExponent").toString());
    }
    mult = pow(10.0f, exponent);
    if (!attrs.hasAttribute("SeriesStart"))
    {
        throw CiftiException("series mapping is missing requred attribute SeriesStart");
    }
    newStart = mult * attrs.value("SeriesStart").toString().toFloat(&ok);
    if (!ok)
    {
        throw CiftiException("unrecognized value for SeriesStart: " + attrs.value("SeriesStart").toString());
    }
    if (!attrs.hasAttribute("SeriesStep"))
    {
        throw CiftiException("series mapping is missing requred attribute SeriesStep");
    }
    newStep = mult * attrs.value("SeriesStep").toString().toFloat(&ok);
    if (!ok)
    {
        throw CiftiException("unrecognized value for SeriesStep: " + attrs.value("SeriesStep").toString());
    }
    if (!attrs.hasAttribute("NumberOfSeriesPoints"))
    {
        throw CiftiException("series mapping is missing requred attribute NumberOfSeriesPoints");
    }
    newLength = attrs.value("NumberOfSeriesPoints").toString().toLongLong(&ok);
    if (!ok)
    {
        throw CiftiException("unrecognized value for NumberOfSeriesPoints: " + attrs.value("NumberOfSeriesPoints").toString());
    }
    if (newLength < 1)
    {
        throw CiftiException("NumberOfSeriesPoints must be positive");
    }
    if (xml.readNextStartElement())
    {
        throw CiftiException("unexpected element in series mapping: " + xml.name().toString());
    }
    m_length = newLength;
    m_start = newStart;
    m_step = newStep;
    m_unit = newUnit;
    CaretAssert(xml.isEndElement() && xml.name() == "MatrixIndicesMap");
}

void CiftiSeriesMap::writeXML1(QXmlStreamWriter& xml) const
{
    CaretAssert(m_length != -1);
    if (m_unit != SECOND)
    {
        throw CiftiException("cifti-1 does not support writing series with non-time units");
    }
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_TIME_POINTS");
    float mult = 1.0f;
    QString unitString = "NIFTI_UNITS_SEC";
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
    xml.writeAttribute("TimeStart", QString::number(mult * m_start, 'f', 10));//even though it is nonstandard, write it, always
    xml.writeAttribute("TimeStep", QString::number(mult * m_step, 'f', 10));
}

void CiftiSeriesMap::writeXML2(QXmlStreamWriter& xml) const
{
    CaretAssert(m_length != -1);
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_SERIES");
    int exponent = 0;
    float test = m_step;
    if (test == 0.0f) test = m_start;
    if (test != 0.0f)
    {
        exponent = 3 * (int)floor((log10(test) - log10(0.05f)) / 3.0f);//some magic to get the exponent that is a multiple of 3 that puts the test value in [0.05, 50]
    }
    float mult = pow(10.0f, -exponent);
    QString unitString;
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
    xml.writeAttribute("NumberOfSeriesPoints", QString::number(m_length));
    xml.writeAttribute("SeriesExponent", QString::number(exponent));
    xml.writeAttribute("SeriesStart", QString::number(mult * m_start, 'f', 10));
    xml.writeAttribute("SeriesStep", QString::number(mult * m_step, 'f', 10));
    xml.writeAttribute("SeriesUnit", unitString);
}
