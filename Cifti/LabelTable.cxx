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
#include <sstream>
#include <iostream>

#include "CiftiAssert.h"
#include "Label.h"
#include "LabelTable.h"

using namespace std;
using namespace cifti;

/**
 * Constructor.
 *
 */
LabelTable::LabelTable()
{
    clear();//actually adds the 0: ??? label
}

/**
 * Destructor
 */
LabelTable::~LabelTable()
{
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != labelsMap.end();
         iter++) {
        delete iter->second;
    }
    this->labelsMap.clear();
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
LabelTable::LabelTable(const LabelTable& glt)
{
    this->copyHelper(glt);
}

/**
 * Assignment operator.
 */
LabelTable&
LabelTable::operator=(const LabelTable& glt)
{
    if (this != &glt) {
        this->copyHelper(glt);
    };
    return *this;
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
LabelTable::copyHelper(const LabelTable& glt)
{
    this->clear();
    
    for (LABELS_MAP_CONST_ITERATOR iter = glt.labelsMap.begin();
         iter != glt.labelsMap.end();
         iter++) {
        Label* myLabel = this->getLabel(iter->second->getKey());
        if (myLabel != NULL)
        {
            *myLabel = *(iter->second);
        } else {
            addLabel(iter->second);
        }
    }
}

/**
 * Clear the labelTable.
 *
 */
void
LabelTable::clear()
{
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != labelsMap.end();
         iter++) {
        delete iter->second;
    }
    this->labelsMap.clear();
    
    Label gl(0, "???", 1.0, 1.0, 1.0, 0.0);
    this->addLabel(&gl);
}

/**
 * Append a label table to this label table.  Since labels may be
 * duplicated, the map returned that converts the keys of
 * the appended LabelTable to keys for "this" label table.
 *
 * @param lt  Label table that is to be appended.
 *
 * @return  A map where the keys are the keys in the label table
 *    that is passed as a parameter and the values are the keys
 *    into "this" label table.
 *
 */
std::map<int32_t,int32_t>
LabelTable::append(const LabelTable& glt)
{
    std::map<int32_t,int32_t> keyConverterMap;
    
    for (LABELS_MAP_CONST_ITERATOR iter = glt.labelsMap.begin();
         iter != glt.labelsMap.end();
         iter++) {
        int32_t key = iter->first;
        int32_t newKey = this->addLabel(iter->second);
        
        keyConverterMap.insert(std::make_pair(key, newKey));
    }
    return keyConverterMap;
}

/**
 * Add a label.  If a label with the name exists, its colors
 * are replaced with these color components.
 * @param labelName Name of label.
 * @param red  Red color component ranging 0.0 to 1.0.
 * @param green Green color component ranging 0.0 to 1.0.
 * @param blue Blue color component ranging 0.0 to 1.0.
 * @param alpha Alpha color component ranging 0.0 to 1.0.
 * @return  Index of the existing label, or, if no label 
 * exists with name, index of new label.
 *
 */
int32_t
LabelTable::addLabel(
                   const AString& labelName,
                   const float red,
                   const float green,
                   const float blue,
                   const float alpha)
{
    const Label gl(Label::getInvalidLabelKey(), labelName, red, green, blue, alpha);
    return this->addLabel(&gl);
}

/**
 * Add a label.  If a label with the name exists, its colors
 * are replaced with these color components.
 * @param labelName Name of label.
 * @param red  Red color component ranging 0.0 to 1.0.
 * @param green Green color component ranging 0.0 to 1.0.
 * @param blue Blue color component ranging 0.0 to 1.0.
 * @return  Index of the existing label, or, if no label 
 * exists with name, index of new label.
 *
 */
int32_t
LabelTable::addLabel(
                   const AString& labelName,
                   const float red,
                   const float green,
                   const float blue)
{
    return this->addLabel(labelName, red, green, blue, 1.0f);
}

/**
 * Add a label.  If a label with the name exists, its colors
 * are replaced with these color components.
 * @param labelName Name of label.
 * @param red  Red color component ranging 0 to 255.
 * @param green Green color component ranging 0 to 255.
 * @param blue Blue color component ranging 0 to 255.
 * @param alpha Alpha color component ranging 0 to 255.
 * @return  Index of the existing label, or, if no label 
 * exists with name, index of new label.
 *
 */
int32_t
LabelTable::addLabel(
                   const AString& labelName,
                   const int32_t red,
                   const int32_t green,
                   const int32_t blue,
                   const int32_t alpha)
{
    const Label gl(Label::getInvalidLabelKey(), labelName, red, green, blue, alpha);
    return this->addLabel(&gl);
}

/**
 * Add a label.  If a label with the name exists, its colors
 * are replaced with these color components.
 * @param labelName Name of label.
 * @param red  Red color component ranging 0 to 255.
 * @param green Green color component ranging 0 to 255.
 * @param blue Blue color component ranging 0 to 255.
 * @return  Index of the existing label, or, if no label 
 * exists with name, index of new label.
 *
 */
int32_t
LabelTable::addLabel(
                   const AString& labelName,
                   const int32_t red,
                   const int32_t green,
                   const int32_t blue)
{
    return this->addLabel(labelName, red, green, blue, 255);
}

/**
 * Add a label to the label table.  If the label's key is already in
 * the label table, a new key is created.  If a label of the same
 * name already exists, the key of the existing label is returned
 * and its color is overridden.
 * @param glIn - Label to add.
 * @return  Key of the label, possibly different than its original key.
 *
 */
int32_t
LabelTable::addLabel(const Label* glIn)
{
    /*
     * First see if a label with the same name already exists
     */
    int32_t key = this->getLabelKeyFromName(glIn->getName());
    
    /*
     * If no label with the name exists, get the key
     * (which may be invalid) from the input label,
     * and check that nothing uses that key
     */
    if (key == Label::getInvalidLabelKey()) {
        int32_t tempkey = glIn->getKey();
        LABELS_MAP_ITERATOR iter = this->labelsMap.find(tempkey);
        if (iter == labelsMap.end())
        {
            key = tempkey;
        }
    }
    
    /*
     * Still need a key, find an unused key
     */
    if (key == Label::getInvalidLabelKey()) {
        key = this->generateUnusedKey();
        
        Label* gl = new Label(*glIn);
        gl->setKey(key);
        this->labelsMap.insert(std::make_pair(key, gl));
        return key;
    }
    
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        /*
         * Update existing label
         */
        Label* gl = iter->second;
        gl->setName(glIn->getName());
        float rgba[4];
        glIn->getColor(rgba);
        gl->setColor(rgba);
        key = iter->first;
    }
    else {
        /*
         * Insert a new label
         */
        this->labelsMap.insert(std::make_pair(key, new Label(*glIn)));
    }
    return key;
}

/**
 * Generate an unused key.
 * @return An unused key.
 */ 
int32_t 
LabelTable::generateUnusedKey() const
{
    const int32_t numKeys = labelsMap.size();
    LABELS_MAP::const_reverse_iterator rbegin = labelsMap.rbegin();//reverse begin is largest key
    if (numKeys > 0 && rbegin->first > 0)//there is at least one positive key
    {
        if (rbegin->first < numKeys)
        {
            CiftiAssert(labelsMap.find(rbegin->first + 1) == labelsMap.end());
            return rbegin->first + 1;//keys are compact unless negatives exist, in which case consider it "compact enough" if positive holes equal number of negative keys
        } else {
            LABELS_MAP::const_iterator begin = labelsMap.begin();
            if (begin->first == 1 && rbegin->first == numKeys)
            {
                CiftiAssert(labelsMap.find(rbegin->first + 1) == labelsMap.end());
                return rbegin->first + 1;//keys are compact but missing 0, do not return 0, so return next
            } else {//there aren't enough negatives to make up for the missing, search for a hole in the positives
                LABELS_MAP::const_iterator iter = labelsMap.upper_bound(0);//start with first positive
                int32_t curVal = 0;//if it isn't one, we can stop early
                while (iter != labelsMap.end() && iter->first == curVal + 1)//it should NEVER hit end(), due to above checks, but if it did, it would return rbegin->first + 1
                {
                    curVal = iter->first;
                    ++iter;
                }
                CiftiAssert(labelsMap.find(curVal + 1) == labelsMap.end());
                return curVal + 1;
            }
        }
    } else {
        CiftiAssert(labelsMap.find(1) == labelsMap.end());
        return 1;//otherwise, no keys exist or all keys are non-positive, return 1
    }
}

/**
 * Remove the label with the specified key.
 * @param key - key of label.
 *
 */
void
LabelTable::deleteLabel(const int32_t key)
{
   if (key == 0)
   {//key 0 is reserved (sort of)
      cerr << "Label 0 DELETED!" << endl;
   }
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        this->labelsMap.erase(iter);
        delete iter->second;
    }
}

/**
 * Remove a label from the label table.
 * This method WILL DELETE the label passed
 * in so the caller should never use the parameter
 * passed after this call.
 * @param label - label to remove.
 *
 */
void
LabelTable::deleteLabel(const Label* label)
{
   if (label->getKey() == 0)
   {//key 0 is reserved (sort of)
      cerr << "Label 0 DELETED!" << endl;
   }
    for (LABELS_MAP_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        if (iter->second == label) {
            this->labelsMap.erase(iter);
            break;
        }
    }
    delete label;
}

/**
 * Remove unused labels from the label table.  Note that the unassigned
 * label is not removed, even if it is unused.
 *
 * @param usedLabelKeys - Color keys that are in use.
 *
 */
void
LabelTable::deleteUnusedLabels(const std::set<int32_t>& usedLabelKeys)
{
    LABELS_MAP newMap;
    int32_t unassignedKey = getUnassignedLabelKey();
    for (LABELS_MAP_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        int32_t key = iter->first;
        Label* gl = iter->second;
        if (key == unassignedKey || usedLabelKeys.find(key) != usedLabelKeys.end()) {//unassigned key gets a free pass
            newMap.insert(std::make_pair(key, gl));
        }
        else {
            delete gl;
        }
    }
    
    this->labelsMap = newMap;
}

/**
 * Insert the label using the labels key.
 * @param labelIn - Label to insert (replaces an existing label
 *    with the same key).
 *
 */
void
LabelTable::insertLabel(const Label* labelIn)
{
    Label* label = new Label(*labelIn);
    int32_t key = label->getKey();
    if (key == Label::getInvalidLabelKey()) {
        key = this->generateUnusedKey();
        label->setKey(key);
    }
    /*
     * Note: A map DOES NOT replace an existing key, so it
     * must be deleted and then added.
     */
    LABELS_MAP_ITERATOR keyPos = this->labelsMap.find(label->getKey());
    if (keyPos != this->labelsMap.end()) {
        Label* gl = keyPos->second;
        this->labelsMap.erase(keyPos);
        delete gl;
    }
        
    this->labelsMap.insert(std::make_pair(label->getKey(), label));
}

/**
 * Get the key of a lable from its name.
 * @param name   Name to search for.
 * @return       Key of Name or Label::getInvalidLabelKey() if not found.
 *
 */
int32_t
LabelTable::getLabelKeyFromName(const AString& name) const
{
    LABELS_MAP newMap;
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        int32_t key = iter->first;
        Label* gl = iter->second;
        if (gl->getName() == name) {
            return key;
        }
    }
    return Label::getInvalidLabelKey();
}

/**
 * Get a Label from its name.
 * @param labelName - Name of label that is sought.
 * @return  Reference to label with name or null if no matching label.
 *
 */
const Label*
LabelTable::getLabel(const AString& labelName) const
{
    LABELS_MAP newMap;
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        Label* gl = iter->second;
        if (gl->getName() == labelName) {
            return gl;
        }
    }
    return NULL;
}

/**
 * Get a Label from its name.
 * @param labelName - Name of label that is sought.
 * @return  Reference to label with name or null if no matching label.
 *
 */
Label*
LabelTable::getLabel(const AString& labelName)
{
    LABELS_MAP newMap;
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        Label* gl = iter->second;
        if (gl->getName() == labelName) {
            return gl;
        }
    }
    return NULL;
}

/**
 * Get the Label at the specified key.
 *
 * @param  key - Key of Label entry.
 * @return       The Label at the specified key or null if the
 *    there is not a label at the specified key.
 *
 */
const Label*
LabelTable::getLabel(const int32_t key) const
{
    LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        return iter->second;
    }
    return NULL;
}

/**
 * Get the Label at the specified key.
 *
 * @param  key - Key of Label entry.
 * @return       The Label at the specified key or null if the
 *    there is not a label at the specified key.
 */
Label*
LabelTable::getLabel(const int32_t key)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        return iter->second;
    }
    return NULL;
}

/**
 * Get the key for the unassigned label.
 * @return  Index of key for unassigned label.
 *          A valid key will always be returned.
 *
 */
int32_t
LabelTable::getUnassignedLabelKey() const
{
    const Label* gl = this->getLabel("???");
    if (gl != NULL) {
        return gl->getKey();
    }

    /*
     * Remove 'constness' from this object so that the 
     * label can be added.
     */
    LabelTable* glt = (LabelTable*)this;
    const int32_t key = glt->addLabel("???", 0.0f, 0.0f, 0.0f, 0.0f);
    return key;
}

/**
 * Get the number of labels.  This value is one greater than the last
 * label key.  Note that not every key may have a label.  If there
 * are no labels this returns 0.
 * @return  Number of labels.
 *
 */
int32_t
LabelTable::getNumberOfLabels() const
{
    return this->labelsMap.size();
}

/**
 * Get the name of the label at the key.  If there is no label at the
 * key an empty string is returned.
 * @param key - key of label.
 * @return  Name of label at inkeydex.
 *
 */
AString
LabelTable::getLabelName(const int32_t key) const
{
    LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        const AString name = iter->second->getName();
        return name;
    }
    return "";
}

/**
 * Set the name of a label.
 * @param key - key of label.
 * @param name - new name of label.
 *
 */
void
LabelTable::setLabelName(
                   const int32_t key,
                   const AString& name)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        iter->second->setName(name);
    }
}

/**
 * Set a label.  If a label with the specified key exists,
 * it is replaced.
 * 
 * @param key    Key for label.
 * @param name   Name of label.
 * @param red    Red color component.
 * @param green  Green color component.
 * @param blue   Blue color component.
 * @param alpha  Alpha color component.
 *
 */
void
LabelTable::setLabel(
                   const int32_t key,
                   const AString& name,
                   const float red,
                   const float green,
                   const float blue,
                   const float alpha)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        Label* gl = iter->second;
        gl->setName(name);
        float rgba[4] = { red, green, blue, alpha };
        gl->setColor(rgba);
    }
    else {
        Label gl(key, name, red, green, blue, alpha);
        this->addLabel(&gl);
    }
}

/**
 * Set a label.  If a label with the specified key exists,
 * it is replaced.
 * 
 * @param key    Key for label.
 * @param name   Name of label.
 * @param red    Red color component.
 * @param green  Green color component.
 * @param blue   Blue color component.
 * @param alpha  Alpha color component.
 * @param x      The X-coordinate.
 * @param y      The Y-coordinate.
 * @param z      The Z-coordinate.
 *
 */
void
LabelTable::setLabel(const int32_t key,
                          const AString& name,
                          const float red,
                          const float green,
                          const float blue,
                          const float alpha,
                          const float x,
                          const float y,
                          const float z)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        Label* gl = iter->second;
        gl->setName(name);
        float rgba[4] = { red, green, blue, alpha };
        gl->setColor(rgba);
        gl->setX(x);
        gl->setY(y);
        gl->setZ(z);
    }
    else {
        Label gl(key, name, red, green, blue, alpha, x, y, z);
        this->addLabel(&gl);
    }
}

/**
 * Get the selection status of the label at the specified key.  If there
 * is no label at the key, false is returned.
 * @param key - key of label
 * @return  selection status of label.
 *
 */
bool
LabelTable::isLabelSelected(const int32_t key) const
{
    LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        return iter->second->isSelected();
    }
    return false;
}

/**
 * Set the selection status of a label.
 * @param key - key of label.
 * @param sel - new selection status.
 *
 */
void
LabelTable::setLabelSelected(
                   const int32_t key,
                   const bool sel)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        iter->second->setSelected(sel);
    }
}

/**
 * Set the selection status for all labels.
 * @param newStatus  New selection status.
 *
 */
void
LabelTable::setSelectionStatusForAllLabels(const bool newStatus)
{
    for (LABELS_MAP_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        Label* gl = iter->second;
        gl->setSelected(newStatus);
    }
}

/**
 * Get the alpha color component for a label.  If the key is not a
 * valid label, an alpha of zero is returned.
 * @param key - Key of label.
 * @return  Alpha for label or zero if invalid key.
 *
 */
float
LabelTable::getLabelAlpha(const int32_t key) const
{
    const Label* gl = this->getLabel(key);
    if (gl != NULL) {
        return gl->getAlpha();
    }
    return 0.0;
}

/**
 * Get the color for a label.
 * @param key - key of label.
 * @return Its color components or null if it is an invalid key.
 *
 */
void
LabelTable::getLabelColor(const int32_t key, float rgbaOut[4]) const
{
    const Label* gl = this->getLabel(key);
    if (gl != NULL) {
        gl->getColor(rgbaOut);
    }
}

/**
 * Set the color of a label.
 * @param key - key of label.
 * @param color - new color of label.
 *
 */
void
LabelTable::setLabelColor(
                   const int32_t key,
                   const float color[])
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        Label* gl = iter->second;
        gl->setColor(color);
    }
}

void
LabelTable::writeXML(XmlWriter& xmlWriter) const
{
    //
    // Write the label tag
    //
    xmlWriter.writeStartElement("LabelTable");
    
    //
    // Write the labels
    //
    std::set<int32_t> keys = this->getKeys();
    for (std::set<int32_t>::const_iterator iter = keys.begin();
            iter != keys.end();
            iter++) {
        int key = *iter;
        const Label* label = this->getLabel(key);
        if (label != NULL) {
            xmlWriter.writeStartElement("Label");
            xmlWriter.writeAttribute("Key", AString_number(key));
            float* rgba = label->getColor();
            xmlWriter.writeAttribute("Red", AString_number(rgba[0]));
            xmlWriter.writeAttribute("Green", AString_number(rgba[1]));
            xmlWriter.writeAttribute("Blue", AString_number(rgba[2]));
            xmlWriter.writeAttribute("Alpha", AString_number(rgba[3]));
            xmlWriter.writeCharacters(label->getName());
            xmlWriter.writeEndElement();
            delete[] rgba;
        }
    }
    
    //
    // Write the closing label tag
    //
    xmlWriter.writeEndElement();
}

void LabelTable::readXml(XmlReader& xml)
{
    clear();
#ifdef CIFTILIB_USE_QT
    if (!xml.isStartElement() || xml.name() != "LabelTable")
    {
        throw CiftiException("tried to read LabelTable when current element is not LabelTable");
    }
    while (xml.readNextStartElement() && !xml.atEnd())
    {
        if (xml.name() != "Label")
        {
            throw CiftiException("unexpected element '" + xml.name().toString() + "' encountered in Label");
        }
        int key;
        float rgba[4];
        QXmlStreamAttributes myAttrs = xml.attributes();
        bool ok = false;
        AString temp = myAttrs.value("Key").toString();
        key = temp.toInt(&ok);
        if (!ok) throw CiftiException("Key attribute of Label missing or noninteger");
        temp = myAttrs.value("Red").toString();
        rgba[0] = temp.toFloat(&ok);
        if (!ok) throw CiftiException("Red attribute of Label missing or not a number");
        temp = myAttrs.value("Green").toString();
        rgba[1] = temp.toFloat(&ok);
        if (!ok) throw CiftiException("Green attribute of Label missing or not a number");
        temp = myAttrs.value("Blue").toString();
        rgba[2] = temp.toFloat(&ok);
        if (!ok) throw CiftiException("Blue attribute of Label missing or not a number");
        temp = myAttrs.value("Alpha").toString();
        if (temp == "")
        {
            rgba[3] = 1.0f;
        } else {
            rgba[3] = temp.toFloat(&ok);
            if (!ok) throw CiftiException("Alpha attribute of Label is not a number");
        }
        temp = xml.readElementText();
        if (xml.hasError()) return;
        setLabel(key, temp, rgba[0], rgba[1], rgba[2], rgba[3]);
    }
#else
#ifdef CIFTILIB_USE_XMLPP
    vector<AString> mandAttrs(4), optAttrs(1, "Alpha");
    mandAttrs[0] = "Key";
    mandAttrs[1] = "Red";
    mandAttrs[2] = "Green";
    mandAttrs[3] = "Blue";
    bool done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
    while(!done && xml.read())
    {
        switch (xml.get_node_type())
        {
            case XmlReader::Element:
            {
                AString name = xml.get_local_name();
                if (name == "Label")
                {
                    XmlAttributesResult myAttrs = XmlReader_parseAttributes(xml, mandAttrs, optAttrs);
                    int key;
                    float rgba[4];
                    bool ok = false;
                    key = AString_toInt(myAttrs.mandatoryVals[0], ok);
                    if (!ok) throw CiftiException("Key attribute of Label is not an integer");
                    rgba[0] = AString_toFloat(myAttrs.mandatoryVals[1], ok);
                    if (!ok) throw CiftiException("Red attribute of Label is not a number");
                    rgba[1] = AString_toFloat(myAttrs.mandatoryVals[2], ok);
                    if (!ok) throw CiftiException("Green attribute of Label is not a number");
                    rgba[2] = AString_toFloat(myAttrs.mandatoryVals[3], ok);
                    if (!ok) throw CiftiException("Blue attribute of Label is not a number");
                    if (myAttrs.optionalVals[0].present)
                    {
                        rgba[3] = AString_toFloat(myAttrs.optionalVals[0].value, ok);
                        if (!ok) throw CiftiException("Alpha attribute of Label is not a number");
                    } else {
                        rgba[3] = 1.0f;
                    }
                    AString name = XmlReader_readElementText(xml);
                    setLabel(key, name, rgba[0], rgba[1], rgba[2], rgba[3]);
                } else {
                    throw CiftiException("unexpected element in LabelTable: " + name);
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
    CiftiAssert(XmlReader_checkEndElement(xml, "LabelTable"));
}

/**
 * Get the valid keys of the labels in ascending order.
 * @return  A Set containing the valid keys of the label in
 *    ascending order.
 *
 */
std::set<int32_t>
LabelTable::getKeys() const
{
    std::set<int32_t> keys;
    for (std::map<int32_t,Label*>::const_iterator iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        keys.insert(iter->first);
    }
    return keys;
}

void LabelTable::getKeys(std::vector<int32_t>& keysOut) const
{
    keysOut.reserve(labelsMap.size());
    for (std::map<int32_t,Label*>::const_iterator iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        keysOut.push_back(iter->first);
    }
}

/**
 * Get all keys and names.
 * 
 * @parm keysAndNamesOut
 *     Map containing the pairs of corresponding keys and names.
 */
void
LabelTable::getKeysAndNames(std::map<int32_t, AString>& keysAndNamesOut) const
{
    keysAndNamesOut.clear();
    
    for (std::map<int32_t,Label*>::const_iterator iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        const Label* gl = iter->second;
        keysAndNamesOut.insert(std::make_pair(iter->first,
                                              gl->getName()));
    }
}

bool LabelTable::matches(const LabelTable& rhs, const bool checkColors, const bool checkCoords) const
{
    if (labelsMap.size() != rhs.labelsMap.size()) return false;
    for (LABELS_MAP::const_iterator iter = labelsMap.begin(); iter != labelsMap.end(); ++iter)
    {
        LABELS_MAP::const_iterator riter = rhs.labelsMap.find(iter->first);
        if (riter == rhs.labelsMap.end()) return false;
        if (!iter->second->matches(*(riter->second), checkColors, checkCoords)) return false;
    }
    return true;
}
