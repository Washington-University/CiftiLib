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

#include "CiftiScalarsMap.h"

#include "CiftiAssert.h"
#include "CiftiException.h"

#include <iostream>

using namespace std;
using namespace cifti;

void CiftiScalarsMap::clear()
{
    m_maps.clear();
}

const MetaData& CiftiScalarsMap::getMapMetadata(const int64_t& index) const
{
    CiftiAssertVectorIndex(m_maps, index);
    return m_maps[index].m_metaData;
}

const AString& CiftiScalarsMap::getMapName(const int64_t& index) const
{
    CiftiAssertVectorIndex(m_maps, index);
    return m_maps[index].m_name;
}

void CiftiScalarsMap::setMapMetadata(const int64_t& index, const MetaData& mdIn)
{
    CiftiAssertVectorIndex(m_maps, index);
    m_maps[index].m_metaData = mdIn;
}

void CiftiScalarsMap::setMapName(const int64_t& index, const AString& mapName)
{
    CiftiAssertVectorIndex(m_maps, index);
    m_maps[index].m_name = mapName;
}

void CiftiScalarsMap::setLength(const int64_t& length)
{
    CiftiAssert(length > 0);
    m_maps.resize(length);
}

bool CiftiScalarsMap::approximateMatch(const CiftiMappingType& rhs) const
{
    switch (rhs.getType())
    {
        case SCALARS:
        case SERIES://maybe?
        case LABELS:
            return getLength() == rhs.getLength();
        default:
            return false;
    }
}

bool CiftiScalarsMap::operator==(const CiftiMappingType& rhs) const
{
    if (rhs.getType() != getType()) return false;
    const CiftiScalarsMap& myrhs = dynamic_cast<const CiftiScalarsMap&>(rhs);
    return (m_maps == myrhs.m_maps);
}

bool CiftiScalarsMap::ScalarMap::operator==(const CiftiScalarsMap::ScalarMap& rhs) const
{
    if (m_name != rhs.m_name) return false;
    return (m_metaData == rhs.m_metaData);
}

void CiftiScalarsMap::readXML1(XmlReader& xml)
{
    cerr << "parsing nonstandard scalars mapping type in cifti-1" << endl;
    clear();
#ifdef CIFTILIB_USE_QT
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if (xml.name() != "NamedMap")
                {
                    throw CiftiException("unexpected element in scalars map: " + xml.name().toString());
                }
                m_maps.push_back(ScalarMap());
                m_maps.back().readXML1(xml);
                if (xml.hasError()) return;
                break;
            }
            default:
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
                if (name == "NamedMap")
                {
                    m_maps.push_back(ScalarMap());
                    m_maps.back().readXML1(xml);
                } else {
                    throw CiftiException("unexpected element in scalars map: " + name);
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
    CiftiAssert(XmlReader_checkEndElement(xml, "MatrixIndicesMap"));
}

void CiftiScalarsMap::readXML2(XmlReader& xml)
{
    clear();
#ifdef CIFTILIB_USE_QT
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if (xml.name() != "NamedMap")
                {
                    throw CiftiException("unexpected element in scalars map: " + xml.name().toString());
                }
                m_maps.push_back(ScalarMap());
                m_maps.back().readXML2(xml);
                if (xml.hasError()) return;
                break;
            }
            default:
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
                if (name == "NamedMap")
                {
                    m_maps.push_back(ScalarMap());
                    m_maps.back().readXML2(xml);
                } else {
                    throw CiftiException("unexpected element in scalars map: " + name);
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
    CiftiAssert(XmlReader_checkEndElement(xml, "MatrixIndicesMap"));
}

void CiftiScalarsMap::ScalarMap::readXML1(XmlReader& xml)
{
    bool haveName = false, haveMetaData = false;
#ifdef CIFTILIB_USE_QT
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                QStringRef name = xml.name();
                if (name == "MetaData")
                {
                    if (haveMetaData)
                    {
                        throw CiftiException("MetaData specified multiple times in one NamedMap");
                    }
                    m_metaData.readCiftiXML1(xml);
                    if (xml.hasError()) return;
                    haveMetaData = true;
                } else if (name == "MapName") {
                    if (haveName)
                    {
                        throw CiftiException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = xml.readElementText();//raises error if element encountered
                    if (xml.hasError()) return;
                    haveName = true;
                } else {
                    throw CiftiException("unexpected element in NamedMap: " + name.toString());
                }
                break;
            }
            default:
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
                    if (haveMetaData)
                    {
                        throw CiftiException("MetaData specified multiple times in one NamedMap");
                    }
                    m_metaData.readCiftiXML1(xml);
                    haveMetaData = true;
                } else if (name == "MapName") {
                    if (haveName)
                    {
                        throw CiftiException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = XmlReader_readElementText(xml);//throws if element encountered
                    haveName = true;
                } else {
                    throw CiftiException("unexpected element in NamedMap: " + name);
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
    CiftiAssert(XmlReader_checkEndElement(xml, "NamedMap"));
    if (!haveName)
    {
        throw CiftiException("NamedMap missing required child element MapName");
    }
}

void CiftiScalarsMap::ScalarMap::readXML2(XmlReader& xml)
{
    bool haveName = false, haveMetaData = false;
#ifdef CIFTILIB_USE_QT
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                QStringRef name = xml.name();
                if (name == "MetaData")
                {
                    if (haveMetaData)
                    {
                        throw CiftiException("MetaData specified multiple times in one NamedMap");
                    }
                    m_metaData.readCiftiXML2(xml);
                    if (xml.hasError()) return;
                    haveMetaData = true;
                } else if (name == "MapName") {
                    if (haveName)
                    {
                        throw CiftiException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = xml.readElementText();//raises error if element encountered
                    if (xml.hasError()) return;
                    haveName = true;
                } else {
                    throw CiftiException("unexpected element in NamedMap: " + name.toString());
                }
                break;
            }
            default:
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
                    if (haveMetaData)
                    {
                        throw CiftiException("MetaData specified multiple times in one NamedMap");
                    }
                    m_metaData.readCiftiXML2(xml);
                    haveMetaData = true;
                } else if (name == "MapName") {
                    if (haveName)
                    {
                        throw CiftiException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = XmlReader_readElementText(xml);//throws if element encountered
                    haveName = true;
                } else {
                    throw CiftiException("unexpected element in NamedMap: " + name);
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
    CiftiAssert(XmlReader_checkEndElement(xml, "NamedMap"));
    if (!haveName)
    {
        throw CiftiException("NamedMap missing required child element MapName");
    }
}

void CiftiScalarsMap::writeXML1(XmlWriter& xml) const
{
    cerr << "writing nonstandard scalars mapping type in cifti-1" << endl;
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_SCALARS");
    int64_t numMaps = (int64_t)m_maps.size();
    for (int64_t i = 0; i < numMaps; ++i)
    {
        xml.writeStartElement("NamedMap");
        xml.writeTextElement("MapName", m_maps[i].m_name);
        m_maps[i].m_metaData.writeCiftiXML1(xml);
        xml.writeEndElement();
    }
}

void CiftiScalarsMap::writeXML2(XmlWriter& xml) const
{
    int64_t numMaps = (int64_t)m_maps.size();
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_SCALARS");
    for (int64_t i = 0; i < numMaps; ++i)
    {
        xml.writeStartElement("NamedMap");
        xml.writeTextElement("MapName", m_maps[i].m_name);
        m_maps[i].m_metaData.writeCiftiXML1(xml);
        xml.writeEndElement();
    }
}
