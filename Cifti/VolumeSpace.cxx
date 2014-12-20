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

#include "VolumeSpace.h"

#include "CiftiException.h"
#include "FloatMatrix.h"

#ifdef CIFTILIB_USE_QT
    #include <QRegExp>
    #include <QStringList>
#else
    #ifdef CIFTILIB_USE_XMLPP
    #else
        #error "not implemented"
    #endif
#endif

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;
using namespace cifti;

VolumeSpace::VolumeSpace()
{
    m_dims[0] = 0;
    m_dims[1] = 0;
    m_dims[2] = 0;
    m_sform = FloatMatrix::identity(4).getMatrix();
    computeInverse();
}

VolumeSpace::VolumeSpace(const int64_t dims[3], const vector<vector<float> >& sform)
{
    setSpace(dims, sform);
}

VolumeSpace::VolumeSpace(const int64_t dims[3], const float sform[16])
{
    setSpace(dims, sform);
}

void VolumeSpace::setSpace(const int64_t dims[3], const vector<vector<float> >& sform)
{
    if (sform.size() < 2 || sform.size() > 4)
    {
        CiftiAssert(false);
        throw CiftiException("VolumeSpace initialized with wrong size sform");
    }
    for (int i = 0; i < (int)sform.size(); ++i)
    {
        if (sform[i].size() != 4)
        {
            CiftiAssert(false);
            throw CiftiException("VolumeSpace initialized with wrong size sform");
        }
    }
    m_dims[0] = dims[0];
    m_dims[1] = dims[1];
    m_dims[2] = dims[2];
    m_sform = sform;
    m_sform.resize(4);//make sure its 4x4
    m_sform[3].resize(4);
    m_sform[3][0] = 0.0f;//force the fourth row to be correct
    m_sform[3][1] = 0.0f;
    m_sform[3][2] = 0.0f;
    m_sform[3][3] = 1.0f;
    computeInverse();
}

void VolumeSpace::setSpace(const int64_t dims[3], const float sform[12])
{
    m_sform = FloatMatrix::identity(4).getMatrix();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            m_sform[i][j] = sform[i * 4 + j];
        }
    }
    m_dims[0] = dims[0];
    m_dims[1] = dims[1];
    m_dims[2] = dims[2];
    computeInverse();
}

void VolumeSpace::computeInverse()
{
    m_inverse = FloatMatrix(m_sform).inverse().getMatrix();
}

void VolumeSpace::spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float& indexOut1, float& indexOut2, float& indexOut3) const
{
    indexOut1 = coordIn1 * m_inverse[0][0] + coordIn2 * m_inverse[0][1] + coordIn3 * m_inverse[0][2] + m_inverse[0][3];
    indexOut2 = coordIn1 * m_inverse[1][0] + coordIn2 * m_inverse[1][1] + coordIn3 * m_inverse[1][2] + m_inverse[1][3];
    indexOut3 = coordIn1 * m_inverse[2][0] + coordIn2 * m_inverse[2][1] + coordIn3 * m_inverse[2][2] + m_inverse[2][3];
}

void VolumeSpace::enclosingVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3) const
{
    float tempInd1, tempInd2, tempInd3;
    spaceToIndex(coordIn1, coordIn2, coordIn3, tempInd1, tempInd2, tempInd3);
    indexOut1 = (int64_t)floor(0.5f + tempInd1);
    indexOut2 = (int64_t)floor(0.5f + tempInd2);
    indexOut3 = (int64_t)floor(0.5f + tempInd3);
}

bool VolumeSpace::matchesVolumeSpace(const VolumeSpace& right) const
{
    for (int i = 0; i < 3; ++i)
    {
        if (m_dims[i] != right.m_dims[i])
        {
            return false;
        }
    }
    const float TOLER_RATIO = 0.999f;//ratio a spacing element can mismatch by
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float leftelem = m_sform[i][j];
            float rightelem = right.m_sform[i][j];
            if ((leftelem != rightelem) && (leftelem == 0.0f || rightelem == 0.0f || (leftelem / rightelem < TOLER_RATIO || rightelem / leftelem < TOLER_RATIO)))
            {
                return false;
            }
        }
    }
    return true;
}

bool VolumeSpace::operator==(const VolumeSpace& right) const
{
    for (int i = 0; i < 3; ++i)
    {
        if (m_dims[i] != right.m_dims[i])
        {
            return false;
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (m_sform[i][j] != right.m_sform[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

void VolumeSpace::getSpacingVectors(Vector3D& iStep, Vector3D& jStep, Vector3D& kStep, Vector3D& origin) const
{
    FloatMatrix(m_sform).getAffineVectors(iStep, jStep, kStep, origin);
}

void VolumeSpace::getOrientAndSpacingForPlumb(OrientTypes* orientOut, float* spacingOut, float* originOut) const
{
    CiftiAssert(isPlumb());
    if (!isPlumb())
    {
        throw CiftiException("orientation and spacing asked for on non-plumb volume space");//this will fail MISERABLY on non-plumb volumes, so throw otherwise
    }
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (m_sform[i][j] != 0.0f)
            {
                spacingOut[j] = m_sform[i][j];
                originOut[j] = m_sform[i][3];
                bool negative;
                if (m_sform[i][j] > 0.0f)
                {
                    negative = true;
                } else {
                    negative = false;
                }
                switch (i)
                {
                case 0:
                    //left/right
                    orientOut[j] = (negative ? RIGHT_TO_LEFT : LEFT_TO_RIGHT);
                    break;
                case 1:
                    //forward/back
                    orientOut[j] = (negative ? ANTERIOR_TO_POSTERIOR : POSTERIOR_TO_ANTERIOR);
                    break;
                case 2:
                    //up/down
                    orientOut[j] = (negative ? SUPERIOR_TO_INFERIOR : INFERIOR_TO_SUPERIOR);
                    break;
                default:
                    //will never get called
                    break;
                };
            }
        }
    }
}

void VolumeSpace::getOrientation(VolumeSpace::OrientTypes orientOut[3]) const
{
    Vector3D ivec, jvec, kvec, origin;
    getSpacingVectors(ivec, jvec, kvec, origin);
    int next = 1, bestarray[3] = {0, 0, 0};
    float bestVal = -1.0f;//make sure at least the first test trips true, if there is a zero spacing vector it will default to report LPI
    for (int first = 0; first < 3; ++first)//brute force search for best fit - only 6 to try
    {
        int third = 3 - first - next;
        float testVal = abs(ivec[first] * jvec[next] * kvec[third]);
        if (testVal > bestVal)
        {
            bestVal = testVal;
            bestarray[0] = first;
            bestarray[1] = next;
        }
        testVal = abs(ivec[first] * jvec[third] * kvec[next]);
        if (testVal > bestVal)
        {
            bestVal = testVal;
            bestarray[0] = first;
            bestarray[1] = third;
        }
        next = 0;
    }
    bestarray[2] = 3 - bestarray[0] - bestarray[1];
    Vector3D spaceHats[3];//to translate into enums without casting
    spaceHats[0] = ivec;
    spaceHats[1] = jvec;
    spaceHats[2] = kvec;
    for (int i = 0; i < 3; ++i)
    {
        bool neg = (spaceHats[i][bestarray[i]] < 0.0f);
        switch (bestarray[i])
        {
            case 0:
                if (neg)
                {
                    orientOut[i] = RIGHT_TO_LEFT;
                } else {
                    orientOut[i] = LEFT_TO_RIGHT;
                }
                break;
            case 1:
                if (neg)
                {
                    orientOut[i] = ANTERIOR_TO_POSTERIOR;
                } else {
                    orientOut[i] = POSTERIOR_TO_ANTERIOR;
                }
                break;
            case 2:
                if (neg)
                {
                    orientOut[i] = SUPERIOR_TO_INFERIOR;
                } else {
                    orientOut[i] = INFERIOR_TO_SUPERIOR;
                }
                break;
            default:
                CiftiAssert(0);
        }
    }
}

bool VolumeSpace::isPlumb() const
{
    char axisUsed = 0;
    char indexUsed = 0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (m_sform[i][j] != 0.0f)
            {
                if (axisUsed & (1<<i))
                {
                    return false;
                }
                if (indexUsed & (1<<j))
                {
                    return false;
                }
                axisUsed |= (1<<i);
                indexUsed |= (1<<j);
            }
        }
    }
    return true;
}

void VolumeSpace::readCiftiXML1(XmlReader& xml)
{
    vector<AString> mandAttrs(1, "VolumeDimensions"), transAttrs(3);
    transAttrs[0] = "UnitsXYZ";
    transAttrs[1] = "DataSpace";//we ignore the values in these, but they are required by cifti-1, so check that they exist
    transAttrs[2] = "TransformedSpace";
    XmlAttributesResult myAttrs = XmlReader_parseAttributes(xml, mandAttrs);
    vector<AString> dimStrings = AString_split(myAttrs.mandatoryVals[0], ',');
    if (dimStrings.size() != 3)
    {
        throw CiftiException("VolumeDimensions attribute of Volume must contain exactly two commas");
    }
    int64_t newDims[3];//don't parse directly into the internal variables
    bool ok = false;
    for (int i = 0; i < 3; ++i)
    {
        newDims[i] = AString_toInt(dimStrings[i], ok);
        if (!ok)
        {
            throw CiftiException("noninteger found in VolumeDimensions attribute of Volume: " + dimStrings[i]);
        }
        if (newDims[i] < 1)
        {
            throw CiftiException("found bad value in VolumeDimensions attribute of Volume: " + dimStrings[i]);
        }
    }
    if (!XmlReader_readNextStartElement(xml))
    {
        throw CiftiException("failed to find TransformationMatrixVoxelIndicesIJKtoXYZ element in Volume");
    }
    if (XmlReader_elementName(xml) != "TransformationMatrixVoxelIndicesIJKtoXYZ")
    {
        throw CiftiException("unexpected element in Volume: " + XmlReader_elementName(xml));
    }
    XmlAttributesResult transAttrsRes = XmlReader_parseAttributes(xml, transAttrs);
    float mult = 0.0f;
    if (transAttrsRes.mandatoryVals[0] == "NIFTI_UNITS_MM")
    {
        mult = 1.0f;
    } else if (transAttrsRes.mandatoryVals[0] == "NIFTI_UNITS_MICRON") {
        mult = 0.001f;
    } else {
        throw CiftiException("unrecognized value for UnitsXYZ in TransformationMatrixVoxelIndicesIJKtoXYZ: " + transAttrsRes.mandatoryVals[0]);
    }
    AString accum = XmlReader_readElementText(xml);
#ifdef CIFTILIB_USE_QT
    if (xml.hasError()) return;
#endif
    vector<AString> matrixStrings = AString_split_whitespace(accum);
    if (matrixStrings.size() != 16)
    {
        throw CiftiException("text content of TransformationMatrixVoxelIndicesIJKtoXYZ must have exactly 16 numbers separated by whitespace");
    }
    FloatMatrix newsform = FloatMatrix::zeros(4, 4);
    for (int j = 0; j < 4; ++j)
    {
        for (int i = 0; i < 4; ++i)
        {
            newsform[j][i] = AString_toFloat(matrixStrings[i + j * 4], ok);
            if (!ok)
            {
                throw CiftiException("non-number in text of TransformationMatrixVoxelIndicesIJKtoXYZ: " + matrixStrings[i + j * 4]);
            }
        }
    }
    if (newsform[3][0] != 0.0f || newsform[3][1] != 0.0f || newsform[3][2] != 0.0f || newsform[3][3] != 1.0f)//yes, using equals, because those are all exact in float
    {
        cerr << "last row of matrix in TransformationMatrixVoxelIndicesIJKtoXYZ is not 0 0 0 1" << endl;//not an exception, because some cifti-1 exist with this wrong
    }
    if (XmlReader_readNextStartElement(xml))//find Volume end element
    {
        throw CiftiException("unexpected element in Volume: " + XmlReader_elementName(xml));
    }
    newsform *= mult;//apply units
    newsform[3][3] = 1.0f;//reset [3][3], since it isn't spatial
    setSpace(newDims, newsform.getMatrix());
    CiftiAssert(XmlReader_checkEndElement(xml, "Volume"));
}

void VolumeSpace::readCiftiXML2(XmlReader& xml)
{//we changed stuff, so separate code
    vector<AString> mandAttrs(1, "VolumeDimensions"), transAttrs(1, "MeterExponent");
    XmlAttributesResult myAttrs = XmlReader_parseAttributes(xml, mandAttrs);
    vector<AString> dimStrings = AString_split(myAttrs.mandatoryVals[0], ',');
    if (dimStrings.size() != 3)
    {
        throw CiftiException("VolumeDimensions attribute of Volume must contain exactly two commas");
    }
    int64_t newDims[3];//don't parse directly into the internal variables
    bool ok = false;
    for (int i = 0; i < 3; ++i)
    {
        newDims[i] = AString_toInt(dimStrings[i], ok);
        if (!ok)
        {
            throw CiftiException("noninteger found in VolumeDimensions attribute of Volume: " + dimStrings[i]);
        }
        if (newDims[i] < 1)
        {
            throw CiftiException("found bad value in VolumeDimensions attribute of Volume: " + dimStrings[i]);
        }
    }
    if (!XmlReader_readNextStartElement(xml))
    {
        throw CiftiException("failed to find TransformationMatrixVoxelIndicesIJKtoXYZ element in Volume");
    }
    if (XmlReader_elementName(xml) != "TransformationMatrixVoxelIndicesIJKtoXYZ")
    {
        throw CiftiException("unexpected element in Volume: " + XmlReader_elementName(xml));
    }
    XmlAttributesResult transAttrsRes = XmlReader_parseAttributes(xml, transAttrs);
    int exponent = AString_toInt(transAttrsRes.mandatoryVals[0], ok);
    if (!ok)
    {
        throw CiftiException("noninteger value for MeterExponent in TransformationMatrixVoxelIndicesIJKtoXYZ: " + transAttrsRes.mandatoryVals[0]);
    }
    float mult = pow(10.0f, exponent + 3);//because our internal units are mm
    AString accum = XmlReader_readElementText(xml);
#ifdef CIFTILIB_USE_QT
    if (xml.hasError()) return;
#endif
    vector<AString> matrixStrings = AString_split_whitespace(accum);
    if (matrixStrings.size() != 16)
    {
        throw CiftiException("text content of TransformationMatrixVoxelIndicesIJKtoXYZ must have exactly 16 numbers separated by whitespace");
    }
    FloatMatrix newsform = FloatMatrix::zeros(4, 4);
    for (int j = 0; j < 4; ++j)
    {
        for (int i = 0; i < 4; ++i)
        {
            newsform[j][i] = AString_toFloat(matrixStrings[i + j * 4], ok);
            if (!ok)
            {
                throw CiftiException("non-number in text of TransformationMatrixVoxelIndicesIJKtoXYZ: " + matrixStrings[i + j * 4]);
            }
        }
    }
    if (newsform[3][0] != 0.0f || newsform[3][1] != 0.0f || newsform[3][2] != 0.0f || newsform[3][3] != 1.0f)//yes, using equals, because those are all exact in float
    {
        throw CiftiException("last row of matrix in TransformationMatrixVoxelIndicesIJKtoXYZ must be 0 0 0 1");
    }
    if (XmlReader_readNextStartElement(xml))//find Volume end element
    {
        throw CiftiException("unexpected element in Volume: " + XmlReader_elementName(xml));
    }
    newsform *= mult;//apply units
    newsform[3][3] = 1.0f;//reset [3][3], since it isn't spatial
    setSpace(newDims, newsform.getMatrix());
    CiftiAssert(XmlReader_checkEndElement(xml, "Volume"));
}

void VolumeSpace::writeCiftiXML1(XmlWriter& xml) const
{
    xml.writeStartElement("Volume");
    AString dimString = AString_number(m_dims[0]) + "," + AString_number(m_dims[1]) + "," + AString_number(m_dims[2]);
    xml.writeAttribute("VolumeDimensions", dimString);
    xml.writeStartElement("TransformationMatrixVoxelIndicesIJKtoXYZ");
    xml.writeAttribute("DataSpace", "NIFTI_XFORM_UNKNOWN");//meaningless attribute
    xml.writeAttribute("TransformedSpace", "NIFTI_XFORM_UNKNOWN");//removed in CIFTI-2, but apparently workbench has been writing this value for CIFTI-1
    xml.writeAttribute("UnitsXYZ", "NIFTI_UNITS_MM");//only other choice in cifti-1 is micron, which we will probably never need in cifti-1
    AString matrixString;
    for (int j = 0; j < 3; ++j)
    {
        matrixString += "\n";
        for (int i = 0; i < 4; ++i)
        {
            matrixString += AString_number_fixed(m_sform[j][i], 7) + " ";
        }
    }
    matrixString += "\n";
    for (int i = 0; i < 3; ++i)
    {
        matrixString += AString_number_fixed(0.0f, 7) + " ";
    }
    matrixString += AString_number_fixed(1.0f, 7);
    xml.writeCharacters(matrixString);
    xml.writeEndElement();//Transfor...
    xml.writeEndElement();//Volume
}

void VolumeSpace::writeCiftiXML2(XmlWriter& xml) const
{
    xml.writeStartElement("Volume");
    AString dimString = AString_number(m_dims[0]) + "," + AString_number(m_dims[1]) + "," + AString_number(m_dims[2]);
    xml.writeAttribute("VolumeDimensions", dimString);
    xml.writeStartElement("TransformationMatrixVoxelIndicesIJKtoXYZ");
    Vector3D vecs[4];
    getSpacingVectors(vecs[0], vecs[1], vecs[2], vecs[3]);
    float minLength = vecs[0].length();
    for (int i = 1; i < 3; ++i)
    {
        minLength = min(minLength, vecs[i].length());
    }
    int myExponent = -3;//if we have a singular spatial dimension somehow, just use mm
    if (minLength != 0.0f)
    {
        myExponent = 3 * (int)floor((log10(minLength) - log10(50.0f)) / 3.0f);//some magic to get the exponent that is a multiple of 3 that puts the length of the smallest spacing vector in [0.05, 50]
    }
    float multiplier = pow(10.0f, -3 - myExponent);//conversion factor from mm
    xml.writeAttribute("MeterExponent", AString_number(myExponent));
    AString matrixString;
    for (int j = 0; j < 3; ++j)
    {
        matrixString += "\n";
        for (int i = 0; i < 4; ++i)
        {
            matrixString += AString_number_fixed(m_sform[j][i] * multiplier, 7) + " ";
        }
    }
    matrixString += "\n";
    for (int i = 0; i < 3; ++i)
    {
        matrixString += AString_number_fixed(0.0f, 7) + " ";
    }
    matrixString += AString_number_fixed(1.0f, 7);//doesn't get multiplied, because it isn't spatial
    xml.writeCharacters(matrixString);
    xml.writeEndElement();//Transfor...
    xml.writeEndElement();//Volume
}
