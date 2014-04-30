#ifndef __METADATA_H__
#define __METADATA_H__

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


#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <stdint.h>

#include <QString>

#include <map>
#include <stdexcept>
#include <vector>

namespace cifti {

class XmlWriter;
    
class MetaData {

public:
    MetaData();

public:
    MetaData(const MetaData& o);

    MetaData& operator=(const MetaData& o);
    
    bool operator==(const MetaData& rhs) const;
    
    bool operator!=(const MetaData& rhs) const { return !((*this) == rhs); }

    virtual ~MetaData();

private:
    void copyHelper(const MetaData& o);

    void initializeMembersMetaData();
    
public:
    void clear();

    void append(const MetaData& smd);

    void replace(const MetaData& smd);

    void set(
                    const QString& name,
                    const QString& value);

    void setInt(
                    const QString& name,
                    const int32_t value);

    void setFloat(
                    const QString& name,
                    const float value);

    void replaceWithMap(const std::map<QString, QString>& map);
    
    std::map<QString, QString> getAsMap() const;
    
    void remove(const QString& name);

    bool exists(const QString& name) const;

    QString get(const QString& name) const;

    int32_t getInt(const QString& name) const;

    float getFloat(const QString& name) const;

    std::vector<QString> getAllMetaDataNames() const;

    void writeCiftiXML1(QXmlStreamWriter& xmlWriter) const;
    void writeCiftiXML2(QXmlStreamWriter& xmlWriter) const;//for style, and in case it changes
    void readCiftiXML1(QXmlStreamReader& xml);
    void readCiftiXML2(QXmlStreamReader& xml);
    
private:
    void readEntry(QXmlStreamReader& xml);
    
    std::map<QString, QString> createTreeMap();

    void replaceName(const QString& oldName,
                     const QString& newName);

public:

private:
    /**the metadata storage. */
    std::map<QString, QString> metadata;    
    typedef std::map<QString, QString>::iterator MetaDataIterator;
    typedef std::map<QString, QString>::const_iterator MetaDataConstIterator;
    
};

} // namespace

#endif // __METADATA_H__
