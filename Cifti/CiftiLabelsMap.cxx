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

#include "CiftiLabelsMap.h"

#include "CiftiAssert.h"
#include "CiftiException.h"

#include <iostream>

using namespace std;
using namespace cifti;

void CiftiLabelsMap::clear()
{
    m_maps.clear();
}

const MetaData& CiftiLabelsMap::getMapMetadata(const int64_t& index) const
{
    CiftiAssertVectorIndex(m_maps, index);
    return m_maps[index].m_metaData;
}

const LabelTable& CiftiLabelsMap::getMapLabelTable(const int64_t& index) const
{
    CiftiAssertVectorIndex(m_maps, index);
    return m_maps[index].m_labelTable;
}

const AString& CiftiLabelsMap::getMapName(const int64_t& index) const
{
    CiftiAssertVectorIndex(m_maps, index);
    return m_maps[index].m_name;
}

void CiftiLabelsMap::setMapMetadata(const int64_t& index, const MetaData& md)
{
    CiftiAssertVectorIndex(m_maps, index);
    m_maps[index].m_metaData = md;
}

void CiftiLabelsMap::setMapLabelTable(const int64_t& index, const LabelTable& lt)
{
    CiftiAssertVectorIndex(m_maps, index);
    m_maps[index].m_labelTable = lt;
}

void CiftiLabelsMap::setMapName(const int64_t& index, const AString& mapName)
{
    CiftiAssertVectorIndex(m_maps, index);
    m_maps[index].m_name = mapName;
}

void CiftiLabelsMap::setLength(const int64_t& length)
{
    CiftiAssert(length > 0);
    m_maps.resize(length);
}

bool CiftiLabelsMap::approximateMatch(const CiftiMappingType& rhs) const
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

bool CiftiLabelsMap::operator==(const CiftiMappingType& rhs) const
{
    if (rhs.getType() != getType()) return false;
    const CiftiLabelsMap& myrhs = dynamic_cast<const CiftiLabelsMap&>(rhs);
    return (m_maps == myrhs.m_maps);
}

bool CiftiLabelsMap::LabelMap::operator==(const LabelMap& rhs) const
{
    if (m_name != rhs.m_name) return false;
    if (m_labelTable != rhs.m_labelTable) return false;
    return (m_metaData == rhs.m_metaData);
}

void CiftiLabelsMap::readXML1(XmlReader& xml)
{
    cerr << "parsing nonstandard labels mapping type in cifti-1" << endl;
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
                    throw CiftiException("unexpected element in labels map: " + xml.name().toString());
                }
                LabelMap tempMap;
                tempMap.readXML1(xml);
                if (xml.hasError()) return;
                m_maps.push_back(tempMap);
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
                    LabelMap tempMap;
                    tempMap.readXML1(xml);
                    m_maps.push_back(tempMap);
                } else {
                    throw CiftiException("unexpected element in labels map: " + name);
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

void CiftiLabelsMap::readXML2(XmlReader& xml)
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
                    throw CiftiException("unexpected element in labels mapping type: " + xml.name().toString());
                }
                LabelMap tempMap;
                tempMap.readXML2(xml);
                if (xml.hasError()) return;
                m_maps.push_back(tempMap);
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
                    LabelMap tempMap;
                    tempMap.readXML2(xml);
                    m_maps.push_back(tempMap);
                } else {
                    throw CiftiException("unexpected element in labels mapping type: " + name);
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

void CiftiLabelsMap::LabelMap::readXML1(XmlReader& xml)
{
    bool haveName = false, haveTable = false, haveMetaData = false;
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
                } else if (name == "LabelTable") {
                    if (haveTable)
                    {
                        throw CiftiException("LabelTable specified multiple times in one NamedMap");
                    }
                    m_labelTable.readXml(xml);
                    if (xml.hasError()) return;
                    haveTable = true;
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
                } else if (name == "LabelTable") {
                    if (haveTable)
                    {
                        throw CiftiException("LabelTable specified multiple times in one NamedMap");
                    }
                    m_labelTable.readXml(xml);
                    haveTable = true;
                } else if (name == "MapName") {
                    if (haveName)
                    {
                        throw CiftiException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = XmlReader_readElementText(xml);//raises error if element encountered
                    haveName = true;
                } else {
                    throw CiftiException("unexpected element in labels mapping type: " + name);
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
    if (!haveTable)
    {
        throw CiftiException("NamedMap in labels mapping missing required child element LabelTable");
    }
}

void CiftiLabelsMap::LabelMap::readXML2(XmlReader& xml)
{
    bool haveName = false, haveTable = false, haveMetaData = false;
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
                } else if (name == "LabelTable") {
                    if (haveTable)
                    {
                        throw CiftiException("LabelTable specified multiple times in one NamedMap");
                    }
                    m_labelTable.readXml(xml);
                    if (xml.hasError()) return;
                    haveTable = true;
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
                } else if (name == "LabelTable") {
                    if (haveTable)
                    {
                        throw CiftiException("LabelTable specified multiple times in one NamedMap");
                    }
                    m_labelTable.readXml(xml);
                    haveTable = true;
                } else if (name == "MapName") {
                    if (haveName)
                    {
                        throw CiftiException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = XmlReader_readElementText(xml);//raises error if element encountered
                    haveName = true;
                } else {
                    throw CiftiException("unexpected element in labels mapping type: " + name);
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
    if (!haveTable)
    {
        throw CiftiException("NamedMap in labels mapping missing required child element LabelTable");
    }
}

void CiftiLabelsMap::writeXML1(XmlWriter& xml) const
{
    cerr << "writing nonstandard labels mapping type in cifti-1" << endl;
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_LABELS");
    int64_t numMaps = (int64_t)m_maps.size();
    for (int64_t i = 0; i < numMaps; ++i)
    {
        xml.writeStartElement("NamedMap");
        xml.writeTextElement("MapName", m_maps[i].m_name);
        m_maps[i].m_metaData.writeCiftiXML1(xml);
        m_maps[i].m_labelTable.writeXML(xml);
        xml.writeEndElement();
    }
}

void CiftiLabelsMap::writeXML2(XmlWriter& xml) const
{
    int64_t numMaps = (int64_t)m_maps.size();
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_LABELS");
    for (int64_t i = 0; i < numMaps; ++i)
    {
        xml.writeStartElement("NamedMap");
        xml.writeTextElement("MapName", m_maps[i].m_name);
        m_maps[i].m_metaData.writeCiftiXML2(xml);
        m_maps[i].m_labelTable.writeXML(xml);
        xml.writeEndElement();
    }
}
