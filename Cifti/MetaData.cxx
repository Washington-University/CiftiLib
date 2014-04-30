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

#include "CaretAssert.h"
#include "MetaData.h"

using namespace cifti;

/**
 * Constructor.
 *
 */
MetaData::MetaData()
{
    this->initializeMembersMetaData();
}

/**
 * Destructor
 */
MetaData::~MetaData()
{
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
MetaData::MetaData(const MetaData& o)
{
    this->initializeMembersMetaData();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
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
MetaData::set(const QString& name,
                   const QString& value)
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
                   const QString& name,
                   const int32_t value)
{
    QString s = QString::number(value);
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
                   const QString& name,
                   const float value)
{
    QString s = QString::number(value);
    this->set(name, s);
}

/**
 * Replace ALL of the metadata with the data in the given map.
 *
 * @param map
 *     New metadata that replaces all existing metadata.
 */
void
MetaData::replaceWithMap(const std::map<QString, QString>& map)
{
    this->metadata = map;
}

/**
 * @return ALL of the metadata in map.
 */
std::map<QString, QString>
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
MetaData::remove(const QString& name)
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
MetaData::exists(const QString& name) const
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
QString
MetaData::get(const QString& name) const
{
    MetaDataConstIterator iter = this->metadata.find(name);
    if (iter != this->metadata.end()) {
        return iter->second;
    }
    return "";
}

/**
 * Get the metadata as an integer value.  If the metadata does not exist
 * of its string representation is not a number, zero is returned.
 * @param name - name of metadata.
 * @return  Integer value associated with the metadata.
 *
 */
int32_t
MetaData::getInt(const QString& name) const
{
    QString s = this->get(name);
    if (s.length() > 0) {
        int32_t i = s.toInt();
        return i;
    }
    return 0;
}

/**
 * Get the metadata as an float value.  If the metadata does not exist
 * of its string representation is not a number, zero is returned.
 * @param name - name of metadata.
 * @return  Float value associated with the metadata.
 *
 */
float
MetaData::getFloat(const QString& name) const
{
    QString s = this->get(name);
    if (s.length() > 0) {
        float f = s.toFloat();
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
std::vector<QString>
MetaData::getAllMetaDataNames() const
{
    std::vector<QString> names;
    
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
                   const QString& oldName,
                   const QString& newName)
{
    MetaDataIterator iter = this->metadata.find(oldName);
    if (iter != this->metadata.end()) {
        QString value = iter->second;
        this->remove(oldName);
        this->set(newName, value);
    }
}

void MetaData::writeCiftiXML1(QXmlStreamWriter& xmlWriter) const
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

void MetaData::writeCiftiXML2(QXmlStreamWriter& xmlWriter) const
{
    writeCiftiXML1(xmlWriter);
}

void MetaData::readCiftiXML1(QXmlStreamReader& xml)
{
    clear();
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
                xml.raiseError("unexpected tag name in MD: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
}

void MetaData::readCiftiXML2(QXmlStreamReader& xml)
{
    readCiftiXML1(xml);
}

void MetaData::readEntry(QXmlStreamReader& xml)
{
    QString key, value;
    bool haveKey = false, haveValue = false;
    while (!xml.atEnd())//don't check the current element's name
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            QStringRef name = xml.name();
            if (name == "Name")
            {
                key = xml.readElementText();
                haveKey = true;
            } else if (name == "Value") {
                value = xml.readElementText();
                haveValue = true;
            } else {
                xml.raiseError("unexpected tag name in MD: " + name.toString());
            }
        } else if (xml.isEndElement()) {
            if (haveKey && haveValue)
            {
                if (exists(key))
                {
                    xml.raiseError("key '" + key + "' used more than once in MetaData");
                } else {
                    set(key, value);
                }
            } else {
                if (haveKey)
                {
                    xml.raiseError("MD element has no Value element");
                } else {
                    if (haveValue)
                    {
                        xml.raiseError("MD element has no Name element");
                    } else {
                        xml.raiseError("MD element has no Name or Value element");
                    }
                }
            }
            break;
        }
    }
}
