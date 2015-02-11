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

#include <algorithm>

#include "CiftiAssert.h"
#include "CiftiException.h"
#include "MetaData.h"

using namespace cifti;

MetaData::MetaData()
{
    this->initializeMembersMetaData();
}

MetaData::~MetaData()
{
}

MetaData::MetaData(const MetaData& o)
{
    this->initializeMembersMetaData();
    this->copyHelper(o);
}

MetaData&
MetaData::operator=(const MetaData& o)
{
    if (this != &o) {
        this->copyHelper(o);
    };
    return *this;
}

bool MetaData::operator==(const MetaData& rhs) const
{
    return (metadata == rhs.metadata);
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
MetaData::copyHelper(const MetaData& o)
{
    this->metadata = o.metadata;
}

void
MetaData::initializeMembersMetaData()
{
}

/**
 * Clear the metadata.
 *
 */
void
MetaData::clear()
{
    metadata.clear();
}

/**
 * Append the metadata to this metadata.  A comment is always appended.
 * Other metadata are added only if the name is not in "this" metadata.
 *
 * @param smd   Metadata that is to be appended to "this".
 *
 */
void
MetaData::append(const MetaData& smd)
{
    for (MetaDataConstIterator iter = smd.metadata.begin();
         iter != smd.metadata.end();
         iter++) {
            this->set(iter->first, iter->second);
    }
}

/**
 * Clears this metadata and then copies all metadata from "smd"
 *
 * @param smd   Metadata that is to be copied to "this".
 *
 */
void
MetaData::replace(const MetaData& smd)
{
    this->metadata = smd.metadata;
}

/**
 * Sets metadata.  If a metadata entry named "name" already
 * exists, it is replaced.  
 *
 * @param name   Name of metadata entry.
 * @param value  Value for metadata entry.
 *
 */
void
MetaData::set(const AString& name,
                   const AString& value)
{
    MetaDataIterator namePos = this->metadata.find(name);
    if (namePos != this->metadata.end()) {
        if (namePos->second != value) {
            namePos->second = value;
        }
    }
    else {
        this->metadata.insert(std::make_pair(name, value));
    }
}

/**
 * Set metadata with an integer value.
 * @param name - name of metadata.
 * @param value - value of metadata.
 *
 */
void
MetaData::setInt(
                   const AString& name,
                   const int32_t value)
{
    AString s = AString_number(value);
    this->set(name, s);
}

/**
 * Set metadata with an float value.
 * @param name - name of metadata.
 * @param value - value of metadata.
 *
 */
void
MetaData::setFloat(
                   const AString& name,
                   const float value)
{
    AString s = AString_number(value);
    this->set(name, s);
}

/**
 * Replace ALL of the metadata with the data in the given map.
 *
 * @param map
 *     New metadata that replaces all existing metadata.
 */
void
MetaData::replaceWithMap(const std::map<AString, AString>& map)
{
    this->metadata = map;
}

/**
 * @return ALL of the metadata in map.
 */
std::map<AString, AString>
MetaData::getAsMap() const
{
    return this->metadata;
}


/**
 * Remove a metadata entry.
 *
 * @param name   Name of metadata entry that is to be removed.
 *
 */
void
MetaData::remove(const AString& name)
{
    this->metadata.erase(name);
}

/**
 * See if a metadata entry "name" exists.
 *
 * @param  name  Name of metadata entry.
 * @return   Returns true if the metadata entry "name" exists, else false.
 *
 */
bool
MetaData::exists(const AString& name) const
{
    if (this->metadata.find(name) != this->metadata.end()) {
        return true;
    }
    return false;
}

/**
 * Get a value for metadata entry.  
 *
 * @param  name  Name of metadata entry.
 * @return       The value of the metadata entry "name".  If the
 *               metadata entry "name" does not exist an empty
 *               string is returned.
 *
 */
AString
MetaData::get(const AString& name) const
{
    MetaDataConstIterator iter = this->metadata.find(name);
    if (iter != this->metadata.end()) {
        return iter->second;
    }
    return "";
}

/**
 * Get the metadata as an integer value.  If the metadata does not exist
 * or its string representation is not a number, zero is returned.
 * @param name - name of metadata.
 * @param ok - is set to false if key not found, or value not integer
 * @return  Integer value associated with the metadata.
 *
 */
int32_t
MetaData::getInt(const AString& name, bool& ok) const
{
    ok = false;
    AString s = this->get(name);
    if (s.length() > 0) {
        int32_t i = AString_toInt(s, ok);
        return i;
    }
    return 0;
}

/**
 * Get the metadata as an float value.  If the metadata does not exist
 * or its string representation is not a number, zero is returned.
 * @param name - name of metadata.
 * @param ok - is set to false if key not found, or value not numeric
 * @return  Float value associated with the metadata.
 *
 */
float
MetaData::getFloat(const AString& name, bool& ok) const
{
    ok = false;
    AString s = this->get(name);
    if (s.length() > 0) {
        float f = AString_toFloat(s, ok);
        return f;
    }
    return 0.0f;
}

/**
 * Get names of all metadata.
 *
 * @return List of all metadata names.
 *
 */
std::vector<AString>
MetaData::getAllMetaDataNames() const
{
    std::vector<AString> names;
    
    for (MetaDataConstIterator iter = this->metadata.begin();
         iter != this->metadata.end();
         iter++) {
        names.push_back(iter->first);   
    }
    return names;
}

/**
 * Replace a metadata name.
 * @param oldName - old name of metadata.
 * @param newName - new name of metadata.
 *
 */
void
MetaData::replaceName(
                   const AString& oldName,
                   const AString& newName)
{
    MetaDataIterator iter = this->metadata.find(oldName);
    if (iter != this->metadata.end()) {
        AString value = iter->second;
        this->remove(oldName);
        this->set(newName, value);
    }
}

void MetaData::writeCiftiXML1(XmlWriter& xmlWriter) const
{
    if (metadata.empty()) return;//don't write an empty tag if we have no metadata
    xmlWriter.writeStartElement("MetaData");
    for (MetaDataConstIterator iter = metadata.begin(); iter != metadata.end(); ++iter)
    {
        xmlWriter.writeStartElement("MD");
        xmlWriter.writeTextElement("Name", iter->first);
        xmlWriter.writeTextElement("Value", iter->second);
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
}

void MetaData::writeCiftiXML2(XmlWriter& xmlWriter) const
{
    writeCiftiXML1(xmlWriter);
}

void MetaData::readCiftiXML1(XmlReader& xml)
{
    clear();
#ifdef CIFTILIB_USE_QT
    while (!xml.atEnd())//don't check the current element's name
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "MD")
            {
                readEntry(xml);
            } else {
                throw CiftiException("unexpected tag name in MetaData: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
#else
#ifdef CIFTILIB_USE_XMLPP
    bool done = xml.is_empty_element();//NOTE: because libxml++ will NOT give a separate close element for <MetaData/>!!!
    while (!done && xml.read())//false means no node was available to read, it will throw on malformed xml
    {
        switch (xml.get_node_type())
        {
            case XmlReader::Element:
            {
                AString name = xml.get_local_name();
                if (name == "MD")
                {
                    readEntry(xml);
                } else {
                    throw CiftiException("unexpected tag name in MetaData: " + name);
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
    CiftiAssert(XmlReader_checkEndElement(xml, "MetaData"));
}

void MetaData::readCiftiXML2(XmlReader& xml)
{
    readCiftiXML1(xml);
}

void MetaData::readEntry(XmlReader& xml)
{
    AString key, value;
    bool haveKey = false, haveValue = false;
#ifdef CIFTILIB_USE_QT
    while (!xml.atEnd())//don't check the current element's name
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "Name")
            {
                if (haveKey) throw CiftiException("MD element has multiple Name elements");
                key = xml.readElementText();
                haveKey = true;
            } else if (name == "Value") {
                if (haveValue) throw CiftiException("MD element has multiple Value elements");
                value = xml.readElementText();
                haveValue = true;
            } else {
                throw CiftiException("unexpected element name in MD: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
#else
#ifdef CIFTILIB_USE_XMLPP
    bool done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
    while (!done && xml.read())
    {
        switch (xml.get_node_type())
        {
            case XmlReader::Element:
            {
                AString name = xml.get_local_name();
                if (name == "Name")
                {
                    if (haveKey) throw CiftiException("MD element has multiple Name elements");
                    key = XmlReader_readElementText(xml);
                    haveKey = true;
                } else if (name == "Value") {
                    if (haveValue) throw CiftiException("MD element has multiple Value elements");
                    name = XmlReader_readElementText(xml);
                    haveValue = true;
                } else {
                    throw CiftiException("unexpected element name in MD: " + name);
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
    if (haveKey && haveValue)
    {
        if (exists(key))
        {
            throw CiftiException("key '" + key + "' used more than once in MetaData");
        } else {
            set(key, value);
        }
    } else {
        if (haveKey)
        {
            throw CiftiException("MD element has no Value element");
        } else {
            if (haveValue)
            {
                throw CiftiException("MD element has no Name element");
            } else {
                throw CiftiException("MD element has no Name or Value element");
            }
        }
    }
    CiftiAssert(XmlReader_checkEndElement(xml, "MD"));
}
