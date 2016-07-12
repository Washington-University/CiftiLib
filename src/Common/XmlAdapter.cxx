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

#include "XmlAdapter.h"

#include "CiftiAssert.h"

using namespace std;
using namespace cifti;

XmlAttributesResult cifti::XmlReader_parseAttributes(XmlReader& xml, const vector<AString>& mandatoryNames, const vector<AString>& optionalNames)
{
    XmlAttributesResult ret;
    int numMandatory = (int)mandatoryNames.size();//if you require more than 2 billion attributes, w3c would like a word with you
    int numOptional = (int)optionalNames.size();
    ret.mandatoryVals.resize(numMandatory);
    ret.optionalVals.resize(numOptional);
    AString elemName;
#ifdef CIFTILIB_USE_QT
    vector<bool> mandatoryPresent(numMandatory, false);
    map<AString, int> mandatoryMap, optionalMap;
    for (int i = 0; i < numMandatory; ++i)//set up map lookups so it isn't n^2
    {
        mandatoryMap[mandatoryNames[i]] = i;
    }
    for (int i = 0; i < numOptional; ++i)
    {
        optionalMap[optionalNames[i]] = i;
    }
    CiftiAssert(xml.isStartElement());
    elemName = xml.name().toString();
    QXmlStreamAttributes myAttrs = xml.attributes();
    int numAttrs = myAttrs.size();
    for (int i = 0; i < numAttrs; ++i)
    {
        QString name = myAttrs[i].name().toString();
        map<AString, int>::iterator iter = mandatoryMap.find(name);
        if (iter == mandatoryMap.end())
        {
            iter = optionalMap.find(name);
            if (iter != optionalMap.end())//NOTE: ignore unrecognized attributes for now, because MatrixIndicesMap has attributes that are used by different objects
            {//other option is to include such attributes in both calls, even if not used there
                ret.optionalVals[iter->second].present = true;
                ret.optionalVals[iter->second].value = myAttrs[i].value().toString();
            }
        } else {
            ret.mandatoryVals[iter->second] = myAttrs[i].value().toString();
            mandatoryPresent[iter->second] = true;
        }
    }
    for (int i = 0; i < numMandatory; ++i)
    {
        if (mandatoryPresent[i] == false)
        {
            throw CiftiException(elemName + " element is missing the " + mandatoryNames[i] + " attribute");
        }
    }
#else
#ifdef CIFTILIB_USE_XMLPP
    CiftiAssert(xml.get_node_type() == XmlReader::Element);
    elemName = xml.get_local_name();
    for (int i = 0; i < numMandatory; ++i)//NOTE: libxml++ (and even libxml2 to some extent) don't have a good interface for iterating through all attributes efficiently
    {//you have to iterate through a linked list of attributes exposed through a "hacking interface"
        ret.mandatoryVals[i] = xml.get_attribute(mandatoryNames[i]);
        if (ret.mandatoryVals[i] == "")//HACK: treat empty value same as missing attribute
        {
            throw CiftiException(elemName + " element is missing the " + mandatoryNames[i] + " attribute");
        }
    }
    for (int i = 0; i < numOptional; ++i)
    {
        AString value = xml.get_attribute(optionalNames[i]);
        if (value != "")//HACK: and again
        {
            ret.optionalVals[i].present = true;
            ret.optionalVals[i].value = value;
        }
    }
#else
#error "not implemented"
#endif
#endif
    return ret;
}

bool cifti::XmlReader_checkEndElement(XmlReader& xml, const AString& elementName)
{
#ifdef CIFTILIB_USE_QT
    return xml.hasError() || (xml.isEndElement() && xml.name() == elementName);//if it has an xml error, don't trip an assert
#else
#ifdef CIFTILIB_USE_XMLPP
    return (xml.get_node_type() == XmlReader::EndElement || xml.is_empty_element()) && xml.get_local_name() == elementName;
#else
#error "not implemented"
#endif
#endif
}

AString cifti::XmlReader_readElementText(XmlReader& xml)
{
#ifdef CIFTILIB_USE_QT
    return xml.readElementText();//NOTE: requires calling code to check for xml.hasError() when using QT
#else
#ifdef CIFTILIB_USE_XMLPP
    AString ret;
    CiftiAssert(xml.get_node_type() == XmlReader::Element);
    AString elemName = xml.get_local_name();
    bool done = xml.is_empty_element();//NOTE: a <blah/> element does NOT give a separate end element state!!!
    while(!done && xml.read())
    {
        switch(xml.get_node_type())
        {
            case XmlReader::Element:
                throw CiftiException("unexpected element inside " + elemName + " element: " + xml.get_local_name());
            case XmlReader::Text:
            case XmlReader::CDATA:
                ret += xml.get_value();
                break;
            case XmlReader::EndElement:
                done = true;
                break;
            default:
                break;
        }
    }
    return ret;
#else
#error "not implemented"
#endif
#endif
}

bool cifti::XmlReader_readNextStartElement(XmlReader& xml)
{
#ifdef CIFTILIB_USE_QT
    return xml.readNextStartElement();//NOTE: requires calling code to check for xml.hasError() when using QT
#else
#ifdef CIFTILIB_USE_XMLPP
    if (xml.is_empty_element()) return false;//NOTE: a <blah/> element does NOT give a separate end element state!!!
    while (xml.read())
    {
        switch (xml.get_node_type())
        {
            case XmlReader::Element:
                return true;
            case XmlReader::EndElement:
                return false;
            default:
                break;
        }
    }
    return false;
#else
#error "not implemented"
#endif
#endif
}

AString cifti::XmlReader_elementName(XmlReader& xml)
{
#ifdef CIFTILIB_USE_QT
    return xml.name().toString();
#else
#ifdef CIFTILIB_USE_XMLPP
    return xml.get_local_name();
#else
#error "not implemented"
#endif
#endif
}