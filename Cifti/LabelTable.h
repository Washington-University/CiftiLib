#ifndef __LABELTABLE_H__
#define __LABELTABLE_H__

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

#include "AString.h"

#include <map>
#include <set>
#include <vector>
#include <stdint.h>

#include "XmlAdapter.h"

namespace cifti {

class Label;
    
class LabelTable {

public:
    LabelTable();

    LabelTable(const LabelTable& glt);

    LabelTable& operator=(const LabelTable& glt);
    
    bool matches(const LabelTable& rhs, const bool checkColors = false, const bool checkCoords = false) const;
    
    bool operator==(const LabelTable& rhs) const { return matches(rhs, true); }
    
    bool operator!=(const LabelTable& rhs) const { return !((*this) == rhs); }
    
    virtual ~LabelTable();

private:
    void copyHelper(const LabelTable& glt);

public:
    void clear();

    std::map<int32_t,int32_t> append(const LabelTable& glt);

    int32_t addLabel(
                    const AString& labelName,
                    const float red,
                    const float green,
                    const float blue,
                    const float alpha);

    int32_t addLabel(
                    const AString& labelName,
                    const float red,
                    const float green,
                    const float blue);

    int32_t addLabel(
                    const AString& labelName,
                    const int32_t red,
                    const int32_t green,
                    const int32_t blue,
                    const int32_t alpha);

    int32_t addLabel(
                    const AString& labelName,
                    const int32_t red,
                    const int32_t green,
                    const int32_t blue);

    int32_t addLabel(const Label* glt);

    void deleteLabel(const int32_t key);

    void deleteLabel(const Label* label);

    void deleteUnusedLabels(const std::set<int32_t>& usedLabelKeys);

    void insertLabel(const Label* label);

    int32_t getLabelKeyFromName(const AString& name) const;

    const Label* getLabel(const AString& labelName) const;

    Label* getLabel(const AString& labelName);
    
    const Label* getLabel(const int32_t key) const;

    Label* getLabel(const int32_t key);
    
    int32_t getUnassignedLabelKey() const;

    int32_t getNumberOfLabels() const;

    AString getLabelName(const int32_t key) const;

    void setLabelName(
                    const int32_t key,
                    const AString& name);

    void setLabel(const int32_t key,
                    const AString& name,
                    const float red,
                    const float green,
                    const float blue,
                    const float alpha);

    void setLabel(const int32_t key,
                  const AString& name,
                  const float red,
                  const float green,
                  const float blue,
                  const float alpha,
                  const float x,
                  const float y, 
                  const float z);
    
    bool isLabelSelected(const int32_t key) const;

    void setLabelSelected(
                    const int32_t key,
                    const bool sel);

    void setSelectionStatusForAllLabels(const bool newStatus);

    float getLabelAlpha(const int32_t key) const;

    void getLabelColor(const int32_t key, float rgbaOut[4]) const;

    void setLabelColor(
                    const int32_t key,
                    const float color[4]);

    void createLabelsForKeys(const std::set<int32_t>& newKeys);

    void writeXML(XmlWriter& xmlWriter) const;

    void readXml(XmlReader& xml);

    std::set<int32_t> getKeys() const;

    void getKeys(std::vector<int32_t>& keysOut) const;

    void getKeysAndNames(std::map<int32_t, AString>& keysAndNamesOut) const;
    
    int32_t generateUnusedKey() const;
    
private:
    typedef std::map<int32_t, Label*> LABELS_MAP;
    typedef std::map<int32_t, Label*>::iterator LABELS_MAP_ITERATOR;
    typedef std::map<int32_t, Label*>::const_iterator LABELS_MAP_CONST_ITERATOR;

    LABELS_MAP labelsMap;

};

} // namespace

#endif // __LABELTABLE_H__
